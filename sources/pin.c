// *** pin.c **********************************************************
// this file implements basic I/O pin controls.

#include "main.h"

char *pin_type_names[] = {
    "digital input",
    "digital output",
    "analog input",
    "analog output",
    "uart input",
    "uart output",
    "frequency output"
};

byte pin_qual_mask[] = {
    1<<pin_qual_inverted | 1<<pin_qual_debounced,  // digital input
    1<<pin_qual_inverted | 1<<pin_qual_open_drain,  // digital output
    1<<pin_qual_inverted | 1<<pin_qual_debounced,  // analog input
    1<<pin_qual_inverted,  // analog output
    0,  // uart input
    0,  // uart output
    0  // frequency output
};

// Keep in-sync with pin_qual.  Each element in this array corresponds to a bit in pin_qual.
char *pin_qual_names[] = {
    "debounced",
    "inverted",
    "open_drain"
};

#define DIO  (1<<pin_type_digital_output|1<<pin_type_digital_input)

const struct pin pins[] = {
#if MCF52221 || MCF52233
    "dtin0", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output,
    "dtin1", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output,
    "dtin2", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output,
    "dtin3", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output,
    "qspi_dout", DIO,
    "qspi_din", DIO,
    "qspi_clk", DIO,
    "qspi_cs0", DIO,
    "utxd1", DIO|1<<pin_type_uart_output,
    "urxd1", DIO|1<<pin_type_uart_input,
    "urts1*", DIO,
    "ucts1*", DIO,
    "utxd0", DIO|1<<pin_type_uart_output,
    "urxd0", DIO|1<<pin_type_uart_input,
    "urts0*", DIO,
    "ucts0*", DIO,
    "an0", DIO|1<<pin_type_analog_input,
    "an1", DIO|1<<pin_type_analog_input,
    "an2", DIO|1<<pin_type_analog_input,
    "an3", DIO|1<<pin_type_analog_input,
    "an4", DIO|1<<pin_type_analog_input,
    "an5", DIO|1<<pin_type_analog_input,
    "an6", DIO|1<<pin_type_analog_input,
    "an7", DIO|1<<pin_type_analog_input,
    "irq0*", 0,
    "irq1*", DIO,
    "irq2*", 0,
    "irq3*", 0,
    "irq4*", DIO,
    "irq5*", 0,
    "irq6*", 0,
    "irq7*", DIO,
#if MCF52233
    "gpt0", DIO,
    "gpt1", DIO,
    "gpt2", DIO,
    "irq11*", DIO,
#endif
    "scl", DIO,
    "sda", DIO,
#elif MCF51JM128
    "pta0", DIO,
    "pta1", DIO,
    "pta2", DIO,
    "pta3", DIO,
    "pta4", DIO,
    "pta5", DIO,
    "ptb0", DIO|1<<pin_type_analog_input,
    "ptb1", DIO|1<<pin_type_analog_input,
    "ptb2", DIO|1<<pin_type_analog_input,
    "ptb3", DIO|1<<pin_type_analog_input,
    "ptb4", DIO|1<<pin_type_analog_input,
    "ptb5", DIO|1<<pin_type_analog_input,
    "ptb6", DIO|1<<pin_type_analog_input,
    "ptb7", DIO|1<<pin_type_analog_input,
    "ptc0", DIO,
    "ptc1", DIO,
    "ptc2", DIO,
    "ptc3", DIO|1<<pin_type_uart_output,
    "ptc4", DIO,
    "ptc5", DIO|1<<pin_type_uart_input,
    "ptc6", DIO,
    "ptd0", DIO|1<<pin_type_analog_input,
    "ptd1", DIO|1<<pin_type_analog_input,
    "ptd2", DIO,
    "ptd3", DIO|1<<pin_type_analog_input,
    "ptd4", DIO|1<<pin_type_analog_input,
    "ptd5", DIO,
    "ptd6", DIO,
    "ptd7", DIO,
    "pte0", DIO|1<<pin_type_uart_output,
    "pte1", DIO|1<<pin_type_uart_input,
    "pte2", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output,
    "pte3", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output,
    "pte4", DIO,
    "pte5", DIO,
    "pte6", DIO,
    "pte7", DIO,
    "ptf0", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output,
    "ptf1", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output,
    "ptf2", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output,
    "ptf3", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output,
    "ptf4", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output,
    "ptf5", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output,
    "ptf6", DIO,
    "ptf7", DIO,
    "ptg0", DIO,
    "ptg1", DIO,
    "ptg2", DIO,
    "ptg3", DIO,
#elif PIC32
    "ra0", DIO,
    "ra1", DIO,
    "ra2", DIO,
    "ra3", DIO,
    "ra4", DIO,
    "ra5", DIO,
    "ra6", DIO,
    "ra7", DIO,
    "ra8", 0,
    "ra9", 0,
    "ra10", 0,
    "ra11", 0,
    "ra12", 0,
    "ra13", 0,
    "ra14", DIO,
    "ra15", DIO,
    "an0", DIO|1<<pin_type_analog_input,  // rb0...
    "an1", DIO|1<<pin_type_analog_input,
    "an2", DIO|1<<pin_type_analog_input,
    "an3", DIO|1<<pin_type_analog_input,
    "an4", DIO|1<<pin_type_analog_input,
    "an5", DIO|1<<pin_type_analog_input,
    "an6", DIO|1<<pin_type_analog_input,
    "an7", DIO|1<<pin_type_analog_input,
    "an8", DIO|1<<pin_type_analog_input,  // U2CTS
    "an9", DIO|1<<pin_type_analog_input,
    "an10", DIO|1<<pin_type_analog_input,
    "an11", DIO|1<<pin_type_analog_input,
    "an12", DIO|1<<pin_type_analog_input,
    "an13", DIO|1<<pin_type_analog_input,
    "an14", DIO|1<<pin_type_analog_input,  // U2RTS
    "an15", DIO|1<<pin_type_analog_input,
    "rc0", 0,
    "rc1", DIO,  // rc1...
    "rc2", DIO,
    "rc3", DIO,
    "rc4", DIO,
    "rc5", 0,
    "rc6", 0,
    "rc7", 0,
    "rc8", 0,
    "rc9", 0,
    "rc10", 0,
    "rc11", 0,
    "rc12", 0,
    "rc13", DIO,  // rc13...
    "rc14", DIO,
    "rd0", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output,  // oc1
    "rd1", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output,  // oc2
    "rd2", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output,  // oc3
    "rd3", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output,  // oc4
    "rd4", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output,  // oc5
    "rd5", DIO,
    "rd6", DIO,
    "rd7", DIO,
    "rd8", DIO,
    "rd9", DIO,
    "rd10", DIO,
    "rd11", DIO,
    "rd12", DIO,
    "rd13", DIO,
    "rd14", DIO,
    "rd15", DIO,
    "re0", DIO,
    "re1", DIO,
    "re2", DIO,
    "re3", DIO,
    "re4", DIO,
    "re5", DIO,
    "re6", DIO,
    "re7", DIO,
    "re8", DIO,
    "re9", DIO,
    "rf0", DIO,
    "rf1", DIO,
    "rf2", DIO|1<<pin_type_uart_input,  // u1rx
    "rf3", DIO,
    "rf4", DIO|1<<pin_type_uart_input,  // u2rx
    "rf5", DIO|1<<pin_type_uart_output,  // u2tx
    "rf6", 0,
    "rf7", 0,
    "rf8", DIO|1<<pin_type_uart_output,  // u1tx
    "rf9", 0,
    "rf10", 0,
    "rf11", 0,
    "rf12", DIO,
    "rf13", DIO,
    "rg0", DIO,
    "rg1", DIO,
    "rg2", 0,
    "rg3", 0,
    "rg4", 0,
    "rg5", 0,
    "rg6", DIO,
    "rg7", DIO,
    "rg8", DIO,
    "rg9", DIO,
    "rg10", 0,
    "rg11", 0,
    "rg12", DIO,
    "rg13", DIO,
    "rg14", DIO,
#else
#error
#endif
};

bool uart_armed[UART_INTS];

const char *uart_names[MAX_UARTS] = {
#if MCF52221 || MCF52233
    "0",
    "1"
#else
    "1",
    "2",
#endif
};

#if MCF51JM128 || PIC32
static bool freq[2];  // 1 -> timer used for freq gen; 0 -> timer used for pwm; -1 -> timer avail
#endif

#if MCF51JM128
#define FREQ_PRESCALE  16
#elif PIC32
#define FREQ_PRESCALE  64
#endif

#if ! STICK_GUEST

// Debounce history for digital inputs.

enum {
    // Tunable depth of digital pin history saved for debouncing.  Currently, the history is used to elect a majority.
    pin_digital_debounce_history_depth = 3
};

enum debounce_ports {
#if MCF52221 || MCF52233
    port_tc,
    port_qs,
    port_ub,
    port_ua,
    port_an,
    port_nq,
    port_as,
#if MCF52233
    port_gp,
    port_ta,
#endif
#elif MCF51JM128
    port_a,
    port_b,
    port_c,
    port_d,
    port_e,
    port_f,
    port_g,
#elif PIC32
    port_a,
    port_b,
    port_c,
    port_d,
    port_e,
    port_f,
    port_g,
#else
#error
#endif
    port_max
};

// This structure records recent samples from digital pins.
#if MCF52221 || MCF52233 || MCF51JM128
static uint8 pin_digital_debounce[pin_digital_debounce_history_depth][port_max];
#elif PIC32
static uint16 pin_digital_debounce[pin_digital_debounce_history_depth][port_max];
#else
#error
#endif

static int pin_digital_debounce_cycle; // indexes into pin_digital_debounce_data.

// compute majority value of the pin's recently polled values.
//
// revisit -- find a way to rework and rename this to
// pin_get_digital() and have it returned debounced or non-debounced
// data based on pin_qual.  Also introduce a pin_set_digital() to
// consolidate setting of pins.
static int
pin_get_digital_debounced(int port_offset, int pin_offset)
{
    int i;
    int value;
    
    assert(pin_offset < sizeof(pin_digital_debounce[0][0]) * 8);

    value = 0;
    for (i = 0; i < pin_digital_debounce_history_depth; i++) {
        value += !!(pin_digital_debounce[i][port_offset] & (1 << pin_offset));
    }
    return value > pin_digital_debounce_history_depth/2;
}

#endif // ! STICK_GUEST

