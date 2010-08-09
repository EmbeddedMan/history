#include "main.h"

// *** flash ****************************************************************

#if FLASH

#define FSYS  48000000  // frequency of M52221DEMO board

static
void
flash_command(uint8 cmd, uint32 *addr, uint32 data)
{
    uint32 *backdoor_addr;

    // N.B. this code generates no relocations so we can run it from RAM!!!

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

    // wait for command buffer empty
    while (! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CBEIF)) {
        // assert no errors
        assert(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));
    }
}

void
flash_erase_pages(uint32 *addr, uint32 npages)
{
    // N.B. this code generates no relocations so we can run it from RAM!!!
    
    while (npages) {
        flash_command(MCF_CFM_CFMCMD_PAGE_ERASE, addr, 0);
        npages--;
        addr += FLASH_PAGE_SIZE/sizeof(uint32);
    }

    // wait for flash command complete
    while(! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CCIF)) {
        // NULL
    };
}

void
flash_write_words(uint32 *addr_in, uint32 *data_in, uint32 nwords_in)
{
    int i;
    uint32 *addr;
    uint32 *data;
    uint32 nwords;
    
    // N.B. this code generates no relocations so we can run it from RAM!!!

    addr = addr_in;
    data = data_in;
    nwords = nwords_in;
    
    while (nwords) {
        flash_command(MCF_CFM_CFMCMD_WORD_PROGRAM, addr, *data);
        nwords--;
        addr++;
        data++;
    }

    // wait for flash command complete
    while(! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CCIF)) {
        // NULL
    };
    
    for (i = 0; i < nwords_in; i++) {
        assert(addr_in[i] == data_in[i]);
    }
}

void
flash_upgrade_prepare(void)
{
    // erase the staging area
    flash_erase_pages((uint32 *)(FLASH_BYTES/2), FLASH_BYTES/2/FLASH_PAGE_SIZE);    
}

void
flash_upgrade(void)
{
    uint32 *addr;
    uint32 *data;
    uint32 nwords;
    uint32 npages;
    uint32 *backdoor_addr;

    // N.B. this code generates no relocations so we can run it from RAM!!!

    // erase the StickOS code
    // flash_erase_pages(NULL, FLASH_BYTES/2/FLASH_PAGE_SIZE)
    addr = NULL;
    npages = FLASH_BYTES/2/FLASH_PAGE_SIZE;
    while (npages) {
        // flash_command()
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
    while(! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CCIF)) {
    };
    
    // and re-flash it from the staging area
    // flash_write_words()
    addr = NULL;
    data = (uint32 *)(FLASH_BYTES/2);
    nwords = FLASH_BYTES/2/sizeof(uint32);
    while (nwords) {
        // flash_command()
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
    while(! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CCIF)) {
    };
    
    // erase the staging area
    // flash_erase_pages(FLASH_BYTES/2, FLASH_BYTES/2/FLASH_PAGE_SIZE)
    addr = (uint32 *)(FLASH_BYTES/2);
    npages = FLASH_BYTES/2/FLASH_PAGE_SIZE;
    while (npages) {
        // flash_command()
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
    while(! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CCIF)) {
    };
    
    // reset the CPUStick
    MCF_RCM_RCR = MCF_RCM_RCR_SOFTRST;
	asm { halt }
}

void
flash_upgrade_end(void)
{
    // NULL
}

void
flash_initialize(void)
{
    if (FSYS > 25600000) {
        MCF_CFM_CFMCLKD = MCF_CFM_CFMCLKD_PRDIV8|MCF_CFM_CFMCLKD_DIV((FSYS-1)/2/8/200000);
    } else {
        MCF_CFM_CFMCLKD = MCF_CFM_CFMCLKD_DIV((FSYS-1)/2/200000);
    }

    MCF_CFM_CFMPROT = 0;
    MCF_CFM_CFMSACC = 0;
    MCF_CFM_CFMDACC = 0;
    MCF_CFM_CFMMCR = 0;
}

#endif
