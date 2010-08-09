// *** startup.c ******************************************************
// this file is where hardware starts execution, at _startup in page0;
// then we call p0_c_startup(), also in page0; finally we call init(),
// outside of page0; init() is responsible for calling main().

// IF YOU EDIT THIS FILE, YOU WILL CHANGE PAGE0 CODE AND FORCE AN
// INCOMPATIBLE UPGRADE!!!  WHENEVER POSSIBLE, MAKE YOUR CHANGES IN
// INIT.C OR SOMEWHERE ELSE.

// N.B. if page0 code changes during an upgrade, the upgrade is said
// to be "incompatible" and we overwrite page0, et. al., while running
// from RAM prior to the upgrade reset; a failure to overwrite results
// in a brick.  this corresponds to essentially "upgrading the
// bootloader", and is more risky but should occur infrequently.
//
// if page0 code has not changed, on the other hand, the upgrade is
// said to be "compatible" (i.e., the "bootloader" has not changed),
// and we never overwrite page0, and page0 will retry the overwrite
// of the remainder of flash from the staging area as many times as
// needed; hence, we are immune to overwrite failures.
//
// N.B. changing flash SECURITY state is always an incompatible upgrade.

#define EXTRACT  0  // 1 uses extracted headers rather than Freescale

#if EXTRACT
#include "extract.h"
#else
#if MCF52233
#include "MCF52235.h"
#elif MCF52221
#include "MCF52221.h"
#else
#error
#endif
#endif

typedef unsigned char bool;
typedef unsigned char byte;

enum {
    false,
    true
};

#if DEBUG
#define assert(x)  if (! (x)) { asm { halt } }
#else
#define assert(x)
#endif
#define ASSERT(x)  if (! (x)) { asm { halt } }

// N.B. we tightly control the headers used by this file so we can't
// inadvertently call out of page0 until we have completed compatible
// upgrade
#include "startup.h"
#include "vectors.h"

extern unsigned char far _SP_INIT[], _SDA_BASE[];
extern unsigned char far __SP_AFTER_RESET[];
extern unsigned char far ___RAMBAR[], ___RAMBAR_SIZE[];
extern unsigned char far __DATA_RAM[], __DATA_ROM[], __DATA_END[];

byte *end_of_static;
uint32 fsys_frequency;
uint32 oscillator_frequency;
bool debugger_attached;
#if PICTOCRYPT
byte big_buffer[8192];
#else
byte big_buffer[2048];
#endif

// *** page0 ***

#pragma explicit_zero_data on
#pragma define_section page0 ".page0" far_absolute R

__declspec(page0)
asm void
_startup(void);

static
__declspec(page0)
void
p0_c_startup(void);

#if ! FLASHER
#define X  0  // we're running in flash
#else
#define X  0x20000000 // we're running in RAM
#endif