static void
pin_declare_internal(IN int pin_number, IN int pin_type, IN int pin_qual, IN bool set)
{
#if ! STICK_GUEST
#if MCF52221 || MCF52233
    int assign;
#endif
#if MCF51JM128
    int n;
    int adc;
#endif
    int offset;

    if (! set && (pin_type == pin_type_digital_output) && (pin_qual & 1<<pin_qual_open_drain)) {
        // on initial declaration, configure open_drain outputs as inputs
        // N.B. this will be reconfigured as an output on pin_set to 0
        pin_type = pin_type_digital_input;
    }

#if MCF52221 || MCF52233
    // configure the MCF52221/MCF52233 pin for the requested function
    switch (pin_number) {
        case PIN_DTIN0:
        case PIN_DTIN1:
        case PIN_DTIN2:
        case PIN_DTIN3:
            offset = pin_number - PIN_DTIN0;
            if (pin_type == pin_type_digital_output || pin_type == pin_type_digital_input) {
                assign = 0;
            } else if (pin_type == pin_type_analog_output) {
                assign = 3;
            } else {
                assert(pin_type == pin_type_frequency_output);
                assign = 2;
            }
            MCF_GPIO_PTCPAR = (MCF_GPIO_PTCPAR &~ (3<<(offset*2))) | (assign<<(offset*2));
            if (pin_type == pin_type_digital_output) {
                MCF_GPIO_DDRTC |= 1 << offset;
            } else if (pin_type == pin_type_digital_input) {
                MCF_GPIO_DDRTC &= ~(1 << offset);
            }
            break;
        case PIN_QSPI_DOUT:
        case PIN_QSPI_DIN:
        case PIN_QSPI_CLK:
        case PIN_QSPI_CS0:
            offset = pin_number - PIN_QSPI_DOUT;
            MCF_GPIO_PQSPAR = 0;
            if (pin_type == pin_type_digital_output) {
                MCF_GPIO_DDRQS |= 1 << offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                MCF_GPIO_DDRQS &= ~(1 << offset);
            }
            break;
        case PIN_UTXD1:
        case PIN_URXD1:
        case PIN_RTS1:
        case PIN_CTS1:
            offset = pin_number - PIN_UTXD1;
            if (pin_type == pin_type_uart_input || pin_type == pin_type_uart_output) {
                assert(pin_number == PIN_URXD1 || pin_number == PIN_UTXD1);
                MCF_GPIO_PUBPAR = (MCF_GPIO_PUBPAR &~ (3 << (offset*2))) | (1 << (offset*2));
            } else {
                MCF_GPIO_PUBPAR &= ~(3 << (offset*2));
                if (pin_type == pin_type_digital_output) {
                    MCF_GPIO_DDRUB |= 1 << offset;
                } else {
                    assert(pin_type == pin_type_digital_input);
                    MCF_GPIO_DDRUB &= ~(1 << offset);
                }
            }
            break;
        case PIN_UTXD0:
        case PIN_URXD0:
        case PIN_RTS0:
        case PIN_CTS0:
            offset = pin_number - PIN_UTXD0;
            if (pin_type == pin_type_uart_input || pin_type == pin_type_uart_output) {
                assert(pin_number == PIN_URXD0 || pin_number == PIN_UTXD0);
                MCF_GPIO_PUAPAR = (MCF_GPIO_PUAPAR &~ (3 << (offset*2))) | (1 << (offset*2));
            } else {
                MCF_GPIO_PUAPAR &= ~(3 << (offset*2));
                if (pin_type == pin_type_digital_output) {
                    MCF_GPIO_DDRUA |= 1 << offset;
                } else {
                    MCF_GPIO_DDRUA &= ~(1 << offset);
                    assert(pin_type == pin_type_digital_input);
                }
            }
            break;
        case PIN_AN0:
        case PIN_AN1:
        case PIN_AN2:
        case PIN_AN3:
        case PIN_AN4:
        case PIN_AN5:
        case PIN_AN6:
        case PIN_AN7:
            offset = pin_number - PIN_AN0;
            if (pin_type == pin_type_analog_input) {
                MCF_GPIO_PANPAR |= 1 << offset;
            } else {
                MCF_GPIO_PANPAR &= ~(1 << offset);
                if (pin_type == pin_type_digital_output) {
                    MCF_GPIO_DDRAN |= 1 << offset;
                } else {
                    MCF_GPIO_DDRAN &= ~(1 << offset);
                    assert(pin_type == pin_type_digital_input);
                }
            }
            break;
        case PIN_IRQ1:
        case PIN_IRQ4:
        case PIN_IRQ7:
            offset = pin_number - PIN_IRQ0;
            if (offset == 1) {
                irq1_enable = false;
                MCF_GPIO_PNQPAR = (MCF_GPIO_PNQPAR &~ (3<<(1*2))) | (0<<(1*2));  // irq1 is gpio
            } else if (offset == 4) {
                irq4_enable = false;
                MCF_GPIO_PNQPAR = (MCF_GPIO_PNQPAR &~ (3<<(4*2))) | (0<<(4*2));  // irq4 is gpio
            }
            if (pin_type == pin_type_digital_output) {
                MCF_GPIO_DDRNQ |= 1 << offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                MCF_GPIO_DDRNQ &= ~(1 << offset);
            }
            break;
#if MCF52233
        case PIN_IRQ11:
            offset = pin_number - PIN_GPT0;
            MCF_GPIO_PGPPAR = 0;
            if (pin_type == pin_type_digital_output) {
                MCF_GPIO_DDRGP |= 1 << offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                MCF_GPIO_DDRGP &= ~(1 << offset);
            }
            break;
        case PIN_GPT0:
        case PIN_GPT1:
        case PIN_GPT2:
            offset = pin_number - PIN_GPT0;
            MCF_GPIO_PTAPAR = 0;
            if (pin_type == pin_type_digital_output) {
                MCF_GPIO_DDRTA |= 1 << offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                MCF_GPIO_DDRTA &= ~(1 << offset);
            }
            break;
#endif
        case PIN_SCL:
        case PIN_SDA:
            offset = pin_number - PIN_SCL;
            MCF_GPIO_PASPAR = 0;
            if (pin_type == pin_type_digital_output) {
                MCF_GPIO_DDRAS |= 1 << offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                MCF_GPIO_DDRAS &= ~(1 << offset);
            }
            break;
        default:
            assert(0);
            break;
    }
#elif MCF51JM128
    // configure the MCF51JM128 pin for the requested function
    switch (pin_number) {
        case PIN_PTA0:
        case PIN_PTA1:
        case PIN_PTA2:
        case PIN_PTA3:
        case PIN_PTA4:
        case PIN_PTA5:
            offset = pin_number - PIN_PTA0;
            if (pin_type == pin_type_digital_output) {
                PTAPE &= ~(1<<offset);
                PTADD |= 1<<offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                PTAPE |= 1<<offset;
                PTADD &= ~(1<<offset);
            }
            break;
        case PIN_PTB0:
        case PIN_PTB1:
        case PIN_PTB2:
        case PIN_PTB3:
        case PIN_PTB4:
        case PIN_PTB5:
        case PIN_PTB6:
        case PIN_PTB7:
            offset = pin_number - PIN_PTB0;
            adc = offset;
            if (pin_type == pin_type_analog_input) {
                assert(adc >= 0 && adc <= 7);
                APCTL1 |= 1<<adc;
            } else {
                APCTL1 &= ~(1<<adc);
                if (pin_type == pin_type_digital_output) {
                    PTBPE &= ~(1<<offset);
                    PTBDD |= 1<<offset;
                } else {
                    assert(pin_type == pin_type_digital_input);
                    PTBPE |= 1<<offset;
                    PTBDD &= ~(1<<offset);
                }
            }
            break;
        case PIN_PTC0:
        case PIN_PTC1:
        case PIN_PTC2:
        case PIN_PTC3:
        case PIN_PTC4:
        case PIN_PTC5:
        case PIN_PTC6:
            offset = pin_number - PIN_PTC0;
            if (pin_type == pin_type_uart_input) {
                assert(offset == 5);
                SCI2C2 |= SCI2C2_RE_MASK;
            } else if (pin_type == pin_type_uart_output) {
                assert(offset == 3);
                SCI2C2 |= SCI2C2_TE_MASK;
            } else {
                if (offset == 5) {
                    SCI2C2 &= ~SCI2C2_RE_MASK;
                } else if (offset == 3) {
                    SCI2C2 &= ~SCI2C2_TE_MASK;
                }
                if (pin_type == pin_type_digital_output) {
                    PTCPE &= ~(1<<offset);
                    PTCDD |= 1<<offset;
                } else {
                    assert(pin_type == pin_type_digital_input);
                    PTCPE |= 1<<offset;
                    PTCDD &= ~(1<<offset);
                }
            }
            break;
        case PIN_PTD0:
        case PIN_PTD1:
        case PIN_PTD2:
        case PIN_PTD3:
        case PIN_PTD4:
        case PIN_PTD5:
        case PIN_PTD6:
        case PIN_PTD7:
            offset = pin_number - PIN_PTD0;
            adc = 8+offset-(offset>2);
            if (pin_type == pin_type_analog_input) {
                assert(adc >= 8 && adc <= 12);
                APCTL2 |= 1<<(adc-8);
            } else {
                if (adc >= 8 && adc <= 12) {
                    APCTL2 &= ~(1<<(adc-8));
                }
                if (pin_type == pin_type_digital_output) {
                    PTDPE &= ~(1<<offset);
                    PTDDD |= 1<<offset;
                } else {
                    assert(pin_type == pin_type_digital_input);
                    PTDPE |= 1<<offset;
                    PTDDD &= ~(1<<offset);
                }
            }
            break;
        case PIN_PTE0:
        case PIN_PTE1:
        case PIN_PTE2:
        case PIN_PTE3:
        case PIN_PTE4:
        case PIN_PTE5:
        case PIN_PTE6:
        case PIN_PTE7:
            offset = pin_number - PIN_PTE0;
            if (pin_type == pin_type_uart_input) {
                assert(offset == 1);
                SCI1C2 |= SCI1C2_RE_MASK;
            } else if (pin_type == pin_type_uart_output) {
                assert(offset == 0);
                SCI1C2 |= SCI1C2_TE_MASK;
            } else if (pin_type == pin_type_analog_output || pin_type == pin_type_frequency_output) {
                assert(offset >= 2 && offset < 4);
                n = 0;
                if (freq[n] != (byte)-1 && (pin_type == pin_type_frequency_output || freq[n] != 0)) {
                    printf("conflicting timer usage\n");
#if STICKOS
                    stop();
#endif
                } else {
                    if (pin_type == pin_type_frequency_output) {
                        assert(freq[n] == (byte)-1);

                        // set prescales to 16
                        if (n) {
                            TPM2SC = TPM2SC_CLKSA_MASK|TPM2SC_PS2_MASK;
                        } else {
                            TPM1SC = TPM1SC_CLKSA_MASK|TPM1SC_PS2_MASK;
                        }
                        assert(FREQ_PRESCALE == 16);
                        
                        // program our channel's frequency mode
                        if (offset == 2) {
                            TPM1C0SC = TPM1C0SC_MS0A_MASK;
                        } else {
                            TPM1C1SC = TPM1C1SC_MS1A_MASK;
                        }
                    } else {
                        if (freq[n] == (byte)-1) {
                            // set prescales to 1
                            TPM1SC = TPM1SC_CLKSA_MASK;
                            
                            // program the counter for pwm generation (shared counter)
                            TPM1MODH = 3300>>8;
                            TPM1MODL = 3300&0xff;
                            TPM1CNTL = 0;
                        }
                        
                        // program our channel's pwm mode
                        if (offset == 2) {
                            TPM1C0SC = TPM1C0SC_MS0B_MASK|TPM1C0SC_ELS0B_MASK;
                        } else {
                            TPM1C1SC = TPM1C1SC_MS1B_MASK|TPM1C1SC_ELS1B_MASK;
                        }
                    }
                    
                    freq[n] = (pin_type == pin_type_frequency_output);
                }
            } else {
                // program our pin's digital mode
                if (offset == 1) {
                    SCI1C2 &= ~SCI1C2_RE_MASK;
                } else if (offset == 0) {
                    SCI1C2 &= ~SCI1C2_TE_MASK;
                } else if (offset == 2) {
                    TPM1C0SC = 0;
                } else if (offset == 3) {
                    TPM1C1SC = 0;
                }
                
                if (pin_type == pin_type_digital_output) {
                    PTEPE &= ~(1<<offset);
                    PTEDD |= 1<<offset;
                } else {
                    assert(pin_type == pin_type_digital_input);
                    PTEPE |= 1<<offset;
                    PTEDD &= ~(1<<offset);
                }
            }
            break;
        case PIN_PTF0:
        case PIN_PTF1:
        case PIN_PTF2:
        case PIN_PTF3:
        case PIN_PTF4:
        case PIN_PTF5:
        case PIN_PTF6:
        case PIN_PTF7:
            offset = pin_number - PIN_PTF0;
            if (pin_type == pin_type_analog_output || pin_type == pin_type_frequency_output) {
                assert(offset >= 0 && offset < 6);
                n = offset > 3;
                if (freq[n] != (byte)-1 && (pin_type == pin_type_frequency_output || freq[n] != 0)) {
                    printf("conflicting timer usage\n");
#if STICKOS
                    stop();
#endif
                } else {
                    if (pin_type == pin_type_frequency_output) {
                        assert(freq[n] == (byte)-1);

                        // set prescales to 16
                        if (n) {
                            TPM2SC = TPM2SC_CLKSA_MASK|TPM2SC_PS2_MASK;
                        } else {
                            TPM1SC = TPM1SC_CLKSA_MASK|TPM1SC_PS2_MASK;
                        }
                        assert(FREQ_PRESCALE == 16);
                        
                        // program our channel's frequency mode
                        if (offset == 0) {
                            TPM1C2SC = TPM1C2SC_MS2A_MASK;
                        } else if (offset == 1) {
                            TPM1C3SC = TPM1C3SC_MS3A_MASK;
                        } else if (offset == 2) {
                            TPM1C4SC = TPM1C4SC_MS4A_MASK;
                        } else if (offset == 3) {
                            TPM1C5SC = TPM1C5SC_MS5A_MASK;
                        } else if (offset == 4) {
                            TPM2C0SC = TPM2C0SC_MS0A_MASK;
                        } else {
                            TPM2C1SC = TPM2C1SC_MS1A_MASK;
                        }
                    } else {
                        if (freq[n] == (byte)-1) {
                            // set prescales to 1
                            if (n) {
                                TPM2SC = TPM1SC_CLKSA_MASK;
                            } else {
                                TPM1SC = TPM1SC_CLKSA_MASK;
                            }
                            
                            // program the counter for pwm generation (shared counter)
                            if (n) {
                                TPM2MODH = 3300>>8;
                                TPM2MODL = 3300&0xff;
                                TPM2CNTL = 0;
                            } else {
                                TPM1MODH = 3300>>8;
                                TPM1MODL = 3300&0xff;
                                TPM1CNTL = 0;
                            }
                        }
                        
                        // program our channel's pwm mode
                        if (offset == 0) {
                            TPM1C2SC = TPM1C2SC_MS2B_MASK|TPM1C2SC_ELS2B_MASK;
                        } else if (offset == 1) {
                            TPM1C3SC = TPM1C3SC_MS3B_MASK|TPM1C3SC_ELS3B_MASK;
                        } else if (offset == 2) {
                            TPM1C4SC = TPM1C4SC_MS4B_MASK|TPM1C4SC_ELS4B_MASK;
                        } else if (offset == 3) {
                            TPM1C5SC = TPM1C5SC_MS5B_MASK|TPM1C5SC_ELS5B_MASK;
                        } else if (offset == 4) {
                            TPM2C0SC = TPM2C0SC_MS0B_MASK|TPM2C0SC_ELS0B_MASK;
                        } else {
                            TPM2C1SC = TPM2C1SC_MS1B_MASK|TPM2C1SC_ELS1B_MASK;
                        }
                    }
                    
                    freq[n] = (pin_type == pin_type_frequency_output);
                }
            } else {
                // program our pin's digital mode
                if (offset == 0) {
                    TPM1C2SC = 0;
                } else if (offset == 1) {
                    TPM1C3SC = 0;
                } else if (offset == 2) {
                    TPM1C4SC = 0;
                } else if (offset == 3) {
                    TPM1C5SC = 0;
                } else if (offset == 4) {
                    TPM2C0SC = 0;
                } else {
                    TPM2C1SC = 0;
                }
                
                if (pin_type == pin_type_digital_output) {
                    PTFPE &= ~(1<<offset);
                    PTFDD |= 1<<offset;
                } else {
                    assert(pin_type == pin_type_digital_input);
                    PTFPE |= 1<<offset;
                    PTFDD &= ~(1<<offset);
                }
            }
            break;
        case PIN_PTG0:
        case PIN_PTG1:
        case PIN_PTG2:
        case PIN_PTG3:
            offset = pin_number - PIN_PTG0;
            if (pin_type == pin_type_digital_output) {
                PTGPE &= ~(1<<offset);
                PTGDD |= 1<<offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                PTGPE |= 1<<offset;
                PTGDD &= ~(1<<offset);
            }
            break;
        default:
            assert(0);
            break;
    }
#elif PIC32
    // configure the PIC32 pin for the requested function
    switch (pin_number) {
        case PIN_RA0:
        case PIN_RA1:
        case PIN_RA2:
        case PIN_RA3:
        case PIN_RA4:
        case PIN_RA5:
        case PIN_RA6:
        case PIN_RA7:
        case PIN_RA14:
        case PIN_RA15:
            offset = pin_number - PIN_RA0;
            assert((unsigned)offset < 16);
            if (pin_type == pin_type_digital_output) {
                TRISACLR = 1<<offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                TRISASET = 1<<offset;
            }
            break;
        case PIN_AN0:
        case PIN_AN1:
        case PIN_AN2:
        case PIN_AN3:
        case PIN_AN4:
        case PIN_AN5:
        case PIN_AN6:
        case PIN_AN7:
        case PIN_AN8:
        case PIN_AN9:
        case PIN_AN10:
        case PIN_AN11:
        case PIN_AN12:
        case PIN_AN13:
        case PIN_AN14:
        case PIN_AN15:
            offset = pin_number - PIN_AN0;
            assert((unsigned)offset < 16);
            if (pin_type == pin_type_analog_input) {
                AD1PCFGCLR = 1<<offset;
                TRISBSET = 1<<offset;
            } else {
                AD1PCFGSET = 1<<offset;
                if (pin_type == pin_type_digital_output) {
                    TRISBCLR = 1<<offset;
                } else {
                    assert(pin_type == pin_type_digital_input);
                    TRISBSET = 1<<offset;
                }
            }
            break;
        case PIN_RC1:
        case PIN_RC2:
        case PIN_RC3:
        case PIN_RC4:
        case PIN_RC13:
        case PIN_RC14:
            offset = pin_number - PIN_RC0;
            assert((unsigned)offset < 16);
            if (pin_type == pin_type_digital_output) {
                TRISCCLR = 1<<offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                TRISCSET = 1<<offset;
            }
            break;
        case PIN_RD0:
        case PIN_RD1:
        case PIN_RD2:
        case PIN_RD3:
        case PIN_RD4:
        case PIN_RD5:
        case PIN_RD6:
        case PIN_RD7:
        case PIN_RD8:
        case PIN_RD9:
        case PIN_RD10:
        case PIN_RD11:
        case PIN_RD12:
        case PIN_RD13:
        case PIN_RD14:
        case PIN_RD15:
            offset = pin_number - PIN_RD0;
            assert((unsigned)offset < 16);
            if (pin_type == pin_type_analog_output) {
                // NULL
            } else if (pin_type == pin_type_frequency_output) {
                if (freq[1] == (byte)-1) {
                    freq[1] = 1;
                    // NULL
                } else {
                    printf("conflicting timer usage\n");
#if STICKOS
                    stop();
#endif
                }
            } else {
                if (offset == 0) {
                    OC1CONCLR = _OC1CON_ON_MASK;
                } else if (offset == 1) {
                    OC2CONCLR = _OC2CON_ON_MASK;
                } else if (offset == 2) {
                    OC3CONCLR = _OC3CON_ON_MASK;
                } else if (offset == 3) {
                   OC4CONCLR = _OC4CON_ON_MASK;
                } else if (offset == 4) {
                    OC5CONCLR = _OC5CON_ON_MASK;
                }

                if (pin_type == pin_type_digital_output) {
                    TRISDCLR = 1<<offset;
                } else {
                    assert(pin_type == pin_type_digital_input);
                    TRISDSET = 1<<offset;
                }
            }        
            break;
        case PIN_RE0:
        case PIN_RE1:
        case PIN_RE2:
        case PIN_RE3:
        case PIN_RE4:
        case PIN_RE5:
        case PIN_RE6:
        case PIN_RE7:
        case PIN_RE8:
        case PIN_RE9:
            offset = pin_number - PIN_RE0;
            assert((unsigned)offset < 16);
            if (pin_type == pin_type_digital_output) {
                TRISECLR = 1<<offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                TRISESET = 1<<offset;
            }
            break;
        case PIN_RF0:
        case PIN_RF1:
        case PIN_RF2:
        case PIN_RF3:
        case PIN_RF4:
        case PIN_RF5:
        case PIN_RF8:
        case PIN_RF12:
        case PIN_RF13:
            offset = pin_number - PIN_RF0;
            assert((unsigned)offset < 16);
            if (pin_type == pin_type_uart_input || pin_type == pin_type_uart_output) {
                if (offset == 2 || offset == 8) {
                    U1MODE |= _U1MODE_UARTEN_MASK;
                } else {
                    assert(offset == 4 || offset == 5);
                    U2MODE |= _U2MODE_UARTEN_MASK;
                }
            } else {
                if (offset == 2 || offset == 8) {
                    U1MODE &= ~_U1MODE_UARTEN_MASK;
                } else if (offset == 4 || offset == 5) {
                    U2MODE &= ~_U2MODE_UARTEN_MASK;
                }

                if (pin_type == pin_type_digital_output) {
                    TRISFCLR = 1<<offset;
                } else {
                    assert(pin_type == pin_type_digital_input);
                    TRISFSET = 1<<offset;
                }
            }
            break;
        case PIN_RG0:
        case PIN_RG1:
        case PIN_RG6:
        case PIN_RG7:
        case PIN_RG8:
        case PIN_RG9:
        case PIN_RG12:
        case PIN_RG13:
        case PIN_RG14:
            offset = pin_number - PIN_RG0;
            assert((unsigned)offset < 16);
            if (pin_type == pin_type_digital_output) {
                TRISGCLR = 1<<offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                TRISGSET = 1<<offset;
            }
            break;
        default:
            assert(0);
            break;
    }
#else
#error
#endif
#endif
}

