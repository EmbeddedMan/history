// *** sleep.h ********************************************************

__declspec(interrupt)
void
sleep_isr(void);

void
sleep_poll(void);

void
sleep_initialize(void);

