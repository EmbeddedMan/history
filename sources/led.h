// *** led.h **********************************************************

#if PICTOCRYPT
enum code {
    code_battery = 1,  // 1
    code_usb  // 2
};

void
led_unknown(void);  // blue
#endif

void
led_unknown_progress(void);  // blue fast (ignored if happy)

#if PICTOCRYPT
void
led_happy(void);  // green

void
led_happy_progress(void);  // green fast

void
led_sad(enum code code);  // red running
#endif

void
led_poll();

void
led_line(int line);  // red crashed

void
led_initialize(void);

