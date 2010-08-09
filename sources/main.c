#include "main.h"

// N.B. be sure to edit m52221demo_sysinit.c:mcf5222x_pll_init() to set
// the proper clock frequency!  MCF_CLOCK_SYNCR_MFD(1) is 48 MHz needed
// by usb.  Also be sure to edit the Linker Entry Point to be
// _startup (rather than start), or your interrupts may not work.

// *** main *****************************************************************

int
main()
{
    // AN 4, 5, 6 are primary; others are gpio output
    MCF_GPIO_PANPAR = 0x70;
    MCF_GPIO_DDRAN = 0x8f;

    // AS is gpio output
    MCF_GPIO_PASPAR = 0;
    MCF_GPIO_DDRAS = 0x3;

    // NQ is gpio output
    MCF_GPIO_PNQPAR = 0;
    MCF_GPIO_DDRNQ = 0xfe;

    // QS is gpio output
    MCF_GPIO_PQSPAR = 0;
    MCF_GPIO_DDRQS = 0x7f;

    // TC is gpio output
    MCF_GPIO_PTCPAR = 0;
    MCF_GPIO_DDRTC = 0xf;

    // UA is gpio output
    MCF_GPIO_SETUA = (uint8)0x08;  // usb power off
    MCF_GPIO_PUAPAR = 0;
    MCF_GPIO_DDRUA = 0xf;

    // UB is gpio output
    MCF_GPIO_PUBPAR = 0;
    MCF_GPIO_DDRUB = 0xf;
    
#if ! RICH
    MCF_GPIO_PORTUB = 0x0c;  // ACG1, ACG2 = 0 (1.5G); ACSL* = 1 (wake)
#endif

    // initialize timer
    timer_initialize();
    
#if ACCEL
    // initialize accelerometer
    accel_initialize();
#endif

#if SCSI || PIMA || CANON
    // initialize usb
    usb_initialize();
#endif

    (void)splx(0);
    initialized = 1;
    
    // everything starts in the timer and usb isrs
    
    for (;;) {
#if SCSI
        // if our usb device is attached...
        if (scsi_attached) {
            scsi_run();
        }
#endif
#if PIMA
        // if our usb device is attached...
        if (pima_attached) {
            pima_run();
        }
#endif
#if CANON
        // if our usb device is attached...
        if (canon_attached) {
            canon_run();
        }
#endif
        
        if (scsi_attached || pima_attached || canon_attached) {
            usb_detach();
        }
    }
}
