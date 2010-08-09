// *** sleep.c ********************************************************
// this file implements the low power sleep mode for stickos.

#include "main.h"

static bool volatile sleep_mode;

// called on sleep entry and exit (sw1 depressed)
static
__declspec(interrupt)
void
sleep_isr(void)
{
    while (! (MCF_EPORT_EPPDR & MCF_EPORT_EPPDR_EPPD1)) {
        // NULL
    }
    delay(100);  // debounce
    while (! (MCF_EPORT_EPPDR & MCF_EPORT_EPPDR_EPPD1)) {
        // NULL
    }
    delay(100);  // debounce

    if (sleep_mode) {
        // we're waking up; program irq1 for falling edge trigger
        MCF_EPORT_EPPAR = MCF_EPORT_EPPAR_EPPA1_FALLING;
    } else {
        // we're going to sleep; program irq1 for level trigger
        MCF_EPORT_EPPAR = MCF_EPORT_EPPAR_EPPA1_LEVEL;
    }

    sleep_mode = ! sleep_mode;
    MCF_EPORT_EPFR = 0x02;
}

static byte ddrnq, ddran, ddras, ddrqs, ddrta, ddrtc, ddrua, ddrub, ddruc;
static byte panpar, paspar, ptapar, ptcpar, puapar, pubpar, pucpar;
static short pnqpar, pqspar;

// this function puts us to sleep if the sleep switch has been pressed
// and wakes us up when it is pressed again.
void
sleep_poll(void)
{
    bool initial;

    initial = true;
    while (sleep_mode) {
        if (initial) {
            // save our data directions and pin assignments
            ddrnq = MCF_GPIO_DDRNQ;
            ddran = MCF_GPIO_DDRAN;
            ddras = MCF_GPIO_DDRAS;
            ddrqs = MCF_GPIO_DDRQS;
            ddrta = MCF_GPIO_DDRTA;
            ddrtc = MCF_GPIO_DDRTC;
            ddrua = MCF_GPIO_DDRUA;
            ddrub = MCF_GPIO_DDRUB;
            ddruc = MCF_GPIO_DDRUC;

            pnqpar = MCF_GPIO_PNQPAR;
            panpar = MCF_GPIO_PANPAR;
            paspar = MCF_GPIO_PASPAR;
            pqspar = MCF_GPIO_PQSPAR;
            ptapar = MCF_GPIO_PTAPAR;
            ptcpar = MCF_GPIO_PTCPAR;
            puapar = MCF_GPIO_PUAPAR;
            pubpar = MCF_GPIO_PUBPAR;
            pucpar = MCF_GPIO_PUCPAR;

            // configure all pins for digital input, except IRQ1*
            MCF_GPIO_DDRNQ = 0x04;
            MCF_GPIO_DDRAN = 0;
            MCF_GPIO_DDRAS = 0;
            MCF_GPIO_DDRQS = 0;
            MCF_GPIO_DDRTA = 0;
            MCF_GPIO_DDRTC = 0;
            MCF_GPIO_DDRUA = 0;
            MCF_GPIO_DDRUB = 0;
            MCF_GPIO_DDRUC = 0;

            MCF_GPIO_PNQPAR = 0x04;
            MCF_GPIO_PANPAR = 0;
            MCF_GPIO_PASPAR = 0;
            MCF_GPIO_PQSPAR = 0;
            MCF_GPIO_PTAPAR = 0;
            MCF_GPIO_PTCPAR = 0;
            MCF_GPIO_PUAPAR = 0;
            MCF_GPIO_PUBPAR = 0;
            MCF_GPIO_PUCPAR = 0;

            initial = false;
        }

        // prepare for stop mode
        MCF_PMM_LPCR = MCF_PMM_LPCR_LPMD_STOP|0x18/*all clocks off*/;

        // allow us to wake from stop mode
        MCF_PMM_LPICR = MCF_PMM_LPICR_ENBSTOP|MCF_PMM_LPICR_XLPM_IPL(0);

        // stop!
        asm {
            stop #0x2000
        };
    }

    if (! initial) {
        // if we need to autoreset...
        if (var_get_flash(FLASH_AUTORESET) == 1) {
            MCF_RCM_RCR = MCF_RCM_RCR_SOFTRST;
            asm { halt }
        }

        // restore our data directions and pin assignments
        MCF_GPIO_DDRNQ = ddrnq;
        MCF_GPIO_DDRAN = ddran;
        MCF_GPIO_DDRAS = ddras;
        MCF_GPIO_DDRQS = ddrqs;
        MCF_GPIO_DDRTA = ddrta;
        MCF_GPIO_DDRTC = ddrtc;
        MCF_GPIO_DDRUA = ddrua;
        MCF_GPIO_DDRUB = ddrub;
        MCF_GPIO_DDRUC = ddruc;

        MCF_GPIO_PNQPAR = pnqpar;
        MCF_GPIO_PANPAR = panpar;
        MCF_GPIO_PASPAR = paspar;
        MCF_GPIO_PQSPAR = pqspar;
        MCF_GPIO_PTAPAR = ptapar;
        MCF_GPIO_PTCPAR = ptcpar;
        MCF_GPIO_PUAPAR = puapar;
        MCF_GPIO_PUBPAR = pubpar;
        MCF_GPIO_PUCPAR = pucpar;

        // reinitialize the adc
        adc_initialize();
    }
}

// this function initializes the sleep module.
void
sleep_initialize(void)
{
    // NQ is gpio output (irq7) and primary (irq1)
    MCF_GPIO_DDRNQ = 0x80;
    MCF_GPIO_PNQPAR = 0x04;

    // we're awake; program irq1 for falling edge trigger
    MCF_EPORT_EPPAR = MCF_EPORT_EPPAR_EPPA1_FALLING;
    MCF_EPORT_EPIER = MCF_EPORT_EPIER_EPIE1;

    // enable irq1 interrupt
    __VECTOR_RAM[65] = (uint32)sleep_isr;
    MCF_INTC0_ICR01 = MCF_INTC_ICR_IL(SPL_IRQ1)|MCF_INTC_ICR_IP(SPL_IRQ1);
    //MCF_INTC0_IMRH &= ~0;
    MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_INT_MASK1;  // irq1
}

