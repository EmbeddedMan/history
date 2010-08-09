#include "main.h"

// *** run ******************************************************************

#if BASIC

int cw7bug;

bool uart_armed[UART_INTS];

bool run_step;
int run_line_number;  // static so we can continue
int run_line_count;

int data_line_number;
int data_line_offset;

bool run_condition;

static int run_sleep_ticks;
static int run_sleep_line_number;

#define MAX_INTS  (MAX_TIMERS+UART_INTS)

#define TIMER_INT(timer)  (timer)

static bool run_isr_pending[MAX_INTS];
static bool run_isr_masked[MAX_INTS];
static byte *run_isr_bytecode[MAX_INTS];
static int run_isr_length[MAX_INTS];

static int timer_interval_ticks[MAX_TIMERS];
static int timer_last_ticks[MAX_TIMERS];

// *** conditional scopes ***

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
} scopes[MAX_SCOPES];

static int max_scopes;

// *** gosub/return stack ***

#define MAX_GOSUBS  10

static
struct running_gosub {
    int return_line_number;
    int return_var_scope;
    int return_scope;
} gosubs[MAX_GOSUBS];

static int max_gosubs;

// *** parameter stack ***

#define MAX_STACK  10

static int stack[MAX_STACK];

static int max_stack;

static bool run_stop;

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

// *** read data ***

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
        line = code_next_line(false, &line_number);
        if (! line) {
            printf("out of data\n");
            stop();
            return 0;
        }

        if (line->bytecode[0] != code_data) {
            continue;
        }

        data_line_number = line->line_number;

        if (line->length > (int)(1+data_line_offset*(sizeof(int)+1))) {
            // we have data
            value = *(int *)(line->bytecode+1+data_line_offset*(sizeof(int)+1)+1);
            data_line_offset++;
            return value;
        }

        data_line_offset = 0;
    }
}

// *** evaluate ***

