#include "main.h"

// *** timer ****************************************************************

bool timer_in_isr;

int volatile ticks;  // incremented by pit0 isr every millisecond
int volatile seconds;  // incremented by pit0 isr every second

bool initialized;  // set when pit0 interrupts are initialized

// called by pit0 every millisecond
static
__declspec(interrupt)
void
timer_isr(void)
{
    static int last_run_line_count;
    
    assert(! timer_in_isr);
    timer_in_isr = true;
    
    MCF_PIT0_PCSR |= MCF_PIT_PCSR_PIF;
    ticks++;
    
    // poll the adc every millisecond
    adc_poll();

    if (ticks%125 == 0) {
        if (! led_assert) {
#if BASIC
            if (run_line_count != last_run_line_count) {
                led_set(3, (ticks/125)%2);  // blink fast
                last_run_line_count = run_line_count;
            } else {
#endif
                led_set(3, (ticks/500)%2);  // blink slow
#if BASIC
            }
#endif
        }
        
        if (ticks%1000 == 0) {
            seconds++;
        }
    }
    
    assert(timer_in_isr);
    timer_in_isr = false;
}

void
timer_initialize(void)
{
    // enable pit0 timer interrupt
    __VECTOR_RAM[119] = (uint32)timer_isr;
    MCF_INTC0_ICR55 = MCF_INTC_ICR_IL(SPL_PIT0)|MCF_INTC_ICR_IP(SPL_PIT0);
    MCF_INTC0_IMRH &= ~MCF_INTC_IMRH_INT_MASK55;  // pit0
    MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_MASKALL;

    // configure pit0 to interrupt every 1 ms
    MCF_PIT0_PCSR = 0;
    MCF_PIT0_PMR = 24000;  // 1 ms @ 48 MHz
    MCF_PIT0_PCSR = MCF_PIT_PCSR_PRE(0)|MCF_PIT_PCSR_OVW|MCF_PIT_PCSR_PIE|MCF_PIT_PCSR_RLD|MCF_PIT_PCSR_EN;
}
