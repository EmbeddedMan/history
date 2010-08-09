// *** timer.c ********************************************************
// this file implements the core interval timer used internally.

#include "main.h"
#if STICK_GUEST && __unix__
#include <signal.h>
#include <sys/time.h>
#endif

volatile bool timer_in_isr;

volatile int ticks;  // incremented by pit0 isr every tick
volatile int msecs;  // incremented by pit0 isr every millisecond
volatile int seconds;  // incremented by pit0 isr every second

volatile static int msecs_in_second;  // incremented by pit0 isr every millisecond

enum { msecs_per_debounce = 4 }; // tunable

// called by pit0 every tick
INTERRUPT
void
#if PIC32
__ISR(4, ipl6) // REVISIT -- ipl?
#endif
timer_isr(void)
{
    bool debouncing;
    
    assert(! timer_in_isr);
    assert((timer_in_isr = true) ? true : true);

#if ! STICK_GUEST
#if MCF52221 || MCF52233
    MCF_PIT0_PCSR |= MCF_PIT_PCSR_PIF;
#elif MCF51JM128
    RTCSC |= RTCSC_RTIF_MASK;
#elif PIC32
    // clear the interrupt flag
    mT1ClearIntFlag();
#endif
#endif // ! STICK_GUEST

    ticks++;

    // If a msec elapsed...
    if ((ticks & (ticks_per_msec - 1)) == 0) {
        msecs++;
        msecs_in_second++;

        debouncing = (msecs & (msecs_per_debounce - 1)) == 0;
    
#if ! PICTOCRYPT && ! FLASHER
        if (debouncing) {
            // poll debounced digital inputs.
            pin_timer_poll();
        }
#endif

        // if an an eighth of a second has elapsed...
        if (msecs_in_second == 125) {
            msecs_in_second = 0;
        
            // if a second has elapsed...
            if (msecs%1000 == 0) {
                seconds++;
            }
        
            // poll the LEDs 8 times a second
            led_timer_poll();

#if PICTOCRYPT
            // poll for panics 8 times a second
            main_timer_poll();
#endif
        }

#if STICKOS
        // profile every msec
        code_timer_poll();
#endif

#if MCF52233 && ! STICK_GUEST
        // 200 Hz cticks
        // REVISIT -- get rid of this divide
        if (msecs%5 == 0) {
            extern volatile unsigned long cticks;
            
            cticks++;
        }
#endif

#if BADGE_BOARD && ! STICK_GUEST
        // manage LED matrix
        jm_timer_poll();
#endif
    } else {
        debouncing = false;
    }

    // poll the adc to occasionally record debouncing data and to record adc data every tick.
    adc_timer_poll(debouncing);

    assert(timer_in_isr);
    assert((timer_in_isr = false) ? true : true);
}

#if STICK_GUEST
static int oms;  // msecs already accounted for
static int tbd;  // ticks to be delivered

static
int
timer_ms(void)
{
    int ms;

#if _WIN32
    ms = GetTickCount();
#else
    ms = times(NULL)*10;
#endif
    ms *= isatty(0)?1:10;

    return ms;
}

// deliver timer ticks for STICK_GUEST builds
void
timer_ticks(bool align)
{
    int ms;

    ms = timer_ms();

    // if we are starting a run...
    if (align) {
        // wait for a time change
        while (timer_ms() == ms) {
            // NULL
        }
        ms = timer_ms();
    }

    // if the time has changed...
    if (ms != oms) {
        // schedule the ticks for delivery
        tbd += (ms-oms)*ticks_per_msec;
        oms = ms;
    }

    // deliver the ticks
    while (tbd) {
        timer_isr();
        tbd--;

        // N.B. if we're runnning (vs. starting a run) we only deliver one tick per call
        if (! align) {
            break;
        }
    }
}
#endif

// this function initializes the timer module.
void
timer_initialize(void)
{
    assert(IS_POWER_OF_2(ticks_per_msec));
    assert(IS_POWER_OF_2(msecs_per_debounce));

#if ! STICK_GUEST
#if MCF52221 || MCF52233
    // enable pit0 timer interrupt
    MCF_INTC0_ICR55 = MCF_INTC_ICR_IL(SPL_PIT0)|MCF_INTC_ICR_IP(SPL_PIT0);
    MCF_INTC0_IMRH &= ~MCF_INTC_IMRH_INT_MASK55;  // pit0
    MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_MASKALL;
    
    // configure pit0 to interrupt every ticks times per msec.
    MCF_PIT0_PCSR = 0;
    MCF_PIT0_PMR = bus_frequency/1000/ticks_per_msec - 1;
    MCF_PIT0_PCSR = MCF_PIT_PCSR_PRE(0)|MCF_PIT_PCSR_DOZE|MCF_PIT_PCSR_OVW|MCF_PIT_PCSR_PIE|MCF_PIT_PCSR_RLD|MCF_PIT_PCSR_EN;
#elif MCF51JM128
    // remap rtc to level 6
    INTC_PL6P7 = 27;

    // ticks_per_msec>4 requires use of IRCLK (32kHz), which is not
    // very accurate.  So, for now, limit ticks_per_msec to 4.
    assert(ticks_per_msec <= 4);
    
    // configure rtc to interrupt every ticks times per msec.
    RTCSC = RTCSC_RTCLKS0_MASK|RTCSC_RTIE_MASK|8;  // use external oscillator with prescale 1000
    RTCMOD = oscillator_frequency/1000/1000/ticks_per_msec - 1;
#elif PIC32
    // configure t1 to interrupt every ticks times per msec.
    T1CON = T1_SOURCE_INT | T1_PS_1_8;
    TMR1 = 0;
    PR1 = bus_frequency/8/1000/ticks_per_msec - 1;
    T1CONSET = T1_ON;

    // set up the timer interrupt with a priority of 6
    ConfigIntTimer1(T1_INT_ON | T1_INT_PRIOR_6);
#else
#error
#endif
#else  // ! STICK_GUEST
    oms = timer_ms();
#endif // ! STICK_GUEST
}

