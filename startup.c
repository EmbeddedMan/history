// *** startup.c ******************************************************
// this file is where hardware starts execution, at _startup, then
// we call init() in this file, and finally we call to main().

#include "main.h"

extern unsigned char far _SP_INIT[], _SDA_BASE[];
extern unsigned char far __SP_AFTER_RESET[];
extern unsigned char far __BSS_START[], __BSS_END[];
extern unsigned char far __DATA_RAM[], __DATA_ROM[], __DATA_END[];

byte *end_of_static;
uint32 fsys_frequency;
uint32 oscillator_frequency;
bool debugger_attached;
bool disable_autorun;
bool host_mode;
uint16 flash_checksum;
#if PICTOCRYPT
byte big_buffer[8192];
#else
byte big_buffer[2048];
#endif

#if MCF52233
uint8 powerup_config_flags;
#endif

extern int main();

// this function performs C initialization before main runs.
static
void
init(void)
{
    byte *p;
    uint32 n;
    uint8 *dp, *sp;

    // Enable debug
    MCF_GPIO_PDDPAR = 0x0F;

    // Move initialized data from ROM to RAM.
    if (__DATA_ROM != __DATA_RAM)
    {
        dp = (uint8 *)__DATA_RAM;
        sp = (uint8 *)__DATA_ROM;
        n = __DATA_END - __DATA_RAM;
        while (n--)
            *dp++ = *sp++;
    }

    // Zero uninitialized data
    if (__BSS_START != __BSS_END)
    {
        sp = (uint8 *)__BSS_START;
        n = __BSS_END - __BSS_START;
        while (n--)
            *sp++ = 0;
    }

    // Disable Software Watchdog Timer
    MCF_SCM_CWCR = 0;

    // flash beyond this point is available for runtime data
    end_of_static = __DATA_ROM + (__DATA_END - __DATA_RAM);

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
    
    // Enable on-chip modules to access internal SRAM
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

    // NQ is gpio output (irq4, 7) and input (irq1)
    MCF_GPIO_PNQPAR = 0;
    MCF_GPIO_DDRNQ = 0xf0;

    // if irq1 is asserted on boot, skip autorun
    if (! (MCF_GPIO_SETNQ & 0x02)) {
        disable_autorun = true;

        while (! (MCF_GPIO_SETNQ & 0x02)) {
            // NULL
        }

        delay(100);  // debounce
    }

    // Set real time clock freq
    MCF_CLOCK_RTCDR = fsys_frequency;

    // compute flash checksum
    for (p = (byte *)0; p < (byte *)(FLASH_BYTES/2); p++) {
        flash_checksum += *p;
    }

#if MCF52233
	// Clear mask all bit to allow interrupts
    MCF_INTC0_IMRL &= ~(MCF_INTC_IMRL_MASKALL);

	// Initialize PLDPAR to enable Ethernet LEDs
	MCF_GPIO_PLDPAR = (0
					| MCF_GPIO_PORTLD_PORTLD0 // ACTLED
					| MCF_GPIO_PORTLD_PORTLD1 // LNKLED
					| MCF_GPIO_PORTLD_PORTLD2 // SPDLED
					| MCF_GPIO_PORTLD_PORTLD3 // DUPLED
					| MCF_GPIO_PORTLD_PORTLD4 // COLLED
					| MCF_GPIO_PORTLD_PORTLD5 // RXLED
					| MCF_GPIO_PORTLD_PORTLD6 // TXLED
					);
	
    mcf5223x_ePHY_init();
#endif
}

asm void
_startup(void);