// this function declares a ram, flash, or pin variable!
void
pin_declare(IN int pin_number, IN int pin_type, IN int pin_qual)
{
    pin_declare_internal(pin_number, pin_type, pin_qual, false);
}

static byte lasttx0;
static byte lasttx1;
static byte mask0 = 0xff;
static byte mask1 = 0xff;

// this function sets a pin variable!
void
pin_set(IN int pin_number, IN int pin_type, IN int pin_qual, IN int value)
{
#if ! STICK_GUEST
    int offset;
    int value2;
    
    if (pin_type == pin_type_analog_output) {
        // trim the analog level
        if (value < 0) {
            value = 0;
        } else if (value > 3300) {
            value = 3300;
        }
    } else if (pin_type == pin_type_frequency_output) {
        // trim the frequency
#if MCF52221 || MCF52233
        if (value < 0) {
            value = 0;
        }
        if (value) {
            value = bus_frequency/value;
            if (value) {
                value--;
            }
        }
        if (! value) {
            value = 0xffffffff;
        }
#else
        if (value < 0) {
            value = 0;
        }
        if (value) {
            value = bus_frequency/FREQ_PRESCALE/value/2;
            if (value) {
                value--;
            }
        }
        if (value > 0xffff) {
            value = 0xffff;
        }
#endif
    }
    
    if (pin_qual & 1<<pin_qual_inverted) {
        if (pin_type == pin_type_digital_output) {
            value2 = value;
            value = ! value;
            ASSERT(value != value2);  // catch CW bug
        } else if (pin_type == pin_type_analog_output) {
            value2 = value;
            value = 3300-value;
            ASSERT(value+value2 == 3300);  // catch CW bug
        }
    }

    // If setting to 1, then disable the driver before setting the data value to 1.  This avoids having the processor drive the
    // open_drain pin, which would be bad if the line is held high by another driver.
    if (value && (pin_qual & 1<<pin_qual_open_drain)) {
        assert(pin_type == pin_type_digital_output);
        pin_declare_internal(pin_number, pin_type_digital_input, pin_qual, true);
    }
    
#if MCF52221 || MCF52233
    // set the MCF52221/MCF52233 pin to value
    switch (pin_number) {
        case PIN_DTIN0:
        case PIN_DTIN1:
        case PIN_DTIN2:
        case PIN_DTIN3:
            offset = pin_number - PIN_DTIN0;
            if (pin_type == pin_type_analog_output) {
                // program MCF_PWM_PWMDTY with values 0 (3.3v) to 255 (0v)
                MCF_PWM_PWMDTY(offset*2) = 255 - value*255/3300;
            } else if (pin_type == pin_type_frequency_output) {
                // program MCF_DTIM_DTRR with bus_frequency (1 Hz) to 1 (bus_frequency Hz)
                MCF_DTIM_DTRR(offset) = value;
                
                // catch missed wraps
                if (MCF_DTIM_DTCN(offset) >= MCF_DTIM_DTRR(offset)) {
                    MCF_DTIM_DTCN(offset) = 0;
                }
            } else {
                if (value) {
                    MCF_GPIO_SETTC = 1 << offset;
                } else {
                    MCF_GPIO_CLRTC = ~(1 << offset);
                }
            }
            break;
        case PIN_QSPI_DOUT:
        case PIN_QSPI_DIN:
        case PIN_QSPI_CLK:
        case PIN_QSPI_CS0:
            offset = pin_number - PIN_QSPI_DOUT;
            if (value) {
                MCF_GPIO_SETQS = 1 << offset;
            } else {
                MCF_GPIO_CLRQS = ~(1 << offset);
            }
            break;
        case PIN_UTXD1:
        case PIN_URXD1:
        case PIN_RTS1:
        case PIN_CTS1:
            offset = pin_number - PIN_UTXD1;
            if (pin_type == pin_type_uart_output) {
                assert(pin_number == PIN_UTXD1);
                while (! (MCF_UART_USR(1) & MCF_UART_USR_TXRDY)) {
                    // revisit -- poll?
                }
                MCF_UART_UTB(1) = value;
                lasttx1 = value;
                uart_armed[UART_INT(1, true)] = true;
            } else {
                if (value) {
                    MCF_GPIO_SETUB = 1 << offset;
                } else {
                    MCF_GPIO_CLRUB = ~(1 << offset);
                }
            }
            break;
        case PIN_UTXD0:
        case PIN_URXD0:
        case PIN_RTS0:
        case PIN_CTS0:
            offset = pin_number - PIN_UTXD0;
            if (pin_type == pin_type_uart_output) {
                assert(pin_number == PIN_UTXD0);
                while (! (MCF_UART_USR(0) & MCF_UART_USR_TXRDY)) {
                    // revisit -- poll?
                }
                MCF_UART_UTB(0) = value;
                lasttx0 = value;
                uart_armed[UART_INT(0, true)] = true;
            } else {
                if (value) {
                    MCF_GPIO_SETUA = 1 << offset;
                } else {
                    MCF_GPIO_CLRUA = ~(1 << offset);
                }
            }
            break;
        case PIN_AN0:
        case PIN_AN1:
        case PIN_AN2:
        case PIN_AN3:
        case PIN_AN4:
        case PIN_AN5:
        case PIN_AN6:
        case PIN_AN7:
            offset = pin_number - PIN_AN0;
            if (value) {
                MCF_GPIO_SETAN = 1 << offset;
            } else {
                MCF_GPIO_CLRAN = ~(1 << offset);
            }
            break;
        case PIN_IRQ1:
        case PIN_IRQ4:
        case PIN_IRQ7:
            offset = pin_number - PIN_IRQ0;
            if (value) {
                MCF_GPIO_SETNQ = 1 << offset;
            } else {
                MCF_GPIO_CLRNQ = ~(1 << offset);
            }
            break;
#if MCF52233
        case PIN_IRQ11:
            offset = pin_number - PIN_GPT0;
            if (value) {
                MCF_GPIO_SETGP = 1 << offset;
            } else {
                MCF_GPIO_CLRGP = ~(1 << offset);
            }
            break;
        case PIN_GPT0:
        case PIN_GPT1:
        case PIN_GPT2:
            offset = pin_number - PIN_GPT0;
            if (value) {
                MCF_GPIO_SETTA = 1 << offset;
            } else {
                MCF_GPIO_CLRTA = ~(1 << offset);
            }
            break;
#endif
        case PIN_SCL:
        case PIN_SDA:
            offset = pin_number - PIN_SCL;
            if (value) {
                MCF_GPIO_SETAS = 1 << offset;
            } else {
                MCF_GPIO_CLRAS = ~(1 << offset);
            }
            break;
        default:
            assert(0);
            break;
    }
#elif MCF51JM128
    // set the MCF51JM128 pin to value
    switch (pin_number) {
        case PIN_PTA0:
        case PIN_PTA1:
        case PIN_PTA2:
        case PIN_PTA3:
        case PIN_PTA4:
        case PIN_PTA5:
            offset = pin_number - PIN_PTA0;
            if (value) {
                PTAD |= 1<<offset;
            } else {
                PTAD &= ~(1<<offset);
            }
            break;
        case PIN_PTB0:
        case PIN_PTB1:
        case PIN_PTB2:
        case PIN_PTB3:
        case PIN_PTB4:
        case PIN_PTB5:
        case PIN_PTB6:
        case PIN_PTB7:
            offset = pin_number - PIN_PTB0;
            if (value) {
                PTBD |= 1<<offset;
            } else {
                PTBD &= ~(1<<offset);
            }
            break;
        case PIN_PTC0:
        case PIN_PTC1:
        case PIN_PTC2:
        case PIN_PTC3:
        case PIN_PTC4:
        case PIN_PTC5:
        case PIN_PTC6:
            offset = pin_number - PIN_PTC0;
            if (pin_type == pin_type_uart_output) {
                assert(offset == 3);
                while (! (SCI2S1 & SCI2S1_TDRE_MASK)) {
                    // revisit -- poll?
                }
                SCI2D = value;
                lasttx1 = value;
                uart_armed[UART_INT(1, true)] = true;
            } else {
                if (value) {
                    PTCD |= 1<<offset;
                } else {
                    PTCD &= ~(1<<offset);
                }
            }
            break;
        case PIN_PTD0:
        case PIN_PTD1:
        case PIN_PTD2:
        case PIN_PTD3:
        case PIN_PTD4:
        case PIN_PTD5:
        case PIN_PTD6:
        case PIN_PTD7:
            offset = pin_number - PIN_PTD0;
            if (value) {
                PTDD |= 1<<offset;
            } else {
                PTDD &= ~(1<<offset);
            }
            break;
        case PIN_PTE0:
        case PIN_PTE1:
        case PIN_PTE2:
        case PIN_PTE3:
        case PIN_PTE4:
        case PIN_PTE5:
        case PIN_PTE6:
        case PIN_PTE7:
            offset = pin_number - PIN_PTE0;
            if (pin_type == pin_type_analog_output || pin_type == pin_type_frequency_output) {
                assert(offset >= 2 && offset < 4);
                if (pin_type == pin_type_frequency_output) {
                    // program the counter for frequency generation (private counter)
                    TPM1MODH = value>>8;
                    TPM1MODL = value&0xff;
                    TPM1CNTL = 0;

                    // hack to work around high minimum frequency
                    if (! value) {
                        // temporarily revert to gpio
                        if (offset == 2) {
                            TPM1C0SC_ELS0x = 0;
                        } else {
                            TPM1C1SC_ELS1x = 0;
                        }
                    } else {
                        // the pin really is frequency
                        if (offset == 2) {
                            TPM1C0SC_ELS0x = 1;
                        } else {
                            TPM1C1SC_ELS1x = 1;
                        }
                    }
                }
                
                // set the channel frequency or analog level
                if (offset == 2) {
                    TPM1C0VH = value>>8;
                    TPM1C0VL = value&0xff;
                } else {
                    TPM1C1VH = value>>8;
                    TPM1C1VL = value&0xff;
                }
            } else if (pin_type == pin_type_uart_output) {
                assert(offset == 0);
                while (! (SCI1S1 & SCI1S1_TDRE_MASK)) {
                    // revisit -- poll?
                }
                SCI1D = value;
                lasttx0 = value;
                uart_armed[UART_INT(0, true)] = true;
            } else {
                if (value) {
                    PTED |= 1<<offset;
                } else {
                    PTED &= ~(1<<offset);
                }
            }
            break;
        case PIN_PTF0:
        case PIN_PTF1:
        case PIN_PTF2:
        case PIN_PTF3:
        case PIN_PTF4:
        case PIN_PTF5:
        case PIN_PTF6:
        case PIN_PTF7:
            offset = pin_number - PIN_PTF0;
            if (pin_type == pin_type_analog_output || pin_type == pin_type_frequency_output) {
                assert(offset >= 0 && offset < 6);
                if (pin_type == pin_type_frequency_output) {
                    // program the counter for frequency generation (private counter)
                    if (offset > 3) {
                        TPM2MODH = value>>8;
                        TPM2MODL = value&0xff;
                        TPM2CNTL = 0;
                    } else {
                        TPM1MODH = value>>8;
                        TPM1MODL = value&0xff;
                        TPM1CNTL = 0;
                    }
                    
                    // hack to work around high minimum frequency
                    if (! value) {
                        // temporarily revert to gpio
                        if (offset == 0) {
                            TPM1C2SC_ELS2x = 0;
                        } else if (offset == 1) {
                            TPM1C3SC_ELS3x = 0;
                        } else if (offset == 2) {
                            TPM1C4SC_ELS4x = 0;
                        } else if (offset == 3) {
                            TPM1C5SC_ELS5x = 0;
                        } else if (offset == 4) {
                            TPM2C0SC_ELS0x = 0;
                        } else {
                            TPM2C1SC_ELS1x = 0;
                        }
                    } else {
                        // the pin really is frequency
                        if (offset == 0) {
                            TPM1C2SC_ELS2x = 1;
                        } else if (offset == 1) {
                            TPM1C3SC_ELS3x = 1;
                        } else if (offset == 2) {
                            TPM1C4SC_ELS4x = 1;
                        } else if (offset == 3) {
                            TPM1C5SC_ELS5x = 1;
                        } else if (offset == 4) {
                            TPM2C0SC_ELS0x = 1;
                        } else {
                            TPM2C1SC_ELS1x = 1;
                        }
                    }
                }
                
                // set the channel frequency or analog level
                if (offset == 0) {
                    TPM1C2VH = value>>8;
                    TPM1C2VL = value&0xff;
                } else if (offset == 1) {
                    TPM1C3VH = value>>8;
                    TPM1C3VL = value&0xff;
                } else if (offset == 2) {
                    TPM1C4VH = value>>8;
                    TPM1C4VL = value&0xff;
                } else if (offset == 3) {
                    TPM1C5VH = value>>8;
                    TPM1C5VL = value&0xff;
                } else if (offset == 4) {
                    TPM2C0VH = value>>8;
                    TPM2C0VL = value&0xff;
                } else {
                    TPM2C1VH = value>>8;
                    TPM2C1VL = value&0xff;
                }
            } else {
                if (value) {
                    PTFD |= 1<<offset;
                } else {
                    PTFD &= ~(1<<offset);
                }
            }
            break;
        case PIN_PTG0:
        case PIN_PTG1:
        case PIN_PTG2:
        case PIN_PTG3:
            offset = pin_number - PIN_PTG0;
            if (value) {
                PTGD |= 1<<offset;
            } else {
                PTGD &= ~(1<<offset);
            }
            break;
        default:
            assert(0);
            break;
    }
#elif PIC32
    // set the PIC32 pin to value
    switch (pin_number) {
        case PIN_RA0:
        case PIN_RA1:
        case PIN_RA2:
        case PIN_RA3:
        case PIN_RA4:
        case PIN_RA5:
        case PIN_RA6:
        case PIN_RA7:
        case PIN_RA14:
        case PIN_RA15:
            offset = pin_number - PIN_RA0;
            assert((unsigned)offset < 16);
            if (value) {
                LATASET = 1<<offset;
            } else {
                LATACLR = 1<<offset;
            }
            break;
        case PIN_AN0:
        case PIN_AN1:
        case PIN_AN2:
        case PIN_AN3:
        case PIN_AN4:
        case PIN_AN5:
        case PIN_AN6:
        case PIN_AN7:
        case PIN_AN8:
        case PIN_AN9:
        case PIN_AN10:
        case PIN_AN11:
        case PIN_AN12:
        case PIN_AN13:
        case PIN_AN14:
        case PIN_AN15:
            offset = pin_number - PIN_AN0;
            assert((unsigned)offset < 16);
            if (value) {
                LATBSET = 1<<offset;
            } else {
                LATBCLR = 1<<offset;
            }
            break;
        case PIN_RC1:
        case PIN_RC2:
        case PIN_RC3:
        case PIN_RC4:
        case PIN_RC13:
        case PIN_RC14:
            offset = pin_number - PIN_RC0;
            assert((unsigned)offset < 16);
            if (value) {
                LATCSET = 1<<offset;
            } else {
                LATCCLR = 1<<offset;
            }
            break;
        case PIN_RD0:
        case PIN_RD1:
        case PIN_RD2:
        case PIN_RD3:
        case PIN_RD4:
        case PIN_RD5:
        case PIN_RD6:
        case PIN_RD7:
        case PIN_RD8:
        case PIN_RD9:
        case PIN_RD10:
        case PIN_RD11:
        case PIN_RD12:
        case PIN_RD13:
        case PIN_RD14:
        case PIN_RD15:
            offset = pin_number - PIN_RD0;
            assert((unsigned)offset < 16);
            if (pin_type == pin_type_analog_output) {
                if (offset == 0) {
                    OC1CONCLR = _OC1CON_ON_MASK;
                    OC1R = 0;
                    OC1RS = value;
                    OC1CON = _OC1CON_ON_MASK|(6<<_OC1CON_OCM0_POSITION);
                } else if (offset == 1) {
                    OC2CONCLR = _OC2CON_ON_MASK;
                    OC2R = 0;
                    OC2RS = value;
                    OC2CON = _OC2CON_ON_MASK|(6<<_OC2CON_OCM0_POSITION);
                } else if (offset == 2) {
                    OC3CONCLR = _OC3CON_ON_MASK;
                    OC3R = 0;
                    OC3RS = value;
                    OC3CON = _OC3CON_ON_MASK|(6<<_OC3CON_OCM0_POSITION);
                } else if (offset == 3) {
                    OC4CONCLR = _OC4CON_ON_MASK;
                    OC4R = 0;
                    OC4RS = value;
                    OC4CON = _OC4CON_ON_MASK|(6<<_OC4CON_OCM0_POSITION);
                } else {
                    assert(offset == 4);
                    OC5CONCLR = _OC5CON_ON_MASK;
                    OC5R = 0;
                    OC5RS = value;
                    OC5CON = _OC5CON_ON_MASK|(6<<_OC5CON_OCM0_POSITION);
                }                
            } else if (pin_type == pin_type_frequency_output) {
                // configure timer 3 for frequency generation
                T3CONCLR = _T3CON_ON_MASK;
                TMR3 = 0;
                PR3 = value;
                if (offset == 0) {
                    OC1CONCLR = _OC1CON_ON_MASK;
                    OC1R = 0;
                    OC1RS = value;
                    // hack to work around high minimum frequency
                    if (value) {
                        OC1CON = _OC1CON_ON_MASK|_OC1CON_OCTSEL_MASK|(3<<_OC1CON_OCM0_POSITION);
                    }
                } else if (offset == 1) {
                    OC2CONCLR = _OC2CON_ON_MASK;
                    OC2R = 0;
                    OC2RS = value;
                    // hack to work around high minimum frequency
                    if (value) {
                        OC2CON = _OC2CON_ON_MASK|_OC2CON_OCTSEL_MASK|(3<<_OC2CON_OCM0_POSITION);
                    }
                } else if (offset == 2) {
                    OC3CONCLR = _OC3CON_ON_MASK;
                    OC3R = 0;
                    OC3RS = value;
                    // hack to work around high minimum frequency
                    if (value) {
                        OC3CON = _OC3CON_ON_MASK|_OC3CON_OCTSEL_MASK|(3<<_OC3CON_OCM0_POSITION);
                    }
                } else if (offset == 3) {
                    OC4CONCLR = _OC4CON_ON_MASK;
                    OC4R = 0;
                    OC4RS = value;
                    // hack to work around high minimum frequency
                    if (value) {
                        OC4CON = _OC4CON_ON_MASK|_OC4CON_OCTSEL_MASK|(3<<_OC4CON_OCM0_POSITION);
                    }
                } else {
                    assert(offset == 4);
                    OC5CONCLR = _OC5CON_ON_MASK;
                    OC5R = 0;
                    OC5RS = value;
                    // hack to work around high minimum frequency
                    if (value) {
                        OC5CON = _OC5CON_ON_MASK|_OC5CON_OCTSEL_MASK|(3<<_OC5CON_OCM0_POSITION);
                    }
                }                
                // set prescale to 64
                T3CON = _T3CON_ON_MASK|(6<<_T3CON_TCKPS0_POSITION);
            } else {
                assert(pin_type == pin_type_digital_output);
                if (value) {
                    LATDSET = 1<<offset;
                } else {
                    LATDCLR = 1 << offset;
                }
            }
            break;
        case PIN_RE0:
        case PIN_RE1:
        case PIN_RE2:
        case PIN_RE3:
        case PIN_RE4:
        case PIN_RE5:
        case PIN_RE6:
        case PIN_RE7:
        case PIN_RE8:
        case PIN_RE9:
            offset = pin_number - PIN_RE0;
            assert((unsigned)offset < 16);
            if (value) {
                LATESET = 1<<offset;
            } else {
                LATECLR = 1<<offset;
            }
            break;
        case PIN_RF0:
        case PIN_RF1:
        case PIN_RF2:
        case PIN_RF3:
        case PIN_RF4:
        case PIN_RF5:
        case PIN_RF8:
        case PIN_RF12:
        case PIN_RF13:
            offset = pin_number - PIN_RF0;
            assert((unsigned)offset < 16);
            if (pin_type == pin_type_uart_output) {
                if (pin_number == PIN_RF8) {
                    while (U1STA & _U1STA_UTXBF_MASK) {
                        // revisit -- poll?
                    }
                    U1TXREG = value;
                    lasttx0 = value;
                    uart_armed[UART_INT(0, true)] = true;
                } else {
                    assert(pin_number == PIN_RF5);
                    while (U2STA & _U2STA_UTXBF_MASK) {
                        // revisit -- poll?
                    }
                    U2TXREG = value;
                    lasttx1 = value;
                    uart_armed[UART_INT(1, true)] = true;
                }
            } else {
                assert(pin_type == pin_type_digital_output);
                if (value) {
                    LATFSET = 1<<offset;
                } else {
                    LATFCLR = 1<<offset;
                }
            }
            break;
        case PIN_RG0:
        case PIN_RG1:
        case PIN_RG6:
        case PIN_RG7:
        case PIN_RG8:
        case PIN_RG9:
        case PIN_RG12:
        case PIN_RG13:
        case PIN_RG14:
            offset = pin_number - PIN_RG0;
            assert((unsigned)offset < 16);
            if (value) {
                LATGSET = 1<<offset;
            } else {
                LATGCLR = 1<<offset;
            }
            break;
        default:
            assert(0);
            break;
    }
#else
#error
#endif // MCF52221 || MCF52233
#endif // ! STICK_GUEST

    // if setting to 0, then enable the pin driver after the pin data value has been set to 0.  This prevents the processor from
    // driving a 1 (which the pin's latch may held at the start of this function).
    if ((! value) && (pin_qual & 1<<pin_qual_open_drain)) {
        assert(pin_type == pin_type_digital_output);
        pin_declare_internal(pin_number, pin_type_digital_output, pin_qual, true);
    }
}

