#include "main.h"

// *** main *****************************************************************

int
main()
{
    // configure leds
    led_initialize();

    // initialize timer
    timer_initialize();
    
    // initialize adc
    adc_initialize();

    // initialize sleep
    sleep_initialize();

#if FLASH
    // initialize flash
    flash_initialize();
#endif

#if USB
    // initialize usb
    usb_initialize();
#endif

#if CPUSTICK
    // register cpustick cbfns
    cpustick_register();
#endif

    // enable interrupts
    (void)splx(0);
    initialized = 1;

#if BASIC
    // initialize basic
    basic_initialize();
#endif

#if CPUSTICK
    // run!
    cpustick_run();
#else
    for (;;) {
        sleep_poll();
    }
#endif

    // stop!
    assert(0);
    return 0;
}
