// *** vars.c *********************************************************
// this file implements the variable access module, including ram,
// pin, and flash variables, as well as the external pin control and
// access module.

// modules:
// *** variable access module ***
// *** external pin control and access ***
// *** RAM control and access ***
// *** flash control and access ***
// *** pin variables ***
// *** interactive debugger ***

#include "main.h"

// the last word of each flash bank is the generation number
#define GENERATION(p)  *(int *)((p)+BASIC_SMALL_PAGE_SIZE-sizeof(int))

// we always pick the newer flash bank
#define FLASH_PARAM_PAGE  ((GENERATION(FLASH_PARAM1_PAGE)+1 > GENERATION(FLASH_PARAM2_PAGE)+1) ? FLASH_PARAM1_PAGE : FLASH_PARAM2_PAGE)

static byte *alternate_flash_param_page;

// *** pin variables ***

struct pin pins[] = {
    "dtin0", pin_type_default,  // pin_type_analog_output
    "dtin1", pin_type_default,  // pin_type_analog_output
    "dtin2", pin_type_default,  // pin_type_analog_output
    "dtin3", pin_type_default,  // pin_type_analog_output
    "qspi_dout", pin_type_default,
    "qspi_din", pin_type_default,
    "qspi_clk", pin_type_default,
    "qspi_cs0", pin_type_default,
    "utxd1", pin_type_uart_output,
    "urxd1", pin_type_uart_input,
    "urts1*", pin_type_default,
    "ucts1*", pin_type_default,
    "utxd0", pin_type_uart_output,
    "urxd0", pin_type_uart_input,
    "urts0*", pin_type_default,
    "ucts0*", pin_type_default,
    "an0", pin_type_analog_input,
    "an1", pin_type_analog_input,
    "an2", pin_type_analog_input,
    "an3", pin_type_analog_input,
    "an4", pin_type_analog_input,
    "an5", pin_type_analog_input,
    "an6", pin_type_analog_input,
    "an7", pin_type_analog_input,
    "irq0*", pin_type_default,
    "irq1*", pin_type_default,
    "irq2*", pin_type_default,
    "irq3*", pin_type_default,
    "irq4*", pin_type_default,
    "irq5*", pin_type_default,
    "irq6*", pin_type_default,
    "irq7*", pin_type_default,
    "scl", pin_type_default,
    "sda", pin_type_default,
};

bool var_trace;

#define MAX_VARS  100

static
struct var {
    char name[15];  // 14 char max variable name
    byte gosubs;
    byte type;
    byte size;  // 4 bytes per integer, vs. 1 byte per byte
    short max_index;
    union {
        int page_offset;
        byte *bytecode;
        struct {
            byte number;
            byte type;  // only 1 bit set
        } pin;
    } u;
} vars[MAX_VARS];

static int max_vars;  // allocated

static int ram_offset;  // allocated in RAM_VARIABLE_PAGE
static int param_offset;  // allocated in FLASH_PARAM_PAGE

// *** flash control and access ***

// this function erases the alternate parameter page in flash memory.
static
void
flash_erase_alternate(void)
{
    // determine the alternate flash page
    assert(FLASH_PARAM_PAGE == FLASH_PARAM1_PAGE || FLASH_PARAM_PAGE == FLASH_PARAM2_PAGE);
    alternate_flash_param_page = (FLASH_PARAM_PAGE == FLASH_PARAM1_PAGE) ? FLASH_PARAM2_PAGE : FLASH_PARAM1_PAGE;

    // erase the alternate flash page
    assert(BASIC_SMALL_PAGE_SIZE >= FLASH_PAGE_SIZE && ! (BASIC_SMALL_PAGE_SIZE%FLASH_PAGE_SIZE));
    flash_erase_pages((uint32 *)alternate_flash_param_page, BASIC_SMALL_PAGE_SIZE/FLASH_PAGE_SIZE);
}

// this function updates the alternate parameter page in flash memory.
static
void
flash_update_alternate(IN uint32 offset, IN int value)
{
    assert(! (offset & (sizeof(int)-1)));

    assert(FLASH_PARAM_PAGE != alternate_flash_param_page);

    // copy the initial words from the primary page to the alternate page
    flash_write_words((uint32 *)alternate_flash_param_page, (uint32 *)FLASH_PARAM_PAGE, offset/sizeof(int));

    // copy the updated word value at the specified offset to the alternate page
    flash_write_words((uint32 *)(alternate_flash_param_page+offset), (uint32 *)&value, 1);

    // copy the final words from the primary page to the alternate page
    flash_write_words((uint32 *)(alternate_flash_param_page+offset+sizeof(int)), (uint32 *)(FLASH_PARAM_PAGE+offset+sizeof(int)), (BASIC_SMALL_PAGE_SIZE-sizeof(int)-(offset+sizeof(int)))/sizeof(int));
}

