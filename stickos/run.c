// *** run.c **********************************************************
// this file implements the bytecode execution engine for stickos,
// including the interrupt service module that invokes BASIC interrupt
// handlers.  it also implements the core of the interactive debugger,
// coupled with the command interpreter and variable access module.

#include "main.h"

int cw7bug;

bool uart_armed[UART_INTS];

bool watch_armed;

bool run_step;
int run_line_number;

int data_line_number;
int data_line_offset;

bool run_condition = true;  // global condition flag

static int run_sleep_ticks;
static int run_sleep_line_number;

#define MAX_INTS  (UART_INTS+MAX_TIMERS+1)

#define WATCH_INT  (MAX_INTS-1)

#define UART_MASK  ((1<<UART_INTS)-1)

#define TIMER_INT(timer)  (UART_INTS+timer)

static uint32 run_isr_enabled;  // bitmask
static uint32 run_isr_pending;  // bitmask
static uint32 run_isr_masked;  // bitmask

static byte *run_isr_bytecode[MAX_INTS];
static int run_isr_length[MAX_INTS];

static int watch_length;
static byte *watch_bytecode;

static int timer_interval_ticks[MAX_TIMERS];
static int timer_last_ticks[MAX_TIMERS];

static bool run_stop;

// these are the conditional scopes

enum scope_type {
    open_if,
    open_while,
    open_isr
};

#define MAX_SCOPES  10

static
struct open_scope {
    enum scope_type type;
    int line_number;

    // revisit -- this is a waste just for for!
    char *for_variable_name;
    int for_variable_index;
    int for_final_value;
    int for_step_value;

    bool condition;
    bool condition_ever;
    bool condition_initial;
    bool condition_restore;
} scopes[MAX_SCOPES];

static int max_scopes;

// these are the gosub/return stack

#define MAX_GOSUBS  10

static
struct running_gosub {
    int return_line_number;
    int return_var_scope;
    int return_scope;
} gosubs[MAX_GOSUBS];

static int max_gosubs;

// these are the parameter stack

#define MAX_STACK  10

static int stack[MAX_STACK];

static int max_stack;

static
void
clear_stack(void)
{
    max_stack = 0;
}

static
void
push_stack(int value)
{
    if (max_stack < MAX_STACK) {
        stack[max_stack] = value;
    } else if (max_stack == MAX_STACK) {
        printf("stack overflow\n");
        stop();
    }
    max_stack++;
}

static
int
pop_stack()
{
    assert(max_stack);
    --max_stack;
    if (max_stack < MAX_STACK) {
        return stack[max_stack];
    } else {
        return 0;
    }
}

// this function reads the next piece of data from the program.
static
int
read_data()
{
    int value;
    struct line *line;
    int line_number;

    if (! run_condition) {
        return 0;
    }

    line_number = data_line_number ? data_line_number-1 : 0;
    for (;;) {
        // find the next line to check
        line = code_next_line(false, &line_number);
        if (! line) {
            printf("out of data\n");
            stop();
            return 0;
        }

        // if it is not a data line...
        if (line->bytecode[0] != code_data) {
            // skip it
            continue;
        }

        data_line_number = line->line_number;

        // if we have data left in the line...
        if (line->length > (int)(1+data_line_offset*(sizeof(int)+1))) {
            value = *(int *)(line->bytecode+1+data_line_offset*(sizeof(int)+1)+1);
            data_line_offset++;
            return value;
        }

        // on to the next line
        data_line_offset = 0;
    }
}

