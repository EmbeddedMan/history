// *** adc.c **********************************************************
// this file polls the analog-to-digital converters coupled with the
// an0-an7 pins when used in analog input mode.

#include "main.h"

static bool adc_initialized;

#if MCF51JM128
static byte adcn = -1;
#endif


enum {
    adc_num_debounce_history = 3
};


// updated every tick
#if ! PICTOCRYPT
static
#endif
volatile uint16 adc_result[adc_num_channel];  // 0..32767

// updated every "debouncing" tick, the rate is determined by timer_isr()
static
volatile uint16 adc_debounce[adc_num_channel][adc_num_debounce_history];

// indexes into adc_debounce: adc_debounce[ch][adc_debounce_set].
static
int adc_debounce_set;


// poll the analog-to-digital converters
void
adc_timer_poll(bool debouncing)
{
#if ! STICK_GUEST
#if MCF52221 || MCF52233 || PIC32
    int i;
#endif
#endif

    if (! adc_initialized) {
        return;
    }

    assert(adc_debounce_set < adc_num_debounce_history);

#if ! STICK_GUEST
#if MCF52221 || MCF52233
    // if all channels are ready...
    if ((MCF_ADC_ADSTAT & 0xff) == 0xff) {
        // read all channel results into adc_result[]
        for (i = 0; i < adc_num_channel; i++) {
            adc_result[i] = MCF_ADC_ADRSLT(i);
            if (debouncing) {
                adc_debounce[i][adc_debounce_set] = adc_result[i];
            }
        }
    } else {
        // prevent debounce increment below because we did not read adc data and to populate the current debounce set.
        debouncing = false;
    }

    // re-start the adc
    MCF_ADC_CTRL1 = MCF_ADC_CTRL1_START0;
#elif MCF51JM128
    // we can only poll one enabled channel every ms
    // each channel is read every 12ms.
    
    // if a previous result just completed...
    if (adcn != (byte)-1 && (ADCSC1 & ADCSC1_COCO_MASK)) {
        // store the result*8 to be compatible with MCF52221/MCF52233
        assert(adcn < adc_num_channel);
        adc_result[adcn] = ADCRH<<(8+3);
        adc_result[adcn] |= ADCRL<<3;
        adc_debounce[adcn][adc_debounce_set] = adc_result[adcn];
    }
    
    // initiate the next result
    // advance the debounce set if wrapping adcn
    if (++adcn >= adc_num_channel) {
        adcn = 0;
        debouncing = true;
    } else {
        debouncing = false;
    }
    ADCSC1 = adcn;
#elif PIC32
    for (i = 0; i < adc_num_channel; i++) {
        // store the result*32 to be compatible with MCF52221/MCF52233
        adc_result[i] = ReadADC10(i)<<5;
    }
#else
#error
#endif
#endif // ! STICK_GUEST

    // if a debounced set if full, then advance the debouncing index to the next set.
    if (debouncing) {
        if (++adc_debounce_set >= adc_num_debounce_history) {
            adc_debounce_set = 0;
        }
    }
}

void
adc_sleep()
{
#if ! STICK_GUEST
#if MCF52221 || MCF52233
    // power off the adc
    MCF_ADC_POWER = MCF_ADC_POWER_PUDELAY(13)|MCF_ADC_POWER_PD2|MCF_ADC_POWER_PD1|MCF_ADC_POWER_PD0;  // disable adc
#endif
#endif // ! STICK_GUEST
}

int
adc_get_value(int offset, int pin_qual)
{
    int value;

    assert(offset < adc_num_channel);

    if (pin_qual & 1<<pin_qual_debounced) {
        uint32 min, max, sum, i;
        min = ~0;
        max = 0;
        sum = 0;

        // Need at least 3 samples to exclude min and max.
        assert(adc_num_debounce_history > 2);

        for (i = 0; i < adc_num_debounce_history; i++) {
            min = MIN(min, adc_debounce[offset][i]);
            max = MAX(max, adc_debounce[offset][i]);
            sum += adc_debounce[offset][i];
        }
        value = (sum-min-max)/(adc_num_debounce_history-2);
    } else {
        value = adc_result[offset];
    }

    value = value*3300/32768;

    return value;
}

// this function initializes the adc module.
void
adc_initialize(void)
{
#if ! STICK_GUEST
#if MCF52221 || MCF52233
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
#elif MCF51JM128
    // initialize adc to read one channel at a time
    ADCCFG = ADCCFG_ADLPC_MASK|ADCCFG_ADIV_MASK|ADCCFG_ADLSMP_MASK|ADCCFG_MODE0_MASK|ADCCFG_ADICLK0_MASK;
#elif PIC32
    // initialize adc to read all channels
    AD1CON1 = _AD1CON1_SSRC_MASK|_AD1CON1_ASAM_MASK;
    AD1CON2 = _AD1CON2_CSCNA_MASK|_AD1CON2_SMPI_MASK;
    assert(bus_frequency == 40000000);
    AD1CON3 = (2<<_AD1CON3_SAMC_POSITION)|2;  // assumes 40 MHz; 2 uS

    AD1CSSL = 0xffff;  // sample all 16 inputs
    AD1CON1 |= _AD1CON1_ON_MASK;
#else
#error
#endif
#endif // ! STICK_GUEST
    
    adc_initialized = true;
}

