#include "MCF52221.h"

extern unsigned char far _SP_INIT[], _SDA_BASE[];
extern unsigned char far __SP_AFTER_RESET[];
extern unsigned char far __BSS_START[], __BSS_END[];
extern unsigned char far __DATA_RAM[], __DATA_ROM[], __DATA_END[];
extern unsigned long far __VECTOR_RAM[];

extern int main();

static void (*_vectors[256])(void);

// *** set_vbr **************************************************************

static
asm void
set_vbr(unsigned long) {
    move.l  4(SP),D0
    movec   d0,VBR 
    nop
    rts    
}    

// *** init *****************************************************************

static
void
init(void)
{
    uint32 n;
    uint8 *dp, *sp;

    /* Enable debug */
    MCF_GPIO_PDDPAR = 0x0F;
    
    /* Set real time clock freq */
    MCF_CLOCK_RTCDR = 48000000;
    
    /* 
     * Copy the vector table to RAM 
     */
    if (__VECTOR_RAM != (unsigned long *)_vectors) {
        for (n = 0; n < 256; n++)
            __VECTOR_RAM[n] = (unsigned long)_vectors[n];
    }
    
    set_vbr((unsigned long)__VECTOR_RAM);

    /*
     * Move initialized data from ROM to RAM.
     */
    if (__DATA_ROM != __DATA_RAM)
    {
        dp = (uint8 *)__DATA_RAM;
        sp = (uint8 *)__DATA_ROM;
        n = __DATA_END - __DATA_RAM;
        while (n--)
            *dp++ = *sp++;
    }

    /*
     * Zero uninitialized data
     */
    if (__BSS_START != __BSS_END)
    {
        sp = (uint8 *)__BSS_START;
        n = __BSS_END - __BSS_START;
        while (n--)
            *sp++ = 0;
    }
    
    /*
     * Disable Software Watchdog Timer
     */
    MCF_SCM_CWCR = 0;
    
    MCF_CLOCK_CCHR = 0x05; // The PLL pre divider - 48MHz / 6 = 8MHz 

    /* The PLL pre-divider affects this!!! 
     * Multiply 8Mhz reference crystal /CCHR by 6 to acheive system clock of 48Mhz
     */

    MCF_CLOCK_SYNCR = MCF_CLOCK_SYNCR_MFD(1) | MCF_CLOCK_SYNCR_CLKSRC| MCF_CLOCK_SYNCR_PLLMODE | MCF_CLOCK_SYNCR_PLLEN ;

    while (!(MCF_CLOCK_SYNSR & MCF_CLOCK_SYNSR_LOCK))
    {
    }

    /*
     * Enable on-chip modules to access internal SRAM
     */
    MCF_SCM_RAMBAR = (0
        | MCF_SCM_RAMBAR_BA(RAMBAR_ADDRESS)
        | MCF_SCM_RAMBAR_BDE);
}

// *** startup **************************************************************

asm void
_startup(void)
{
    /* disable interrupts */
    move.w   #0x2700,sr

    /* Initialize RAMBAR: locate SRAM and validate it */
    move.l   #__RAMBAR+0x21,d0
    movec    d0,RAMBAR
    
    /* Initialize IPSBAR */
    move.l   #__IPSBAR,d0
    andi.l   #0xC0000000,d0  // need to mask
    add.l    #0x1,d0
    move.l   d0,0x40000000

    /* Initialize FLASHBAR */
    move.l   #__FLASHBAR,d0
    andi.l   #0xFFF80000,d0  // need to mask
    add.l    #0x61,d0
    movec    d0,FLASHBAR

    /* setup the stack pointer */
    lea      _SP_INIT,a7

    /* setup A6 dummy stackframe */
    movea.l  #0,a6
    link     a6,#0

	/* setup A5 */
    lea      _SDA_BASE,a5

    /* initialize any hardware specific issues */
    jsr      init   

    /* call main */
    jsr      main

    nop
    halt
}

// *** vector table *********************************************************

#define x4(_x)  _x, _x, _x, _x
#define x16(_y)  x4(_y), x4(_y), x4(_y), x4(_y)
#define x64(_z)  x16(_z), x16(_z), x16(_z), x16(_z)

asm void
asm_exception_handler(void)
{
    halt
    rte
}

#pragma define_section vectortable ".vectortable" far_absolute R

__declspec(vectortable)
void (*_vectors[256])(void) = {  // Interrupt vector table
   (void *)__SP_AFTER_RESET,     //   0 (0x00000000) Initial supervisor SP
   _startup,                     //   1 (0x00000004) Initial PC
   asm_exception_handler,        //   2
   asm_exception_handler,        //   3
   x4(asm_exception_handler),    //   4
   x4(asm_exception_handler),    //   8
   x4(asm_exception_handler),    //   12
   x16(asm_exception_handler),   //   16
   x16(asm_exception_handler),   //   32
   x16(asm_exception_handler),   //   48
   x64(asm_exception_handler),   //   64
   x64(asm_exception_handler),   //   128
   x64(asm_exception_handler),   //   192
};


// *** CFM config ***********************************************************

#define KEY_UPPER   0
#define KEY_LOWER   0
#define CFMPROT     0
#define CFMSACC     0
#define CFMDACC     0
#define CFMSEC      0

#pragma define_section cfmconfig ".cfmconfig"  far_absolute R
#pragma explicit_zero_data on

__declspec(cfmconfig)
unsigned long _cfm[6] = {
    KEY_UPPER,  // (0x00000404)
    KEY_LOWER,  // (0x00000404)
    CFMPROT,    // (0x00000408)
    CFMSACC,    // (0x0000040C)
    CFMDACC,    // (0x00000410)
    CFMSEC,     // (0x00000414)
};

// **************************************************************************
