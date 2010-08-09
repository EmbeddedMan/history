// *** led.c **********************************************************
// this file implements basic LED controls for stickos.

#include "main.h"

bool led_disable_autorun;

static bool led_assert;

// this function turns an LED on or off.
void
led_set(int n, int on)
{
    assert(n < 4);

    if (led_assert) {
        return;
    }

#if DEMO
    if (on) {
        MCF_GPIO_SETTC = (uint8)(1 << n);
    } else {
        MCF_GPIO_CLRTC = (uint8)~(1 << n);
    }
#endif

    if (on) {
        MCF_GPIO_CLRNQ = (uint8)~(1 << (n+4));
    } else {
        MCF_GPIO_SETNQ = (uint8)(1 << (n+4));
    }
}

// this function displays a diagnostic code on a LED.
void
led_line(int line)
{
    int i;

    led_assert = 1;

    for (;;) {
        for (i = 11; i >= 0; i--) {
            if (line & (1 << i)) {
                led_set(3, 1);
                delay(400);
                led_set(3, 0);
                delay(200);
            } else {
                led_set(3, 1);
                delay(200);
                led_set(3, 0);
                delay(400);
            }
        }
        led_set(3, 0);
        delay(2000);
    }
}

// this function initializes the led module.
void
led_initialize(void)
{
#if DEMO
    // TC is gpio output
    MCF_GPIO_PTCPAR = 0;
    MCF_GPIO_DDRTC = 0xf;
#endif

    // NQ is gpio output (irq4, 7) and input (irq1)
    MCF_GPIO_PNQPAR = 0;
    MCF_GPIO_DDRNQ = 0xf0;

    // if irq1 is asserted on boot, skip autorun
    if (! (MCF_GPIO_SETNQ & 0x02)) {
        led_disable_autorun = true;

        while (! (MCF_GPIO_SETNQ & 0x02)) {
            // NULL
        }

        delay(100);  // debounce
    }
}