// this function evaluates a bytecode expression.
static
int
evaluate(byte *bytecode_in, int length, OUT int *value)
{
    int lhs;
    int rhs;
    byte code;
    byte *bytecode;

    bytecode = bytecode_in;

    clear_stack();

    while (bytecode < bytecode_in+length && *bytecode != code_comma) {
        code = *bytecode++;
        switch (code) {
            case code_add:
            case code_subtract:
            case code_multiply:
            case code_divide:
            case code_mod:
            case code_shift_right:
            case code_shift_left:
            case code_bitwise_and:
            case code_bitwise_or:
            case code_bitwise_xor:
            case code_logical_and:
            case code_logical_or:
            case code_logical_xor:
            case code_greater:
            case code_less:
            case code_equal:
            case code_greater_or_equal:
            case code_less_or_equal:
            case code_not_equal:
                // get our sides in a deterministic order!
                rhs = pop_stack();
                lhs = pop_stack();
        }

        switch (code) {
            case code_load_and_push_immediate:
            case code_load_and_push_immediate_hex:
                push_stack(*(int *)bytecode);
                bytecode += sizeof(int);
                break;

            case code_load_and_push_var:  // variable name, '\0'
                push_stack(var_get((char *)bytecode, 0));
                bytecode += strlen((char *)bytecode)+1;
                break;

            case code_load_and_push_var_indexed:  // index on stack; variable name, '\0'
                push_stack(var_get((char *)bytecode, pop_stack()));
                bytecode += strlen((char *)bytecode)+1;
                break;

            case code_logical_not:
                push_stack(! pop_stack());
                break;

            case code_bitwise_not:
                push_stack(~ pop_stack());
                break;

            case code_unary_plus:
                push_stack(pop_stack());
                break;

            case code_unary_minus:
                push_stack(-pop_stack());
                break;

            case code_add:
                push_stack(lhs + rhs);
                break;

            case code_subtract:
                push_stack(lhs - rhs);
                break;

            case code_multiply:
                push_stack(lhs * rhs);
                break;

            case code_divide:
                if (! rhs) {
                    if (run_condition) {
                        printf("divide by 0\n");
                        stop();
                    }
                    push_stack(0);
                } else {
                    push_stack(lhs / rhs);
                }
                break;

            case code_mod:
                if (! rhs) {
                    if (run_condition) {
                        printf("divide by 0\n");
                        stop();
                    }
                    push_stack(0);
                } else {
                    push_stack(lhs % rhs);
                }
                break;

            case code_shift_right:
                push_stack(lhs >> rhs);
                break;

            case code_shift_left:
                push_stack(lhs << rhs);
                break;

            case code_bitwise_and:
                push_stack(lhs & rhs);
                break;

            case code_bitwise_or:
                push_stack(lhs | rhs);
                break;

            case code_bitwise_xor:
                push_stack(lhs ^ rhs);
                break;

            case code_logical_and:
                push_stack(lhs && rhs);
                break;

            case code_logical_or:
                push_stack(lhs || rhs);
                break;

            case code_logical_xor:
                push_stack(!!lhs != !!rhs);
                break;

            case code_greater:
                push_stack(lhs > rhs);
                break;

            case code_less:
                push_stack(lhs < rhs);
                break;

            case code_equal:
                push_stack(lhs == rhs);
                break;

            case code_greater_or_equal:
                push_stack(lhs >= rhs);
                break;

            case code_less_or_equal:
                push_stack(lhs <= rhs);
                break;

            case code_not_equal:
                push_stack(lhs != rhs);
                break;

            default:
                assert(0);
                break;
        }
    }

    *value = pop_stack();
    assert(! max_stack);
    return bytecode - bytecode_in;
}