// this function gets a pin variable!
int
pin_get(IN int pin_number, IN int pin_type, IN int pin_qual)
{
#if ! STICK_GUEST
#if MCF51JM128
    int adc;
#endif
    int value;
    int offset;
    
    value = 0;

#if MCF52221 || MCF52233
    // get the value of the MCF52221/MCF52233 pin
    switch (pin_number) {
        case PIN_DTIN0:
        case PIN_DTIN1:
        case PIN_DTIN2:
        case PIN_DTIN3:
            offset = pin_number - PIN_DTIN0;
            if (pin_type == pin_type_analog_output) {
                value = (255-MCF_PWM_PWMDTY(offset*2))*3300/255;
            } else if (pin_type == pin_type_frequency_output) {
                if (MCF_DTIM_DTRR(offset) == -1) {
                    value = 0;
                } else {
                    value = bus_frequency/(MCF_DTIM_DTRR(offset) + 1);
                }                        
            } else if (pin_qual & 1<<pin_qual_debounced) {
                value = pin_get_digital_debounced(port_tc, offset);
            } else {
                value = !!(MCF_GPIO_SETTC & (1 << offset));
            }
            break;
        case PIN_QSPI_DOUT:
        case PIN_QSPI_DIN:
        case PIN_QSPI_CLK:
        case PIN_QSPI_CS0:
            offset = pin_number - PIN_QSPI_DOUT;
            if (pin_qual & 1<<pin_qual_debounced) {
                value = pin_get_digital_debounced(port_qs, offset);
            } else {
                value = !!(MCF_GPIO_SETQS & (1 << offset));
            }
            break;
        case PIN_UTXD1:
        case PIN_URXD1:
        case PIN_RTS1:
        case PIN_CTS1:
            offset = pin_number - PIN_UTXD1;
            if (pin_type == pin_type_uart_input) {
                assert(pin_number == PIN_URXD1);
                if (MCF_UART_USR(1) & MCF_UART_USR_RXRDY) {
                    value = MCF_UART_URB(1) & mask1;
                    uart_armed[UART_INT(1, false)] = true;
                } else {
                    value = 0;
                }
            } else if (pin_type == pin_type_uart_output) {
                value = (MCF_UART_USR(1) & MCF_UART_USR_TXEMP)?0:lasttx1;
            } else if (pin_qual & 1<<pin_qual_debounced) {
                value = pin_get_digital_debounced(port_ub, offset);
            } else {
                value = !!(MCF_GPIO_SETUB & (1 << offset));
            }
            break;
        case PIN_UTXD0:
        case PIN_URXD0:
        case PIN_RTS0:
        case PIN_CTS0:
            offset = pin_number - PIN_UTXD0;
            if (pin_type == pin_type_uart_input) {
                assert(pin_number == PIN_URXD0);
                if (MCF_UART_USR(0) & MCF_UART_USR_RXRDY) {
                    value = MCF_UART_URB(0) & mask0;
                    uart_armed[UART_INT(0, false)] = true;
                } else {
                    value = 0;
                }
            } else if (pin_type == pin_type_uart_output) {
                value = (MCF_UART_USR(0) & MCF_UART_USR_TXEMP)?0:lasttx0;
            } else if (pin_qual & 1<<pin_qual_debounced) {
                value = pin_get_digital_debounced(port_ua, offset);
            } else {
                value = !!(MCF_GPIO_SETUA & (1 << offset));
            }
            break;
        case PIN_AN0:
        case PIN_AN1:
        case PIN_AN2:
        case PIN_AN3:
        case PIN_AN4:
        case PIN_AN5:
        case PIN_AN6:
        case PIN_AN7:
            offset = pin_number - PIN_AN0;
            if (pin_type == pin_type_analog_input) {
                value = adc_get_value(offset, pin_qual);
            } else if (pin_qual & 1<<pin_qual_debounced) {
                value = pin_get_digital_debounced(port_an, offset);
            } else {
                value = !!(MCF_GPIO_SETAN & (1 << offset));
            }
            break;
        case PIN_IRQ1:
        case PIN_IRQ4:
        case PIN_IRQ7:
            offset = pin_number - PIN_IRQ0;
            if (pin_qual & 1<<pin_qual_debounced) {
                value = pin_get_digital_debounced(port_nq, offset);
            } else {
                value = !!(MCF_GPIO_SETNQ & (1 << offset));
            }
            break;
#if MCF52233
        case PIN_IRQ11:
            offset = pin_number - PIN_GPT0;
            if (pin_qual & 1<<pin_qual_debounced) {
                value = pin_get_digital_debounced(port_gp, offset);
            } else {
                value = !!(MCF_GPIO_SETGP & (1 << offset));
            }
            break;
        case PIN_GPT0:
        case PIN_GPT1:
        case PIN_GPT2:
            offset = pin_number - PIN_GPT0;
            if (pin_qual & 1<<pin_qual_debounced) {
                value = pin_get_digital_debounced(port_ta, offset);
            } else {
                value = !!(MCF_GPIO_SETTA & (1 << offset));
            }
            break;
#endif
        case PIN_SCL:
        case PIN_SDA:
            offset = pin_number - PIN_SCL;
            if (pin_qual & 1<<pin_qual_debounced) {
                value = pin_get_digital_debounced(port_as, offset);
            } else {
                value = !!(MCF_GPIO_SETAS & (1 << offset));
            }
            break;
        default:
            assert(0);
            break;
    }
#elif MCF51JM128
    // get the value of the MCF51JM128 pin
    switch (pin_number) {
        case PIN_PTA0:
        case PIN_PTA1:
        case PIN_PTA2:
        case PIN_PTA3:
        case PIN_PTA4:
        case PIN_PTA5:
            offset = pin_number - PIN_PTA0;
            if (pin_qual & 1<<pin_qual_debounced) {
                value = pin_get_digital_debounced(port_a, offset);
            } else {
                value = !!(PTAD & (1<<offset));
            }
            break;
        case PIN_PTB0:  // adc0
        case PIN_PTB1:  // adc1
        case PIN_PTB2:  // adc2
        case PIN_PTB3:  // adc3
        case PIN_PTB4:  // adc4
        case PIN_PTB5:  // adc5
        case PIN_PTB6:  // adc6
        case PIN_PTB7:  // adc7
            offset = pin_number - PIN_PTB0;
            if (pin_type == pin_type_analog_input) {
                adc = offset;
                assert(adc >= 0 && adc <= 7);
                value = adc_get_value(adc, pin_qual);
            } else if (pin_qual & 1<<pin_qual_debounced) {
                value = pin_get_digital_debounced(port_b, offset);
            } else {
                value = !!(PTBD & (1<<offset));
            }
            break;
        case PIN_PTC0:
        case PIN_PTC1:
        case PIN_PTC2:
        case PIN_PTC3:
        case PIN_PTC4:
        case PIN_PTC5:
        case PIN_PTC6:
            offset = pin_number - PIN_PTC0;
            if (pin_type == pin_type_uart_input) {
                assert(offset == 5);
                if (SCI2S1 & SCI2S1_RDRF_MASK) {
                    value = SCI2D & mask1;
                    uart_armed[UART_INT(1, false)] = true;
                } else {
                    value = 0;
                }
            } else if (pin_type == pin_type_uart_output) {
                assert(offset == 3);
                value = (SCI2S1 & SCI2S1_TC_MASK)?0:lasttx1;
            } else if (pin_qual & 1<<pin_qual_debounced) {
                value = pin_get_digital_debounced(port_c, offset);
            } else {
                value = !!(PTCD & (1<<offset));
            }
            break;
        case PIN_PTD0:  // adc8
        case PIN_PTD1:  // adc9
        case PIN_PTD2:
        case PIN_PTD3:  // adc10
        case PIN_PTD4:  // adc11
        case PIN_PTD5:
        case PIN_PTD6:
        case PIN_PTD7:
            offset = pin_number - PIN_PTD0;
            if (pin_type == pin_type_analog_input) {
                adc = 8+offset-(offset>2);
                assert(adc >= 8 && adc <= 11);
                value = adc_get_value(adc, pin_qual);
            } else if (pin_qual & 1<<pin_qual_debounced) {
                value = pin_get_digital_debounced(port_d, offset);
            } else {
                value = !!(PTDD & (1<<offset));
            }
            break;
        case PIN_PTE0:
        case PIN_PTE1:
        case PIN_PTE2:
        case PIN_PTE3:
        case PIN_PTE4:
        case PIN_PTE5:
        case PIN_PTE6:
        case PIN_PTE7:
            offset = pin_number - PIN_PTE0;
            if (pin_type == pin_type_analog_output || pin_type == pin_type_frequency_output) {
                assert(offset >= 2 && offset < 4);
                // read the pwm/frequency
                if (offset == 2) {
                    value = (TPM1C0VH << 8) | TPM1C0VL;
                } else {
                    value = (TPM1C1VH << 8) | TPM1C1VL;
                }
                
                if (pin_type == pin_type_frequency_output) {
                    if (! value) {
                        value = 0x10000;
                    }
                    value = bus_frequency/FREQ_PRESCALE/(value+1)/2;
                }
            } else if (pin_type == pin_type_uart_input) {
                assert(offset == 1);
                if (SCI1S1 & SCI1S1_RDRF_MASK) {
                    value = SCI1D & mask0;
                    uart_armed[UART_INT(0, false)] = true;
                } else {
                    value = 0;
                }
            } else if (pin_type == pin_type_uart_output) {
                assert(offset == 0);
                value = (SCI1S1 & SCI1S1_TC_MASK)?0:lasttx0;
            } else if (pin_qual & 1<<pin_qual_debounced) {
                value = pin_get_digital_debounced(port_e, offset);
            } else {
                value = !!(PTED & (1<<offset));
            }
            break;
        case PIN_PTF0:
        case PIN_PTF1:
        case PIN_PTF2:
        case PIN_PTF3:
        case PIN_PTF4:
        case PIN_PTF5:
        case PIN_PTF6:
        case PIN_PTF7:
            offset = pin_number - PIN_PTF0;
            if (pin_type == pin_type_analog_output || pin_type == pin_type_frequency_output) {
                assert(offset >= 0 && offset < 6);
                // read the pwm/frequency
                if (offset == 0) {
                    value = (TPM1C2VH << 8) | TPM1C2VL;
                } else if (offset == 1) {
                    value = (TPM1C3VH << 8) | TPM1C3VL;
                } else if (offset == 2) {
                    value = (TPM1C4VH << 8) | TPM1C4VL;
                } else if (offset == 3) {
                    value = (TPM1C5VH << 8) | TPM1C5VL;
                } else if (offset == 4) {
                    value = (TPM2C0VH << 8) | TPM2C0VL;
                } else {
                    value = (TPM2C1VH << 8) | TPM2C1VL;
                }
                
                if (pin_type == pin_type_frequency_output) {
                    if (! value) {
                        value = 0x10000;
                    }
                    value = bus_frequency/FREQ_PRESCALE/(value+1)/2;
                }
            } else if (pin_qual & 1<<pin_qual_debounced) {
                value = pin_get_digital_debounced(port_f, offset);
            } else {
                value = !!(PTFD & (1<<offset));
            }
            break;
        case PIN_PTG0:
        case PIN_PTG1:
        case PIN_PTG2:
        case PIN_PTG3:
            offset = pin_number - PIN_PTG0;
            if (pin_qual & 1<<pin_qual_debounced) {
                value = pin_get_digital_debounced(port_g, offset);
            } else {
                value = !!(PTGD & (1<<offset));
            }
            break;
        default:
            assert(0);
            break;
    }
#elif PIC32
    // get the value of the PIC32 pin
    switch (pin_number) {
        case PIN_RA0:
        case PIN_RA1:
        case PIN_RA2:
        case PIN_RA3:
        case PIN_RA4:
        case PIN_RA5:
        case PIN_RA6:
        case PIN_RA7:
        case PIN_RA14:
        case PIN_RA15:
            offset = pin_number - PIN_RA0;
            assert((unsigned)offset < 16);
            if (pin_qual & 1<<pin_qual_debounced) {
                value = pin_get_digital_debounced(port_a, offset);
            } else {
                value = !! (PORTA & 1 << offset);
            }
            break;
        case PIN_AN0:
        case PIN_AN1:
        case PIN_AN2:
        case PIN_AN3:
        case PIN_AN4:
        case PIN_AN5:
        case PIN_AN6:
        case PIN_AN7:
        case PIN_AN8:
        case PIN_AN9:
        case PIN_AN10:
        case PIN_AN11:
        case PIN_AN12:
        case PIN_AN13:
        case PIN_AN14:
        case PIN_AN15:
            offset = pin_number - PIN_AN0;
            assert((unsigned)offset < 16);
            if (pin_type == pin_type_analog_input) {
                value = adc_get_value(offset, pin_qual);
            } else {
                if (pin_qual & 1<<pin_qual_debounced) {
                    value = pin_get_digital_debounced(port_b, offset);
                } else {
                    value = !! (PORTB & 1 << offset);
                }
            }
            break;
        case PIN_RC1:
        case PIN_RC2:
        case PIN_RC3:
        case PIN_RC4:
        case PIN_RC13:
        case PIN_RC14:
            offset = pin_number - PIN_RC0;
            assert((unsigned)offset < 16);
            if (pin_qual & 1<<pin_qual_debounced) {
                value = pin_get_digital_debounced(port_c, offset);
            } else {
                value = !! (PORTC & 1 << offset);
            }
            break;
        case PIN_RD0:
        case PIN_RD1:
        case PIN_RD2:
        case PIN_RD3:
        case PIN_RD4:
        case PIN_RD5:
        case PIN_RD6:
        case PIN_RD7:
        case PIN_RD8:
        case PIN_RD9:
        case PIN_RD10:
        case PIN_RD11:
        case PIN_RD12:
        case PIN_RD13:
        case PIN_RD14:
        case PIN_RD15:
            offset = pin_number - PIN_RD0;
            assert((unsigned)offset < 16);
            if (pin_type == pin_type_analog_output || pin_type == pin_type_frequency_output) {
                if (offset == 0) {
                    value = OC1RS;
                } else if (offset == 1) {
                    value = OC2RS;
                } else if (offset == 2) {
                    value = OC3RS;
                } else if (offset == 3) {
                    value = OC4RS;
                } else {
                    assert(offset == 4);
                    value = OC5RS;
                }
                if (pin_type == pin_type_frequency_output) {
                    if (! value) {
                        value = 0x10000;
                    }
                    value = bus_frequency/FREQ_PRESCALE/(value+1)/2;
                }
            } else if (pin_qual & 1<<pin_qual_debounced) {
                value = pin_get_digital_debounced(port_d, offset);
            } else {
                value = !! (PORTD & 1<<offset);
            }
            break;
        case PIN_RE0:
        case PIN_RE1:
        case PIN_RE2:
        case PIN_RE3:
        case PIN_RE4:
        case PIN_RE5:
        case PIN_RE6:
        case PIN_RE7:
        case PIN_RE8:
        case PIN_RE9:
            offset = pin_number - PIN_RE0;
            assert((unsigned)offset < 16);
            if (pin_qual & 1<<pin_qual_debounced) {
                value = pin_get_digital_debounced(port_e, offset);
            } else {
                value = !! (PORTE & 1 << offset);
            }
            break;
        case PIN_RF0:
        case PIN_RF1:
        case PIN_RF2:
        case PIN_RF3:
        case PIN_RF4:
        case PIN_RF5:
        case PIN_RF8:
        case PIN_RF12:
        case PIN_RF13:
            offset = pin_number - PIN_RF0;
            assert((unsigned)offset < 16);
            if (pin_type == pin_type_uart_input) {
                if (offset == 2) {
                    if (U1STA & _U1STA_URXDA_MASK) {
                        value = U1RXREG & mask0;
                        uart_armed[UART_INT(0, false)] = true;
                    } else {
                        value = 0;
                    }
                } else {
                    assert(offset == 4);
                    if (U2STA & _U2STA_URXDA_MASK) {
                        value = U2RXREG & mask1;
                        uart_armed[UART_INT(1, false)] = true;
                    } else {
                        value = 0;
                    }
                }
            } else if (pin_type == pin_type_uart_output) {
                if (offset == 8) {
                    value = (U1STA & _U1STA_TRMT_MASK)?0:lasttx0;
                } else {
                    assert(offset == 5);
                    value = (U2STA & _U2STA_TRMT_MASK)?0:lasttx1;
                }
            } else {
                if (pin_qual & 1<<pin_qual_debounced) {
                    value = pin_get_digital_debounced(port_f, offset);
                } else {
                    value = !! (PORTF & 1 << offset);
                }
            }
            break;
        case PIN_RG0:
        case PIN_RG1:
        case PIN_RG6:
        case PIN_RG7:
        case PIN_RG8:
        case PIN_RG9:
        case PIN_RG12:
        case PIN_RG13:
        case PIN_RG14:
            offset = pin_number - PIN_RG0;
            assert((unsigned)offset < 16);
            if (pin_qual & 1<<pin_qual_debounced) {
                value = pin_get_digital_debounced(port_g, offset);
            } else {
                value = !! (PORTG & 1 << offset);
            }
            break;
        default:
            assert(0);
            break;
    }
#else
#error
#endif

    if (pin_qual & (1<<pin_qual_inverted)) {
        if (pin_type == pin_type_digital_input || pin_type == pin_type_digital_output) {
            value = ! value;
        } else if (pin_type == pin_type_analog_input || pin_type == pin_type_analog_output) {
            value = 3300-value;
        }
    }

    return value;
#else
    return 0;
#endif
}

