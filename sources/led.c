#include "main.h"

// *** led ******************************************************************

//                    VDD   1 | 2   IRQ4*       G4
//                    GND   3 | 4   RSTI*
//   G0  ACG1       UTXD1   5 | 6   RSTO*
//   G1  ACG2       URXD1   7 | 8     NC
//   G2             URTS1*  9 | 10   AN0        R4
//   G3  ACSL*      UCTS1* 11 | 12   AN1        R5
//                    AN2  13 | 14   AN2        R6
//                    AN3  15 | 16   AN3        R7
//   B0         QSPI_DOUT  17 | 18   AN4   ACX
//   B1          QSPI_DIN  19 | 20   AN5   ACY
//   B2          QSPI_SCK  21 | 22   AN6   ACZ
//   B3          QSPI_CS0  23 | 24   AN7
//   B4             UTXD0  25 | 26   SCL        G6
//   B5             URXD0  27 | 28   SDA        G7
//   B6             URTS0* 29 | 30    NC
//       USB*       UCTS0* 31 | 32    NC
//   B7              IRQ1* 33 | 34 DTIN0        R0/LED1
//                    VRH  35 | 36 DTIN1        R1/LED2
//                    VRL  37 | 38 DTIN2        R2/LED3
//   G5              IRQ7* 39 | 40 DTIN3        R3/LED4


void
led_set(int n, int color)
{
#pragma unused(n, color)
#if LED
    assert(n < 8);

    if (color == COLOR_RED) {
        if (n < 4) {
            MCF_GPIO_CLRTC = (uint8)~(1 << n);
        } else {
#if RICH
            MCF_GPIO_CLRAN = (uint8)~(1 << (n-4));
#endif
        }
    } else {
        if (n < 4) {
            MCF_GPIO_SETTC = (uint8)(1 << n);
        } else {
#if RICH
            MCF_GPIO_SETAN = (uint8)(1 << (n-4));
#endif
        }
    }

#if RICH
    if (color == COLOR_GREEN) {
        if (n < 4) {
            MCF_GPIO_CLRUB = (uint8)~(1 << n);
        } else if (n < 6) {
            MCF_GPIO_CLRNQ = (uint8)~(1 << (n == 4 ? 4 : 7));
        } else {
            MCF_GPIO_CLRAS = (uint8)~(1 << (n-6));
        }
    } else {
        if (n < 4) {
            MCF_GPIO_SETUB = (uint8)(1 << n);
        } else if (n < 6) {
            MCF_GPIO_SETNQ = (uint8)(1 << (n == 4 ? 4 : 7));
        } else {
            MCF_GPIO_SETAS = (uint8)(1 << (n-6));
        }
    }

    if (color == COLOR_BLUE) {
        if (n < 4) {
            MCF_GPIO_CLRQS = (uint8)~(1 << n);
        } else if (n != 7) {
            MCF_GPIO_CLRUA = (uint8)~(1 << (n-4));
        } else {
            MCF_GPIO_CLRNQ = (uint8)~0x02;
        }
    } else {
        if (n < 4) {
            MCF_GPIO_SETQS = (uint8)(1 << n);
        } else if (n != 7) {
            MCF_GPIO_SETUA = (uint8)(1 << (n-4));
        } else {
            MCF_GPIO_SETNQ = (uint8)0x02;
        }
    }
#endif
#endif
}

void
led_happy(void)
{
#if LED
    static int rgb;

    rgb++;
    led_set(7, 1+(rgb%3));  // blink w/o black
    delay(15);
#endif
}

void
led_sad(void)
{
#if LED
    led_set(7, COLOR_OFF);
#endif
}
