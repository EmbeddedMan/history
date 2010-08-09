// *** flash.c ********************************************************
// this file implements the low level flash control and access, as well
// as the "s19 upgrade" functionality for upgrading firmware.

#include "main.h"

static
void
flash_command(uint8 cmd, uint32 *addr, uint32 data)
{
    uint32 *backdoor_addr;

    // assert we're initialized
    assert(MCF_CFM_CFMCLKD & MCF_CFM_CFMCLKD_DIVLD);

    // assert we're ready
    assert(MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CBEIF);

    // assert no errors
    assert(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));

    // write the flash thru the backdoor address
    backdoor_addr = (uint32 *)(__IPSBAR+0x04000000+(int)addr);
    *backdoor_addr = data;

    // write the command
    MCF_CFM_CFMCMD = cmd;

    // launch the command (N.B. this clears CBEIF!)
    MCF_CFM_CFMUSTAT = MCF_CFM_CFMUSTAT_CBEIF;

    // busy wait for command buffer empty
    while (! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CBEIF)) {
        // assert no errors
        assert(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));
        // NULL
    }
}

// this function erases the specified pages of flash memory.
void
flash_erase_pages(uint32 *addr, uint32 npages)
{
    int x;
    
    x = splx(7);
    
    // while there are more pages to erase...
    while (npages) {
        flash_command(MCF_CFM_CFMCMD_PAGE_ERASE, addr, 0);
        npages--;
        addr += FLASH_PAGE_SIZE/sizeof(uint32);
    }

    // busy wait for flash command complete
    while (! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CCIF)) {
        // NULL
    };
    
    (void)splx(x);
}

// this function writes the specified words of flash memory.
void
flash_write_words(uint32 *addr_in, uint32 *data_in, uint32 nwords_in)
{
    int i;
    int x;
    uint32 *addr;
    uint32 *data;
    uint32 nwords;

    addr = addr_in;
    data = data_in;
    nwords = nwords_in;
    
    x = splx(7);

    // while there are more words to program...
    while (nwords) {
        flash_command(MCF_CFM_CFMCMD_WORD_PROGRAM, addr, *data);
        nwords--;
        addr++;
        data++;
    }

    // busy wait for flash command complete
    while (! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CCIF)) {
        // NULL
    };
    
    (void)splx(x);

    for (i = 0; i < nwords_in; i++) {
        assert(addr_in[i] == data_in[i]);
    }
}

// this function performs the final step of a firmware flash upgrade.
void
flash_upgrade_ram_begin(bool compatible)
{
    uint32 *addr;
    uint32 *data;
    uint32 nwords;
    uint32 npages;
    uint32 *backdoor_addr;

    // N.B. this code generates no relocations so we can run it from RAM!!!

    // erase the firmware code
    // flash_erase_pages()
    addr = NULL;
    npages = FLASH_BYTES/2/FLASH_PAGE_SIZE;
    if (compatible) {
        // N.B. we skip page0
        npages--;
        addr += FLASH_PAGE_SIZE/sizeof(uint32);
    }
    while (npages) {
        assert(MCF_CFM_CFMCLKD & MCF_CFM_CFMCLKD_DIVLD);
        assert(MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CBEIF);
        assert(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));
        backdoor_addr = (uint32 *)(__IPSBAR+0x04000000+(int)addr);
        *backdoor_addr = 0;
        MCF_CFM_CFMCMD = MCF_CFM_CFMCMD_PAGE_ERASE;
        MCF_CFM_CFMUSTAT = MCF_CFM_CFMUSTAT_CBEIF;
        while (! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CBEIF)) {
            assert(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));
        }
        npages--;
        addr += FLASH_PAGE_SIZE/sizeof(uint32);
    }
    while (! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CCIF)) {
    };

    // and re-flash it from the staging area
    // flash_write_words()
    addr = NULL;
    data = (uint32 *)(FLASH_BYTES/2);
    nwords = FLASH_BYTES/2/sizeof(uint32);
    if (compatible) {
        // N.B. we skip page0
        nwords -= FLASH_PAGE_SIZE/sizeof(uint32);
        addr += FLASH_PAGE_SIZE/sizeof(uint32);
        data += FLASH_PAGE_SIZE/sizeof(uint32);
    }
    while (nwords) {
        assert(MCF_CFM_CFMCLKD & MCF_CFM_CFMCLKD_DIVLD);
        assert(MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CBEIF);
        assert(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));
        backdoor_addr = (uint32 *)(__IPSBAR+0x04000000+(int)addr);
        *backdoor_addr = *data;
        MCF_CFM_CFMCMD = MCF_CFM_CFMCMD_WORD_PROGRAM;
        MCF_CFM_CFMUSTAT = MCF_CFM_CFMUSTAT_CBEIF;
        while (! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CBEIF)) {
            assert(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));
        }
        nwords--;
        addr++;
        data++;
    }
    while (! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CCIF)) {
    };

    // erase the staging area
    // flash_erase_pages(FLASH_BYTES/2, FLASH_BYTES/2/FLASH_PAGE_SIZE)
    addr = (uint32 *)(FLASH_BYTES/2);
    npages = FLASH_BYTES/2/FLASH_PAGE_SIZE;
    while (npages) {
        assert(MCF_CFM_CFMCLKD & MCF_CFM_CFMCLKD_DIVLD);
        assert(MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CBEIF);
        assert(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));
        backdoor_addr = (uint32 *)(__IPSBAR+0x04000000+(int)addr);
        *backdoor_addr = 0;
        MCF_CFM_CFMCMD = MCF_CFM_CFMCMD_PAGE_ERASE;
        MCF_CFM_CFMUSTAT = MCF_CFM_CFMUSTAT_CBEIF;
        while (! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CBEIF)) {
            assert(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));
        }
        npages--;
        addr += FLASH_PAGE_SIZE/sizeof(uint32);
    }
    while (! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CCIF)) {
    };

    // reset the MCU
    MCF_RCM_RCR = MCF_RCM_RCR_SOFTRST;
    asm { halt }
}