// N.B. parity: 0 -> even; 1 -> odd; 2 -> none
void
pin_uart_configure(int uart, int baud, int data, byte parity, byte loopback)
{
#if ! STICK_GUEST
    int divisor;

#if MCF52221 || MCF52233
    // configure the uart for the requested protocol and speed
    MCF_UART_UCR(uart) = MCF_UART_UCR_RESET_MR|MCF_UART_UCR_TX_DISABLED|MCF_UART_UCR_RX_DISABLED;

    MCF_UART_UMR(uart)/*1*/ = (parity==0?MCF_UART_UMR_PM_ODD:(parity==1?MCF_UART_UMR_PM_EVEN:MCF_UART_UMR_PM_NONE))|(data-5);
    MCF_UART_UMR(uart)/*2*/ = (loopback?MCF_UART_UMR_CM_LOCAL_LOOP:0)|MCF_UART_UMR_SB_STOP_BITS_2;

    MCF_UART_UCSR(uart) = MCF_UART_UCSR_RCS_SYS_CLK|MCF_UART_UCSR_TCS_SYS_CLK;

    divisor = bus_frequency/baud/16;
    if (divisor >= 0x10000) {
        divisor = 0xffff;
    }
    MCF_UART_UBG1(uart) = (uint8)(divisor/0x100);
    MCF_UART_UBG2(uart) = (uint8)(divisor%0x100);

    MCF_UART_UCR(uart) = MCF_UART_UCR_TX_ENABLED|MCF_UART_UCR_RX_ENABLED;
    
    if (! uart) {
        mask0 = data==8?0xff:0x7f;
    } else {
        mask1 = data==8?0xff:0x7f;
    }
#elif MCF51JM128
    // configure the uart for the requested protocol and speed
    if (! uart) {
        SCI1C1 = (loopback?SCI1C1_LOOPS_MASK:0)|((data==8&&parity!=2)?SCI1C1_M_MASK:0)|((parity!=2)?SCI1C1_PE_MASK:0)|(parity==1?SCI1C1_PT_MASK:0);
        
        divisor = bus_frequency/baud/16;
        if (divisor >= 0x2000) {
            divisor = 0x1fff;
        }
        SCI1BDH = (uint8)(divisor/0x100);
        SCI1BDL = (uint8)(divisor%0x100);
        
        mask0 = data==8?0xff:0x7f;
    } else {
        SCI2C1 = (loopback?SCI1C1_LOOPS_MASK:0)|((data==8&&parity!=2)?SCI1C1_M_MASK:0)|((parity!=2)?SCI1C1_PE_MASK:0)|(parity==1?SCI1C1_PT_MASK:0);
        
        divisor = bus_frequency/baud/16;
        if (divisor >= 0x2000) {
            divisor = 0x1fff;
        }
        SCI2BDH = (uint8)(divisor/0x100);
        SCI2BDL = (uint8)(divisor%0x100);

        mask1 = data==8?0xff:0x7f;
    }
#elif PIC32
    if (! uart) {
        U1MODE = (loopback?_U1MODE_LPBACK_MASK:0)|((data==8&&parity!=2)?(parity?_U1MODE_PDSEL1_MASK:_U1MODE_PDSEL0_MASK):0)|_U1MODE_STSEL_MASK;

        divisor = bus_frequency/baud/16;
        if (divisor >= 0x10000) {
            divisor = 0xffff;
        }
        U1BRG = divisor;

        U1STA = _U1STA_URXEN_MASK|_U1STA_UTXEN_MASK;

        mask0 = data==8?0xff:0x7f;
    } else {
        U2MODE = (loopback?_U2MODE_LPBACK_MASK:0)|((data==8&&parity!=2)?(parity?_U2MODE_PDSEL1_MASK:_U2MODE_PDSEL0_MASK):0)|_U2MODE_STSEL_MASK;

        divisor = bus_frequency/baud/16;
        if (divisor >= 0x10000) {
            divisor = 0xffff;
        }
        U2BRG = divisor;

        U2STA = _U2STA_URXEN_MASK|_U2STA_UTXEN_MASK;

        mask1 = data==8?0xff:0x7f;
    }
#else
#error
#endif
#endif
}

