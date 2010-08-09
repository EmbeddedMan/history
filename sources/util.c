#include "main.h"

// *** utility **************************************************************

// N.B. the usb controller bdt data structures are defined to be little
// endian and the coldfire core is big endian, so we have to byteswap.

uint32
byteswap(uint32 x)
{
    asm {
        move.l     x,D0
        byterev.l  d0
        move.l     D0,x
    }

    return x;
}

uint16
byteswapshort(uint16 x)
{
    return byteswap(x) >> 16;
}

// set the current interrupt mask level and return the old one
int
splx(int level)
{
    short oldlevel = 0;

    level = (level & 7) << 8;

    // enable cpu interrupts
    asm {
        move.w     sr,d1
        move.w     d1,oldlevel  // get the old level from the sr
        and        #0xf8ff,d1
        or         level,d1     // insert the new level into the sr
        move.w     d1,sr
    }

    return (oldlevel >> 8) & 7;
}

// delay for the specified number of milliseconds
void
delay(int ms)
{
    int t;
    int x;
    static int g;

    // if interrupts are initialized...
    if (initialized) {
        // make sure timer interrupts (at least) are enabled while we wait
        x = splx(SPL_PIT0-1);
        if (x < SPL_PIT0-1) {
            (void)splx(x);
        }

        // wait for the pit0 to count off the ticks
        t = ticks;
        while (ticks-t < ms+1) {
            // NULL
        }

        (void)splx(x);
    } else {
        // otherwise; make a good guess
        while (ms--) {
            for (x = 0; x < 10000; x++) {
                g++;
            }
        }
    }
}

// **************************************************************************