// this function just demarcates the end of flash_upgrade_ram_begin().
void
flash_upgrade_ram_end(void)
{
    // NULL
}

// this function downloads a new s19 firmware file to a staging
// area, and then installs it by calling a RAM copy of
// flash_upgrade_ram_begin().
void
flash_upgrade()
{
    int b;
    int i;
    int n;
    int x;
    int y;
    char c;
    int sum;
    int addr;
    int count;
    char *s19;
    bool done;
    uint32 data;
    bool s0_found;
    bool compatible;
    flash_upgrade_ram_begin_f fn;
    uint32 buffer[16];

    if ((int)end_of_static > FLASH_BYTES/2) {
        printf("code exceeds half of flash\n");
        return;
    }

    // erase the staging area
    flash_erase_pages((uint32 *)(FLASH_BYTES/2), FLASH_BYTES/2/FLASH_PAGE_SIZE);

    printf("paste S19 upgrade file now...\n");
    terminal_echo = false;

    y = 0;
    done = false;
    s0_found = false;

    do {
        // wait for an s19 command line
        if (main_command) {
            main_command = NULL;
            terminal_command_ack(false);
        }

        while (! main_command) {
            terminal_poll();
        }

        s19 = main_command;
        while (isspace(*s19)) {
            s19++;
        }

        if (! *s19) {
            continue;
        }

        sum = 0;

        // parse s19 header
        if (*s19++ != 'S') {
            printf("\nbad record\n");
            break;
        }
        c = *s19++;

        if (c == '0') {
            s0_found = true;
        } else if (c == '3') {
            // parse data record
            // 1 byte of count
            n = get2hex(&s19);
            if (n == -1) {
                printf("\nbad count\n");
                break;
            }
            sum += n;
            count = n;

            // we flash 4 bytes at a time!
            if ((count-1) % 4) {
                printf("\nbad count\n");
                break;
            }

            // 4 bytes of address
            addr = 0;
            for (i = 0; i < 4; i++) {
                n = get2hex(&s19);
                if (n == -1) {
                    printf("\nbad address\n");
                    break;
                }
                sum += n;
                addr = addr*256+n;
            }
            if (i != 4) {
                break;
            }

            assert(count > 4);
            count -= 4;

            // while there is more data
            b = 0;
            while (count > 1) {
                assert((count-1) % 4 == 0);

                // get 4 bytes of data
                data = 0;
                for (i = 0; i < 4; i++) {
                    n = get2hex(&s19);
                    if (n == -1) {
                        printf("\nbad data\n");
                        break;
                    }
                    sum += n;
                    data = data*256+n;
                }
                if (i != 4) {
                    break;
                }

                // accumulate the words
                buffer[b++] = data;
                if (b == LENGTHOF(buffer)) {
                    if ((int)addr < FLASH_BYTES/2) {
                        // program the words
                        flash_write_words((uint32 *)(FLASH_BYTES/2+addr), buffer, b);
                    }
                    b = 0;
                    addr += b*4;
                }

                assert(count > 4);
                count -= 4;
            }
            if (count > 1) {
                break;
            }
            
            // process leftover data
            if (b) {
                if ((int)addr < FLASH_BYTES/2) {
                    // program the words
                    flash_write_words((uint32 *)(FLASH_BYTES/2+addr), buffer, b);
                }
                b = 0;
                addr += b*4;
            }

            // verify 1 byte of checksum
            assert(count == 1);
            n = get2hex(&s19);
            if (n == -1) {
                printf("\nbad checksum\n");
                break;
            }
            sum += n;
            if ((sum & 0xff) != 0xff) {
                printf("\nbad checksum 0x%x\n", sum & 0xff);
                break;
            }

            if (++y%4 == 0) {
                printf(".");
#if MCF52233
            } else if (y%2) {
                // fec gets confused with interrupts off and no
                // characters in flight!
                printf("%c", '\0');
#endif
            }
        } else if (c == '7') {
            done = true;
        } else {
            // unrecognized record
            break;
        }
    } while (! done);

    if (! s0_found || ! done) {
        if (main_command) {
            main_command = NULL;
            terminal_command_ack(false);
        }

        // we're in trouble!
        if (! s0_found) {
            printf("s0 record not found\n");
        }
        if (! done) {
            printf("s7 record not found\n");
        }
        printf("upgrade failed\n");
        terminal_echo = true;

#if STICKOS
        // erase the staging area
        code_new();
#endif
    } else {
        printf("\npaste done!\n");
        
        compatible = ! memcmp((void *)0, (void *)(FLASH_BYTES/2), FLASH_PAGE_SIZE);
        
        printf("programming flash for %s upgrade...\n", compatible?"compatible":"incompatible");
#if STICKOS
        printf("wait for CPUStick LED e1 to blink!\n");
#else
        printf("wait for LED to blink!\n");
#endif
        delay(100);

        // if this is a compatible upgrade...
        if (compatible) {
            // reset the MCU; init() will take care of the rest
            MCF_RCM_RCR = MCF_RCM_RCR_SOFTRST;
            asm { halt }
        } else {
            // N.B. this is an incompatible upgrade; we have to get at least
            // page0 copied before we crash.
            
            // disable interrupts
            x = splx(7);
            
            // copy our new flash upgrade routine to RAM
            assert((int)VECTOR_NEW_FLASH_UPGRADE_RAM_END - (int)VECTOR_NEW_FLASH_UPGRADE_RAM_BEGIN <= sizeof(big_buffer));
            memcpy(big_buffer, (void *)(FLASH_BYTES/2+VECTOR_NEW_FLASH_UPGRADE_RAM_BEGIN), (int)VECTOR_NEW_FLASH_UPGRADE_RAM_END - (int)VECTOR_NEW_FLASH_UPGRADE_RAM_BEGIN);

            // and run it!
            fn = (void *)big_buffer;
            fn(false);

            // we should not come back!
            ASSERT(0);  // stop!
        }
    }
}

// this function initializes the flash module.
void
flash_initialize(void)
{
    assert((int)flash_upgrade_ram_end - (int)flash_upgrade_ram_begin <= sizeof(big_buffer));

    if (fsys_frequency > 25600000) {
        MCF_CFM_CFMCLKD = MCF_CFM_CFMCLKD_PRDIV8|MCF_CFM_CFMCLKD_DIV((fsys_frequency-1)/2/8/200000);
    } else {
        MCF_CFM_CFMCLKD = MCF_CFM_CFMCLKD_DIV((fsys_frequency-1)/2/200000);
    }

    MCF_CFM_CFMPROT = 0;
    MCF_CFM_CFMSACC = 0;
    MCF_CFM_CFMDACC = 0;
    MCF_CFM_CFMMCR = 0;
}