// this function clears the alternate parameter page in flash memory.
static
void
flash_clear_alternate(void)
{
    int value;
    uint32 offset;

    assert(FLASH_PARAM_PAGE != alternate_flash_param_page);

    value = 0;

    // for all user variable offsets...
    for (offset = 0; offset < FLASH_OFFSET(0); offset += sizeof(int)) {
        assert(! (offset & (sizeof(int)-1)));

        // copy the zero word to the alternate page
        flash_write_words((uint32 *)(alternate_flash_param_page+offset), (uint32 *)&value, 1);
    }

    // copy system variables at end of flash page
    flash_write_words((uint32 *)(alternate_flash_param_page+FLASH_OFFSET(0)), (uint32 *)(FLASH_PARAM_PAGE+FLASH_OFFSET(0)), FLASH_LAST);
}

// this function promotes the alternate parameter page in flash memory
// to become current.
static
void
flash_promote_alternate(void)
{
    int generation;

    assert(FLASH_PARAM_PAGE != alternate_flash_param_page);

    // update the generation of the alternate page, to make it primary!
    generation = GENERATION(FLASH_PARAM_PAGE)+1;
    flash_write_words((uint32 *)(alternate_flash_param_page+BASIC_SMALL_PAGE_SIZE-sizeof(int)), (uint32 *)&generation, 1);

    assert(FLASH_PARAM_PAGE == alternate_flash_param_page);
    assert(GENERATION(FLASH_PARAM1_PAGE) != GENERATION(FLASH_PARAM2_PAGE));

    delay(500);  // this always takes a while!
}

// this function finds the specified variable in our variable table.
static
struct var *
var_find(IN char *name)
{
    int i;

    // for all declared variables...
    for (i = max_vars-1; i >= 0; i--) {
        assert(vars[i].type);
        // if the variable name matches...
        if (vars[i].name[0] == name[0] && ! strncmp(vars[i].name, name, sizeof(vars[i].name)-1)) {
            return vars+i;
        }
    }
    return NULL;
}

// this function opens a new gosub variable scope; variables declared
// in the gosub will be automatically undeclared when the gosub
// returns.
int
var_open_scope(void)
{
    return max_vars;
}

// this function closes a gosub scope when the gosub returns,
// automatically undeclaring any variables declared in the gosub.
void
var_close_scope(IN int scope)
{
    max_vars = scope;
}

