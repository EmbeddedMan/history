// *** init.c *********************************************************
// this file contains non-page0 initialization, which runs after we
// have completed compatible upgrade.  this file subsequently calls
// main(), wasting a word of stack.

#include "main.h"

#if ! PIC32 && ! MC9S08QE128 && ! MC9S12DT256
extern unsigned char far __BSS_START[], __BSS_END[];
extern unsigned char far __DATA_RAM[], __DATA_ROM[], __DATA_END[];
#endif

#if COMPAT
uint32 incompat;
#endif

bool disable_autorun;
uint16 flash_checksum;
bool usb_host_mode;

#if ! PIC32 && ! MC9S08QE128 && ! MC9S12DT256

#if BADGE_BOARD || DEMO_KIT
extern void pre_main(void);
#endif
extern int main();

// N.B. this is outside of page0
void
init(void)
{
    byte *p;
    uint32 n;
    uint8 *dp, *sp;
    
#if COMPAT
    if (__DATA_ROM != __DATA_RAM) {
        dp = (uint8 *)__DATA_RAM;
        sp = (uint8 *)__DATA_ROM;
        n = __DATA_END - __DATA_RAM;
        while (n--) {
            *dp++ = *sp++;
        }
    }
#endif

#if MCF51JM128
    PTGPE = 0x01;
#elif MCF51QE128
    PTAPE = 0x04;
#endif

    // move initialized data from ROM to RAM.
    // N.B. we do this here since these variables will likely change,
    // and we don't want that to result in an incompatible upgrade!
    if (__DATA_ROM != __DATA_RAM) {
        dp = (uint8 *)__DATA_RAM;
        sp = (uint8 *)__DATA_ROM;
        n = __DATA_END - __DATA_RAM;
        while (n--) {
            *dp++ = *sp++;
        }
    }

    // flash beyond this point is available for runtime data
    end_of_static = __DATA_ROM + (__DATA_END - __DATA_RAM);

#if STICKOS || SKELETON
#if MCF52221 || MCF52233
    // NQ is gpio output (irq7) and input (irq1)
    MCF_GPIO_PNQPAR = 0;
    MCF_GPIO_DDRNQ = 0x80;

    // if irq1 is asserted on boot, skip autorun
    if (! (MCF_GPIO_SETNQ & 0x02)) {
        disable_autorun = true;
    }
#elif MCF5211
    // NQ is gpio output (irq7) and input (irq4)
    MCF_GPIO_PNQPAR = 0;
    MCF_GPIO_DDRNQ = 0x80;

    // if irq4 is asserted on boot, skip autorun
    if (! (MCF_GPIO_SETNQ & 0x10)) {
        disable_autorun = true;
    }
#elif MCF52259
    // NQ is gpio output (irq7) and input (irq5)
    MCF_GPIO_PNQPAR = 0;
    MCF_GPIO_DDRNQ = 0x80;

    // if irq5 is asserted on boot, skip autorun
    if (! (MCF_GPIO_SETNQ & 0x20)) {
        disable_autorun = true;
    }
#elif MCF51JM128
    // if ptg0 is asserted on boot, skip autorun
    // N.B. pull-up was enabled early
    if (! (PTGD & 0x01)) {
        disable_autorun = true;
    }
#elif MCF51QE128
    // if pta2 is asserted on boot, skip autorun
    // N.B. pull-up was enabled early
    if (! (PTAD & 0x04)) {
        disable_autorun = true;
    }
#else
#error
#endif
#endif

    // compute flash checksum
    for (p = (byte *)0; p < (byte *)(FLASH_BYTES/2); p++) {
        flash_checksum += *p;
    }

#if BADGE_BOARD || DEMO_KIT
    pre_main();
#endif
    // finally, call main()
    main();
}
#endif

