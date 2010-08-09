// *** util.c *********************************************************
// this file implements generic utility functions.

#include "main.h"

// N.B. the usb controller bdt data structures are defined to be little
// endian and the coldfire core is big endian, so we have to byteswap.

uint32
byteswap(uint32 x, uint32 size)
{
    // byteswap all bytes of x within size
    switch (size) {
        case 4:
            asm {
                move.l     x,D0
                byterev.l  D0
                move.l     D0,x
            }
            break;
        case 2:
            asm {
                move.l     x,D0
                byterev.l  D0
                move.w     #0,D0
                swap       D0
                move.l     D0,x
            }
            break;
        case 1:
            break;
        default:
            assert(0);
            break;
    }
    return x;
}

// set the current interrupt mask level and return the old one
int
splx(int level)
{
    short oldlevel = 0;

    level = (level & 7) << 8;

    // update the sr
    asm {
        move.w     sr,d0
        move.w     d0,oldlevel  // get the old level from the sr
        and        #0xf8ff,d0
        or         level,d0     // insert the new level into the sr
        move.w     d0,sr
    }

    return (oldlevel >> 8) & 7;
}

static volatile int g;

// delay for the specified number of milliseconds
void
delay(int ms)
{
    int t;
    int x;

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

        splx(x);
    // otherwise; make a good guess with a busywait
    } else {
        while (ms--) {
            for (x = 0; x < 10000; x++) {
                g++;
            }
        }
    }
}

