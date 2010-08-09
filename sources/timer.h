// *** timer.h ********************************************************

INTERRUPT
void
timer_isr(void);

extern bool timer_in_isr;

extern volatile int ticks;  // incremented by pit0 isr every millisecond
extern volatile int seconds;  // incremented by pit0 isr every second

void
timer_initialize(void);