// this function performs assembly language initialization before
// main runs, and then calls init (C), and then calls main (C).
asm void
_startup(void)
{
    /* disable interrupts */
    move.w   #0x2700,sr
    
#if FLASHER
    // set VBR to 0x20000000
    move.l   #0x20000000,d0
    movec    d0,VBR
#endif

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

#define x4(_x)  _x, _x, _x, _x
#define x16(_y)  x4(_y), x4(_y), x4(_y), x4(_y)
#define x64(_z)  x16(_z), x16(_z), x16(_z), x16(_z)

static
asm void
asm_exception_handler(void)
{
    halt
    rte
}

#pragma define_section vectortable ".vectortable" far_absolute R

#if MCF52233
__declspec(interrupt)
void
fec_isr(void);
#endif

// this is the interrupt vector table.
__declspec(vectortable)
void (*_vect[256])(void) = {  // Interrupt vector table
    (void *)__SP_AFTER_RESET, // 0 (0x00000000) Initial supervisor SP
    _startup,                 // 1 (0x00000004) Initial PC
    asm_exception_handler,    // 2
    asm_exception_handler,    // 3
    asm_exception_handler,    // 4
    asm_exception_handler,    // 5
    asm_exception_handler,    // 6
    asm_exception_handler,    // 7
    asm_exception_handler,    // 8
    asm_exception_handler,    // 9
    asm_exception_handler,    // 10
    asm_exception_handler,    // 11
    asm_exception_handler,    // 12
    asm_exception_handler,    // 13
    asm_exception_handler,    // 14
    asm_exception_handler,    // 15
    asm_exception_handler,    // 16
    asm_exception_handler,    // 17
    asm_exception_handler,    // 18
    asm_exception_handler,    // 19
    asm_exception_handler,    // 20
    asm_exception_handler,    // 21
    asm_exception_handler,    // 22
    asm_exception_handler,    // 23
    asm_exception_handler,    // 24
    asm_exception_handler,    // 25
    asm_exception_handler,    // 26
    asm_exception_handler,    // 27
    asm_exception_handler,    // 28
    asm_exception_handler,    // 29
    asm_exception_handler,    // 30
    asm_exception_handler,    // 31
    asm_exception_handler,    // 32
    asm_exception_handler,    // 33
    asm_exception_handler,    // 34
    asm_exception_handler,    // 35
    asm_exception_handler,    // 36
    asm_exception_handler,    // 37
    asm_exception_handler,    // 38
    asm_exception_handler,    // 39
    asm_exception_handler,    // 40
    asm_exception_handler,    // 41
    asm_exception_handler,    // 42
    asm_exception_handler,    // 43
    asm_exception_handler,    // 44
    asm_exception_handler,    // 45
    asm_exception_handler,    // 46
    asm_exception_handler,    // 47
    asm_exception_handler,    // 48
    asm_exception_handler,    // 49
    asm_exception_handler,    // 50
    asm_exception_handler,    // 51
    asm_exception_handler,    // 52
    asm_exception_handler,    // 53
    asm_exception_handler,    // 54
    asm_exception_handler,    // 55
    asm_exception_handler,    // 56
    asm_exception_handler,    // 57
    asm_exception_handler,    // 58
    asm_exception_handler,    // 59
    asm_exception_handler,    // 60
    asm_exception_handler,    // 61
    asm_exception_handler,    // 62
    asm_exception_handler,    // 63
    asm_exception_handler,    // 64
#if ! FLASHER
    sleep_isr,                // 65
#else
    asm_exception_handler,    // 65
#endif
    asm_exception_handler,    // 66
    asm_exception_handler,    // 67
    asm_exception_handler,    // 68
    asm_exception_handler,    // 69
    asm_exception_handler,    // 70
    asm_exception_handler,    // 71
    asm_exception_handler,    // 72
    asm_exception_handler,    // 73
    asm_exception_handler,    // 74
    asm_exception_handler,    // 75
    asm_exception_handler,    // 76
    asm_exception_handler,    // 77
    asm_exception_handler,    // 78
    asm_exception_handler,    // 79
    asm_exception_handler,    // 80
    asm_exception_handler,    // 81
    asm_exception_handler,    // 82
    asm_exception_handler,    // 83
    asm_exception_handler,    // 84
    asm_exception_handler,    // 85
    asm_exception_handler,    // 86
#if MCF52233
    fec_isr,                  // 87 ifec.c
    fec_isr,                  // 88 ifec.c
    fec_isr,                  // 89 ifec.c
    fec_isr,                  // 90 ifec.c
    fec_isr,                  // 91 ifec.c
    fec_isr,                  // 92 ifec.c
    fec_isr,                  // 93 ifec.c
    fec_isr,                  // 94 ifec.c
    fec_isr,                  // 95 ifec.c
    fec_isr,                  // 96 ifec.c
    fec_isr,                  // 97 ifec.c
    fec_isr,                  // 98 ifec.c
    fec_isr,                  // 99 ifec.c
#else
    asm_exception_handler,    // 87
    asm_exception_handler,    // 88
    asm_exception_handler,    // 89
    asm_exception_handler,    // 90
    asm_exception_handler,    // 91
    asm_exception_handler,    // 92
    asm_exception_handler,    // 93
    asm_exception_handler,    // 94
    asm_exception_handler,    // 95
    asm_exception_handler,    // 96
    asm_exception_handler,    // 97
    asm_exception_handler,    // 98
    asm_exception_handler,    // 99
#endif
    asm_exception_handler,    // 100
    asm_exception_handler,    // 101
    asm_exception_handler,    // 102
    asm_exception_handler,    // 103
    asm_exception_handler,    // 104
    asm_exception_handler,    // 105
    asm_exception_handler,    // 106
    asm_exception_handler,    // 107
    asm_exception_handler,    // 108
    asm_exception_handler,    // 109
    asm_exception_handler,    // 110
    asm_exception_handler,    // 111
    asm_exception_handler,    // 112
    asm_exception_handler,    // 113
    asm_exception_handler,    // 114
    asm_exception_handler,    // 115
    asm_exception_handler,    // 116
#if MCF52221 && ! FLASHER
    usb_isr,                  // 117 usb.c
#else
    asm_exception_handler,    // 117
#endif
    asm_exception_handler,    // 118
    timer_isr,                // 119 timer.c
    asm_exception_handler,    // 120
    asm_exception_handler,    // 121
    asm_exception_handler,    // 122
    asm_exception_handler,    // 123
    asm_exception_handler,    // 124
    asm_exception_handler,    // 125
    asm_exception_handler,    // 126
    asm_exception_handler,    // 127
    asm_exception_handler,    // 128
    asm_exception_handler,    // 129
    asm_exception_handler,    // 130
    asm_exception_handler,    // 131
    asm_exception_handler,    // 132
    asm_exception_handler,    // 133
    asm_exception_handler,    // 134
    asm_exception_handler,    // 135
    asm_exception_handler,    // 136
    asm_exception_handler,    // 137
    asm_exception_handler,    // 138
    asm_exception_handler,    // 139
    asm_exception_handler,    // 140
    asm_exception_handler,    // 141
    asm_exception_handler,    // 142
    asm_exception_handler,    // 143
    asm_exception_handler,    // 144
    asm_exception_handler,    // 145
    asm_exception_handler,    // 146
    asm_exception_handler,    // 147
    asm_exception_handler,    // 148
    asm_exception_handler,    // 149
    asm_exception_handler,    // 150
    asm_exception_handler,    // 151
    asm_exception_handler,    // 152
    asm_exception_handler,    // 153
    asm_exception_handler,    // 154
    asm_exception_handler,    // 155
    asm_exception_handler,    // 156
    asm_exception_handler,    // 157
    asm_exception_handler,    // 158
    asm_exception_handler,    // 159
    asm_exception_handler,    // 160
    asm_exception_handler,    // 161
    asm_exception_handler,    // 162
    asm_exception_handler,    // 163
    asm_exception_handler,    // 164
    asm_exception_handler,    // 165
    asm_exception_handler,    // 166
    asm_exception_handler,    // 167
    asm_exception_handler,    // 168
    asm_exception_handler,    // 169
    asm_exception_handler,    // 170
    asm_exception_handler,    // 171
    asm_exception_handler,    // 172
    asm_exception_handler,    // 173
    asm_exception_handler,    // 174
    asm_exception_handler,    // 175
    asm_exception_handler,    // 176
    asm_exception_handler,    // 177
    asm_exception_handler,    // 178
    asm_exception_handler,    // 179
    asm_exception_handler,    // 180
    asm_exception_handler,    // 181
    asm_exception_handler,    // 182
    asm_exception_handler,    // 183
    asm_exception_handler,    // 184
    asm_exception_handler,    // 185
    asm_exception_handler,    // 186
    asm_exception_handler,    // 187
    asm_exception_handler,    // 188
    asm_exception_handler,    // 189
    asm_exception_handler,    // 190
    asm_exception_handler,    // 191
    asm_exception_handler,    // 192
    asm_exception_handler,    // 193
    asm_exception_handler,    // 194
    asm_exception_handler,    // 195
    asm_exception_handler,    // 196
    asm_exception_handler,    // 197
    asm_exception_handler,    // 198
    asm_exception_handler,    // 199
    asm_exception_handler,    // 200
    asm_exception_handler,    // 201
    asm_exception_handler,    // 202
    asm_exception_handler,    // 203
    asm_exception_handler,    // 204
    asm_exception_handler,    // 205
    asm_exception_handler,    // 206
    asm_exception_handler,    // 207
    asm_exception_handler,    // 208
    asm_exception_handler,    // 209
    asm_exception_handler,    // 210
    asm_exception_handler,    // 211
    asm_exception_handler,    // 212
    asm_exception_handler,    // 213
    asm_exception_handler,    // 214
    asm_exception_handler,    // 215
    asm_exception_handler,    // 216
    asm_exception_handler,    // 217
    asm_exception_handler,    // 218
    asm_exception_handler,    // 219
    asm_exception_handler,    // 220
    asm_exception_handler,    // 221
    asm_exception_handler,    // 222
    asm_exception_handler,    // 223
    asm_exception_handler,    // 224
    asm_exception_handler,    // 225
    asm_exception_handler,    // 226
    asm_exception_handler,    // 227
    asm_exception_handler,    // 228
    asm_exception_handler,    // 229
    asm_exception_handler,    // 230
    asm_exception_handler,    // 231
    asm_exception_handler,    // 232
    asm_exception_handler,    // 233
    asm_exception_handler,    // 234
    asm_exception_handler,    // 235
    asm_exception_handler,    // 236
    asm_exception_handler,    // 237
    asm_exception_handler,    // 238
    asm_exception_handler,    // 239
    asm_exception_handler,    // 240
    asm_exception_handler,    // 241
    asm_exception_handler,    // 242
    asm_exception_handler,    // 243
    asm_exception_handler,    // 244
    asm_exception_handler,    // 245
    asm_exception_handler,    // 246
    asm_exception_handler,    // 247
    asm_exception_handler,    // 248
    asm_exception_handler,    // 249
    asm_exception_handler,    // 250
    asm_exception_handler,    // 251
    asm_exception_handler,    // 252
    asm_exception_handler,    // 253
    asm_exception_handler,    // 254
    asm_exception_handler     // 255
};


#define KEY_UPPER   0
#define KEY_LOWER   0
#define CFMPROT     0xffffffff
#define CFMSACC     0
#define CFMDACC     0
#if SECURE
#define CFMSEC      MCF_CFM_CFMSEC_SECSTAT|0x4AC8
#else
#define CFMSEC      0
#endif

#pragma define_section cfmconfig ".cfmconfig"  far_absolute R
#pragma explicit_zero_data on

// this is the cfm config
__declspec(cfmconfig)
unsigned long _cfm[6] = {
    KEY_UPPER,  // (0x00000400)
    KEY_LOWER,  // (0x00000404)
    CFMPROT,    // (0x00000408)
    CFMSACC,    // (0x0000040C)
    CFMDACC,    // (0x00000410)
    CFMSEC,     // (0x00000414)
};

