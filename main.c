// *** main.c *********************************************************
// this is the main program that is launched by startup.c and init.c;
// it just initializes all of the modules of the os and then runs the
// main application program loop.

#include "main.h"

int
#if MCF52233
main2()  // the tasking system is called by startup.c, before us
#else
main()  // we're called directly by startup.c
#endif
{
#if ! FLASHER
    // configure leds
    led_initialize();
#endif

    // initialize timer
    timer_initialize();
    
    // initialize qspi
    qspi_initialize();

#if ! FLASHER
    // initialize adc
    adc_initialize();

#if PICTOCRYPT
    // determine if we're in host or device mode
    if (adc_result[0] < 10000) {
        usb_host_mode = false;
    } else {
        usb_host_mode = true;
    }

#endif
    // initialize sleep
    sleep_initialize();

    // initialize flash
    flash_initialize();

#if MCF52221 && ! FLASHER
    // initialize usb
    usb_initialize();
#endif
#endif

    // initialize the application
    main_initialize();

#if ! MCF52233
    // enable interrupts
    splx(0);
#endif
    initialized = 1;
    
    // run the main application program loop
    main_run();

    ASSERT(0);  // stop!
    return 0;
}

