// *** led.c **********************************************************
// this file implements basic LED controls.

#include "main.h"

enum led {
#if PICTOCRYPT
    led_red = 0,  // dtin0
    led_green,  // dtin1
    led_blue,  // dtin2
#else
    led_red = 0,  // dtin3
    led_blue = 0,  // dtin3
#endif
    led_max
};

static int led_count[led_max];
static int last_led_count[led_max];

static int led_code;

static enum led led_blink;


#if PICTOCRYPT
void
led_unknown(void)
{
    led_code = 0;
    led_blink = led_blue;
}
#endif

void
led_unknown_progress(void)
{
    led_count[led_blue]++;
    
#if PICTOCRYPT
    // go to sleep SLEEP_DELAY seconds after progress stops
    sleep_delay(SLEEP_DELAY);
#endif
}

#if PICTOCRYPT
void
led_happy(void)
{
    led_code = 0;
    led_blink = led_green;
}

void
led_happy_progress(void)
{
    led_count[led_green]++;
}

void
led_sad(enum code code)
{
    led_code = code;
    led_blink = led_red;
}
#endif

// this function turns an LED on or off.
static
void
led_set(enum led n, int on)
{
#pragma unused(n)
    assert (n >= 0 && n < led_max);
    
#if PICTOCRYPT
    // red LED workaround; tri-state when off!
    if (on) {
        MCF_GPIO_DDRTC = 1 << n;
    } else {
        MCF_GPIO_DDRTC = 0;
    }

    MCF_GPIO_SETTC = (uint8)~(1 << n);
    if (on) {
        MCF_GPIO_CLRTC = (uint8)~(1 << n);
    } else {
        MCF_GPIO_SETTC = (uint8)(1 << n);
    }
#else
#if DEMO
    if (on) {
        MCF_GPIO_SETTC = (uint8)(1 << 3);
    } else {
        MCF_GPIO_CLRTC = (uint8)~(1 << 3);
    }
#endif

    if (on) {
        MCF_GPIO_CLRNQ = (uint8)~(1 << (7));
    } else {
        MCF_GPIO_SETNQ = (uint8)(1 << (7));
    }
#endif
}

void
led_poll()
{
#if PICTOCRYPT
    int i;
    static bool green;
#endif
    static int calls;
    
    calls++;
    
#if PICTOCRYPT
    // if we have a run-time error code to display...
    if (led_code) {
        i = calls%(led_code*2+12);
        if (i >= led_code*2) {
            led_set(led_blink, 0);
        } else {
            led_set(led_blink, i%2);
        }
        
        return;
    }
    
    // if we have a green blip to display...
    if (green || (calls%8 == 0 && led_count[led_green] != last_led_count[led_green])) {
        green = true;
        led_set((calls/2)%2?led_green:led_blink, calls%2);  // blink green fast
        if (calls%8 == 7) {
            green = false;
        }
        last_led_count[led_green] = led_count[led_green];
        return;
    }
#endif

    // normal led processing
    if (led_count[led_blink] != last_led_count[led_blink]) {
        led_set(led_blink, calls%2);  // blink fast
        last_led_count[led_blink] = led_count[led_blink];
    } else {
        led_set(led_blink, (calls/4)%2);  // blink slow
    }
}

// this function displays a diagnostic code on a LED.
void
led_line(int line)
{
    int i;
    int j;
    
    if (debugger_attached) {
        asm {
            halt
        };
    } else {
        splx(7);
        initialized = 0;
        
        for (;;) {
            for (i = 1000; i > 0; i /= 10) {
                if (line < i) {
                    continue;
                }
                j = (line/i)%10;
                if (! j) {
                    j = 10;
                }
                while (j--) {
                    led_set(led_red, 1);
                    delay(200);
                    led_set(led_red, 0);
                    delay(200);
                }
                delay(1000);
            }
            delay(3000);
        }
    }
}

// this function initializes the led module.
void
led_initialize(void)
{
    // TC is gpio output
    MCF_GPIO_PTCPAR = 0;
    MCF_GPIO_DDRTC = 0xf;
#if PICTOCRYPT
    MCF_GPIO_CLRTC = ~0xf;  // all LEDs on to indicate reset!
#else
    MCF_GPIO_SETTC = 0xf;  // all LEDs on to indicate reset!
#endif
}

