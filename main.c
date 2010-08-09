// *** main.c *********************************************************
// this is the main program that is launched by startup.c and init.c;
// it just initializes all of the modules of the os and then runs the
// main application program loop.

#if PIC32
    #pragma config UPLLEN   = ON            // USB PLL Enabled
    #pragma config FPLLMUL  = MUL_20        // PLL Multiplier
    #pragma config UPLLIDIV = DIV_2         // USB PLL Input Divider
    #pragma config FPLLIDIV = DIV_2         // PLL Input Divider
    #pragma config FPLLODIV = DIV_1         // PLL Output Divider
    #pragma config FPBDIV   = DIV_2         // Peripheral Clock divisor
    #pragma config FWDTEN   = OFF           // Watchdog Timer 
    #pragma config WDTPS    = PS1           // Watchdog Timer Postscale
    #pragma config FCKSM    = CSDCMD        // Clock Switching & Fail Safe Clock Monitor
    #pragma config OSCIOFNC = OFF           // CLKO Enable
    #pragma config POSCMOD  = HS            // Primary Oscillator
    #pragma config IESO     = OFF           // Internal/External Switch-over
    #pragma config FSOSCEN  = OFF           // Secondary Oscillator Enable
    #pragma config FNOSC    = PRIPLL        // Oscillator Selection
    #pragma config CP       = OFF           // Code Protect
    #pragma config BWP      = OFF           // Boot Flash Write Protect
    #pragma config PWP      = OFF           // Program Flash Write Protect
    #pragma config ICESEL   = ICS_PGx2      // ICE/ICD Comm Channel Select
    #pragma config DEBUG    = OFF           // Debugger Disabled for Starter Kit
#endif

#include "main.h"

#if PIC32
void _general_exception_context(void)
{
    led_line(_CP0_GET_CAUSE());
}
#endif

int
#if MCF52233
main2()  // the tasking system is called by startup.c, before us
#else
main()  // we're called directly by startup.c
#endif
{
    int x;

#if PIC32
    byte *p;
    extern unsigned char _data_image_begin[];

    SYSTEMConfig(80000000L, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);
    INTEnableSystemMultiVectoredInt();
    (void)splx(7);
    debugger_attached = true;

    cpu_frequency = 80000000;
    oscillator_frequency = 8000000;
    bus_frequency = 40000000;

    end_of_static = _data_image_begin;

    // compute flash checksum
    for (p = (byte *)FLASH_START; p < (byte *)(FLASH_START+FLASH_BYTES/2); p++) {
        flash_checksum += *p;
    }
#endif

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
    adc_timer_poll(false);

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

#if SERIAL_DRIVER
    serial_initialize();
#endif

#if MCF52221 || MCF51JM128 || PIC32
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