// this function executes a bytecode statement.
bool  // end
run_bytecode(bool immediate, byte *bytecode, int length)
{
    int i;
    int n;
    int uart;
    int baud;
    int data;
    byte parity;
    byte loopback;
    byte device;
    bool end;
    byte code;
#if ! _WIN32
    int divisor;
#endif
    int interrupt;
    int blen;
    int value;
    byte *p;
    int index;
    int oindex;
    char *name;
    int size;
    int timer;
    int interval;
    int csiv;
    int max_index;
    int var_type;
    int pin_number;
    int pin_type;
    int line_number;
    bool hex;
    bool output;
    int isr_length;
    byte *isr_bytecode;
    int nodeid;

    end = false;

    index = 0;

    run_condition = max_scopes ? scopes[max_scopes-1].condition && scopes[max_scopes-1].condition_initial : true;
    run_condition |= immediate;

    code = bytecode[index++];
    switch (code) {
        case code_deleted:
            // nothing to do
            break;

        case code_rem:
            index = length;  // skip the comment
            break;

        case code_on:
        case code_off:
        case code_mask:
        case code_unmask:
            // get the device
            device = *(bytecode+index);
            index++;

            if (device == device_timer) {
                // *** timer control ***
                // get the timer number
                timer = *(int *)(bytecode + index);
                index += sizeof(int);
                assert(timer >= 0 && timer < MAX_TIMERS);

                interrupt = TIMER_INT(timer);

                // if we are enabling the interrupt...
                if (code == code_on) {
                    timer_last_ticks[timer] = ticks;
                }
            } else if (device == device_uart) {
                // *** uart control ***
                // get the uart number
                uart = *(int *)(bytecode + index);
                index += sizeof(int);
                assert(uart >= 0 && uart < MAX_UARTS);

                // get the input/output flag
                output = *(bytecode+index);
                index++;

                interrupt = UART_INT(uart, output);
            } else if (device == device_watch) {
                interrupt = WATCH_INT;

                // this is the expression to watch
                watch_length = *(int *)(bytecode + index);
                index += sizeof(int);
                watch_bytecode = bytecode+index;
                index += watch_length;

                assert(bytecode[index] == code_comma);
                index++;
            } else {
                assert(0);
            }

            if (code == code_on) {
                // this is the handler
                isr_length = *(int *)(bytecode + index);
                index += sizeof(int);
                isr_bytecode = bytecode+index;
                index += isr_length;
            }

            if (run_condition) {
                if (code == code_mask) {
                    run_isr_masked |= 1<<interrupt;
                } else if (code == code_unmask) {
                    run_isr_masked &= ~(1<<interrupt);
                } else if (code == code_off) {
                    run_isr_enabled &= ~(1<<interrupt);
                    run_isr_length[interrupt] = 0;
                    run_isr_bytecode[interrupt] = NULL;
                } else {
                    assert(code == code_on);
                    run_isr_enabled |= 1<<interrupt;
                    run_isr_length[interrupt] = isr_length;
                    run_isr_bytecode[interrupt] = isr_bytecode;
                }
            }
            break;

        case code_configure:
            // get the device
            device = *(bytecode+index);
            index++;

            if (device == device_timer) {
                // *** timer control ***
                // get the timer number and interval
                timer = *(int *)(bytecode+index);
                index += sizeof(int);
                interval = *(int *)(bytecode+index);
                index += sizeof(int);

                if (run_condition) {
                    timer_interval_ticks[timer] = interval;
                }

            } else if (device == device_uart) {
                // *** uart control ***
                // get the uart number and protocol and optional loopback specifier
                uart = *(int *)(bytecode+index);
                index += sizeof(int);
                baud = *(int *)(bytecode+index);
                index += sizeof(int);
                data = *(int *)(bytecode+index);
                index += sizeof(int);
                parity = *(bytecode+index);
                index++;
                loopback = *(bytecode+index);
                index++;

                assert(uart == 0 || uart == 1);
#if ! _WIN32
                if (run_condition) {
                    // configure the uart for the requested protocol and speed
                    MCF_UART_UCR(uart) = MCF_UART_UCR_RESET_MR|MCF_UART_UCR_TX_DISABLED|MCF_UART_UCR_RX_DISABLED;

                    MCF_UART_UMR(uart)/*1*/ = (parity==0?MCF_UART_UMR_PM_ODD:(parity==1?MCF_UART_UMR_PM_EVEN:MCF_UART_UMR_PM_NONE))|(data-5);
                    MCF_UART_UMR(uart)/*2*/ = (loopback?MCF_UART_UMR_CM_LOCAL_LOOP:0)|MCF_UART_UMR_SB_STOP_BITS_2;

                    MCF_UART_UCSR(uart) = MCF_UART_UCSR_RCS_SYS_CLK|MCF_UART_UCSR_TCS_SYS_CLK;

                    divisor = fsys_frequency/baud/32;
                    assert(divisor < 0x10000);
                    MCF_UART_UBG1(uart) = (uint8)(divisor/0x100);
                    MCF_UART_UBG2(uart) = (uint8)(divisor%0x100);

                    MCF_UART_UCR(uart) = MCF_UART_UCR_TX_ENABLED|MCF_UART_UCR_RX_ENABLED;
                }
#endif
            } else if (device == device_qspi) {
                // get the timer number and interval
                csiv = *(int *)(bytecode+index);
                index += sizeof(int);

                if (run_condition) {
#if ! _WIN32
                    qspi_inactive(csiv);
#endif
                }
                
            } else {
                assert(0);
            }
            break;

        case code_assert:
            // *** interactive debugger ***
            // evaluate the assertion expression
            index += evaluate(bytecode+index, length-index, &value);
            if (run_condition && ! value) {
                printf("assertion failed\n");
                stop();
            }
            break;

        case code_read:
            cw7bug++;  // CW7 BUG
            // while there are more variables to read to...
            do {
                // skip commas
                if (bytecode[index] == code_comma) {
                    index++;
                }

                // if we're reading into a simple variable...
                if (bytecode[index] == code_load_and_push_var) {
                    // use array index 0
                    index++;
                    max_index = 0;
                } else {
                    // we're reading into an array element
                    assert(bytecode[index] == code_load_and_push_var_indexed);
                    index++;

                    blen = *(int *)(bytecode+index);
                    index += sizeof(int);

                    // evaluate the array index
                    index += evaluate(bytecode+index, blen, &max_index);
                }

                // get the variable name
                name = (char *)bytecode+index;
                index += strlen(name)+1;

                // get the next data
                value = read_data();

                // assign the variable with the next data
                var_set(name, max_index, value);
            } while (index < length && bytecode[index] == code_comma);
            break;

        case code_data:
            index = length;  // skip the data
            break;

        case code_restore:
            if (run_condition) {
                if (bytecode[index]) {
                    line_number = code_line(code_label, bytecode+index);
                    if (! line_number) {
                        printf("missing label\n");
                        goto XXX_SKIP_XXX;
                    }
                } else {
                    line_number = 0;
                }
                data_line_number = line_number;
                data_line_offset = 0;
            }

            index += strlen((char *)bytecode+index)+1;
            break;

        case code_dim:
            cw7bug++;  // CW7 BUG
            // while there are more variables to dimension...
            do {
                // skip commas
                if (bytecode[index] == code_comma) {
                    index++;
                }

                // if we're dimensioning a simple variable
                if (bytecode[index] == code_load_and_push_var) {
                    // set the array length to 1
                    index++;
                    max_index = 1;
                } else {
                    // we're dimensioning an array
                    assert(bytecode[index] == code_load_and_push_var_indexed);
                    index++;

                    blen = *(int *)(bytecode+index);
                    index += sizeof(int);

                    // evaluate the array length
                    index += evaluate(bytecode+index, blen, &max_index);
                }

                // get the variable name
                name = (char *)bytecode+index;
                index += strlen(name)+1;

                // get the size and type specifier
                size = bytecode[index++];
                var_type = bytecode[index++];

                assert(size == sizeof(byte) || size == sizeof(short) || size == sizeof(int));

                // if this is a memory variable...
                if (var_type == code_ram || var_type == code_flash) {
                    var_declare(name, max_gosubs, var_type, size, max_index, 0, 0, -1);
                // otherwise, if this is a pin variable...
                } else if (var_type == code_pin) {
                    // get the pin number (from the pin name) and pin type (from the pin usage)
                    pin_number = bytecode[index++];
                    pin_type = bytecode[index++];

                    assert(pin_number >= 0 && pin_number < PIN_LAST);

                    var_declare(name, max_gosubs, var_type, size, max_index, pin_number, pin_type, -1);
                } else {
                    // this is a remote variable
                    assert(var_type == code_nodeid);
                    
                    nodeid = *(int *)(bytecode+index);
                    index += sizeof(int);
                    
                    var_declare(name, max_gosubs, var_type, size, max_index, 0, 0, nodeid);
                }
            } while (index < length && bytecode[index] == code_comma);
            break;

        case code_let:
            cw7bug++;  // CW7 BUG
            // while there are more items to print...
            do {
                // skip commas
                if (bytecode[index] == code_comma) {
                    index++;
                }

                // revisit -- share with code_for!
                // if we're assigning a simple variable...
                if (bytecode[index] == code_load_and_push_var) {
                    // use array index 0
                    index++;
                    max_index = 0;
                } else {
                    // we're assigning an array element
                    assert(bytecode[index] == code_load_and_push_var_indexed);
                    index++;

                    blen = *(int *)(bytecode+index);
                    index += sizeof(int);

                    // evaluate the array index
                    index += evaluate(bytecode+index, blen, &max_index);
                }

                // get the variable name
                name = (char *)bytecode+index;
                index += strlen(name)+1;

                // evaluate the assignment expression
                index += evaluate(bytecode+index, length-index, &value);

                // assign the variable with the assignment expression
                var_set(name, max_index, value);
            } while (index < length && bytecode[index] == code_comma);
            break;

        case code_print:
            cw7bug++;  // CW7 BUG
            hex = false;
            // while there are more items to print...
            do {
                // skip commas
                if (bytecode[index] == code_comma) {
                    if (run_condition) {
                        printf(" ");
                    }
                    index++;
                }

                // if we're changing format specifiers...
                if (bytecode[index] == code_hex || bytecode[index] == code_dec) {
                    hex = bytecode[index] == code_hex;
                    index++;
                }
                
                // if we're printing a string...
                if (bytecode[index] == code_string) {
                    index++;

                    if (run_condition) {
                        printf("%s", bytecode+index);
                    }
                    index += strlen((char *)bytecode+index)+1;
                } else {
                    // we're printing an expression
                    assert(bytecode[index] == code_expression);
                    index++;

                    // evaluate the expression
                    index += evaluate(bytecode+index, length-index, &value);
                    if (run_condition) {
                        if (hex) {
                            printf("0x%x", value);
                        } else {
                            printf("%d", value);
                        }
                    }
                }
            } while (index < length && bytecode[index] == code_comma);
            if (run_condition) {
                printf("\n");
            }
            break;
#endif
            
        case code_qspi:
            // we'll walk the variable list twice
            oindex = index;
            
            // while there are more variables to qspi from...
            p = big_buffer;
            do {
                // skip commas
                if (bytecode[index] == code_comma) {
                    index++;
                }

                // if we're reading into a simple variable...
                if (bytecode[index] == code_load_and_push_var) {
                    // use array index 0
                    index++;
                    max_index = 0;
                } else {
                    // we're reading into an array element
                    assert(bytecode[index] == code_load_and_push_var_indexed);
                    index++;

                    blen = *(int *)(bytecode+index);
                    index += sizeof(int);

                    // evaluate the array index
                    index += evaluate(bytecode+index, blen, &max_index);
                }

                // get the variable name
                name = (char *)bytecode+index;
                index += strlen(name)+1;

                // get the variable data and size
                value = var_get(name, max_index);
                size = var_get_size(name);
                
                // pack it into the qspi buffer
                if (size == sizeof(byte)) {
                    *(byte *)p = value;
                    p += sizeof(byte);
                } else if (size == sizeof(short)) {
                    *(short *)p = value;
                    p += sizeof(short);
                } else {
                    assert(size == sizeof(int));
                    *(int *)p = value;
                    p += sizeof(int);
                }
            } while (index < length && bytecode[index] == code_comma);
            
            // perform the qspi transfer
#if ! _WIN32
            qspi_transfer(big_buffer, p-big_buffer);
#endif

            // now update the variables
            index = oindex;
            
            // while there are more variables to qspi to...
            p = big_buffer;
            do {
                // skip commas
                if (bytecode[index] == code_comma) {
                    index++;
                }

                // if we're reading into a simple variable...
                if (bytecode[index] == code_load_and_push_var) {
                    // use array index 0
                    index++;
                    max_index = 0;
                } else {
                    // we're reading into an array element
                    assert(bytecode[index] == code_load_and_push_var_indexed);
                    index++;

                    blen = *(int *)(bytecode+index);
                    index += sizeof(int);

                    // evaluate the array index
                    index += evaluate(bytecode+index, blen, &max_index);
                }

                // get the variable name
                name = (char *)bytecode+index;
                index += strlen(name)+1;

                // get the variable size
                size = var_get_size(name);
                
                // unpack it from the qspi buffer
                if (size == sizeof(byte)) {
                    value = *(byte *)p;
                    p += sizeof(byte);
                } else if (size == sizeof(short)) {
                    value = *(short *)p;
                    p += sizeof(short);
                } else {
                    assert(size == sizeof(int));
                    value = *(int *)p;
                    p += sizeof(int);
                }
                
                // set the variable
                var_set(name, max_index, value);
            } while (index < length && bytecode[index] == code_comma);
            break;

        case code_if:
            if (max_scopes >= MAX_SCOPES-2) {
                printf("too many scopes\n");
                goto XXX_SKIP_XXX;
            }
            // open a new conditional scope
            scopes[max_scopes].line_number = run_line_number;
            scopes[max_scopes].type = open_if;
            max_scopes++;

            // evaluate the condition
            index += evaluate(bytecode+index, length-index, &value);

            // incorporate the condition
            scopes[max_scopes-1].condition = !! value;
            scopes[max_scopes-1].condition_ever = !! value;
            scopes[max_scopes-1].condition_initial = run_condition;
            scopes[max_scopes-1].condition_restore = false;
            break;

        case code_elseif:
            if (! max_scopes || scopes[max_scopes-1].type != open_if) {
                printf("mismatched elseif\n");
                goto XXX_SKIP_XXX;
            }

            // reevaluate the condition
            run_condition = scopes[max_scopes-1].condition_initial;
            index += evaluate(bytecode+index, length-index, &value);

            // if the condition has ever been true...
            if (scopes[max_scopes-1].condition_ever) {
                // elseif's remain false, regardless of the new condition
                scopes[max_scopes-1].condition = false;
            } else {
                scopes[max_scopes-1].condition = !! value;
                scopes[max_scopes-1].condition_ever |= !! value;
            }
            break;

        case code_else:
            if (! max_scopes || scopes[max_scopes-1].type != open_if) {
                printf("mismatched else\n");
                goto XXX_SKIP_XXX;
            }

            // flip the condition
            scopes[max_scopes-1].condition = ! scopes[max_scopes-1].condition_ever;
            break;

        case code_endif:
            if (! max_scopes) {
                printf("missing if\n");
                goto XXX_SKIP_XXX;
            }
            if (scopes[max_scopes-1].type != open_if) {
                printf("mismatched endif\n");
                goto XXX_SKIP_XXX;
            }

            // close the conditional scope
            assert(max_scopes);
            max_scopes--;
            break;

        case code_while:
        case code_for:
        case code_do:
            if (max_scopes >= MAX_SCOPES-2) {
                printf("too many scopes\n");
                goto XXX_SKIP_XXX;
            }
            // open a new conditional scope
            scopes[max_scopes].line_number = run_line_number;
            scopes[max_scopes].type = open_while;
            max_scopes++;

            // if this is a for loop...
            if (code == code_for) {
                // revisit -- share with code_let!
                // if we're assigning a simple variable...
                if (bytecode[index] == code_load_and_push_var) {
                    // we use array index 0
                    index++;
                    max_index = 0;
                } else {
                    // we're assigning an array element
                    assert(bytecode[index] == code_load_and_push_var_indexed);
                    index++;

                    blen = *(int *)(bytecode+index);
                    index += sizeof(int);

                    // evaluate the array index
                    index += evaluate(bytecode+index, blen, &max_index);
                }

                // get the variable name
                name = (char *)bytecode+index;
                index += strlen(name)+1;

                // evaluate and set the initial value
                index += evaluate(bytecode+index, length-index, &value);
                var_set(name, max_index, value);

                assert(name);
                scopes[max_scopes-1].for_variable_name = name;
                scopes[max_scopes-1].for_variable_index = max_index;

                assert(bytecode[index] == code_comma);
                index++;

                // evaluate the final value
                index += evaluate(bytecode+index, length-index, &scopes[max_scopes-1].for_final_value);

                // if there is a step value...
                if (index < length) {
                    assert(bytecode[index] == code_comma);
                    index++;

                    // evaluate the step value
                    index += evaluate(bytecode+index, length-index, &scopes[max_scopes-1].for_step_value);
                } else {
                    scopes[max_scopes-1].for_step_value = 1;
                }

                // set the initial condition
                if (scopes[max_scopes-1].for_step_value >= 0) {
                    value = value <= scopes[max_scopes-1].for_final_value;
                } else {
                    value = value >= scopes[max_scopes-1].for_final_value;
                }
            } else if (code == code_while) {
                scopes[max_scopes-1].for_variable_name = NULL;
                scopes[max_scopes-1].for_variable_index = 0;
                scopes[max_scopes-1].for_final_value = 0;
                scopes[max_scopes-1].for_step_value = 0;

                // evaluate the condition
                index += evaluate(bytecode+index, length-index, &value);
            } else {
                assert(code == code_do);

                value = 1;
            }

            // incorporate the condition
            scopes[max_scopes-1].condition = !! value;
            scopes[max_scopes-1].condition_ever = !! value;
            scopes[max_scopes-1].condition_initial = run_condition;
            scopes[max_scopes-1].condition_restore = false;
            break;

        case code_break:
        case code_continue:
            // get the break/continue level
            n = *(int *)(bytecode + index);
            index += sizeof(int);
            assert(n);

            if (! max_scopes) {
                printf("missing while/for\n");
                goto XXX_SKIP_XXX;
            }

            // find the outermost while loop
            for (i = max_scopes-1; i >= 0; i--) {
                if (scopes[i].type == open_while) {
                    if (! --n) {
                        break;
                    }
                }
            }
            if (i < 0) {
                printf("break/continue without while/for\n");
                goto XXX_SKIP_XXX;
            }

            assert(! n);
            if (run_condition) {
                // negate all conditions
                while (i < max_scopes) {
                    assert(scopes[i].condition_initial);
                    assert(scopes[i].condition);
                    if (code == code_break) {
                        scopes[i].condition_initial = false;
                    } else {
                        scopes[i].condition = false;
                        if (! n) {
                            // the outermost continue gets to run again
                            assert(scopes[i].type == open_while);
                            scopes[i].condition_restore = true;
                        }
                    }
                    i++;
                    n++;
                }
            }
            break;

        case code_next:
        case code_endwhile:
        case code_until:
            if (! max_scopes) {
                printf("missing while/for\n");
                goto XXX_SKIP_XXX;
            }
            if (scopes[max_scopes-1].type != open_while ||
                (code == code_next && ! scopes[max_scopes-1].for_variable_name) ||
                ((code == code_endwhile || code == code_until) && scopes[max_scopes-1].for_variable_name)) {
                printf("mismatched endwhile/until/next\n");
                goto XXX_SKIP_XXX;
            }

            // close the conditional scope
            assert(max_scopes);
            max_scopes--;

            if (scopes[max_scopes].condition_restore) {
                scopes[max_scopes].condition = true;
                run_condition = true;
            }

            // if this is an until loop...
            if (code == code_until) {
                // evaluate the condition
                index += evaluate(bytecode+index, length-index, &value);
            }

            if (run_condition) {
                // if this is a for loop...
                if (code == code_next) {
                    value = var_get(scopes[max_scopes].for_variable_name, scopes[max_scopes].for_variable_index);
                    value += scopes[max_scopes].for_step_value;

                    // if the stepped value is still in range...
                    if (scopes[max_scopes].for_step_value >= 0) {
                        if (value <= scopes[max_scopes].for_final_value) {
                            // set the variable to the stepped value
                            var_set(scopes[max_scopes].for_variable_name, scopes[max_scopes].for_variable_index, value);
                        } else {
                            run_condition = false;
                        }
                    } else {
                        if (value >= scopes[max_scopes].for_final_value) {
                            // set the variable to the stepped value
                            var_set(scopes[max_scopes].for_variable_name, scopes[max_scopes].for_variable_index, value);
                        } else {
                            run_condition = false;
                        }
                    }

                    // conditionally go back for more (skip the for line)!
                    if (run_condition) {
                        // N.B we re-open the scope here!
                        run_line_number = scopes[max_scopes++].line_number;
                    }
                } else if (code == code_endwhile) {
                    // go back for more (including the while line)!
                    run_line_number = scopes[max_scopes].line_number-1;
                } else {
                    // if the condition has not been achieved...
                    if (! value) {
                        // go back for more!
                        // N.B we re-open the scope here!
                        run_line_number = scopes[max_scopes++].line_number;
                    }
                }
            }
            break;

        case code_gosub:
            if (run_condition) {
                if (max_gosubs >= MAX_GOSUBS) {
                    printf("too many gosubs\n");
                    goto XXX_SKIP_XXX;
                }

                // open a new gosub scope
                gosubs[max_gosubs].return_line_number = run_line_number;
                gosubs[max_gosubs].return_var_scope = var_open_scope();
                gosubs[max_gosubs].return_scope = max_scopes;
                max_gosubs++;

                // jump to the gosub line
                line_number = code_line(code_sub, bytecode+index);
                if (! line_number) {
                    printf("missing sub\n");
                    goto XXX_SKIP_XXX;
                }
                run_line_number = line_number-1;
            }
            index += strlen((char *)bytecode+index)+1;
            break;

        case code_label:
        case code_sub:
            // we do nothing here
            // revisit -- we could push false conditional scope for sub (and pop on endsub)
            index += strlen((char *)bytecode+index)+1;
            break;

        case code_return:
        case code_endsub:
            if (run_condition) {
                if (! max_gosubs) {
                    printf("missing gosub\n");
                    goto XXX_SKIP_XXX;
                }

                // close the gosub scope
                assert(max_gosubs);
                max_gosubs--;
                var_close_scope(gosubs[max_gosubs].return_var_scope);
                max_scopes = gosubs[max_gosubs].return_scope;

                // and jump to the return line
                run_line_number = gosubs[max_gosubs].return_line_number;
            }
            break;

        case code_sleep:
            // N.B. sleeps occur in the main loop so we can service interrupts
            // evaluate the sleep time
            index += evaluate(bytecode+index, length-index, &value);
            if (run_condition) {
                // prepare to sleep
                run_sleep_ticks = ticks+value;
                assert(! run_sleep_line_number);
                run_sleep_line_number = run_line_number;
            }
            break;

        case code_stop:
            if (run_condition) {
                stop();
            }
            break;

        case code_end:
            if (run_condition) {
                // we end in a stopped infinite loop
                run_line_number = run_line_number-1;
                end = true;
            }
            break;

        default:
            assert(0);
            break;
    }

    assert(index == length);
    return end;

XXX_SKIP_XXX:
    stop();
    return false;
}

