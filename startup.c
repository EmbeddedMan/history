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

#include "config.h"

#ifndef MAIN_INCLUDED

#if EXTRACT && ! MCF51JM128 && ! MCF51CN128 && ! MCF51QE128
#include "extract.h"
#else
#if MCF52233
#include "MCF52235.h"
#elif MCF52221
#include "MCF52221.h"
#elif MCF52259
#include "MCF52259.h"
#elif MCF5211
#include "MCF5211.h"
#elif MCF51JM128
#include "MCF51JM128.h"
#include "compat.h"
#elif MCF51CN128
#include "MCF51CN128.h"
#include "compat.h"
#elif MCF51QE128
#include "MCF51QE128.h"
#include "compat.h"
#elif MC9S08QE128
#include "MC9S08QE128.h"
#include "compat.h"
#elif MC9S12DT256
#include "MC9S12DT256.h"
#include "compat.h"
#elif MC9S12DP512
#include "MC9S12DP512.h"
#include "compat.h"
#elif PIC32
#include <plib.h>
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
#else
#error
#endif
#endif

#if GCC
#define far
#define __IPSBAR ((volatile uint8 *)0x40000000)
extern uint8 __RAMBAR[];
#define RAMBAR_ADDRESS ((uint32)__RAMBAR)
#define asm_halt() __asm__("halt")
#else
#define asm_halt() asm { halt }
#endif

typedef unsigned char bool;
#if ! MCF51JM128 && ! MCF51CN128 && ! MCF51QE128
typedef unsigned char byte;
#endif

#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
typedef uint16 size_t;
#else
#if ! PIC32
typedef uint32 size_t;
#endif
#endif

enum {
    false,
    true
};

#if SODEBUG
#define assert(x)  if (! (x)) { asm_halt(); }
#else
#define assert(x)
#endif
#define ASSERT(x)  if (! (x)) { asm_halt(); }

#endif  // MAIN_INCLUDED

// N.B. we tightly control the headers used by this file so we can't
// inadvertently call out of page0 until we have completed compatible
// upgrade
#include "startup.h"
#include "vectors.h"

// routines defined in util.c
extern int memcmp(const void *d, const void *s, size_t n);
extern void *memcpy(void *d, const void *s, size_t n);
extern void *memset(void *p, int d, size_t n);

#if ! PIC32 && ! MC9S08QE128 && ! MC9S12DT256 && ! MC9S12DP512
extern unsigned char far _SP_INIT[], _SDA_BASE[];
extern unsigned char far __SP_AFTER_RESET[];
extern unsigned char far ___RAMBAR[], ___RAMBAR_SIZE[];
extern unsigned char far __DATA_RAM[], __DATA_ROM[], __DATA_END[];
#endif

byte *end_of_static;

uint32 cpu_frequency;
uint32 oscillator_frequency;
uint32 bus_frequency;

bool debugger_attached;
// N.B. big_buffer may not be aligned; it is up to the caller to align it if needed.
#if PICTOCRYPT
byte big_buffer[8192];
#else
byte big_buffer[512];
#endif

#if ! PIC32 && ! MC9S08QE128 && ! MC9S12DT256 && ! MC9S12DP512

// *** page0 ***

#if GCC
extern void _startup(void);
#else // GCC
DECLSPEC_PAGE0_CODE
asm void _startup(void);
#endif // GCC

#if ! BADGE_BOARD && ! DEMO_KIT

#if ! FLASHER
#define X  0  // we're running in flash
#else
#define X  0x20000000 // we're running in RAM
#endif

// this is the hardware interrupt vector table, that re-dispatches to
// the software interrupt vector table in page1 from vectors.c.
DECLSPEC_PAGE0_DATA
const uint32 _vect[] = {
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
    //X+0x0f80, X+0x0f88, X+0x0f90, X+0x0f98, X+0x0fa0, X+0x0fa8, X+0x0fb0, X+0x0fb8,
    //X+0x0fc0, X+0x0fc8, X+0x0fd0, X+0x0fd8, X+0x0fe0, X+0x0fe8, X+0x0ff0, X+0x0ff8
};
    
