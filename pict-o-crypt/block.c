#include "main.h"

#if USB_HOST || PICTOCRYPT
#define OPTIMIZE  1

#define RETRIES  2

#define NLRUS  4

#if OPTIMIZE
static struct lru {
    byte buffer[512];
    uint32 sector;
    int ticks;
    bool filled;
    int dirty;  // non-0 is ticks -> write back on purge
} lrus[NLRUS];
#endif

int PhysReadWriteSector(bool read, uint8_t *buffer, uint32_t sector, uint32_t count)
{
    int rv;
    bool retry;
    byte cdb[12];
    byte buf[36];
    int max_rv;
    byte max_lun;
    struct setup setup;
    static uint32 last_scsi_attached_count;
    
    //printf("%s sector %d for %d\n", read?"read":"write", sector, count);
    
    retry = false;
    
    if (scsi_attached_count != last_scsi_attached_count) {
        last_scsi_attached_count = scsi_attached_count;
    
        // set interface
        usb_setup(0, SETUP_TYPE_STANDARD, SETUP_RECIP_INTERFACE, 0x0b, 0, 0, 0, &setup);
        rv = usb_control_transfer(&setup, NULL, 0);
        //assert(rv == 0);

        // bulk only reset
        //usb_setup(0, SETUP_TYPE_CLASS, SETUP_RECIP_INTERFACE, 0xff, 0, 0, 0, &setup);
        //rv = usb_control_transfer(&setup, NULL, 0);
        //assert(rv == 0);

        // get max lun
        usb_setup(1, SETUP_TYPE_CLASS, SETUP_RECIP_INTERFACE, 0xfe, 0, 0, sizeof(max_lun), &setup);
        max_rv = usb_control_transfer(&setup, &max_lun, sizeof(max_lun));
        //assert(max_rv == 1 && max == 0);
        
        scsi_lun = 0;
XXX_NEXTLUN_XXX:        
        // inquiry
        memset(cdb, 0, sizeof(cdb));
        cdb[0] = 0x12;  // inquiry
        cdb[4] = 36;
        rv = scsi_bulk_transfer(1, cdb, 6, buf, 36);
        if (rv < 0) {
            return rv;
        }
        assert(rv == 36);
        
        // test unit ready
        memset(cdb, 0, sizeof(cdb));
        cdb[0] = 0x00;  // test unit ready
        rv = scsi_bulk_transfer(0, cdb, 6, NULL, 0);
        
        // request sense
        memset(cdb, 0, sizeof(cdb));
        cdb[0] = 0x03;  // request sense
        cdb[4] = 18;
        rv = scsi_bulk_transfer(1, cdb, 6, buf, 18);
        if (rv < 0) {
            return rv;
        }
        assert(rv);
        
        // test unit ready
        memset(cdb, 0, sizeof(cdb));
        cdb[0] = 0x00;  // test unit ready
        rv = scsi_bulk_transfer(0, cdb, 6, NULL, 0);
        if (rv < 0 && max_rv == 1 && scsi_lun < max_lun) {
            scsi_lun++;
            goto XXX_NEXTLUN_XXX;
        }
        if (rv < 0) {
            return rv;
        }
            
        // read format capacities
        memset(cdb, 0, sizeof(cdb));
        cdb[0] = 0x23;  // read format capacities
        cdb[8] = sizeof(buf);
        rv = scsi_bulk_transfer(1, cdb, 12, buf, sizeof(buf));
        assert(rv);
        
        // read capacity
        memset(cdb, 0, sizeof(cdb));
        cdb[0] = 0x25;  // read capacity
        rv = scsi_bulk_transfer(1, cdb, 10, buf, 8);
        assert(rv == 8);
        
        // request sense
        memset(cdb, 0, sizeof(cdb));
        cdb[0] = 0x03;  // request sense
        cdb[4] = 18;
        rv = scsi_bulk_transfer(1, cdb, 6, buf, 18);
        assert(rv);

        // test unit ready
        memset(cdb, 0, sizeof(cdb));
        cdb[0] = 0x00;  // test unit ready
        rv = scsi_bulk_transfer(0, cdb, 6, NULL, 0);
    }
    
    // read block
XXX_RETRY_XXX:
    memset(cdb, 0, sizeof(cdb));
    cdb[0] = read?0x28:0x2a;  // read10/write10
    cdb[2] = sector>>24;
    cdb[3] = sector>>16;
    cdb[4] = sector>>8;
    cdb[5] = sector>>0;
    assert(count < 256);
    cdb[8] = count;
    rv = scsi_bulk_transfer(read, cdb, 10, buffer, count*512);
    if (rv < 0) {
        if (retry++ > RETRIES) {
            return rv;
        }

        // request sense
        memset(cdb, 0, sizeof(cdb));
        cdb[0] = 0x03;  // request sense
        cdb[4] = 18;
        rv = scsi_bulk_transfer(1, cdb, 6, buf, 18);
        //assert(rv);

        // test unit ready
        memset(cdb, 0, sizeof(cdb));
        cdb[0] = 0x00;  // test unit ready
        rv = scsi_bulk_transfer(0, cdb, 6, NULL, 0);
            
        goto XXX_RETRY_XXX;
    }
    
    assert(rv == count*512);
    led_unknown_progress();
    return 0;
}

