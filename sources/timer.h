// *** timer.h ********************************************************

#if ! PIC32
INTERRUPT
void
timer_isr(void);
#endif

#if PICTOCRYPT
enum { ticks_per_msec = 1 }; // tunable
#else
enum { ticks_per_msec = 4 }; // tunable
#endif

extern volatile int ticks;  // incremented by pit0 isr every tick
extern volatile int msecs;  // incremented by pit0 isr every millisecond
extern volatile int seconds;  // incremented by pit0 isr every second

extern bool volatile timer_in_isr;

#if STICK_GUEST
void
timer_ticks(bool align);
#endif

void
timer_initialize(void);

