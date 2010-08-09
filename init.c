// *** init.c *********************************************************
// this file contains non-page0 initialization, which runs after we
// have completed compatible upgrade.  this file subsequently calls
// main(), wasting a word of stack.

#include "main.h"

extern unsigned char far __BSS_START[], __BSS_END[];
extern unsigned char far __DATA_RAM[], __DATA_ROM[], __DATA_END[];

#if COMPAT
uint32 incompat;
#endif

bool disable_autorun;
uint16 flash_checksum;
bool usb_host_mode;

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
#endif

    // compute flash checksum
    for (p = (byte *)0; p < (byte *)(FLASH_BYTES/2); p++) {
        flash_checksum += *p;
    }

    // finally, call main()
    main();
}

