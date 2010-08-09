// *** led.h **********************************************************

extern bool led_disable_autorun;

void
led_set(int n, int on);

void
led_line(int line);

void
led_initialize(void);