void
pin_uart_pending(OUT int *rx_full_out, OUT int *tx_empty_out)
{
#if ! STICK_GUEST
    int i;
    int usr;
    int rx_full;
    int tx_empty;
    
    rx_full = 0;
    tx_empty = 0;

#if MCF52221 || MCF52233
    for (i = 0; i < MAX_UARTS; i++) {
        usr = MCF_UART_USR(i);

        // if the uart transmitter is empty...
        if (usr & MCF_UART_USR_TXEMP) {
            tx_empty |= 1<<UART_INT(i, true);
        }

        // if the uart receiver is full...
        if (usr & MCF_UART_USR_RXRDY) {
            // mark the interrupt as pending
            rx_full |= 1<<UART_INT(i, true);
        }
    }
#elif MCF51JM128
    for (i = 0; i < MAX_UARTS; i++) {
        usr = i?SCI2S1:SCI1S1;

        // if the uart transmitter is empty...
        if (usr & SCI2S1_TC_MASK) {
            tx_empty |= 1<<UART_INT(i, true);
        }

        // if the uart receiver is full...
        if (usr & SCI2S1_RDRF_MASK) {
            // mark the interrupt as pending
            rx_full |= 1<<UART_INT(i, true);
        }
    }
#elif PIC32
    for (i = 0; i < MAX_UARTS; i++) {
        usr = i?U2STA:U1STA;

        // if the uart transmitter is empty...
        if (usr & _U1STA_TRMT_MASK) {
            tx_empty |= 1<<UART_INT(i, true);
        }

        // if the uart receiver is full...
        if (usr & _U1STA_URXDA_MASK) {
            // mark the interrupt as pending
            rx_full |= 1<<UART_INT(i, true);
        }
    }
#else
#error
#endif

    *rx_full_out = rx_full;
    *tx_empty_out = tx_empty;
#else
    *rx_full_out = 0;
    *tx_empty_out = 0;
#endif
}

