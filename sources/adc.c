// *** adc.c **********************************************************
// this file polls the analog-to-digital converters coupled with the
// an0-an7 pins when used in analog input mode.

#include "main.h"

volatile short adc_result[8];

// poll the analog-to-digital converters
bool
adc_poll(void)
{
    int i;

    // if any channel is not ready...
    if ((MCF_ADC_ADSTAT & 0xff) != 0xff) {
        // return failure
        return 0;
    }

    // read all channel results into adc_result[]
    for (i = 0; i < 8; i++) {
        adc_result[i] = MCF_ADC_ADRSLT(i);
    }

    // re-start the adc
    MCF_ADC_CTRL1 = MCF_ADC_CTRL1_START0;

    // return success
    return 1;
}

// this function initializes the adc module.
void
adc_initialize(void)
{
#if PICTOCRYPT
    bool boo;
#endif

    // initialize adc to read all channels
    MCF_ADC_CTRL1 = MCF_ADC_CTRL1_SMODE(0);  // once sequential
    MCF_ADC_CTRL2 = 0x0005;  // divisor for 48 MHz
    MCF_ADC_ADLST1 = 0x3210;
    MCF_ADC_ADLST2 = 0x7654;
    MCF_ADC_ADSDIS = 0x00;
    MCF_ADC_POWER = MCF_ADC_POWER_PUDELAY(13);  // enable adc

    // AN is primary
    MCF_GPIO_PANPAR = 0xff;

    delay(10);

    // start the adc
    MCF_ADC_CTRL1 = MCF_ADC_CTRL1_START0;

    delay(10);
#if PICTOCRYPT
    boo = adc_poll();
    assert(boo);
    delay(10);
#endif
}