// this is the hardware interrupt vector table, that re-dispatches to
// the software interrupt vector table in page1 from vectors.c.
__declspec(page0)
uint32 _vect[256] = {
    (uint32)__SP_AFTER_RESET, (uint32)_startup,
                        X+0x0810, X+0x0818, X+0x0820, X+0x0828, X+0x0830, X+0x0838,
    X+0x0840, X+0x0848, X+0x0850, X+0x0858, X+0x0860, X+0x0868, X+0x0870, X+0x0878,
    X+0x0880, X+0x0888, X+0x0890, X+0x0898, X+0x08a0, X+0x08a8, X+0x08b0, X+0x08b8,
    X+0x08c0, X+0x08c8, X+0x08d0, X+0x08d8, X+0x08e0, X+0x08e8, X+0x08f0, X+0x08f8,
    X+0x0900, X+0x0908, X+0x0910, X+0x0918, X+0x0920, X+0x0928, X+0x0930, X+0x0938,
    X+0x0940, X+0x0948, X+0x0950, X+0x0958, X+0x0960, X+0x0968, X+0x0970, X+0x0978,
    X+0x0980, X+0x0988, X+0x0990, X+0x0998, X+0x09a0, X+0x09a8, X+0x09b0, X+0x09b8,
    X+0x09c0, X+0x09c8, X+0x09d0, X+0x09d8, X+0x09e0, X+0x09e8, X+0x09f0, X+0x09f8,
    X+0x0a00, X+0x0a08, X+0x0a10, X+0x0a18, X+0x0a20, X+0x0a28, X+0x0a30, X+0x0a38,
    X+0x0a40, X+0x0a48, X+0x0a50, X+0x0a58, X+0x0a60, X+0x0a68, X+0x0a70, X+0x0a78,
    X+0x0a80, X+0x0a88, X+0x0a90, X+0x0a98, X+0x0aa0, X+0x0aa8, X+0x0ab0, X+0x0ab8,
    X+0x0ac0, X+0x0ac8, X+0x0ad0, X+0x0ad8, X+0x0ae0, X+0x0ae8, X+0x0af0, X+0x0af8,
    X+0x0b00, X+0x0b08, X+0x0b10, X+0x0b18, X+0x0b20, X+0x0b28, X+0x0b30, X+0x0b38,
    X+0x0b40, X+0x0b48, X+0x0b50, X+0x0b58, X+0x0b60, X+0x0b68, X+0x0b70, X+0x0b78,
    X+0x0b80, X+0x0b88, X+0x0b90, X+0x0b98, X+0x0ba0, X+0x0ba8, X+0x0bb0, X+0x0bb8,
    X+0x0bc0, X+0x0bc8, X+0x0bd0, X+0x0bd8, X+0x0be0, X+0x0be8, X+0x0bf0, X+0x0bf8,
    X+0x0c00, X+0x0c08, X+0x0c10, X+0x0c18, X+0x0c20, X+0x0c28, X+0x0c30, X+0x0c38,
    X+0x0c40, X+0x0c48, X+0x0c50, X+0x0c58, X+0x0c60, X+0x0c68, X+0x0c70, X+0x0c78,
    X+0x0c80, X+0x0c88, X+0x0c90, X+0x0c98, X+0x0ca0, X+0x0ca8, X+0x0cb0, X+0x0cb8,
    X+0x0cc0, X+0x0cc8, X+0x0cd0, X+0x0cd8, X+0x0ce0, X+0x0ce8, X+0x0cf0, X+0x0cf8,
    X+0x0d00, X+0x0d08, X+0x0d10, X+0x0d18, X+0x0d20, X+0x0d28, X+0x0d30, X+0x0d38,
    X+0x0d40, X+0x0d48, X+0x0d50, X+0x0d58, X+0x0d60, X+0x0d68, X+0x0d70, X+0x0d78,
    X+0x0d80, X+0x0d88, X+0x0d90, X+0x0d98, X+0x0da0, X+0x0da8, X+0x0db0, X+0x0db8,
    X+0x0dc0, X+0x0dc8, X+0x0dd0, X+0x0dd8, X+0x0de0, X+0x0de8, X+0x0df0, X+0x0df8,
    X+0x0e00, X+0x0e08, X+0x0e10, X+0x0e18, X+0x0e20, X+0x0e28, X+0x0e30, X+0x0e38,
    X+0x0e40, X+0x0e48, X+0x0e50, X+0x0e58, X+0x0e60, X+0x0e68, X+0x0e70, X+0x0e78,
    X+0x0e80, X+0x0e88, X+0x0e90, X+0x0e98, X+0x0ea0, X+0x0ea8, X+0x0eb0, X+0x0eb8,
    X+0x0ec0, X+0x0ec8, X+0x0ed0, X+0x0ed8, X+0x0ee0, X+0x0ee8, X+0x0ef0, X+0x0ef8,
    X+0x0f00, X+0x0f08, X+0x0f10, X+0x0f18, X+0x0f20, X+0x0f28, X+0x0f30, X+0x0f38,
    X+0x0f40, X+0x0f48, X+0x0f50, X+0x0f58, X+0x0f60, X+0x0f68, X+0x0f70, X+0x0f78,
    X+0x0f80, X+0x0f88, X+0x0f90, X+0x0f98, X+0x0fa0, X+0x0fa8, X+0x0fb0, X+0x0fb8,
    X+0x0fc0, X+0x0fc8, X+0x0fd0, X+0x0fd8, X+0x0fe0, X+0x0fe8, X+0x0ff0, X+0x0ff8
};
    
