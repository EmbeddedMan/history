// *** adc.h **********************************************************

extern volatile short adc_result[8];

bool
adc_poll();

void
adc_sleep();

void
adc_initialize(void);

