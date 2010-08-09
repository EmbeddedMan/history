// *** vars.c *********************************************************
// this file implements the variable access module, including ram,
// pin, and flash variables, as well as the external pin control and
// access module.

#include "main.h"

// the last word of each flash bank is the generation number
#define GENERATION(p)  *(int *)((p)+BASIC_SMALL_PAGE_SIZE-sizeof(int))

// we always pick the newer flash bank
#define FLASH_PARAM_PAGE  ((GENERATION(FLASH_PARAM1_PAGE)+1 > GENERATION(FLASH_PARAM2_PAGE)+1) ? FLASH_PARAM1_PAGE : FLASH_PARAM2_PAGE)

static byte *alternate_flash_param_page;

// *** pin variables ***

const struct pin pins[] = {
    "dtin0", (enum pin_type)(pin_type_analog_output|pin_type_frequency_output),
    "dtin1", (enum pin_type)(pin_type_analog_output|pin_type_frequency_output),
    "dtin2", (enum pin_type)(pin_type_analog_output|pin_type_frequency_output),
    "dtin3", (enum pin_type)(pin_type_analog_output|pin_type_frequency_output),
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
    "irq0*", pin_type_unused,
    "irq1*", pin_type_default,
    "irq2*", pin_type_unused,
    "irq3*", pin_type_unused,
    "irq4*", pin_type_default,
    "irq5*", pin_type_unused,
    "irq6*", pin_type_unused,
    "irq7*", pin_type_default,
#if MCF52233
    "irq8*", pin_type_unused,
    "irq9*", pin_type_unused,
    "irq10*", pin_type_unused,
    "irq11*", pin_type_default,
    "irq12*", pin_type_unused,
    "irq13*", pin_type_unused,
    "irq14*", pin_type_unused,
    "irq15*", pin_type_unused,
    "gpt0", pin_type_default,
    "gpt1", pin_type_default,
    "gpt2", pin_type_default,
    "gpt3", pin_type_default,
#endif
    "scl", pin_type_default,
    "sda", pin_type_default,
};

struct {
    char *name;
    int *integer;
} systems[] = {
#if ! _WIN32
    "drops", &drops,
    "failures", &failures,
    "nodeid", &zb_nodeid,
    "receives", &receives,
    "retries", &retries,
    "seconds", (int *)&seconds,
    "ticks", (int *)&ticks,
    "transmits", &transmits
#else
    "dummy", NULL
#endif
};

bool var_trace;

#define MAX_VARS  100

#define VAR_NAME_SIZE  15

static
struct var {
    char name[VAR_NAME_SIZE];  // 14 char max variable name
    byte gosubs;
    byte type;
    byte size;  // 4 bytes per integer, 2 bytes per short, 1 byte per byte
    short max_index;
    union {
        int page_offset;
        byte *bytecode;
        struct {
            byte number;
            byte type;  // only 1 bit set
        } pin;
    } u;
    int nodeid;  // for remote variable sets
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
    // reclaim ram space
    if (max_vars > scope) {
        assert(vars[scope].type == code_ram || vars[scope].type == code_nodeid);
        ram_offset = vars[scope].u.page_offset;
        max_vars = scope;
        memset(RAM_VARIABLE_PAGE+ram_offset, 0, sizeof(RAM_VARIABLE_PAGE)-ram_offset);
    }
}