static
int
evaluate(byte *bytecode_in, int length, OUT int *value)
{
    int lhs;
    int rhs;
    byte code;
    byte *bytecode;

    bytecode = bytecode_in;

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
            case code_load_and_push_immediate:  // integer
            case code_load_and_push_immediate_hex:  // integer
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

// *** run ***

// revisit -- there should only be one of thse, shared between startup, timer, flash, run!
#define FSYS  48000000  // frequency of M52221DEMO board

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
    int index;
    char *name;
    int size;
    int timer;
    int interval;
    int max_index;
    int var_type;
    int pin_number;
    int pin_type;
    int line_number;
    bool output;
    int isr_length;
    byte *isr_bytecode;
    
    end = false;

    index = 0;

    run_condition = max_scopes ? scopes[max_scopes-1].condition && scopes[max_scopes-1].condition_initial : true;
    run_condition |= immediate;

    code = bytecode[index++];
    switch (code) {
        case code_deleted:
            break;

        case code_rem:
            index = length;  // skip it
            break;

        case code_on:
        case code_off:
        case code_mask:
        case code_unmask:
            device = *(bytecode+index);
            index++;

            if (device == device_timer) {
                timer = *(int *)(bytecode + index);
                index += sizeof(int);
                assert(timer >= 0 && timer < MAX_TIMERS);
                
                interrupt = TIMER_INT(timer);
                if (code == code_on) {
                    timer_last_ticks[timer] = ticks;

                    isr_length = *(int *)(bytecode + index);
                    index += sizeof(int);
                    isr_bytecode = bytecode+index;
                    index += isr_length;
                }
            } else if (device == device_uart) {
                uart = *(int *)(bytecode + index);
                index += sizeof(int);
                assert(uart >= 0 && uart < MAX_UARTS);
                
                output = *(bytecode+index);
                index++;
                
                interrupt = UART_INT(uart, output);
                if (code == code_on) {
                    isr_length = *(int *)(bytecode + index);
                    index += sizeof(int);
                    isr_bytecode = bytecode+index;
                    index += isr_length;
                }
            } else {
                assert(0);
            }

            if (run_condition) {
                if (code == code_mask) {
                    run_isr_masked[interrupt] = true;
                } else if (code == code_unmask) {
                    run_isr_masked[interrupt] = false;
                } else if (code == code_off) {
                    run_isr_length[interrupt] = 0;
                    run_isr_bytecode[interrupt] = NULL;
                } else {
                    assert(code == code_on);
                    run_isr_length[interrupt] = isr_length;
                    run_isr_bytecode[interrupt] = isr_bytecode;
                }
            }
            break;

        case code_configure:
            device = *(bytecode+index);
            index++;

            if (device == device_timer) {
                timer = *(int *)(bytecode+index);
                index += sizeof(int);
                interval = *(int *)(bytecode+index);
                index += sizeof(int);

                if (run_condition) {
                    timer_interval_ticks[timer] = interval;
                }
            } else if (device == device_uart) {
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
                    MCF_UART_UCR(uart) = MCF_UART_UCR_RESET_MR|MCF_UART_UCR_TX_DISABLED|MCF_UART_UCR_RX_DISABLED;

                    MCF_UART_UMR(uart)/*1*/ = (parity==0?MCF_UART_UMR_PM_ODD:(parity==1?MCF_UART_UMR_PM_EVEN:MCF_UART_UMR_PM_NONE))|(data-5);
                    MCF_UART_UMR(uart)/*2*/ = (loopback?MCF_UART_UMR_CM_LOCAL_LOOP:0)|MCF_UART_UMR_SB_STOP_BITS_2;
                    
                    MCF_UART_UCSR(uart) = MCF_UART_UCSR_RCS_SYS_CLK|MCF_UART_UCSR_TCS_SYS_CLK;
                    
                    divisor = FSYS/baud/32;
                    assert(divisor < 0x10000);
                    MCF_UART_UBG1(uart) = (uint8)(divisor/0x100);
                    MCF_UART_UBG2(uart) = (uint8)(divisor%0x100);
                    
                    MCF_UART_UCR(uart) = MCF_UART_UCR_TX_ENABLED|MCF_UART_UCR_RX_ENABLED;
                }
#endif
            } else {
                assert(0);
            }
            break;

        case code_assert:
            index += evaluate(bytecode+index, length-index, &value);
            if (run_condition && ! value) {
                printf("assertion failed\n");
                stop();
            }
            break;

        case code_read:
            cw7bug++;  // CW7 BUG
            do {
                if (bytecode[index] == code_comma) {
                    index++;
                }

                if (bytecode[index] == code_load_and_push_var) {
                    index++;
                    max_index = 0;
                } else {
                    assert(bytecode[index] == code_load_and_push_var_indexed);
                    index++;

                    blen = *(int *)(bytecode+index);
                    index += sizeof(int);

                    index += evaluate(bytecode+index, blen, &max_index);
                }
                
                name = (char *)bytecode+index;
                index += strlen(name)+1;

                // get the next data
                value = read_data();

                // assign the variable with the next data
                var_set(name, max_index, value);
            } while (index < length && bytecode[index] == code_comma);
            break;

        case code_data:
            index = length;  // skip it
            break;

        case code_restore:
            if (run_condition) {
                data_line_number = *(int *)(bytecode+index);
                data_line_offset = 0;
            }
            index += sizeof(int);
            break;

        case code_dim:
            cw7bug++;  // CW7 BUG
            do {
                if (bytecode[index] == code_comma) {
                    index++;
                }

                if (bytecode[index] == code_load_and_push_var) {
                    index++;
                    max_index = 1;
                } else {
                    assert(bytecode[index] == code_load_and_push_var_indexed);
                    index++;

                    blen = *(int *)(bytecode+index);
                    index += sizeof(int);

                    index += evaluate(bytecode+index, blen, &max_index);
                }
                
                name = (char *)bytecode+index;
                index += strlen(name)+1;
                
                if (bytecode[index] == code_as) {
                    index++;

                    size = bytecode[index++];
                    var_type = bytecode[index++];
                    
                    assert(size == sizeof(byte) || size == sizeof(int));
                    
                    if (var_type == code_ram || var_type == code_flash) {
                        var_declare(name, max_gosubs, var_type, size, max_index, 0, 0);
                    } else {
                        assert(var_type == code_pin);
                        
                        pin_number = bytecode[index++];
                        pin_type = bytecode[index++];
                        
                        assert(pin_number >= 0 && pin_number < PIN_LAST);

                        var_declare(name, max_gosubs, var_type, size, max_index, pin_number, pin_type);
                    }
                } else {
                    var_type = code_ram;
                    var_declare(name, max_gosubs, var_type, sizeof(int), max_index, 0, 0);
                }
            } while (index < length && bytecode[index] == code_comma);
            break;

        case code_let:
            // revisit -- share with code_for!
            if (bytecode[index] == code_load_and_push_var) {
                index++;
                max_index = 0;
            } else {
                assert(bytecode[index] == code_load_and_push_var_indexed);
                index++;

                blen = *(int *)(bytecode+index);
                index += sizeof(int);

                index += evaluate(bytecode+index, blen, &max_index);
            }

            name = (char *)bytecode+index;
            index += strlen(name)+1;
            
            index += evaluate(bytecode+index, length-index, &value);
            var_set(name, max_index, value);
            break;

        case code_print:
            cw7bug++;  // CW7 BUG
            do {
                if (index > 1) {
                    assert(bytecode[index] == code_comma);
                    if (run_condition) {
                        printf(" ");
                    }
                    index++;
                }

                if (bytecode[index] == code_string) {
                    index++;

                    if (run_condition) {
                        printf("%s", bytecode+index);
                    }
                    index += strlen((char *)bytecode+index)+1;
                } else {
                    assert(bytecode[index] == code_expression);
                    index++;

                    index += evaluate(bytecode+index, length-index, &value);
                    if (run_condition) {
                        printf("%d", value);
                    }
                }
            } while (index < length && bytecode[index] == code_comma);
            if (run_condition) {
                printf("\n");
            }
            break;

        case code_if:
            if (max_scopes >= MAX_SCOPES-1) {
                printf("too many ifs\n");
                return true;
            }
            scopes[max_scopes].line_number = run_line_number;
            scopes[max_scopes].type = open_if;
            max_scopes++;

            // evaluate the condition
            index += evaluate(bytecode+index, length-index, &value);

            // incorporate the condition
            scopes[max_scopes-1].condition = !! value;
            scopes[max_scopes-1].condition_ever = !! value;
            scopes[max_scopes-1].condition_initial = run_condition;
            break;

        case code_elseif:
            if (scopes[max_scopes-1].type != open_if) {
                printf("mismatched elseif\n");
                return true;
            }

            // reevaluate the condition
            run_condition = scopes[max_scopes-1].condition_initial;
            index += evaluate(bytecode+index, length-index, &value);

            if (scopes[max_scopes-1].condition_ever) {
                scopes[max_scopes-1].condition = false;
            } else {
                scopes[max_scopes-1].condition = !! value;
                scopes[max_scopes-1].condition_ever |= !! value;
            }
            break;

        case code_else:
            if (scopes[max_scopes-1].type != open_if) {
                printf("mismatched else\n");
                return true;
            }

            // flip the condition
            scopes[max_scopes-1].condition = ! scopes[max_scopes-1].condition_ever;
            break;

        case code_endif:
            if (! max_scopes) {
                printf("missing if\n");
                return true;
            }
            if (scopes[max_scopes-1].type != open_if) {
                printf("mismatched endif\n");
                return true;
            }
            assert(max_scopes);
            max_scopes--;
            break;

            break;

        case code_while:
        case code_for:
            if (max_scopes >= MAX_SCOPES-1) {
                printf("too many whiles\n");
                return true;
            }
            scopes[max_scopes].line_number = run_line_number;
            scopes[max_scopes].type = open_while;
            max_scopes++;

            if (code == code_for) {
                // revisit -- share with code_let!
                if (bytecode[index] == code_load_and_push_var) {
                    index++;
                    max_index = 0;
                } else {
                    assert(bytecode[index] == code_load_and_push_var_indexed);
                    index++;

                    blen = *(int *)(bytecode+index);
                    index += sizeof(int);

                    index += evaluate(bytecode+index, blen, &max_index);
                }

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
            } else {
                assert(code == code_while);

                scopes[max_scopes-1].for_variable_name = NULL;
                scopes[max_scopes-1].for_variable_index = 0;
                scopes[max_scopes-1].for_final_value = 0;
                scopes[max_scopes-1].for_step_value = 0;

                // evaluate the condition
                index += evaluate(bytecode+index, length-index, &value);
            }

            // incorporate the condition
            scopes[max_scopes-1].condition = !! value;
            scopes[max_scopes-1].condition_ever = !! value;
            scopes[max_scopes-1].condition_initial = run_condition;
            break;

        case code_break:
            n = *(int *)(bytecode + index);
            index += sizeof(int);
            assert(n);

            if (! max_scopes) {
                printf("missing while\n");
                return true;
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
                printf("break without while\n");
                return true;
            }

            if (run_condition) {
                // negate all conditions
                while (i < max_scopes) {
                    scopes[i].condition_initial = false;
                    i++;
                }
            }
            break;

        case code_next:
        case code_endwhile:
            if (! max_scopes) {
                printf("missing for/while\n");
                return true;
            }
            if (scopes[max_scopes-1].type != open_while ||
                (code == code_next && ! scopes[max_scopes-1].for_variable_name) ||
                (code == code_endwhile && scopes[max_scopes-1].for_variable_name)) {
                printf("mismatched next/endwhile\n");
                return true;
            }
            if (code == code_endwhile) {
                assert(max_scopes);
                max_scopes--;
            }

            if (run_condition) {
                if (code == code_next) {
                    value = var_get(scopes[max_scopes-1].for_variable_name, scopes[max_scopes-1].for_variable_index);
                    value += scopes[max_scopes-1].for_step_value;
                    if (scopes[max_scopes-1].for_step_value >= 0) {
                        if (value <= scopes[max_scopes-1].for_final_value) {
                            var_set(scopes[max_scopes-1].for_variable_name, scopes[max_scopes-1].for_variable_index, value);
                        } else {
                            run_condition = false;
                        }
                    } else {
                        if (value >= scopes[max_scopes-1].for_final_value) {
                            var_set(scopes[max_scopes-1].for_variable_name, scopes[max_scopes-1].for_variable_index, value);
                        } else {
                            run_condition = false;
                        }
                    }

                    // conditionally go back for more (skip the for line)!
                    if (run_condition) {
                        run_line_number = scopes[max_scopes-1].line_number;
                    } else {
                        assert(max_scopes);
                        max_scopes--;
                    }
                } else {
                    assert(code == code_endwhile);
                    // go back for more (including the while line)!
                    run_line_number = scopes[max_scopes].line_number-1;
                }
            } else {
                if (code == code_next) {
                    assert(max_scopes);
                    max_scopes--;
                }
            }
            break;

        case code_gosub:
            if (run_condition) {
                if (max_gosubs >= MAX_GOSUBS) {
                    printf("too many gosubs\n");
                    return true;
                }
                gosubs[max_gosubs].return_line_number = run_line_number;
                gosubs[max_gosubs].return_var_scope = var_open_scope();
                gosubs[max_gosubs].return_scope = max_scopes;
                max_gosubs++;
                line_number = code_sub_line(bytecode+index);
                if (! line_number) {
                    printf("missing sub\n");
                    return true;
                }
                run_line_number = line_number-1;
            }
            index += strlen((char *)bytecode+index)+1;
            break;

        case code_sub:
            // we do nothing here
            // revisit -- we could push false conditional scope (and pop on endsub)
            index += strlen((char *)bytecode+index)+1;
            break;

        case code_return:
        case code_endsub:
            if (run_condition) {
                if (! max_gosubs) {
                    printf("missing gosub\n");
                    return true;
                }
                assert(max_gosubs);
                max_gosubs--;
                var_close_scope(gosubs[max_gosubs].return_var_scope);
                run_line_number = gosubs[max_gosubs].return_line_number;
                max_scopes = gosubs[max_gosubs].return_scope;
            }
            break;

        case code_sleep:
            // N.B. sleeps occur in the main loop so we can service interrupts
            index += evaluate(bytecode+index, length-index, &value);
            if (run_condition) {
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
                run_line_number = run_line_number-1;
                end = true;
            }
            break;

        default:
            assert(0);
    }

    assert(index == length);
    return end;
}