void
pin_clear(void)
{
#if ! STICK_GUEST
#if MCF51JM128
    // we have to manage shared timer resources across pins
    freq[0] = (byte)-1;
    freq[1] = (byte)-1;
#endif
#if PIC32
    // we have to manage shared timer resources across pins
    // REVISIT -- for now, we force timer 2 to pwm mode, disallowing two
    // different frequency output pins; we can do better in the long run
    // by dynamically allocating both timer 2 and 3 as needed.
    freq[0] = (byte)0;  // pwm on timer 2
    freq[1] = (byte)-1;  // frequency on timer 3
#endif
#endif
}

// only called on debouncing ticks from timer_isr()
void
pin_timer_poll(void)
{
#if ! STICK_GUEST
    // for each port...
#if MCF52221 || MCF52233
    pin_digital_debounce[pin_digital_debounce_cycle][port_tc] = MCF_GPIO_SETTC;
    pin_digital_debounce[pin_digital_debounce_cycle][port_qs] = MCF_GPIO_SETQS;
    pin_digital_debounce[pin_digital_debounce_cycle][port_ub] = MCF_GPIO_SETUB;
    pin_digital_debounce[pin_digital_debounce_cycle][port_ua] = MCF_GPIO_SETUA;
    pin_digital_debounce[pin_digital_debounce_cycle][port_an] = MCF_GPIO_SETAN;
    pin_digital_debounce[pin_digital_debounce_cycle][port_nq] = MCF_GPIO_SETNQ;
    pin_digital_debounce[pin_digital_debounce_cycle][port_as] = MCF_GPIO_SETAS;
#if MCF52233
    pin_digital_debounce[pin_digital_debounce_cycle][port_gp] = MCF_GPIO_SETGP;
    pin_digital_debounce[pin_digital_debounce_cycle][port_ta] = MCF_GPIO_SETTA;
#endif
#elif MCF51JM128
    pin_digital_debounce[pin_digital_debounce_cycle][port_a] = PTAD;
    pin_digital_debounce[pin_digital_debounce_cycle][port_b] = PTBD;
    pin_digital_debounce[pin_digital_debounce_cycle][port_c] = PTCD;
    pin_digital_debounce[pin_digital_debounce_cycle][port_d] = PTDD;
    pin_digital_debounce[pin_digital_debounce_cycle][port_e] = PTED;
    pin_digital_debounce[pin_digital_debounce_cycle][port_f] = PTFD;
    pin_digital_debounce[pin_digital_debounce_cycle][port_g] = PTGD;
#elif PIC32
    pin_digital_debounce[pin_digital_debounce_cycle][port_a] = PORTA;
    pin_digital_debounce[pin_digital_debounce_cycle][port_b] = PORTB;
    pin_digital_debounce[pin_digital_debounce_cycle][port_c] = PORTC;
    pin_digital_debounce[pin_digital_debounce_cycle][port_d] = PORTD;
    pin_digital_debounce[pin_digital_debounce_cycle][port_e] = PORTE;
    pin_digital_debounce[pin_digital_debounce_cycle][port_f] = PORTF;
    pin_digital_debounce[pin_digital_debounce_cycle][port_g] = PORTG;
#endif

    if (++pin_digital_debounce_cycle >= pin_digital_debounce_history_depth) {
        pin_digital_debounce_cycle = 0;
    }
#endif // ! STICK_GUEST
}