int DFS_HostReadSector(uint8_t *buffer, uint32_t sector, uint32_t count)
{
#if OPTIMIZE    
    int i;
    int rv;
    int oi;
    int oticks;
#endif
    
    //DFS_HostFlush(5000);
    
    if (! OPTIMIZE || count > 1) {
        return PhysReadWriteSector(1, buffer, sector, count);
    }
    
#if OPTIMIZE    
    // reuse an existing slot
    for (i = 0; i < LENGTHOF(lrus); i++) {
        if (lrus[i].filled && lrus[i].sector == sector) {
XXX_USE_XXX:
            if (! lrus[i].filled) {
                rv = PhysReadWriteSector(1, lrus[i].buffer, sector, 1);
                if (rv) {
                    return rv;
                }
            }
            
            memcpy(buffer, lrus[i].buffer, 512);
            lrus[i].sector = sector;
            lrus[i].ticks = ticks;
            lrus[i].filled = true;
            return 0;
        }
    }
    
    
    // use an empty slot
    for (i = 0; i < LENGTHOF(lrus); i++) {
        if (! lrus[i].filled) {
            goto XXX_USE_XXX;
        }
    }
    
    // flush the oldest slot
    oticks = ticks+1000;  // wraparound?
    for (i = 0; i < LENGTHOF(lrus); i++) {
        assert(lrus[i].filled);
        if (lrus[i].ticks < oticks) {
            oi = i;
            oticks = lrus[i].ticks;
        }
    }
    i = oi;
    
    if (lrus[i].dirty) {
        rv = PhysReadWriteSector(0, lrus[i].buffer, lrus[i].sector, 1);
        if (rv) {
            return rv;
        }
        lrus[i].dirty = 0;
    }
    lrus[i].filled = false;
    
    goto XXX_USE_XXX;
#endif
}

int DFS_HostWriteSector(uint8_t *buffer, uint32_t sector, uint32_t count)
{
#if OPTIMIZE
    int i;
    int rv;
    int oi;
    int oticks;
#endif
    
    //DFS_HostFlush(5000);

    if (! OPTIMIZE || count > 1) {
        return PhysReadWriteSector(0, buffer, sector, count);
    }
    
#if OPTIMIZE
    // reuse an existing slot
    for (i = 0; i < LENGTHOF(lrus); i++) {
        if (lrus[i].filled && lrus[i].sector == sector) {
XXX_USE_XXX:
            memcpy(lrus[i].buffer, buffer, 512);
            lrus[i].sector = sector;
            lrus[i].ticks = ticks;
            lrus[i].filled = true;
            if (! lrus[i].dirty) {
                lrus[i].dirty = ticks;
            }
            return 0;
        }
    }
    
    // use an empty slot
    for (i = 0; i < LENGTHOF(lrus); i++) {
        if (! lrus[i].filled) {
            goto XXX_USE_XXX;
        }
    }
    
    // flush the oldest slot
    oticks = ticks+1000;  // wraparound?
    for (i = 0; i < LENGTHOF(lrus); i++) {
        assert(lrus[i].filled);
        if (lrus[i].ticks < oticks) {
            oi = i;
            oticks = lrus[i].ticks;
        }
    }
    i = oi;
    
    if (lrus[i].dirty) {
        rv = PhysReadWriteSector(0, lrus[i].buffer, lrus[i].sector, 1);
        if (rv) {
            return rv;
        }
        lrus[i].dirty = 0;
    }
    lrus[i].filled = false;
    
    goto XXX_USE_XXX;
#endif
}

void DFS_HostFlush(int ms)
{
#if OPTIMIZE
    int i;
    int rv;
    
    for (i = 0; i < LENGTHOF(lrus); i++) {
        if (lrus[i].filled && lrus[i].dirty && ticks-lrus[i].dirty >= ms) {
            rv = PhysReadWriteSector(0, lrus[i].buffer, lrus[i].sector, 1);
            //assert(! rv);  // revisit -- what to do???
            lrus[i].dirty = 0;
        }
    }
#endif
}

void DFS_HostPurge(void)
{
#if OPTIMIZE
    int i;
    
    for (i = 0; i < LENGTHOF(lrus); i++) {
        lrus[i].filled = 0;
    }
#endif
}
#endif

