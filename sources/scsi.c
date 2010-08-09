#include "main.h"

#if SCSI
// perform a usb host/device scsi bulk transfer
int
scsi_bulk_transfer(int in, byte *cdb, int cdb_length, byte *buffer, int length)
{
    int rv;
    int total;
    struct cbw cbw;
    struct csw csw;
    static int tag;

    memset(&cbw, 0, sizeof(cbw));
    cbw.signature = byteswap(0x43425355);
    cbw.tag = tag++;
    cbw.datatransferlength = byteswap(length);
    cbw.flags = in?FLAGS_DATA_IN:FLAGS_DATA_OUT;
    cbw.lun = 0;
    cbw.cdblength = (byte)cdb_length;
    memcpy(cbw.cdb, cdb, cdb_length);

    rv = usb_bulk_transfer(0, (byte *)&cbw, CBW_LENGTH, 0);
    if (rv < 0) {
        return rv;
    }
    assert(rv == CBW_LENGTH);

    total = usb_bulk_transfer(in, buffer, length, 0);
    if (total < 0) {
        return total;
    }

    rv = usb_bulk_transfer(1, (byte *)&csw, CSW_LENGTH, 0);
    if (rv < 0) {
        return rv;
    } else {
        assert(rv == CSW_LENGTH);
        assert(csw.status == 0);
    }

    return total;
}

void
scsi_run(void)
{
    int rv;
    int once;
    byte cdb[10];
    byte block[512];

    once = 0;
    for (;;) {
        // read block 0
        memset(cdb, 0, sizeof(cdb));
        cdb[0] = 0x28;  // read10
        cdb[8] = 0x01;
        rv = scsi_bulk_transfer(1, cdb, 10, block, sizeof(block));
        if (rv == -1) {
            break;
        }
        assert(rv == sizeof(block));
        led_happy();

        if (! once) {
            // write it back, just once!!!
            memset(cdb, 0, sizeof(cdb));
            cdb[0] = 0x2a;  // write10
            cdb[8] = 0x01;
            rv = scsi_bulk_transfer(0, cdb, 10, block, sizeof(block));
            if (rv == -1) {
                break;
            }
            assert(rv == sizeof(block));
            led_happy();

            once = 1;
        }
    }
}
#endif
