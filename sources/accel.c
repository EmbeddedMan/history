#include "main.h"

// *** accelerometer ********************************************************

#if ACCEL
// our low pass filter coefficients
static short coef[] = {
      67,
     449,
     874,
    1261,
    1531,
    1628,
    1531,
    1261,
     874,
     449,
      67,
};

// our low pass filter order
#define ORDER  (sizeof(coef)/sizeof(coef[0]))

// to compute our swing period
static int lastzero;
static bool armedx;
static int period;

// poll the accelerometer
void
accel_poll()
{
    int i;
    int x;
    int y;
    int z;
    static int rgb;
    static int lastx;
    static int s;
    static int xs[ORDER];
    static int ys[ORDER];
    static int zs[ORDER];

    // read the adc
    assert(MCF_ADC_ADSTAT & MCF_ADC_ADSTAT_RDY0);
    x = MCF_ADC_ADRSLT0;
    assert(MCF_ADC_ADSTAT & MCF_ADC_ADSTAT_RDY1);
    y = MCF_ADC_ADRSLT1;
    assert(MCF_ADC_ADSTAT & MCF_ADC_ADSTAT_RDY2);
    z = MCF_ADC_ADRSLT2;

    // re-start the adc
    MCF_ADC_CTRL1 = MCF_ADC_CTRL1_START0;

    // low pass filter
    xs[s] = x;
    ys[s] = y;
    zs[s] = z;
    x = 0;
    y = 0;
    z = 0;
    for (i = 0; i < ORDER; i++) {
        x += coef[i]*xs[(s+i)%ORDER];
        y += coef[i]*ys[(s+i)%ORDER];
        z += coef[i]*zs[(s+i)%ORDER];
    }
    x /= 10000;
    y /= 10000;
    z /= 10000;
    s = (s+1)%ORDER;

    // set the LEDs
    if (x > 0x1000 && x < 0x7000) {
        if (x < 0x3000) {
            led_set(0, COLOR_RED);
        } else if (x > 0x5000) {
            led_set(0, COLOR_GREEN);
        } else {
            led_set(0, COLOR_BLUE);
        }
    } else {
        led_set(0, COLOR_OFF);
    }
    if (y > 0x1000 && y < 0x7000) {
        if (y < 0x3000) {
            led_set(1, COLOR_RED);
        } else if (y > 0x5000) {
            led_set(1, COLOR_GREEN);
        } else {
            led_set(1, COLOR_BLUE);
        }
    } else {
        led_set(1, COLOR_OFF);
    }
    if (z > 0x1000 && x < 0x7000) {
        if (z < 0x3000) {
            led_set(2, COLOR_RED);
        } else if (z > 0x5000) {
            led_set(2, COLOR_GREEN);
        } else {
            led_set(2, COLOR_BLUE);
        }
    } else {
        led_set(2, COLOR_OFF);
    }

    // compute the swing period
    if (armedx && lastx < 0x4000 && x >= 0x4000) {
        period = ticks - lastzero;
        rgb++;
        lastzero = ticks;
        armedx = 0;
    }
    if (lastx < 0x3000) {
        armedx = 1;
    }

    lastx = x;

    {
        int n;
        int t;
        int p;

        // display the swing position
        for (n = 4; n < 7; n++) {
            led_set(n, COLOR_OFF);
        }
        p = period;
        if (p) {
            t = ticks-lastzero;
            n = t*8/p;
            if (n >= 4 && n < 7) {
                // turn on each led in timed sequence
                led_set(n, 1+(rgb%3));
            }
        }
    }
}

void
accel_initialize(void)
{
    // initialize adc to read accelerometer
    MCF_ADC_CTRL1 = MCF_ADC_CTRL1_SMODE(0);  // once sequential
    MCF_ADC_CTRL2 = 0x0005;  // divisor for 48 MHz
    MCF_ADC_ADLST1 = 0x7654;  // channels 4 = x; 5 = y; 6 = z; 7 = analog
    MCF_ADC_ADLST2 = 0x0000;
    MCF_ADC_ADSDIS = 0xf0;
    MCF_ADC_POWER = MCF_ADC_POWER_PUDELAY(13);  // enable adc

    delay(10);

    // start the adc
    MCF_ADC_CTRL1 = MCF_ADC_CTRL1_START0;
}
#endif