// this is the cfm config
__declspec(page0)
unsigned long _cfm[6] = {
    0,                              // (0x00000400) KEY_UPPER
    0,                              // (0x00000404) KEY_LOWER
    0xffffffff,                     // (0x00000408) CFMPROT
    0,                              // (0x0000040C) CFMSACC
    0,                              // (0x00000410) CFMDACC
#if SECURE
    MCF_CFM_CFMSEC_SECSTAT|0x4AC8,  // (0x00000414) CFMSEC
#else
    0,                              // (0x00000414) CFMSEC
#endif
};

// this function performs assembly language initialization from
// reset, and then calls C initialization, which calls main().
__declspec(page0)
asm void
_startup(void)
{
    // disable interrupts
    move.w   #0x2700,sr
    
    // initialize RAMBAR
    move.l   #__RAMBAR+0x21,d0
    movec    d0,RAMBAR

    // initialize IPSBAR
    move.l   #__IPSBAR,d0
    andi.l   #0xC0000000,d0  // need to mask
    add.l    #0x1,d0
    move.l   d0,0x40000000

    // initialize FLASHBAR
    move.l   #__FLASHBAR,d0
    andi.l   #0xFFF80000,d0  // need to mask
    add.l    #0x61,d0
    movec    d0,FLASHBAR

    // set up the fixed stack pointer
    lea      __SP_AFTER_RESET,a7

    // set up short data base A5
    lea      _SDA_BASE,a5

    // set up A6 dummy stackframe
    movea.l  #0,a6
    link     a6,#0

    // early C initialization, and compatible upgrade (page0)
    jsr      p0_c_startup

    // set up the real stack pointer
    lea      _SP_INIT,a7

    // late C initialization, post-upgrade (init(), page1 indirect)
    lea      VECTOR_OLD_INIT,a0
    move.l   (a0),a0
    jsr      (a0)
    
    nop
    halt
}

static
__declspec(page0)
void *
p0_memcpy(void *d,  const void *s, uint32 n)
{
    void *dd;
    
    dd = d;
    while (n--) {
        *(((char *)d)++) = *(((char *)s)++);
    }
    return dd;
}

static
__declspec(page0)
void *
p0_memset(void *p,  int d, uint32 n)
{
    void *pp;
    
    pp = p;
    while (n--) {
        *(((char *)p)++) = d;
    }
    return pp;
}

static
__declspec(page0)
int
p0_memcmp(const void *d,  const void *s, uint32 n)
{
    char c;
    
    while (n--) {
        c = *(((char *)d)++) - *(((char *)s)++);
        if (c) {
            return c;
        }
    }
    return 0;
}

