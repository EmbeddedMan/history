// *** pin.h **********************************************************

#if MCF52259
#define MAX_UARTS  3
#else
#define MAX_UARTS  2
#endif

#define UART_INTS  (2*MAX_UARTS)

#define UART_INT(uart, output)  ((uart)*2+output)

// up to 16 bits
enum pin_type {
    pin_type_digital_input,
    pin_type_digital_output,
    pin_type_analog_input,
    pin_type_analog_output,
    pin_type_uart_input,
    pin_type_uart_output,
    pin_type_frequency_output,
    pin_type_last
};

extern char *pin_type_names[];

// up to 8 bits.  keep in-sync with pin_qual_names.
enum pin_qual {
    pin_qual_debounced,
    pin_qual_inverted,
    pin_qual_open_drain,
    pin_qual_last
};

extern byte pin_qual_mask[];

extern char *pin_qual_names[];

// N.B. pins marked with *** may affect zigbee or other system operation
enum pin_number {
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    PIN_DTIN0,
    PIN_DTIN1,
    PIN_DTIN2,
    PIN_DTIN3,
    PIN_QSPI_DOUT,  // *** zigbee/clone
    PIN_QSPI_DIN,  // *** zigbee/clone
    PIN_QSPI_CLK,  // *** zigbee/clone
    PIN_QSPI_CS0,  // *** zigbee/clone
#if MCF52259
    PIN_UTXD2,
    PIN_URXD2,
    PIN_RTS2,
    PIN_CTS2,
#endif
    PIN_UTXD1,
    PIN_URXD1,
    PIN_RTS1,
    PIN_CTS1,
    PIN_UTXD0,
    PIN_URXD0,
    PIN_RTS0,
    PIN_CTS0,
    PIN_AN0,
    PIN_AN1,
    PIN_AN2,  // *** zigbee
    PIN_AN3,  // *** zigbee
    PIN_AN4,  // *** zigbee
    PIN_AN5,
    PIN_AN6,
    PIN_AN7,
    PIN_IRQ0,  // unused
    PIN_IRQ1,  // *** sleep switch
    PIN_IRQ2,  // unused
    PIN_IRQ3,  // unused
    PIN_IRQ4,  // *** zigbee
    PIN_IRQ5,  // unused
    PIN_IRQ6,  // unused
    PIN_IRQ7,  // *** activity led
#if MCF52233
    PIN_IRQ8,  // unused
    PIN_IRQ9,  // unused
    PIN_IRQ10,  // unused
    PIN_IRQ11,
#endif
#if MCF52233 || MCF52259 || MCF5211
    PIN_GPT0,  // *** zigbee
    PIN_GPT1,  // *** zigbee
    PIN_GPT2,
    PIN_GPT3,
#endif
    PIN_SCL,  // *** zigbee/clone
    PIN_SDA,  // *** zigbee
#elif MCF51JM128
    PIN_PTA0 = 0,
    PIN_PTA1,
    PIN_PTA2,
    PIN_PTA3,
    PIN_PTA4,
    PIN_PTA5,
    PIN_PTB0,
    PIN_PTB1,
    PIN_PTB2,
    PIN_PTB3,
    PIN_PTB4,
    PIN_PTB5,  // *** zigbee
    PIN_PTB6,
    PIN_PTB7,
    PIN_PTC0,
    PIN_PTC1,
    PIN_PTC2,
    PIN_PTC3,
    PIN_PTC4,
    PIN_PTC5,
    PIN_PTC6,
    PIN_PTD0,
    PIN_PTD1,
    PIN_PTD2,
    PIN_PTD3,
    PIN_PTD4,
    PIN_PTD5,
    PIN_PTD6,
    PIN_PTD7,
    PIN_PTE0,
    PIN_PTE1,
    PIN_PTE2,  // *** zigbee
    PIN_PTE3,  // *** zigbee
    PIN_PTE4,  // *** zigbee
    PIN_PTE5,  // *** zigbee
    PIN_PTE6,  // *** zigbee
    PIN_PTE7,  // *** zigbee
    PIN_PTF0,  // *** activity led
    PIN_PTF1,
    PIN_PTF2,
    PIN_PTF3,
    PIN_PTF4,
    PIN_PTF5,
    PIN_PTF6,
    PIN_PTF7,
    PIN_PTG0,
    PIN_PTG1,
    PIN_PTG2,
    PIN_PTG3,
#elif MCF51QE128 || MC9S08QE128
    PIN_PTA0 = 0,
    PIN_PTA1,
    PIN_PTA2,
    PIN_PTA3,
    PIN_PTA4,
    PIN_PTA5,
    PIN_PTA6,
    PIN_PTA7,
    PIN_PTB0,
    PIN_PTB1,
    PIN_PTB2,  // *** zigbee
    PIN_PTB3,  // *** zigbee
    PIN_PTB4,  // *** zigbee
    PIN_PTB5,  // *** zigbee
    PIN_PTB6,
    PIN_PTB7,
    PIN_PTC0,  // *** zigbee
    PIN_PTC1,  // *** zigbee
    PIN_PTC2,  // activity led
    PIN_PTC3,
    PIN_PTC4,
    PIN_PTC5,
    PIN_PTC6,
    PIN_PTC7,
    PIN_PTD0,
    PIN_PTD1,
    PIN_PTD2,
    PIN_PTD3,
    PIN_PTD4,
    PIN_PTD5,
    PIN_PTD6,
    PIN_PTD7,
    PIN_PTE0,
    PIN_PTE1,
    PIN_PTE2,
    PIN_PTE3,
    PIN_PTE4,
    PIN_PTE5,
    PIN_PTE6,
    PIN_PTE7,
    PIN_PTF0,
    PIN_PTF1,  // *** zigbee
    PIN_PTF2,
    PIN_PTF3,
    PIN_PTF4,
    PIN_PTF5,
    PIN_PTF6,
    PIN_PTF7,
    PIN_PTG0,
    PIN_PTG1,
    PIN_PTG2,
    PIN_PTG3,
#elif MC9S12DT256
    PIN_PAD00,
    PIN_PAD01,  // *** zigbee
    PIN_PAD02,
    PIN_PAD03,
    PIN_PAD04,
    PIN_PAD05,
    PIN_PAD06,
    PIN_PAD07,
    PIN_PA0,
    PIN_PA1,
    PIN_PA2,
    PIN_PA3,
    PIN_PA4,
    PIN_PA5,
    PIN_PA6,
    PIN_PA7,
    PIN_PB0,
    PIN_PB1,
    PIN_PB2,
    PIN_PB3,
    PIN_PB4,
    PIN_PB5,
    PIN_PB6,
    PIN_PB7,
    PIN_PE0,
    PIN_PE1,
    PIN_PE2,
    PIN_PE3,
    PIN_PE4,
    PIN_PE5,
    PIN_PE6,
    PIN_PE7,
    PIN_PJ0,
    PIN_PJ1,
    PIN_PJ2,
    PIN_PJ3,
    PIN_PJ4,
    PIN_PJ5,
    PIN_PJ6,
    PIN_PJ7,
    PIN_PM0,
    PIN_PM1,
    PIN_PM2,  // *** zigbee
    PIN_PM3,  // *** zigbee
    PIN_PM4,  // *** zigbee
    PIN_PM5,  // *** zigbee
    PIN_PM6,
    PIN_PM7,
    PIN_PP0,
    PIN_PP1,
    PIN_PP2,
    PIN_PP3,
    PIN_PP4,
    PIN_PP5,
    PIN_PP6,
    PIN_PP7,
    PIN_PS0,
    PIN_PS1,
    PIN_PS2,
    PIN_PS3,
    PIN_PT0,  // *** zigbee
    PIN_PT1,  // *** zigbee
    PIN_PT2,
    PIN_PT3,
    PIN_PT4,
    PIN_PT5,
    PIN_PT6,
    PIN_PT7,
#elif PIC32
    PIN_RA0,
    PIN_RA1,
    PIN_RA2,
    PIN_RA3,
    PIN_RA4,
    PIN_RA5,
    PIN_RA6,
    PIN_RA7,
    PIN_RA8, // unused
    PIN_RA9, // unused
    PIN_RA10, // unused
    PIN_RA11, // unused
    PIN_RA12, // unused
    PIN_RA13, // unused
    PIN_RA14,
    PIN_RA15,
    PIN_AN0,  // rb0...
    PIN_AN1,
    PIN_AN2,
    PIN_AN3,
    PIN_AN4,
    PIN_AN5,
    PIN_AN6,
    PIN_AN7,
    PIN_AN8,
    PIN_AN9,
    PIN_AN10,
    PIN_AN11,
    PIN_AN12,
    PIN_AN13,
    PIN_AN14,
    PIN_AN15,
    PIN_RC0,  // unused
    PIN_RC1,
    PIN_RC2,
    PIN_RC3,
    PIN_RC4,
    PIN_RC5,  // unused
    PIN_RC6,  // unused
    PIN_RC7,  // unused
    PIN_RC8,  // unused
    PIN_RC9,  // unused
    PIN_RC10,  // unused
    PIN_RC11,  // unused
    PIN_RC12,  // unused
    PIN_RC13,
    PIN_RC14,
    PIN_RD0,  // oc1
    PIN_RD1,  // oc2
    PIN_RD2,  // oc3
    PIN_RD3,  // oc4
    PIN_RD4,  // oc5
    PIN_RD5,
    PIN_RD6,
    PIN_RD7,
    PIN_RD8,
    PIN_RD9,
    PIN_RD10,
    PIN_RD11,
    PIN_RD12,
    PIN_RD13,
    PIN_RD14,
    PIN_RD15,
    PIN_RE0,
    PIN_RE1,
    PIN_RE2,
    PIN_RE3,
    PIN_RE4,
    PIN_RE5,
    PIN_RE6,
    PIN_RE7,
    PIN_RE8,
    PIN_RE9,
    PIN_RF0,
    PIN_RF1,
    PIN_RF2,
    PIN_RF3,
    PIN_RF4,
    PIN_RF5,
    PIN_RF6,  // unused
    PIN_RF7,  // unused
    PIN_RF8,
    PIN_RF9,  // unused
    PIN_RF10,  // unused
    PIN_RF11,  // unused
    PIN_RF12,
    PIN_RF13,
    PIN_RG0,
    PIN_RG1,
    PIN_RG2,  // unused
    PIN_RG3,  // unused
    PIN_RG4,  // unused
    PIN_RG5,  // unused
    PIN_RG6,
    PIN_RG7,
    PIN_RG8,
    PIN_RG9,
    PIN_RG10,  // unused
    PIN_RG11,  // unused
    PIN_RG12,
    PIN_RG13,
    PIN_RG14,
#else
#error
#endif
    PIN_LAST
};

extern uint16 pin_analog;

const extern struct pin {
    char *name;
    uint16 pin_type_mask;
} pins[];  // indexed by pin_number

extern const char *uart_names[MAX_UARTS];

extern bool uart_armed[UART_INTS];


// this function declares a pin variable!
void
pin_declare(IN int pin_number, IN int pin_type, IN int pin_qual);

// this function sets a pin variable!
void
pin_set(IN int pin_number, IN int pin_type, IN int pin_qual, IN int32 value);

// this function gets a pin variable!
int32
pin_get(IN int pin_number, IN int pin_type, IN int pin_qual);


void
pin_uart_configure(int uart, int baud, int data, byte parity, bool loopback);

bool
pin_uart_tx_ready(int uart);

bool
pin_uart_tx_empty(int uart);

bool
pin_uart_rx_ready(int uart);

void
pin_uart_tx(int uart, byte value);

byte
pin_uart_rx(int uart);


void
pin_clear(void);

void
pin_timer_poll(void);

extern void
pin_initialize(void);
