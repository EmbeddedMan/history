// *** timer.h ********************************************************

__declspec(interrupt)
void
timer_isr(void);

extern bool timer_in_isr;

extern volatile int ticks;  // incremented by pit0 isr every millisecond
extern volatile int seconds;  // incremented by pit0 isr every second

extern bool initialized;  // set when pit0 interrupts are initialized

void
timer_initialize(void);