extern void
pin_initialize(void)
{
    assert(pin_type_last < (sizeof(uint16)*8));
    assert(pin_qual_last < (sizeof(byte)*8));
    assert(LENGTHOF(pins) == PIN_LAST);

#if ! STICK_GUEST
#if MCF52221 || MCF52233
    // enable pwm channel 0, 2, 4, 6
    MCF_PWM_PWME = MCF_PWM_PWME_PWME0|MCF_PWM_PWME_PWME2|MCF_PWM_PWME_PWME4|MCF_PWM_PWME_PWME6;
    
    // set prescales to 4
    MCF_PWM_PWMPRCLK = MCF_PWM_PWMPRCLK_PCKA(2)|MCF_PWM_PWMPRCLK_PCKB(2);
    
    // set periods to 0xff
    MCF_PWM_PWMPER0 = 0xff;
    MCF_PWM_PWMPER2 = 0xff;
    MCF_PWM_PWMPER4 = 0xff;
    MCF_PWM_PWMPER6 = 0xff;
        
    // set dma timer mode registers for frequency output
    MCF_DTIM0_DTMR = MCF_DTIM_DTMR_OM|MCF_DTIM_DTMR_FRR|MCF_DTIM_DTMR_CLK_DIV1|MCF_DTIM_DTMR_RST;
    MCF_DTIM1_DTMR = MCF_DTIM_DTMR_OM|MCF_DTIM_DTMR_FRR|MCF_DTIM_DTMR_CLK_DIV1|MCF_DTIM_DTMR_RST;
    MCF_DTIM2_DTMR = MCF_DTIM_DTMR_OM|MCF_DTIM_DTMR_FRR|MCF_DTIM_DTMR_CLK_DIV1|MCF_DTIM_DTMR_RST;
    MCF_DTIM3_DTMR = MCF_DTIM_DTMR_OM|MCF_DTIM_DTMR_FRR|MCF_DTIM_DTMR_CLK_DIV1|MCF_DTIM_DTMR_RST;
#elif MCF51JM128    
    // we have to manage shared timer resources across pins
    pin_clear();
    
    // enable all pullups
    /*
    PTAPE = 0xff;
    PTBPE = 0xff;
    PTCPE = 0xff;
    PTDPE = 0xff;
    PTEPE = 0xff;
    PTFPE = 0xff;
    PTGPE = 0xff;
    */
#elif PIC32
    // we have to manage shared timer resources across pins
    pin_clear();

    // enable all pullups
    CNPUE = 0x3fffff;

    // configure timer 2 for pwm generation
    // set prescale to 1
    TMR2 = 0;
    PR2 = 3299;
    T2CON = _T2CON_ON_MASK;
#else
#error
#endif
#endif
}