// this function executes a BASIC program!!!
bool
run(bool cont, int start_line_number)
{
#if ! _WIN32
    byte usr;
#endif
    int i;
    int tick;
    bool isr;
    uint32 mask;
    int length;
    int value;
    int line_number;
    int last_tick;
    int tx_empty;
    struct line *line;

    last_tick = ticks;

    if (! cont) {
        // prepare for a new run

        var_clear(false);

        max_scopes = 0;
        max_gosubs = 0;

        run_isr_enabled = 0;
        run_isr_pending = 0;
        run_isr_masked = 0;
        for (i = 0; i < MAX_INTS; i++) {
            run_isr_bytecode[i] = NULL;
            run_isr_length[i] = 0;
        }

        for (i = 0; i < MAX_TIMERS; i++) {
            timer_interval_ticks[i] = 0;
            timer_last_ticks[i] = ticks;
        }

        memset(uart_armed, 1, sizeof(uart_armed));

        watch_armed = true;

        run_line_number = start_line_number?start_line_number-1:0;

        data_line_number = 0;
        data_line_offset = 0;
    } else {
        // continue a stopped run
        run_line_number = start_line_number?start_line_number-1:run_line_number;
    }
    run_sleep_line_number = 0;

    isr = false;
    run_stop = 1;

    // this is the main run loop that executes program statements!
    for (;;) {
        // if we're still sleeping...
        if (run_line_number && run_sleep_line_number == run_line_number) {
            if (ticks >= run_sleep_ticks) {
                run_sleep_line_number = 0;
            }
        } else {
            // we're running; get the next statement to execute
            line = code_next_line(false, &run_line_number);
            if (! line) {
                // we fell off the end
                break;
            }

            // run the statement's bytecodes
            line_number = run_line_number;
            if (run_bytecode(false, line->bytecode, line->length)) {
                // we explicitly ended
                stop();
                break;
            }
        }

        // if we're in single-step mode...
        if (run_step) {
            // stop after every line
            stop();
        }

        // if we stopped for any reason...
        if (! run_stop) {
            printf("STOP at line %d!\n", line_number);
            break;
        }

        // *** interrupt service module ***
        
        // if it has been a tick since we last checked...
        tick = ticks;
        if (tick != last_tick) {
#if ! _WIN32
            // see if the sleep switch was pressed
            basic_poll();
            
            led_unknown_progress();
#endif

            // see if any timers have expired
            for (i = 0; i < MAX_TIMERS; i++) {
                // if the timer is set...
                if (run_isr_bytecode[TIMER_INT(i)] && timer_interval_ticks[i]) {
                    // if its time is due...
                    if ((int)(tick - timer_last_ticks[i]) >= timer_interval_ticks[i]) {
                        // if it is not already pending...
                        if (! (run_isr_pending & (1 << TIMER_INT(i)))) {
                            // mark the interrupt as pending
                            run_isr_pending |= 1<<TIMER_INT(i);
                            timer_last_ticks[i] = tick;
                        }
                    }
                } else {
                    timer_last_ticks[i] = tick;
                }
            }
            last_tick = tick;
        }

        // if any uart ints are enabled...
        if (run_isr_enabled & UART_MASK) {
            
#if ! _WIN32
                usr = MCF_UART_USR(i);

#if ! _WIN32
                if (run_isr_bytecode[UART_INT(i, true)] && uart_armed[UART_INT(i, true)] && (usr & MCF_UART_USR_TXEMP)) {
                if (run_isr_bytecode[UART_INT(i, true)] && uart_armed[UART_INT(i, true)] && (tx_empty & (1<<UART_INT(i, true)))) {
                    // mark the interrupt as pending
                    run_isr_pending |= 1<<UART_INT(i, true);
                    uart_armed[UART_INT(i, true)] = false;
                }
                // if the uart receiver is not empty...
                if (run_isr_bytecode[UART_INT(i, false)] && uart_armed[UART_INT(i, false)] && (usr & MCF_UART_USR_RXRDY)) {
                if (run_isr_bytecode[UART_INT(i, false)] && uart_armed[UART_INT(i, false)] && (rx_full & (1<<UART_INT(i, true)))) {
                    // mark the interrupt as pending
                    run_isr_pending |= 1<<UART_INT(i, false);
                    uart_armed[UART_INT(i, false)] = false;
                }
#endif
            }
        }

        // if the watch int is enabled
        if (run_isr_enabled & (1 << WATCH_INT)) {
            condition = run_condition;
            run_condition = true;

            length = evaluate(watch_bytecode, watch_length, &value);
            assert(length == watch_length);

            run_condition = condition;

            // if the watch is non-0...
            if (value) {
                // if this transition has not yet been delivered...
                if (watch_armed) {
                    // mark the interrupt as pending
                    run_isr_pending |= 1 << WATCH_INT;
                    watch_armed = false;
                }
            } else {
                watch_armed = true;
            }
        }

        // if we're not already running an isr...
        if (run_isr_pending && ! isr) {
            for (i = 0; i < MAX_INTS; i++) {
                // if we have an isr to run...
                mask = 1<<i;
                if ((run_isr_pending & mask) && ! (run_isr_masked & mask) && run_isr_bytecode[i]) {
                    // open a new temporary (unconditional) scope
                    assert(max_scopes < MAX_SCOPES);
                    scopes[max_scopes].line_number = run_line_number;
                    scopes[max_scopes].type = open_isr;
                    max_scopes++;

                    scopes[max_scopes-1].condition = true;
                    scopes[max_scopes-1].condition_ever = true;
                    scopes[max_scopes-1].condition_initial = true;
                    scopes[max_scopes-1].condition_restore = false;

                    // *** interrupt handler ***
                    
                    // run the isr, starting with the handler statement (which might be a gosub)
                    assert(! isr);
                    isr = true;
                    run_line_number = -1;
                    if (run_bytecode(false, run_isr_bytecode[i], run_isr_length[i])) {
                        stop();
                    }

                    run_isr_pending &= ~(1<<i);

                    break;
                }
            }
        }

        // if we're returning from our isr...
        if (run_line_number == -1) {
            run_line_number = scopes[max_scopes-1].line_number;

            // close the temporary (unconditional) scope
            assert(max_scopes);
            assert(scopes[max_scopes-1].type == open_isr);
            max_scopes--;

            assert(isr);
            isr = false;
        }
    }

    if (! run_stop) {
        // we stopped or ended
        return false;
    } else {
        // we fell off the end
        while (max_scopes) {
            if (scopes[max_scopes-1].type != open_isr) {
                printf("missing %s\n", scopes[max_scopes-1].type == open_if ? "endif" : "endwhile/next");
            }
            assert(max_scopes);
            max_scopes--;
        }

        run_stop = false;
        return true;  // we completed
    }
}

// this function stops execution of the BASIC program.
void
stop()
{
    run_stop = 0;
}

