// *** clone.c ********************************************************
// this file clones the contents of flash to the qspi master port,
// which can be connected to the ezport on a slave cpu for initial
// code load.

#include "main.h"

#define SLAVE_FSYS    8000000  // frequency of target board

#define CLONE_PAGE_SIZE  256

#define WRITE_CONFIGURATION_REGISTER  0x01
#define PAGE_PROGRAM  0x02
#define READ_DATA  0x03
#define READ_STATUS_REGISTER  0x05
#define WRITE_ENABLE  0x06
#define BULK_ERASE  0xc7

#define WRITE_ERROR  0x40
#define WRITE_ENABLE_STATUS  0x02
#define WRITE_IN_PROGRESS  0x01

static
int
flash_status(void)
{
    byte buffer[2];

    // read the status register
    buffer[0] = READ_STATUS_REGISTER;
    qspi_transfer(buffer, 1+1);

    return buffer[1];
}

static
bool
flash_write(byte *buffer, int length)
{
    byte status;
    byte command;

    status = flash_status();
    if (status & (WRITE_ERROR|WRITE_IN_PROGRESS)) {
        return false;
    }

    // enable the write
    command = WRITE_ENABLE;
    qspi_transfer(&command, 1);

    status = flash_status();
    if (! (status & WRITE_ENABLE_STATUS)) {
        return false;
    }

    // do the write
    qspi_transfer(buffer, length);

    // wait for the write to complete
    do {
        status = flash_status();
        if (status & WRITE_ERROR) {
            return false;
        }
    } while (status & WRITE_IN_PROGRESS);

    return true;
}

void
clone(bool and_run)
{
    int i;
    unsigned n;
    byte status;

    assert(sizeof(big_buffer) >= 4+CLONE_PAGE_SIZE);
    
    // AS is gpio output; bit 0 is slave rsti*
    MCF_GPIO_PASPAR = 0;
    MCF_GPIO_DDRAS = 0x3;
    
    qspi_inactive(0);

    // pulse slave rsti* low
    MCF_GPIO_CLRAS = ~1;
    delay(1);
    MCF_GPIO_SETAS = 1;
    delay(1);
    
    qspi_inactive(1);

    status = flash_status();
    if (status & (WRITE_ERROR|WRITE_IN_PROGRESS)) {
        printf("initialization failed\n");
        return;
    }

    // write configuration register
    big_buffer[0] = WRITE_CONFIGURATION_REGISTER;
    if (SLAVE_FSYS > 25600000) {
        big_buffer[1] = MCF_CFM_CFMCLKD_PRDIV8|MCF_CFM_CFMCLKD_DIV((SLAVE_FSYS-1)/2/8/200000);
    } else {
        big_buffer[1] = MCF_CFM_CFMCLKD_DIV((SLAVE_FSYS-1)/2/200000);
    }
    if (! flash_write(big_buffer, 2+0)) {
        printf("write configuration register failed\n");
        return;
    }

    // bulk erase
    big_buffer[0] = BULK_ERASE;
    if (! flash_write(big_buffer, 1+0)) {
        printf("bulk erase failed\n");
        return;
    }

    // for all bytes to clone...
    for (n = 0; n < FLASH_BYTES; n += CLONE_PAGE_SIZE) {
        if (SECURE && n >= FLASH_BYTES/2) {
            // reference data is secure; just erase it
            memset(big_buffer+4, -1, CLONE_PAGE_SIZE);
        } else {
            // get the reference data from our flash
            memcpy(big_buffer+4, (void *)n, CLONE_PAGE_SIZE);
        }

        // program the data to the target device
        big_buffer[0] = PAGE_PROGRAM;
        big_buffer[1] = n/0x10000;
        big_buffer[2] = n/0x100;
        big_buffer[3] = n;
        if (! flash_write(big_buffer, 4+CLONE_PAGE_SIZE)) {
            printf("\npage program failed\n");
            return;
        }

        memset(big_buffer+4, 0x5a, CLONE_PAGE_SIZE);

        // read the data back from the target device
        big_buffer[0] = READ_DATA;
        big_buffer[1] = n/0x10000;
        big_buffer[2] = n/0x100;
        big_buffer[3] = n;
        qspi_transfer(big_buffer, 4+CLONE_PAGE_SIZE);

        // verify the data
        if (SECURE && n >= FLASH_BYTES/2) {
            // reference data was erased
            for (i = 0; i < CLONE_PAGE_SIZE; i++) {
                if (*(char *)(big_buffer+4+i) != (char)-1) {
                    printf("\nverification failed at offset 0x%x\n", n+i);
                    return;
                }
            }
        } else {
            // reference data came from our flash
            if (memcmp(big_buffer+4, (void *)n, CLONE_PAGE_SIZE)) {
                printf("\nverification failed at offset ~0x%x\n", n);
                return;
            }
        }

#if ! FLASHER
        printf(".");
#else
        if (n/CLONE_PAGE_SIZE % 2) {
            printf(".");
        }
        if (n/CLONE_PAGE_SIZE % 160 == 159) {
            printf("\n");
        }
#endif
    }

    if (and_run) {
        // allow the target to run!
        // pulse slave rsti* low
        MCF_GPIO_CLRAS = ~1;
        delay(1);
        MCF_GPIO_SETAS = 1;
        delay(1);
    }

    printf("\nclone done!\n");
}