// this is the cfm config
DECLSPEC_PAGE0_DATA
const uint32 _cfm[] = {
#if ! MCF51JM128 && ! MCF51CN128 && ! MCF51QE128
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
#else
    0,                              // 400 key
    0,                              // 404 key
    0,                              // 408 reserved
    0x00ff0000                      // 40c prot/opt
#endif
};
#else  // ! BADGE_BOARD && ! DEMO_KIT
extern
void
init(void);
#endif  // ! BADGE_BOARD && ! DEMO_KIT

DECLSPEC_PAGE0_CODE
void
p0_c_startup(void);

// this function performs assembly language initialization from
// reset, and then calls C initialization, which calls main().
DECLSPEC_PAGE0_CODE
BEGIN_NAKED(_startup)
{
    // disable interrupts
    Q3(move.w,  #0x2700,sr)
    
#if ! MCF51JM128 && ! MCF51CN128 && ! MCF51QE128
    // initialize RAMBAR
    Q3(move.l,  #__RAMBAR+0x21,d0)
    Q3(movec,   d0,RAMBAR)

    // initialize IPSBAR
    Q3(move.l,  #__IPSBAR,d0)
    Q3(andi.l,  #0xC0000000,d0)  // need to mask
    Q3(add.l,   #0x1,d0)
    Q3(move.l,  d0,0x40000000)

    // initialize FLASHBAR
    Q3(move.l,  #__FLASHBAR,d0)
    Q3(andi.l,  #0xFFF80000,d0)  // need to mask
#if MCF52259
    Q3(add.l,   #0x21,d0)
#else
    Q3(add.l,   #0x61,d0)
#endif
    Q3(movec,   d0,FLASHBAR)
#else
    Q3(move.l,  #0xc0000000,d0)
    Q3(movec,   d0,CPUCR)
#endif

    // set up the fixed stack pointer
    Q3(lea,     __SP_AFTER_RESET,a7)

    // set up short data base A5
    Q3(lea,     _SDA_BASE,a5)

    // set up A6 dummy stackframe
    Q3(movea.l, #0,a6)
    Q3(link,    a6,#0)

    // early C initialization, and compatible upgrade (page0)
    Q2(jsr,     p0_c_startup)

    // set up the real stack pointer
    Q3(lea,     _SP_INIT,a7)

#if ! BADGE_BOARD && ! DEMO_KIT
    // late C initialization, post-upgrade (init(), page1 indirect)
    Q3(lea,     VECTOR_OLD_INIT,a0)
    Q3(move.l,  (a0),a0)
    Q2(jsr,     (a0))
#else
    Q2(jsr,      init)
#endif
    
    Q1(nop)
    Q1(halt)
}
END_NAKED

// this function performs C initialization before main() runs.
DECLSPEC_PAGE0_CODE
void
p0_c_startup(void)
{
#if ! FLASHER
    flash_upgrade_ram_begin_f fn;
#endif
    
    // N.B. we are not allowed to call out of this file or use
    // initialized data until we have completed compatible upgrade!!!
    
#if INCOMPAT
    memset((void *)__DATA_RAM, 0, (uint32)__SP_AFTER_RESET - (uint32)__DATA_RAM - 64);
#endif

    // zero all of RAM now, so we can set globals in this file
    memset((void *)__DATA_RAM, 0, (uint32)__SP_AFTER_RESET - (uint32)__DATA_RAM - 64);

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
#if ! MCF5211
    // enable debug
    MCF_GPIO_PDDPAR = 0x0F;
#endif

    // disable Software Watchdog Timer
    MCF_SCM_CWCR = 0;
#elif MCF51JM128
    // disable Software Watchdog Timer
    SOPT1 = SOPT1_STOPE_MASK;
    SOPT2 &= ~SOPT2_USB_BIGEND_MASK;
#elif MCF51CN128
    // disable Software Watchdog Timer
    SOPT1 = SOPT1_STOPE_MASK;
#elif MCF51QE128
    // disable Software Watchdog Timer
    SOPT1 = SOPT1_STOPE_MASK|SOPT1_BKGDPE_MASK;
#else
#error
#endif

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
#if MCF5211
        // and multiply by 8 to get 64MHz
        MCF_CLOCK_SYNCR = MCF_CLOCK_SYNCR_MFD(2)|MCF_CLOCK_SYNCR_CLKSRC|MCF_CLOCK_SYNCR_PLLMODE|MCF_CLOCK_SYNCR_PLLEN;
        
        // no USB
        cpu_frequency = 64000000;
        bus_frequency = cpu_frequency/2;
        oscillator_frequency = 8000000;
#elif MCF52221
    // if we don't have a crystal...
    if (MCF_CLOCK_SYNSR & MCF_CLOCK_SYNSR_OCOSC) {
        // we use the 8MHz internal oscillator divided by 1
        MCF_CLOCK_CCHR = 0;

        // and multiply by 6 to get 48MHz
        MCF_CLOCK_SYNCR = MCF_CLOCK_SYNCR_MFD(1)|MCF_CLOCK_SYNCR_CLKSRC|MCF_CLOCK_SYNCR_PLLMODE|MCF_CLOCK_SYNCR_PLLEN;
        
        // USB uses fsys
        cpu_frequency = 48000000;
        bus_frequency = cpu_frequency/2;
        oscillator_frequency = 8000000;
    } else {
        // we use the 48MHz crystal divided by 6
        MCF_CLOCK_CCHR = 5;

        // and multiply by 8 to get 64MHz
        MCF_CLOCK_SYNCR = MCF_CLOCK_SYNCR_MFD(2)|MCF_CLOCK_SYNCR_CLKSRC|MCF_CLOCK_SYNCR_PLLMODE|MCF_CLOCK_SYNCR_PLLEN;
        
        // USB uses oscillator
        cpu_frequency = 64000000;
        bus_frequency = cpu_frequency/2;
        oscillator_frequency = 48000000;
    }
#elif MCF52233
    // we use the 25MHz crystal divided by 5
    MCF_CLOCK_CCHR = 4;

    // and multiply by 12 to get 60MHz
    MCF_CLOCK_SYNCR = MCF_CLOCK_SYNCR_MFD(4)|MCF_CLOCK_SYNCR_CLKSRC|MCF_CLOCK_SYNCR_PLLMODE|MCF_CLOCK_SYNCR_PLLEN;
    
    // no USB
    cpu_frequency = 60000000;
    bus_frequency = cpu_frequency/2;
    oscillator_frequency = 25000000;
#elif MCF52259
    MCF_CLOCK_OCLR = 0xC0;  //turn on crystal
    MCF_CLOCK_CCLR = 0x00;  //switch to crystal 
    MCF_CLOCK_OCHR = 0x00;  //turn off relaxation osc

    // we use the 48MHz crystal divided by 6
    MCF_CLOCK_CCHR = 5;
    
    // and multiply by 10 to get 80MHz
    MCF_CLOCK_SYNCR = MCF_CLOCK_SYNCR_MFD(3)|MCF_CLOCK_SYNCR_CLKSRC|MCF_CLOCK_SYNCR_PLLMODE|MCF_CLOCK_SYNCR_PLLEN;

    // USB uses oscillator
    cpu_frequency = 80000000;
    bus_frequency = cpu_frequency/2;
    oscillator_frequency = 48000000;
    
    // shut down non-essential clocks
    MCF_SCM_PPMRL = MCF_SCM_PPMRL_CDFEC|MCF_SCM_PPMRL_CDMINIBUS;
    MCF_SCM_PPMRH = MCF_SCM_PPMRH_CDPIT1;
#else
#error
#endif

    // wait for pll lock
    while (!(MCF_CLOCK_SYNSR & MCF_CLOCK_SYNSR_LOCK)) {
        // NULL
    }
    
    // set real time clock freq
#if MCF52259
    MCF_RTC_RTCGOCU = 0x8000>>16;
    MCF_RTC_RTCGOCL = 0x8000;
#elif ! MCF5211
    MCF_CLOCK_RTCDR = cpu_frequency;
#endif

    // enable on-chip modules to access internal SRAM
    MCF_SCM_RAMBAR = (0|MCF_SCM_RAMBAR_BA(RAMBAR_ADDRESS)|MCF_SCM_RAMBAR_BDE);
        
    // we read MCF_GPIO_PORTAN's initial value to determine if the debugger is
    // attached
    // N.B. this value must be set by the debugger's cfg file!
    if (! MCF_GPIO_PORTAN) {
        debugger_attached = true;
    } else {
        // turn off pstclk to reduce emi
        MCF_CLOCK_SYNCR |= MCF_CLOCK_SYNCR_DISCLK;
    }
#elif MCF51JM128
    /* switch from FEI to FBE (FLL bypassed external) */ 
    /* enable external clock source */
    MCGC2 = MCGC2_HGO_MASK       /* oscillator in high gain mode */
          | MCGC2_EREFS_MASK   /* because crystal is being used */
          | MCGC2_RANGE_MASK   /* 12 MHz is in high freq range */
          | MCGC2_ERCLKEN_MASK;     /* activate external reference clock */
    while (MCGSC_OSCINIT == 0)
    ;
    /* select clock mode */
    MCGC1 = (2<<6)         /* CLKS = 10 -> external reference clock. */
        | (3<<3);          /* RDIV = 3 -> 12MHz/8=1.5 MHz */

    /* wait for mode change to be done */
    while (MCGSC_IREFST != 0)
    ;
    while (MCGSC_CLKST != 2)
    ;

    /* switch from FBE to PBE (PLL bypassed internal) mode */
    MCGC3=MCGC3_PLLS_MASK
        | (8<<0);     /* VDIV=6 -> multiply by 32 -> 1.5MHz * 32 = 48MHz */
    while(MCGSC_PLLST != 1)
    ;
    while(MCGSC_LOCK != 1)
    ;
    /* finally switch from PBE to PEE (PLL enabled external mode) */
    MCGC1 = (0<<6)         /* CLKS = 0 -> PLL or FLL output clock. */
          | (3<<3);        /* RDIV = 3 -> 12MHz/8=1.5 MHz */
    while(MCGSC_CLKST!=3)
    ;

    /* Now MCGOUT=48MHz, BUS_CLOCK=24MHz */  
    cpu_frequency = 48000000;
    bus_frequency = cpu_frequency/2;
    oscillator_frequency = 12000000;

    // we read KBI1SC's initial value to determine if the debugger is attached
    // N.B. this value must be set by the debugger's cmd file!
    if (KBI1SC == 0x01) {
        debugger_attached = true;
    }
#elif MCF51CN128
#define EXTAL_PIN_INIT                  PTDPF1_D4 = 0b11;   // extal pin ON
#define XTAL_PIN_INIT                   PTDPF1_D5 = 0b11;   // xtal pin ON
    EXTAL_PIN_INIT;
    XTAL_PIN_INIT;

  /* switch from FEI to FBE (FLL bypassed external) */ 
  /* enable external clock source */

    MCGC2 = MCGC2_ERCLKEN_MASK  // activate external reference clock
          | MCGC2_EREFS_MASK    // because crystal is being used
          | MCGC2_RANGE_MASK;   // high range  
            
    /* select clock mode */
    MCGC1 = (0b10<<6)           // CLKS = 10 -> external reference clock
          | (0b100<<3)          // RDIV = 2^4 -> 25MHz/16 = 1.5625 MHz
          | MCGC1_IRCLKEN_MASK; // IRCLK to RTC enabled
          // also clear IREFs
      
    /* wait for mode change to be done */
    while (MCGSC_IREFST | (MCGSC_CLKST != 0b10)) // wait for Reference Status bit to update
    ;                                            // and for clock status bits to update 

    /* switch from FBE to PBE (PLL bypassed internal) mode */
    MCGC3 =  (0b1000<<0)        // set PLL multi 50MHz
          |  MCGC3_PLLS_MASK;   // select PLL

    while (!MCGSC_PLLST | !MCGSC_LOCK) /* wait for PLL status bit to update */
    ;   /* Wait for LOCK bit to set */

    /* Now running PBE Mode */
    /* finally switch from PBE to PEE (PLL enabled external mode) */
    MCGC1_CLKS  = 0b00; // PLL clock to system (MCGOUT)

    /* Wait for clock status bits to update */
    while (MCGSC_CLKST != 0b11)           
    ;

    cpu_frequency = 50000000;
    bus_frequency = cpu_frequency/2;
    oscillator_frequency = 32768;
    
    // we read KBI1SC's initial value to determine if the debugger is attached
    // N.B. this value must be set by the debugger's cmd file!
    if (KBI1SC == 0x01) {
        debugger_attached = true;
    }
#elif MCF51QE128
#define setReg8(RegName, val)  (RegName = (byte)(val))
#define clrSetReg8Bits(RegName, ClrMask, SetMask)  (RegName = (byte)((byte)((byte)RegName & (~(byte)(ClrMask))) | (byte)(SetMask)))

    /*  System clock initialization */
    /* ICSC1: CLKS=0,RDIV=0,IREFS=1,IRCLKEN=1,IREFSTEN=0 */
    setReg8(ICSC1, 0x06);                /* Initialization of the ICS control register 1 */ 
    /* ICSC2: BDIV=0,RANGE=0,HGO=0,LP=0,EREFS=0,ERCLKEN=0,EREFSTEN=0 */
    setReg8(ICSC2, 0x00);                /* Initialization of the ICS control register 2 */ 
    while(!ICSSC_IREFST) {               /* Wait until the source of reference clock is internal clock */
    }
    /* ICSSC: DRST_DRS=2,DMX32=0 */
    clrSetReg8Bits(ICSSC, 0x60, 0x80);   /* Initialization of the ICS status and control */ 
    while((ICSSC & 0xC0) != 0x80) {      /* Wait until the FLL switches to High range DCO mode */
    }
    
    cpu_frequency = 50330000;
    bus_frequency = cpu_frequency/2;
    oscillator_frequency = 32768;
    
    // we read KBI1SC's initial value to determine if the debugger is attached
    // N.B. this value must be set by the debugger's cmd file!
    if (KBI1SC == 0x01) {
        debugger_attached = true;
    }
#else
#error
#endif

#if ! FLASHER
    // ******** COMPATIBLE UPGRADE ********

    // if we're in the middle of a compatible upgrade...
    if (! memcmp((void *)0, (void *)(FLASH_BYTES/2), FLASH_PAGE_SIZE) && ! *((uint32 *)FLASH_BYTES - 1)) {
        // initialize the flash module
        if (bus_frequency > 12800000) {
            MCF_CFM_CFMCLKD = MCF_CFM_CFMCLKD_PRDIV8|MCF_CFM_CFMCLKD_DIV((bus_frequency-1)/8/200000);
        } else {
            MCF_CFM_CFMCLKD = MCF_CFM_CFMCLKD_DIV((bus_frequency-1)/200000);
        }

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
        MCF_CFM_CFMPROT = 0;
        MCF_CFM_CFMSACC = 0;
        MCF_CFM_CFMDACC = 0;
        MCF_CFM_CFMMCR = 0;
#endif
        
        // N.B. this was verified earlier
        //ASSERT((int)VECTOR_NEW_FLASH_UPGRADE_RAM_END - (int)VECTOR_NEW_FLASH_UPGRADE_RAM_BEGIN <= sizeof(big_buffer)-4);
        
        // copy our new flash upgrade routine to RAM
        fn = (flash_upgrade_ram_begin_f)(((uint32)big_buffer+3)&~3);
        memcpy(fn, (void *)(FLASH_BYTES/2+VECTOR_NEW_FLASH_UPGRADE_RAM_BEGIN), (int)VECTOR_NEW_FLASH_UPGRADE_RAM_END - (int)VECTOR_NEW_FLASH_UPGRADE_RAM_BEGIN);

        // and run it!
        fn(true);

        // we should not come back!
        for (;;) { asm_halt(); }
    }
#endif
}
#endif // ! PIC32 && ! MC9S08QE128 && ! MC9S12DT256 && ! MC9S12DP512

