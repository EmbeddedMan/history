extern bool led_disable_autorun;

void
led_set(int n, int on);

void
led_happy(void);

void
led_sad(void);

void
led_trigger(void);

extern bool led_assert;

void
led_line(int line);

void
led_initialize(void);