// this function declares a ram, flash, or pin variable!
void
var_declare(IN char *name, IN int gosubs, IN int type, IN int size, IN int max_index, IN int pin_number, IN int pin_type, IN int nodeid)
{
#if ! _WIN32
    int assign;
    int offset;
#endif
    struct var *var;

    assert(name);
    assert(type >= code_deleted && type < code_max);
    assert(max_index);

    if (! run_condition) {
        return;
    }

    if ((type == code_flash || type == code_pin) && gosubs) {
        printf("flash or pin variable declared in sub\n");
        stop();
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
    assert(size == sizeof(byte) || size == sizeof(short) || size == sizeof(int));
    vars[max_vars].size = size;
    vars[max_vars].max_index = max_index;
    vars[max_vars].nodeid = nodeid;

    switch (type) {
        case code_nodeid:
            assert(size == 4);  // integer only
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
                    offset = pin_number - PIN_IRQ8;
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
                case PIN_GPT3:
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
#endif
            break;

        default:
            assert(0);
            break;
    }

    max_vars++;
}

typedef struct remote_set {
    char name[VAR_NAME_SIZE];  // 14 char max variable name
    int index;
    int value;
} remote_set_t;

static remote_set_t set;

static bool remote;

#if ! _WIN32
static void
class_remote_set(int nodeid, int length, byte *buffer)
{
#pragma unused(nodeid)
#pragma unused(length)
    // remember to set the variable as requested by the remote node
    assert(length == sizeof(set));
    set = *(remote_set_t *)buffer;
}

void
var_poll(void)
{
    bool condition;

    if (set.name[0]) {
        assert(! remote);
        remote = true;
        condition = run_condition;
        run_condition = true;

        // set the variable as requested by the remote node
        var_set(set.name, set.index, set.value);
        set.name[0] = '\0';

        run_condition = condition;
        assert(remote);
        remote = false;
    }
}
#endif

static byte lasttx0;
static byte lasttx1;

// this function sets the value of a ram, flash, or pin variable!
void
var_set(IN char *name, IN int index, IN int value)
{
#if ! _WIN32
    int offset;
    remote_set_t set;
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
            case code_nodeid:
                if (! zb_present) {
                    printf("zigbee not present\n");
                    stop();
                    break;
#if ! _WIN32
                } else if (zb_nodeid == -1) {
                    printf("zigbee nodeid not set\n");
                    stop();
                    break;
#endif
                }
                
#if ! _WIN32
                // if we're not being set from a remote node...
                if (! remote) {
                    // forward the variable set request to the remote node
                    strcpy(set.name, name);
                    set.index = index;
                    set.value = value;
                    if (! zb_send(var->nodeid, zb_class_remote_set, sizeof(set), (byte *)&set)) {
                        value = -1;
                    }
                }
                // fall thru
#endif
                
            case code_ram:
                // *** RAM control and access ***
                // set the ram variable to value
                if (var->size == sizeof(int)) {
                    *(int *)(RAM_VARIABLE_PAGE+var->u.page_offset+index*sizeof(int)) = value;
                } else if (var->size == sizeof(short)) {
                    *(short *)(RAM_VARIABLE_PAGE+var->u.page_offset+index*sizeof(short)) = value;
                } else {
                    assert(var->size == sizeof(byte));
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
                if (! (var->u.pin.type & (pin_type_digital_output|pin_type_analog_output|pin_type_frequency_output|pin_type_uart_output))) {
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
                            if (var->u.pin.type == pin_type_analog_output) {
                                // program MCF_PWM_PWMDTY with values 0 (3.3v) to 255 (0v)
                                if (value < 0) {
                                    value = 0;
                                } else if (value > 3300) {
                                    value = 3300;
                                }
                                MCF_PWM_PWMDTY(offset*2) = 255 - value*255/3300;
                            } else if (var->u.pin.type == pin_type_frequency_output) {
                                // program MCF_DTIM_DTRR with fsys_frequency/2 (1Hz) to 1 (fsys_frequency/2)
                                if (value < 0) {
                                    value = 0;
                                } else if (value > fsys_frequency/2) {
                                    value = fsys_frequency/2;
                                }
                                if (value) {
                                    MCF_DTIM_DTRR(offset) = fsys_frequency/2/value;
                                } else {
                                    MCF_DTIM_DTRR(offset) = -1;
                                }
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
#if MCF52233
                        case PIN_IRQ11:
                            offset = var->u.pin.number - PIN_IRQ8;
                            if (value) {
                                MCF_GPIO_SETGP = 1 << offset;
                            } else {
                                MCF_GPIO_CLRGP = ~(1 << offset);
                            }
                            break;
                        case PIN_GPT0:
                        case PIN_GPT1:
                        case PIN_GPT2:
                        case PIN_GPT3:
                            offset = var->u.pin.number - PIN_GPT0;
                            if (value) {
                                MCF_GPIO_SETTA = 1 << offset;
                            } else {
                                MCF_GPIO_CLRTA = ~(1 << offset);
                            }
                            break;
#endif
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
    int i;
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
    if (! var && ! index) {
        // see if this could be a special system variable
        for (i = 0; i < LENGTHOF(systems); i++) {
            if (! strcmp(name, systems[i].name)) {
                return *systems[i].integer;
            }
        }
    }
    if (! var) {
        printf("var '%s' undefined\n", name);
        stop();
    } else if (index >= var->max_index) {
        printf("var '%s' index %d out of range\n", name, index);
        stop();
    } else {
        switch (var->type) {
            case code_nodeid:
            case code_ram:
                // *** RAM control and access ***
                // get the value of the ram variable
                if (var->size == sizeof(int)) {
                    value = *(int *)(RAM_VARIABLE_PAGE+var->u.page_offset+index*sizeof(int));
                } else if (var->size == sizeof(short)) {
                    value = *(unsigned short *)(RAM_VARIABLE_PAGE+var->u.page_offset+index*sizeof(short));
                } else {
                    assert(var->size == sizeof(byte));
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
                        offset = var->u.pin.number - PIN_DTIN0;
                        if (var->u.pin.type == pin_type_analog_output) {
                            value = (255-MCF_PWM_PWMDTY(offset*2))*3300/255;
                        } else if (var->u.pin.type == pin_type_frequency_output) {
                            if (MCF_DTIM_DTRR(offset) == -1) {
                                value = 0;
                            } else if (MCF_DTIM_DTRR(offset)) {
                                value = fsys_frequency/2/MCF_DTIM_DTRR(offset);
                            } else {
                                value = fsys_frequency/2;
                            }                        
                        } else {
                            value = !!(MCF_GPIO_SETTC & (1 << (var->u.pin.number - PIN_DTIN0)));
                        }
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
                            value = adc_result[offset]*3300/32768;
                        } else {
                            value = !!(MCF_GPIO_SETAN & (1 << offset));
                        }
                        break;
                    case PIN_IRQ1:
                    case PIN_IRQ4:
                    case PIN_IRQ7:
                        value = !!(MCF_GPIO_SETNQ & (1 << (var->u.pin.number - PIN_IRQ0)));
                        break;
#if MCF52233
                    case PIN_IRQ11:
                        value = !!(MCF_GPIO_SETGP & (1 << (var->u.pin.number - PIN_IRQ8)));
                        break;
                    case PIN_GPT0:
                    case PIN_GPT1:
                    case PIN_GPT2:
                    case PIN_GPT3:
                        value = !!(MCF_GPIO_SETTA & (1 << (var->u.pin.number - PIN_GPT0)));
                        break;
#endif
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

// this function gets the size of a ram, flash, or pin variable!
int
var_get_size(IN char *name)
{
    int size;
    struct var *var;
    
    size = 1;
    
    var = var_find(name);
    if (! var) {
        printf("var '%s' undefined\n", name);
        stop();
    } else {
        size = var->size;
    }
    
    return size;
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

void
var_initialize(void)
{
#if ! _WIN32
    // enable pwm channel 0, 2, 4, 6
    MCF_PWM_PWME = MCF_PWM_PWME_PWME0|MCF_PWM_PWME_PWME2|MCF_PWM_PWME_PWME4|MCF_PWM_PWME_PWME6;
    
    // set prescales to 4 (6MHz)
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

    zb_register(zb_class_remote_set, class_remote_set);
#endif
}