// this function declares a ram, flash, or pin variable!
void
var_declare(IN char *name, IN int gosubs, IN int type, IN int size, IN int max_index, IN int pin_number, IN int pin_type)
{
#if ! _WIN32
    int offset;
#endif
    struct var *var;

    assert(name);
    assert(type >= code_deleted && type < code_max);
    assert(max_index);

    if (! run_condition) {
        return;
    }

    var = var_find(name);
    // if the var already exists...
    if (var) {
        // if the var exists at the same scope...
        if (var->gosubs == gosubs) {
            // this is a repeat dimension
            // if the type does not match...
            if (var->type != type || var->size != size || var->max_index != max_index) {
                printf("var '%s' dimension mismatch\n", name);
                stop();
                return;
            }
            // we're good to go
            return;
        } else {
            assert(gosubs > var->gosubs);
        }
    }

    // if we're out of vars...
    if (max_vars >= MAX_VARS) {
        printf("too many variables\n");
        stop();
        return;
    }

    // declare the var
    strncpy(vars[max_vars].name, name, sizeof(vars[max_vars].name)-1);
    vars[max_vars].gosubs = gosubs;
    vars[max_vars].type = type;
    assert(size == 1 || size == sizeof(int));
    vars[max_vars].size = size;
    vars[max_vars].max_index = max_index;

    switch (type) {
        case code_ram:
            assert(! pin_number);
            // if we're out of ram space...
            if (ram_offset+max_index*vars[max_vars].size > BASIC_SMALL_PAGE_SIZE) {
                vars[max_vars].max_index = 0;
                printf("out of variable ram\n");
                stop();
                return;
            }
            // *** RAM control and access ***
            // allocate the ram var
            vars[max_vars].u.page_offset = ram_offset;
            ram_offset += max_index*vars[max_vars].size;
            break;

        case code_flash:
            assert(! pin_number);
            assert(size == 4);  // integer only
            // if we're out of flash space...
            if (param_offset+max_index*vars[max_vars].size > BASIC_SMALL_PAGE_SIZE-(FLASH_LAST+1)*sizeof(int)) {
                vars[max_vars].max_index = 0;
                printf("out of parameter flash\n");
                stop();
                return;
            }
            // *** flash control and access ***
            // allocate the flash var
            assert(! (param_offset & (sizeof(int)-1)));  // integer only
            vars[max_vars].u.page_offset = param_offset;
            assert(vars[max_vars].size == sizeof(int));  // integer only
            param_offset += max_index*vars[max_vars].size;
            break;

        case code_pin:
            // *** external pin control and access ***
            assert(max_index == 1);
            assert(! (pin_type & (pin_type-1)));  // only 1 bit set
            if (pin_type != pin_type_digital_input && pin_type != pin_type_digital_output) {
                // N.B. this was checked when we parsed
                assert(pins[pin_number].pin_types & pin_type);
            }
            vars[max_vars].u.pin.number = pin_number;
            vars[max_vars].u.pin.type = pin_type;

#if ! _WIN32
            // configure the MCF52221 pin for the requested function
            switch (pin_number) {
                case PIN_DTIN0:
                case PIN_DTIN1:
                case PIN_DTIN2:
                case PIN_DTIN3:
                    offset = pin_number - PIN_DTIN0;
                    MCF_GPIO_PTCPAR = 0;
                    if (pin_type == pin_type_digital_output) {
                        MCF_GPIO_DDRTC |= 1 << offset;
                    } else {
                        assert(pin_type == pin_type_digital_input);
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
                        MCF_GPIO_PUBPAR = (MCF_GPIO_PUBPAR &~ (3 << (2*offset))) | (1 << (2*offset));
                    } else {
                        MCF_GPIO_PUBPAR &= ~(3 << (2*offset));
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
                        MCF_GPIO_PUAPAR = (MCF_GPIO_PUAPAR &~ (3 << (2*offset))) | (1 << (2*offset));
                    } else {
                        MCF_GPIO_PUAPAR &= ~(3 << (2*offset));
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
                    MCF_GPIO_PNQPAR = 0;
                    if (pin_type == pin_type_digital_output) {
                        MCF_GPIO_DDRNQ |= 1 << offset;
                    } else {
                        assert(pin_type == pin_type_digital_input);
                        MCF_GPIO_DDRNQ &= ~(1 << offset);
                    }
                    break;
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
#endif
            break;

        default:
            assert(0);
            break;
    }

    max_vars++;
}

static byte lasttx0;
static byte lasttx1;

// this function sets the value of a ram, flash, or pin variable!
void
var_set(IN char *name, IN int index, IN int value)
{
#if ! _WIN32
    int offset;
#endif
    struct var *var;

    if (! run_condition) {
        return;
    }

    var = var_find(name);
    if (! var) {
        printf("var '%s' undefined\n", name);
        stop();
    } else if (index >= var->max_index) {
        printf("var '%s' index %d out of range\n", name, index);
        stop();
    } else {
        switch (var->type) {
            case code_ram:
                // *** RAM control and access ***
                // set the ram variable to value
                if (var->size == sizeof(int)) {
                    *(int *)(RAM_VARIABLE_PAGE+var->u.page_offset+index*sizeof(int)) = value;
                } else {
                    assert(var->size == 1);
                    *(byte *)(RAM_VARIABLE_PAGE+var->u.page_offset+index) = (byte)value;
                }
                // *** interactive debugger ***
                // if debug tracing is enabled...
                if (var_trace) {
                    if (var->max_index > 1) {
                        printf("%4d let %s[%d] = %d\n", run_line_number, name, index, value);
                    } else {
                        printf("%4d let %s = %d\n", run_line_number, name, value);
                    }
                }
                break;

            case code_flash:
                assert(var->size == sizeof(int));

                // *** flash control and access ***
                // if the flash variable is not already equal to value
                if (*(int *)(FLASH_PARAM_PAGE+var->u.page_offset+index*sizeof(int)) != value) {
                    // set the flash variable to value
                    flash_erase_alternate();
                    flash_update_alternate(var->u.page_offset+index*sizeof(int), value);
                    flash_promote_alternate();

                    // *** interactive debugger ***
                    // if debug tracing is enabled...
                    if (var_trace) {
                        if (var->max_index > 1) {
                            printf("%4d let %s[%d] = %d\n", run_line_number, name, index, value);
                        } else {
                            printf("%4d let %s = %d\n", run_line_number, name, value);
                        }
                    }
                }
                break;

            case code_pin:
                // *** external pin control and access ***
                if (! (var->u.pin.type & (pin_type_digital_output|pin_type_uart_output))) {
                    printf("var '%s' readonly\n", name);
                    stop();
                } else {
                    // *** interactive debugger ***
                    // if debug tracing is enabled...
                    if (var_trace) {
                        printf("%4d let %s = %d\n", run_line_number, name, value);
                    }

#if ! _WIN32
                    // set the MCF52221 pin to value
                    switch (var->u.pin.number) {
                        case PIN_DTIN0:
                        case PIN_DTIN1:
                        case PIN_DTIN2:
                        case PIN_DTIN3:
                            offset = var->u.pin.number - PIN_DTIN0;
                            if (value) {
                                MCF_GPIO_SETTC = 1 << offset;
                            } else {
                                MCF_GPIO_CLRTC = ~(1 << offset);
                            }
                            break;
                        case PIN_QSPI_DOUT:
                        case PIN_QSPI_DIN:
                        case PIN_QSPI_CLK:
                        case PIN_QSPI_CS0:
                            offset = var->u.pin.number - PIN_QSPI_DOUT;
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
                            offset = var->u.pin.number - PIN_UTXD1;
                            if (var->u.pin.type == pin_type_uart_output) {
                                assert(var->u.pin.number == PIN_UTXD1);
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
                            offset = var->u.pin.number - PIN_UTXD0;
                            if (var->u.pin.type == pin_type_uart_output) {
                                assert(var->u.pin.number == PIN_UTXD0);
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
                            offset = var->u.pin.number - PIN_AN0;
                            if (value) {
                                MCF_GPIO_SETAN = 1 << offset;
                            } else {
                                MCF_GPIO_CLRAN = ~(1 << offset);
                            }
                            break;
                        case PIN_IRQ1:
                        case PIN_IRQ4:
                        case PIN_IRQ7:
                            offset = var->u.pin.number - PIN_IRQ0;
                            if (value) {
                                MCF_GPIO_SETNQ = 1 << offset;
                            } else {
                                MCF_GPIO_CLRNQ = ~(1 << offset);
                            }
                            break;
                        case PIN_SCL:
                        case PIN_SDA:
                            offset = var->u.pin.number - PIN_SCL;
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
#endif
                }
                break;

            default:
                assert(0);
                break;
        }
    }
}

// this function gets the value of a ram, flash, or pin variable!
int
var_get(IN char *name, IN int index)
{
    int value;
#if ! _WIN32
    int offset;
#endif
    struct var *var;

    if (! run_condition) {
        return 0;
    }

    value = 0;

    var = var_find(name);
    if (! var) {
        printf("var '%s' undefined\n", name);
        stop();
    } else if (index >= var->max_index) {
        printf("var '%s' index %d out of range\n", name, index);
        stop();
    } else {
        switch (var->type) {
            case code_ram:
                // *** RAM control and access ***
                // get the value of the ram variable
                if (var->size == sizeof(int)) {
                    value = *(int *)(RAM_VARIABLE_PAGE+var->u.page_offset+index*sizeof(int));
                } else {
                    assert(var->size == 1);
                    value = *(byte *)(RAM_VARIABLE_PAGE+var->u.page_offset+index);
                }
                break;

            case code_flash:
                assert(var->size == sizeof(int));

                // *** flash control and access ***
                // get the value of the flash variable
                value = *(int *)(FLASH_PARAM_PAGE+var->u.page_offset+index*sizeof(int));
                break;

            case code_pin:
                // *** external pin control and access ***
#if ! _WIN32
                // get the value of the MCF52221 pin
                switch (var->u.pin.number) {
                    case PIN_DTIN0:
                    case PIN_DTIN1:
                    case PIN_DTIN2:
                    case PIN_DTIN3:
                        value = !!(MCF_GPIO_SETTC & (1 << (var->u.pin.number - PIN_DTIN0)));
                        break;
                    case PIN_QSPI_DOUT:
                    case PIN_QSPI_DIN:
                    case PIN_QSPI_CLK:
                    case PIN_QSPI_CS0:
                        value = !!(MCF_GPIO_SETQS & (1 << (var->u.pin.number - PIN_QSPI_DOUT)));
                        break;
                    case PIN_UTXD1:
                    case PIN_URXD1:
                    case PIN_RTS1:
                    case PIN_CTS1:
                        if (var->u.pin.type == pin_type_uart_input) {
                            assert(var->u.pin.number == PIN_URXD1);
                            if (MCF_UART_USR(1) & MCF_UART_USR_RXRDY) {
                                value = MCF_UART_URB(1);
                                uart_armed[UART_INT(1, false)] = true;
                            } else {
                                value = 0;
                            }
                        } else if (var->u.pin.type == pin_type_uart_output) {
                            value = (MCF_UART_USR(1) & MCF_UART_USR_TXEMP)?0:lasttx1;
                        } else {
                            value = !!(MCF_GPIO_SETUB & (1 << (var->u.pin.number - PIN_UTXD1)));
                        }
                        break;
                    case PIN_UTXD0:
                    case PIN_URXD0:
                    case PIN_RTS0:
                    case PIN_CTS0:
                        if (var->u.pin.type == pin_type_uart_input) {
                            assert(var->u.pin.number == PIN_URXD0);
                            if (MCF_UART_USR(0) & MCF_UART_USR_RXRDY) {
                                value = MCF_UART_URB(0);
                                uart_armed[UART_INT(0, false)] = true;
                            } else {
                                value = 0;
                            }
                        } else if (var->u.pin.type == pin_type_uart_output) {
                            value = (MCF_UART_USR(0) & MCF_UART_USR_TXEMP)?0:lasttx0;
                        } else {
                            value = !!(MCF_GPIO_SETUA & (1 << (var->u.pin.number - PIN_UTXD0)));
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
                        offset = var->u.pin.number - PIN_AN0;
                        if (var->u.pin.type == pin_type_analog_input) {
                            value = adc_result[offset];
                        } else {
                            value = !!(MCF_GPIO_SETAN & (1 << offset));
                        }
                        break;
                    case PIN_IRQ1:
                    case PIN_IRQ4:
                    case PIN_IRQ7:
                        value = !!(MCF_GPIO_SETNQ & (1 << (var->u.pin.number - PIN_IRQ0)));
                        break;
                    case PIN_SCL:
                    case PIN_SDA:
                        value = !!(MCF_GPIO_SETAS & (1 << (var->u.pin.number - PIN_SCL)));
                        break;
                    default:
                        assert(0);
                        break;
                }
#endif
                break;

            default:
                assert(0);
                break;
        }
    }

    return value;
}

// *** flash control and access ***

// this function sets the value of a flash mode parameter
void
var_set_flash(IN enum flash_var var, IN int value)
{
    assert(var >= 0 && var < FLASH_LAST);

    if (*(int *)(FLASH_PARAM_PAGE+FLASH_OFFSET(var)) != value) {
        flash_erase_alternate();
        flash_update_alternate(FLASH_OFFSET(var), value);
        flash_promote_alternate();
    }
}

// this function gets the value of a flash mode parameter.
int
var_get_flash(IN enum flash_var var)
{
    assert(var >= 0 && var < FLASH_LAST);

    return *(int *)(FLASH_PARAM_PAGE+FLASH_OFFSET(var));
}

// this function clears variables before a BASIC program run.
void
var_clear(IN bool flash)
{
    max_vars = 0;
    ram_offset = 0;
    param_offset = 0;
    memset(RAM_VARIABLE_PAGE, 0, sizeof(RAM_VARIABLE_PAGE));

    if (flash) {
        flash_erase_alternate();
        flash_clear_alternate();
        flash_promote_alternate();
    }
}

// this function prints variable memory usage.
void
var_mem(void)
{
    printf("%3d%% ram variable bytes used\n", ram_offset*100/(BASIC_SMALL_PAGE_SIZE-sizeof(int)));
    printf("%3d%% flash parameter bytes used\n", param_offset*100/(BASIC_SMALL_PAGE_SIZE-(FLASH_LAST+1)*sizeof(int)));
    printf("%3d%% variables used\n", max_vars*100/MAX_VARS);
}