// this function performs C initialization before main() runs.
static
__declspec(page0)
void
p0_c_startup(void)
{
#if ! FLASHER
    flash_upgrade_ram_begin_f fn;
#endif
    
    // N.B. we are not allowed to call out of this file or use
    // initialized data until we have completed compatible upgrade!!!
    
#if INCOMPAT
    p0_memset((void *)__DATA_RAM, 0, (uint32)__SP_AFTER_RESET - (uint32)__DATA_RAM - 64);
#endif

    // zero all of RAM now, so we can set globals in this file
    p0_memset((void *)__DATA_RAM, 0, (uint32)__SP_AFTER_RESET - (uint32)__DATA_RAM - 64);

    // enable debug
    MCF_GPIO_PDDPAR = 0x0F;

    // disable Software Watchdog Timer
    MCF_SCM_CWCR = 0;

#if MCF52221
    // if we don't have a crystal...
    MCF_GPIO_PNQPAR = 0;
    if (MCF_CLOCK_SYNSR & MCF_CLOCK_SYNSR_OCOSC) {
        // we use the 8MHz internal oscillator divided by 1
        MCF_CLOCK_CCHR = 0;

        // and multiply by 6 to get 48MHz
        MCF_CLOCK_SYNCR = MCF_CLOCK_SYNCR_MFD(1)|MCF_CLOCK_SYNCR_CLKSRC|MCF_CLOCK_SYNCR_PLLMODE|MCF_CLOCK_SYNCR_PLLEN;
        
        // USB uses fsys
        fsys_frequency = 48000000;
        oscillator_frequency = 8000000;
    } else {
        // we use the 48MHz crystal divided by 6
        MCF_CLOCK_CCHR = 5;

        // and multiply by 8 to get 64MHz
        MCF_CLOCK_SYNCR = MCF_CLOCK_SYNCR_MFD(2)|MCF_CLOCK_SYNCR_CLKSRC|MCF_CLOCK_SYNCR_PLLMODE|MCF_CLOCK_SYNCR_PLLEN;
        
        // USB uses oscillator
        fsys_frequency = 64000000;
        oscillator_frequency = 48000000;
    }
#elif MCF52233
    // we use the 25MHz crystal divided by 5
    MCF_CLOCK_CCHR = 4;

    // and multiply by 12 to get 64MHz
    MCF_CLOCK_SYNCR = MCF_CLOCK_SYNCR_MFD(4)|MCF_CLOCK_SYNCR_CLKSRC|MCF_CLOCK_SYNCR_PLLMODE|MCF_CLOCK_SYNCR_PLLEN;
    
    // no USB
    fsys_frequency = 60000000;
    oscillator_frequency = 25000000;
#else
#error
#endif

    // wait for pll lock
    while (!(MCF_CLOCK_SYNSR & MCF_CLOCK_SYNSR_LOCK)) {
        // NULL
    }
    
    // set real time clock freq
    MCF_CLOCK_RTCDR = fsys_frequency;

    // enable on-chip modules to access internal SRAM
    MCF_SCM_RAMBAR = (0
        | MCF_SCM_RAMBAR_BA(RAMBAR_ADDRESS)
        | MCF_SCM_RAMBAR_BDE);
        
    // we read MCF_GPIO_PORTAN's initial value to determine if the debugger is
    // attached
    // N.B. this value musy be set by the debugger's cfg file!
    if (! MCF_GPIO_PORTAN) {
        debugger_attached = true;
    } else {
        // turn off pstclk to reduce emi
        MCF_CLOCK_SYNCR |= MCF_CLOCK_SYNCR_DISCLK;
    }

#if ! FLASHER
    // if we're in the middle of a compatible upgrade...
    if (! p0_memcmp((void *)0, (void *)(FLASH_BYTES/2), FLASH_PAGE_SIZE)) {
        // initialize the flash module
        if (fsys_frequency > 25600000) {
            MCF_CFM_CFMCLKD = MCF_CFM_CFMCLKD_PRDIV8|MCF_CFM_CFMCLKD_DIV((fsys_frequency-1)/2/8/200000);
        } else {
            MCF_CFM_CFMCLKD = MCF_CFM_CFMCLKD_DIV((fsys_frequency-1)/2/200000);
        }

        MCF_CFM_CFMPROT = 0;
        MCF_CFM_CFMSACC = 0;
        MCF_CFM_CFMDACC = 0;
        MCF_CFM_CFMMCR = 0;
        
        // copy our new flash upgrade routine to RAM
        assert((int)VECTOR_NEW_FLASH_UPGRADE_RAM_END - (int)VECTOR_NEW_FLASH_UPGRADE_RAM_BEGIN <= sizeof(big_buffer));
        p0_memcpy(big_buffer, (void *)(FLASH_BYTES/2+VECTOR_NEW_FLASH_UPGRADE_RAM_BEGIN), (int)VECTOR_NEW_FLASH_UPGRADE_RAM_END - (int)VECTOR_NEW_FLASH_UPGRADE_RAM_BEGIN);

        // and run it!
        fn = (void *)big_buffer;
        fn(true);

        // we should not come back!
        ASSERT(0);  // stop!
    }
#endif
}

