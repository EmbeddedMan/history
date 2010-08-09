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
    1<<pin_qual_inverted,  // digital input
    1<<pin_qual_inverted,  // digital output
    1<<pin_qual_inverted,  // analog input
    1<<pin_qual_inverted,  // analog output
    0,  // uart input
    0,  // uart output
    0  // frequency output
};

char *pin_qual_names[] = {
    "inverted",
};

#define DIO  (1<<pin_type_digital_output|1<<pin_type_digital_input)

const struct pin pins[] = {
#if ! MCF51JM128
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
#else  // ! MCF51JM128
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
#endif
};

bool uart_armed[UART_INTS];

const char *uart_names[MAX_UARTS] = {
#if ! MCF51JM128
    "0",
    "1"
#else
    "1",
    "2",
#endif
};

#if MCF51JM128
static bool freq[2];

#define FREQ_PRESCALE  16
#endif

// this function declares a ram, flash, or pin variable!
void
pin_declare(IN int pin_number, IN int pin_type, IN int pin_qual)
{
#if ! _WIN32
#pragma unused(pin_qual)
#if MCF51JM128
    int n;
    int adc;
#endif
    int offset;
#if ! MCF51JM128
    int assign;
    
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
#else  // ! MCF51JM128
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
                    PTEPE &= ~(1<<offset);
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
#endif
#endif
}

static byte lasttx0;
static byte lasttx1;
static byte mask0 = 0xff;
static byte mask1 = 0xff;

// this function sets a pin variable!
void
pin_set(IN int pin_number, IN int pin_type, IN int pin_qual, IN int value)
{
#if ! _WIN32
#pragma unused(pin_qual)
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
#if ! MCF51JM128
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
    
    if (pin_qual & (1<<pin_qual_inverted)) {
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
    
#if ! MCF51JM128
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
#else  // ! MCF51JM128
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
#endif
#endif
}

// this function gets a pin variable!
int
pin_get(IN int pin_number, IN int pin_type, IN int pin_qual)
{
#if ! _WIN32
#pragma unused(pin_qual)
#if MCF51JM128
    int adc;
#endif
    int value;
    int offset;
    
    value = 0;

#if ! MCF51JM128
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
            } else {
                value = !!(MCF_GPIO_SETTC & (1 << (pin_number - PIN_DTIN0)));
            }
            break;
        case PIN_QSPI_DOUT:
        case PIN_QSPI_DIN:
        case PIN_QSPI_CLK:
        case PIN_QSPI_CS0:
            value = !!(MCF_GPIO_SETQS & (1 << (pin_number - PIN_QSPI_DOUT)));
            break;
        case PIN_UTXD1:
        case PIN_URXD1:
        case PIN_RTS1:
        case PIN_CTS1:
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
            } else {
                value = !!(MCF_GPIO_SETUB & (1 << (pin_number - PIN_UTXD1)));
            }
            break;
        case PIN_UTXD0:
        case PIN_URXD0:
        case PIN_RTS0:
        case PIN_CTS0:
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
            } else {
                value = !!(MCF_GPIO_SETUA & (1 << (pin_number - PIN_UTXD0)));
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
                value = adc_result[offset]*3300/32768;
            } else {
                value = !!(MCF_GPIO_SETAN & (1 << offset));
            }
            break;
        case PIN_IRQ1:
        case PIN_IRQ4:
        case PIN_IRQ7:
            value = !!(MCF_GPIO_SETNQ & (1 << (pin_number - PIN_IRQ0)));
            break;
#if MCF52233
        case PIN_IRQ11:
            value = !!(MCF_GPIO_SETGP & (1 << (pin_number - PIN_GPT0)));
            break;
        case PIN_GPT0:
        case PIN_GPT1:
        case PIN_GPT2:
            value = !!(MCF_GPIO_SETTA & (1 << (pin_number - PIN_GPT0)));
            break;
#endif
        case PIN_SCL:
        case PIN_SDA:
            value = !!(MCF_GPIO_SETAS & (1 << (pin_number - PIN_SCL)));
            break;
        default:
            assert(0);
            break;
    }
#else  // ! MCF51JM128
    // get the value of the MCF51JM128 pin
    switch (pin_number) {
        case PIN_PTA0:
        case PIN_PTA1:
        case PIN_PTA2:
        case PIN_PTA3:
        case PIN_PTA4:
        case PIN_PTA5:
            offset = pin_number - PIN_PTA0;
            value = !!(PTAD & (1<<offset));
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
                value = adc_result[adc]*3300/32768;
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
                assert(adc >= 8 && adc <= 12);
                value = adc_result[adc]*3300/32768;
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
            } else {
                value = !!(PTFD & (1<<offset));
            }
            break;
        case PIN_PTG0:
        case PIN_PTG1:
        case PIN_PTG2:
        case PIN_PTG3:
            offset = pin_number - PIN_PTG0;
            value = !!(PTGD & (1<<offset));
            break;
        default:
            assert(0);
            break;
    }
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

void
pin_uart_configure(int uart, int baud, int data, byte parity, byte loopback)
{
#if ! _WIN32
    int divisor;

#if ! MCF51JM128
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
#else // ! MCF51JM128
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
#endif
#endif
}

void
pin_uart_pending(OUT int *rx_full_out, OUT int *tx_empty_out)
{
#if ! _WIN32
    int i;
    byte usr;
    int rx_full;
    int tx_empty;
    
    rx_full = 0;
    tx_empty = 0;

#if ! MCF51JM128
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
#else  // ! MCF51JM128
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
#if ! _WIN32
#if MCF51JM128
    freq[0] = (byte)-1;
    freq[1] = (byte)-1;
#endif
#endif
}

extern void
pin_initialize(void)
{
    assert(pin_type_last < (sizeof(uint16)*8));
    assert(pin_qual_last < (sizeof(byte)*8));
#if ! _WIN32
#if ! MCF51JM128
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
#else  // ! MCF51JM128    
    // we have to manage shared timer resources across pins
    memset(freq, -1, sizeof(freq));
    
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
#endif
#endif
}

