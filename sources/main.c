// *** main.c *********************************************************
// this is the main program that is launched by startup.c; it just
// initializes all of the modules of stickos and then runs the cpustick
// main program loop.

#include "main.h"

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

    // initialize flash
    flash_initialize();

    // initialize usb
    usb_initialize();

    // register cpustick cbfns
    cpustick_register();

    // enable interrupts
    splx(0);
    initialized = 1;

    // initialize basic
    basic_initialize();

    // run!
    cpustick_run();

    // stop!
    assert(0);
    return 0;
}

