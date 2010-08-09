// *** vars.h *********************************************************

enum pin_type {
    pin_type_default = 0,
    pin_type_digital_input = 1,
    pin_type_digital_output = 2,
    pin_type_analog_input = 4,
    pin_type_analog_output = 8,
    pin_type_uart_input = 16,
    pin_type_uart_output = 32,
    pin_type_last = 32
};

enum pin_number {
    PIN_DTIN0 = 0,
    PIN_DTIN1,
    PIN_DTIN2,
    PIN_DTIN3,
    PIN_QSPI_DOUT,
    PIN_QSPI_DIN,
    PIN_QSPI_CLK,
    PIN_QSPI_CS0,
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
    PIN_AN2,
    PIN_AN3,
    PIN_AN4,
    PIN_AN5,
    PIN_AN6,
    PIN_AN7,
    PIN_IRQ0,  // unused
    PIN_IRQ1,
    PIN_IRQ2,  // unused
    PIN_IRQ3,  // unused
    PIN_IRQ4,
    PIN_IRQ5,  // unused
    PIN_IRQ6,  // unused
    PIN_IRQ7,
    PIN_SCL,
    PIN_SDA,
    PIN_LAST
};

extern bool var_trace;

extern struct pin {
    char *name;
    enum pin_type pin_types;
} pins[PIN_LAST];  // indexed by pin_number

// variables at end of flash page
enum flash_var {
    FLASH_AUTORUN,
    FLASH_AUTORESET,
    FLASH_LAST
};

#define FLASH_OFFSET(var)  (BASIC_SMALL_PAGE_SIZE-sizeof(int)-(FLASH_LAST-var)*sizeof(int))


int var_open_scope();
void var_close_scope(int scope);

// called at runtime to manipulate variables
void var_declare(char *name, int gosubs, int type, int size, int max_index, int pin_number, int pin_type);
void var_set(char *name, int index, int value);
int var_get(char *name, int index);

void var_set_flash(enum flash_var var, int value);
int var_get_flash(enum flash_var var);

// resets volatile variables (and flash params) prior to a run
void var_clear(bool flash);

void var_mem(void);