bool
run(bool cont, int start_line_number)
{
    int i;
    int tick;
    bool isr;
    int line_number;
    struct line *line;

    if (! cont) {
        var_clear(false);

        max_scopes = 0;
        max_gosubs = 0;

        clear_stack();

        for (i = 0; i < MAX_INTS; i++) {
            run_isr_pending[i] = false;
            run_isr_masked[i] = false;
            run_isr_bytecode[i] = NULL;
            run_isr_length[i] = 0;
        }
        
        for (i = 0; i < MAX_TIMERS; i++) {
            timer_interval_ticks[i] = 0;
            timer_last_ticks[i] = ticks;
        }
        
        memset(uart_armed, 1, sizeof(uart_armed));
        
        run_line_number = start_line_number?start_line_number-1:0;

        data_line_number = 0;
        data_line_offset = 0;
    } else {
        run_line_number = start_line_number?start_line_number-1:run_line_number;
    }
    run_sleep_line_number = 0;

    isr = false;
    run_stop = 1;

    for (;;) {
#if ! _WIN32
        sleep_poll();
#endif
        
        clear_stack();
        
        run_line_count++;

        if (run_line_number && run_sleep_line_number == run_line_number) {
            if (ticks >= run_sleep_ticks) {
                run_sleep_line_number = 0;
            }
        } else {
            line = code_next_line(false, &run_line_number);
            if (! line) {
                break;
            }

            line_number = run_line_number;
            if (run_bytecode(false, line->bytecode, line->length)) {
                stop();
                break;
            }
        }

        if (run_step) {
            stop();
        }

        if (! run_stop) {
            printf("STOP at line %d!\n", line_number);
            break;
        }
        
        // see if any timers have expired
        tick = ticks;
        for (i = 0; i < MAX_TIMERS; i++) {
            // if the timer is set...
            if (run_isr_bytecode[TIMER_INT(i)] && timer_interval_ticks[i]) {
                // if its time is due...
                if ((int)(tick - timer_last_ticks[i]) >= timer_interval_ticks[i]) {
                    // if it is not already pending...
                    if (! run_isr_pending[TIMER_INT(i)]) {
                        // mark the interrupt as pending
                        run_isr_pending[TIMER_INT(i)] = true;
                        timer_last_ticks[i] = tick;
                    }
                }
            } else {
                timer_last_ticks[i] = tick;
            }
        }
        
        // see if any uarts have transferred data
        for (i = 0; i < MAX_UARTS; i++) {
#if ! _WIN32
            if (run_isr_bytecode[UART_INT(i, true)] && uart_armed[UART_INT(i, true)] && (MCF_UART_USR(i) & MCF_UART_USR_TXEMP)) {
                run_isr_pending[UART_INT(i, true)] = true;
                uart_armed[UART_INT(i, true)] = false;
            }

            if (run_isr_bytecode[UART_INT(i, false)] && uart_armed[UART_INT(i, false)] && (MCF_UART_USR(i) & MCF_UART_USR_RXRDY)) {
                run_isr_pending[UART_INT(i, false)] = true;
                uart_armed[UART_INT(i, false)] = false;
            }
#endif
        }

        // if we're not already running an isr...
        if (! isr) {
            for (i = 0; i < MAX_INTS; i++) {
                // if we have an isr to run...
                if (run_isr_pending[i] && ! run_isr_masked[i] && run_isr_bytecode[i]) {
                    // open a new temporary (unconditional) scope
                    assert(max_scopes < MAX_SCOPES);
                    scopes[max_scopes].line_number = run_line_number;
                    scopes[max_scopes].type = open_isr;
                    max_scopes++;

                    scopes[max_scopes-1].condition = true;
                    scopes[max_scopes-1].condition_ever = true;
                    scopes[max_scopes-1].condition_initial = true;

                    // run the isr
                    assert(! isr);
                    isr = true;
                    run_line_number = -1;
                    if (run_bytecode(false, run_isr_bytecode[i], run_isr_length[i])) {
                        stop();
                    }

                    run_isr_pending[i] = false;

                    break;
                }
            }
        }

        // if we're returning from our isr...
        if (run_line_number == -1) {
            run_line_number = scopes[max_scopes-1].line_number;

            assert(max_scopes);
            assert(scopes[max_scopes-1].type == open_isr);
            max_scopes--;
            
            assert(isr);
            isr = false;
        }
    }

    if (! run_stop) {
        return false;  // we stopped
    } else {
        while (max_scopes) {
            if (scopes[max_scopes-1].type != open_isr) {
                printf("missing %s\n", scopes[max_scopes-1].type == open_if ? "endif" : "next/endwhile");
            }
            assert(max_scopes);
            max_scopes--;
        }

        run_stop = false;
        return true;  // we completed
    }
}

void
stop()
{
    run_stop = 0;
}

#endif
