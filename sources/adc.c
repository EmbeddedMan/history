#include "main.h"

// *** adc ******************************************************************

volatile short adc_result[8];

// poll the adc
bool
adc_poll()
{
    int i;
    
    // if any channel is not ready...
    if ((MCF_ADC_ADSTAT & 0xff) != 0xff) {
        return 0;
    }
    
    // read all channel results
    for (i = 0; i < 8; i++) {
        adc_result[i] = MCF_ADC_ADRSLT(i);
    }

    // re-start the adc
    MCF_ADC_CTRL1 = MCF_ADC_CTRL1_START0;

    return 1;
}

void
adc_initialize(void)
{
    // initialize adc to read all channels
    MCF_ADC_CTRL1 = MCF_ADC_CTRL1_SMODE(0);  // once sequential
    MCF_ADC_CTRL2 = 0x0005;  // divisor for 48 MHz
    MCF_ADC_ADLST1 = 0x3210;
    MCF_ADC_ADLST2 = 0x7654;
    MCF_ADC_ADSDIS = 0x00;
    MCF_ADC_POWER = MCF_ADC_POWER_PUDELAY(13);  // enable adc

    delay(10);

    // start the adc
    MCF_ADC_CTRL1 = MCF_ADC_CTRL1_START0;

    delay(10);
}

