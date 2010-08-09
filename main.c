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
    int x;

    // configure leds
    led_initialize();

    // initialize timer
    timer_initialize();
    
    // calibrate our busywait
    x = splx(0);
    delay(20);
    splx(x);
    
    // initialize qspi
    qspi_initialize();

    // initialize adc
    adc_initialize();
    
#if ! FLASHER && ! PICTOCRYPT
    // initialize pins
    pin_initialize();
#endif

#if PICTOCRYPT
    adc_timer_poll();

    // determine if we're in host or device mode
    if (adc_result[0] < 10000) {
        usb_host_mode = false;
    } else {
        usb_host_mode = true;
    }

#endif
    // initialize sleep
    sleep_initialize();

#if ! FLASHER
    // initialize flash
    flash_initialize();

#if MCF52221 || MCF51JM128
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

#if ! FLASHER && ! PICTOCRYPT
    // initialize zigbee
    zb_initialize();
    
    // initialize the terminal interface
    terminal_initialize();
#endif
    
#if BADGE_BOARD
    // initialize badge board
    jm_initialize();
#endif

    // run the main application program loop
    main_run();

    ASSERT(0);  // stop!
    return 0;
}

