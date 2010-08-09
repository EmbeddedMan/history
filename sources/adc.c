// *** adc.c **********************************************************
// this file polls the analog-to-digital converters coupled with the
// an0-an7 pins when used in analog input mode.

#include "main.h"

static bool adc_initialized;

static byte adcn = -1;

volatile short adc_result[12];

// poll the analog-to-digital converters
void
adc_timer_poll(void)
{
#if ! MCF51JM128
    int i;
#endif

    if (! adc_initialized) {
        return;
    }
    
#if ! MCF51JM128
    // if all channels are ready...
    if ((MCF_ADC_ADSTAT & 0xff) == 0xff) {
        // read all channel results into adc_result[]
        for (i = 0; i < 8; i++) {
            adc_result[i] = MCF_ADC_ADRSLT(i);
        }
    }

    // re-start the adc
    MCF_ADC_CTRL1 = MCF_ADC_CTRL1_START0;
#else  // ! MCF51JM128
    // we can only poll one enabled channel every ms
    
    // if a previous result just completed...
    if (adcn != (byte)-1 && (ADCSC1 & ADCSC1_COCO_MASK)) {
        // store the result*8 to be compatible with MCF52221/MCF52233
        adc_result[adcn] = ADCRH<<(8+3);
        adc_result[adcn] |= ADCRL<<3;
    }
    
    // initiate the next result
    adcn = (adcn+1)%12;
    ADCSC1 = adcn;
#endif
}

void
adc_sleep()
{
#if ! MCF51JM128
    // power off the adc
    MCF_ADC_POWER = MCF_ADC_POWER_PUDELAY(13)|MCF_ADC_POWER_PD2|MCF_ADC_POWER_PD1|MCF_ADC_POWER_PD0;  // disable adc
#endif
}

// this function initializes the adc module.
void
adc_initialize(void)
{
#if ! MCF51JM128
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
#else  // ! MCF51JM128
    // initialize adc
    ADCCFG = ADCCFG_ADLPC_MASK|ADCCFG_ADIV_MASK|ADCCFG_ADLSMP_MASK|ADCCFG_MODE0_MASK|ADCCFG_ADICLK0_MASK;
#endif

    adc_initialized = true;
}

