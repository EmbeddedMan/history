extern int ticks;  // incremented by pit0 isr every millisecond
extern bool initialized;  // set when pit0 interrupts are initialized

void
timer_initialize(void);
