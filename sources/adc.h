// *** adc.h **********************************************************

extern volatile short adc_result[12];

void
adc_timer_poll();

void
adc_sleep();

void
adc_initialize(void);

