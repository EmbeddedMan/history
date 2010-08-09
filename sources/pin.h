// *** pin.h **********************************************************

#define MAX_UARTS  2  // REVISIT -- changing this requires StickOS rebuild!

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

// up to 8 bits
enum pin_qual {
    pin_qual_inverted,
    pin_qual_last
};

extern byte pin_qual_mask[];

extern char *pin_qual_names[];

// N.B. pins marked with *** may affect zigbee or other system operation
enum pin_number {
#if ! MCF51JM128
    PIN_DTIN0,
    PIN_DTIN1,
    PIN_DTIN2,
    PIN_DTIN3,
    PIN_QSPI_DOUT,  // *** zigbee/clone
    PIN_QSPI_DIN,  // *** zigbee/clone
    PIN_QSPI_CLK,  // *** zigbee/clone
    PIN_QSPI_CS0,  // *** zigbee/clone
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
    PIN_GPT0,  // *** zigbee
    PIN_GPT1,  // *** zigbee
    PIN_GPT2,
    PIN_IRQ11,
#endif
    PIN_SCL,  // *** zigbee/clone
    PIN_SDA,  // *** zigbee
#else // ! MCF51JM128
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
#endif
    PIN_LAST
};

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
pin_set(IN int pin_number, IN int pin_type, IN int pin_qual, IN int value);

// this function gets a pin variable!
int
pin_get(IN int pin_number, IN int pin_type, IN int pin_qual);

void
pin_uart_configure(int uart, int baud, int data, byte parity, byte loopback);

void
pin_uart_pending(OUT int *rx_full, OUT int *tx_empty);

void
pin_clear(void);

extern void
pin_initialize(void);

