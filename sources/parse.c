// *** parse.c ********************************************************
// this file implements the bytecode compiler and de-compiler for
// stickos.

// modules:
// *** bytecode compiler ***
// *** bytecode de-compiler ***

#include "main.h"


static
struct op {
    char *op;
    enum bytecode code;
    int precedence;
} ops[] = {
    // double character
    "||", code_logical_or, 0,
    "&&", code_logical_and, 1,
    "^^", code_logical_xor, 2,
    "==", code_equal, 6,
    "!=", code_not_equal, 6,
    "<=", code_less_or_equal, 7,
    ">=", code_greater_or_equal, 7,
    ">>", code_shift_right, 8,
    "<<", code_shift_left, 8,

    // single character (follow double character)
    "|", code_bitwise_or, 3,
    "^", code_bitwise_xor, 4,
    "&", code_bitwise_and, 5,
    "<", code_less, 7,
    ">", code_greater, 7,
    "+", code_add, 9,
    "+", code_unary_plus, 12,  // N.B. must follow code_add
    "-", code_subtract, 9,
    "-", code_unary_minus, 12,  // N.B. must follow code_subtract
    "*", code_multiply, 10,
    "/", code_divide, 10,
    "%", code_mod, 10,
    "!", code_logical_not, 11,  // right to left associativity
    "~", code_bitwise_not, 11,  // right to left associativity
};

static struct keyword {
    char *keyword;
    enum bytecode code;
} keywords[] = {
    "assert", code_assert,
    "break", code_break,
    "configure", code_configure,
    "data", code_data,
    "dim", code_dim,
    "elseif", code_elseif,
    "else", code_else,  // N.B. must follow code_elseif
    "endif", code_endif,
    "endsub", code_endsub,
    "endwhile", code_endwhile,
    "end", code_end,  // N.B. must follow code_endif, code_endsub, and code_endwhile
    "for", code_for,
    "gosub", code_gosub,
    "if", code_if,
    "let", code_let,
    "mask", code_mask,
    "next", code_next,
    "off", code_off,
    "on", code_on,
    "print", code_print,
    "read", code_read,
    "rem", code_rem,
    "restore", code_restore,
    "return", code_return,
    "sleep", code_sleep,
    "stop", code_stop,
    "sub", code_sub,
    "unmask", code_unmask,
    "while", code_while
};


// revisit -- merge these with basic.c/parse.c???

static
void
parse_trim(IN char **p)
{
    // advance *p past any leading spaces
    while (isspace(**p)) {
        (*p)++;
    }
}

static
bool
parse_char(IN OUT char **text, IN char c)
{
    if (**text != c) {
        return false;
    }

    // advance *text past c
    (*text)++;

    parse_trim(text);
    return true;
}

static
bool
parse_word(IN OUT char **text, IN char *word)
{
    int len;

    len = strlen(word);

    if (strncmp(*text, word, len)) {
        return false;
    }

    // advance *text past word
    (*text) += len;

    parse_trim(text);
    return true;
}

static
char *
find_keyword(IN OUT char *text, IN char* word)
{
    int n;
    bool w;
    char *p;

    n = strlen(word);

    // find delimited keyword in text
    w = false;
    for (p = text; *p; p++) {
        if (! w && ! strncmp(p, word, n) && ! isalpha(*(p+n))) {
            return p;
        }
        w = isalpha(*p);
    }
    return NULL;
}

static
bool
parse_tail(IN OUT char **text, IN char* tail)
{
    char *p;

    // find delimited keyword in text
    p = find_keyword(*text, tail);
    if (! p) {
        return false;
    }

    // make sure nothing follows
    *p = '\0';
    p += strlen(tail);
    parse_trim(&p);
    if (*p) {
        return false;
    }

    return true;
}

static
bool
parse_const(IN OUT char **text, IN OUT int *length, IN OUT byte *bytecode)
{
    char c;
    int value;

    if (! isdigit((*text)[0])) {
        return false;
    }

    // parse constant value and advance *text past constant
    value = 0;
    if ((*text)[0] == '0' && (*text)[1] == 'x') {
        (*text) += 2;
        for (;;) {
            c = (*text)[0];
            if (c >= 'a' && c <= 'f') {
                value = value*16 + 10 + c - 'a';
                (*text)++;
            } else if (isdigit(c)) {
                value = value*16 + c - '0';
                (*text)++;
            } else {
                break;
            }
        }
    } else {
        while (isdigit((*text)[0])) {
            value = value*10 + (*text)[0] - '0';
            (*text)++;
        }
    }

    *(int *)(bytecode+*length) = value;
    *length += sizeof(int);

    parse_trim(text);
    return true;
}

static
char *
match_paren(char *p)
{
    char c;
    char cc;
    int level;

    assert(*p == '(' || *p == '[');
    cc = *p;

    // return a pointer to the matching close parenthesis or brace of p
    level = 0;
    for (;;) {
        c = *p;
        if (! c) {
            break;
        }
        if (c == cc) {
            level++;
        } else if (c == ((cc == '(') ? ')' : ']')) {
            assert(level);
            level--;
            if (! level) {
                return p;
            }
        }
        p++;
    }

    return NULL;
}

static
char *
match_quote(char *p)
{
    char c;

    assert(*p == '"');

    // return a pointer to the matching close quote of p
    p++;
    for (;;) {
        c = *p;
        if (! c) {
            break;
        }
        if (c == '"') {
            return p;
        }
        p++;
    }

    return NULL;
}

// *** bytecode compiler ***

static
bool
parse_expression(IN int obase, IN OUT char **text, IN OUT int *length, IN OUT byte *bytecode);

static
int
unparse_expression(int tbase, byte *bytecode_in, int length, char **out);

// this function parses (compiles) a variable to bytecode.
static
bool
parse_var(IN bool lvalue, IN int obase, IN OUT char **text, IN OUT int *length, IN OUT byte *bytecode)
{
    int i;
    char *p;
    int *hole;
    int olength;
    char name[BASIC_LINE_SIZE];

    if (! isalpha(**text)) {
        return false;
    }

    // extract the variable name and advance *text past name
    i = 0;
    while (isalpha(**text) || isdigit(**text) || **text == '_') {
        name[i++] = *(*text)++;
    }
    name[i] = '\0';

    // if the variable is an array element...
    if (**text == '[') {
        if (lvalue) {
            // for lvalues we generate the code first, followed by the index expression length
            bytecode[(*length)++] = code_load_and_push_var_indexed;
            hole = (int *)(bytecode + *length);
            *length += sizeof(int);
            olength = *length;
        }

        // find the end of the array index expression
        p = match_paren(*text);
        if (! p) {
            return false;
        }
        assert(*p == ']');
        *p = '\0';
        assert(**text == '[');
        (*text)++;

        // parse the array index expression
        if (! parse_expression(obase, text, length, bytecode)) {
            return false;
        }

        *text = p;
        (*text)++;

        if (lvalue) {
            // fill in the index expression length from the parse
            *hole = *length - olength;
        } else {
            // for rvalues we generate the code last, following the index expression
            bytecode[(*length)++] = code_load_and_push_var_indexed;
        }
    } else {
        // for simple variables we just generate the code
        bytecode[(*length)++] = code_load_and_push_var;
    }

    // generate the variable name to bytecode, and advance *length past the name
    for (i = 0; name[i]; i++) {
        bytecode[(*length)++] = name[i];
    }
    bytecode[(*length)++] = '\0';

    parse_trim(text);
    return true;
}

#define MAX_OP_STACK  10

static struct op *op_stack[MAX_OP_STACK];

// this function determines which stacked operators need to be popped.
static
void
parse_clean(IN int obase, IN OUT int *otop, IN int precedence, IN OUT int *length, IN OUT byte *bytecode)
{
    int j;
    bool pop;

    for (j = *otop-1; j >= obase+1; j--) {
        assert(op_stack[j]->precedence >= op_stack[j-1]->precedence);
    }

    // for pending operators at the top of the stack...
    for (j = *otop-1; j >= obase; j--) {
        // if the operator has right to left associativity...
        if (op_stack[j]->code == code_logical_not || op_stack[j]->code == code_bitwise_not) {
            // pop if the operator precedence is greater than the current precedence
            pop = op_stack[j]->precedence > precedence;
        } else {
            // pop if the operator precedence is greater than or equal to the current precedence
            pop = op_stack[j]->precedence >= precedence;
        }

        // if the operator needs to be popped...
        if (pop) {
            // pop it
            *otop = j;

            // and generate its bytecode
            bytecode[(*length)++] = op_stack[j]->code;
        } else {
            // keep the remaining (low precedence) operators on the stack for now
            break;
        }
    }
}

// this function parses (compiles) an expression to bytecode.
static
bool
parse_expression(IN int obase, IN OUT char **text, IN OUT int *length, IN OUT byte *bytecode)
{
    int i;
    char c;
    char *p;
    int len;
    int otop;
    bool number;

    otop = obase;

    number = true;
    for (;;) {
        c = **text;
        if (! c || c == ',') {
            if (number) {
                return false;
            }
            break;
        }

        // if this is the start of a number...
        if (isdigit(c)) {
            if (! number) {
                return false;
            }

            // generate the bytecode and parse the constant
            if ((*text)[0] == '0' && (*text)[1] == 'x') {
                bytecode[(*length)++] = code_load_and_push_immediate_hex;
            } else {
                bytecode[(*length)++] = code_load_and_push_immediate;
            }
            if (! parse_const(text, length, bytecode)) {
                return false;
            }
            number = false;

        // otherwise, if this is the start of a variable...
        } else if (isalpha(c)) {
            if (! number) {
                return false;
            }

            // generate the bytecode and parse the variable
            if (! parse_var(false, otop, text, length, bytecode)) {
                return false;
            }
            number = false;

        } else {
            // if this is the start of a parenthetical expression...
            if (c == '(') {
                if (! number) {
                    return false;
                }
                p = match_paren(*text);
                if (! p) {
                    return false;
                }
                assert(*p == ')');
                *p = '\0';
                assert(**text == '(');
                (*text)++;

                // parse the sub-expression
                if (! parse_expression(otop, text, length, bytecode)) {
                    return false;
                }

                *text = p;
                (*text)++;
                number = false;

            // otherwise, this should be an operator...
            } else {
                for (i = 0; i < LENGTHOF(ops); i++) {
                    len = strlen(ops[i].op);
                    if (! strncmp(*text, ops[i].op, len)) {
                        break;
                    }
                }
                if (i == LENGTHOF(ops)) {
                    return false;
                }

                if (number && ops[i].code == code_add) {
                    i++;  // convert to unary
                    assert(ops[i].code == code_unary_plus);
                } else if (number && ops[i].code == code_subtract) {
                    i++;  // convert to unary
                    assert(ops[i].code == code_unary_minus);
                } else {
                    if (ops[i].code == code_logical_not || ops[i].code == code_bitwise_not) {
                        if (! number) {
                            return false;
                        }
                    } else {
                        if (number) {
                            return false;
                        }
                    }

                    // if we need to clean the top of the op stack
                    parse_clean(obase, &otop, ops[i].precedence, length, bytecode);
                    if (ops[i].code != code_logical_not && ops[i].code != code_bitwise_not) {
                        number = true;
                    }
                }

                (*text) += len;

                if (otop >= MAX_OP_STACK) {
                    return false;
                }

                // push the new top
                op_stack[otop++] = &ops[i];
            }

            parse_trim(text);
        }
    }

    // clean the entire op stack!
    parse_clean(obase, &otop, 0, length, bytecode);
    assert(otop == obase);
    return true;
}

// this function parses (compiles) a statement line to bytecode.
bool
parse_line(IN char *text_in, OUT int *length_out, OUT byte *bytecode, OUT int *syntax_error)
{
    int i;
    char *p;
    int len;
    int size;
    bool neg;
    int length;
    char *text;
    int pin_type;
    int pin_number;

    text = text_in;
    parse_trim(&text);

    for (i = 0; i < LENGTHOF(keywords); i++) {
        len = strlen(keywords[i].keyword);
        if (! strncmp(text, keywords[i].keyword, len)) {
            text += len;
            break;
        }
    }
    if (i == LENGTHOF(keywords)) {
        goto XXX_ERROR_XXX;
    }

    parse_trim(&text);

    length = 0;
    bytecode[length++] = keywords[i].code;

    switch (keywords[i].code) {
        case code_rem:
            // generate the comment to bytecode
            while (*text) {
                bytecode[length++] = *text++;
            }
            bytecode[length++] = *text;
            break;

        case code_on:
        case code_off:
        case code_mask:
        case code_unmask:
            // parse the device type
            if (parse_word(&text, "timer")) {
                bytecode[length++] = device_timer;

                // parse the timer number
                if (! parse_const(&text, &length, bytecode)) {
                    goto XXX_ERROR_XXX;
                }
                assert(length >= sizeof(int));
                if (*(int *)(bytecode+length-sizeof(int)) < 0 || *(int *)(bytecode+length-sizeof(int)) >= MAX_TIMERS) {
                    goto XXX_ERROR_XXX;
                }

            } else if (parse_word(&text, "uart")) {
                bytecode[length++] = device_uart;

                // parse the uart number
                if (! parse_const(&text, &length, bytecode)) {
                    goto XXX_ERROR_XXX;
                }
                assert(length >= sizeof(int));
                if (*(int *)(bytecode+length-sizeof(int)) < 0 || *(int *)(bytecode+length-sizeof(int)) >= MAX_UARTS) {
                    goto XXX_ERROR_XXX;
                }

                // parse the uart data direction
                if (parse_word(&text, "output")) {
                    bytecode[length++] = 1;
                } else if (parse_word(&text, "input")) {
                    bytecode[length++] = 0;
                } else {
                    goto XXX_ERROR_XXX;
                }
            } else {
                goto XXX_ERROR_XXX;
            }

            // if we're enabling interrupts...
            if (keywords[i].code == code_on) {
                // parse the interrupt handler statement
                if (! parse_line(text, &len, bytecode+length+sizeof(int), syntax_error)) {
                    goto XXX_ERROR_XXX;
                }
                *(int *)(bytecode+length) = len;
                length += sizeof(int);
                length += len;
                text += strlen(text);
            }
            break;

        case code_configure:
            // parse the device type
            if (parse_word(&text, "timer")) {
                bytecode[length++] = device_timer;

                // parse the timer number
                if (! parse_const(&text, &length, bytecode)) {
                    goto XXX_ERROR_XXX;
                }
                assert(length >= sizeof(int));
                if (*(int *)(bytecode+length-sizeof(int)) < 0 || *(int *)(bytecode+length-sizeof(int)) >= MAX_TIMERS) {
                    goto XXX_ERROR_XXX;
                }

                if (! parse_word(&text, "for")) {
                    goto XXX_ERROR_XXX;
                }

                // parse the timer interval
                if (! parse_const(&text, &length, bytecode)) {
                    goto XXX_ERROR_XXX;
                }
                if (! parse_word(&text, "ms")) {
                    goto XXX_ERROR_XXX;
                }

            } else if (parse_word(&text, "uart")) {
                bytecode[length++] = device_uart;

                // parse the uart number
                if (! parse_const(&text, &length, bytecode)) {
                    goto XXX_ERROR_XXX;
                }
                assert(length >= sizeof(int));
                if (*(int *)(bytecode+length-sizeof(int)) < 0 || *(int *)(bytecode+length-sizeof(int)) >= MAX_UARTS) {
                    goto XXX_ERROR_XXX;
                }

                if (! parse_word(&text, "for")) {
                    goto XXX_ERROR_XXX;
                }

                // parse the baud rate
                if (! parse_const(&text, &length, bytecode)) {
                    goto XXX_ERROR_XXX;
                }
                if (! parse_word(&text, "baud")) {
                    goto XXX_ERROR_XXX;
                }

                // parse the data bits
                if (! parse_const(&text, &length, bytecode)) {
                    goto XXX_ERROR_XXX;
                }
                if (! parse_word(&text, "data")) {
                    goto XXX_ERROR_XXX;
                }
                assert(length >= sizeof(int));
                if (*(int *)(bytecode+length-sizeof(int)) < 5 || *(int *)(bytecode+length-sizeof(int)) > 8) {
                    goto XXX_ERROR_XXX;
                }

                // parse the parity
                if (parse_word(&text, "even")) {
                    bytecode[length++] = 0;
                } else if (parse_word(&text, "odd")) {
                    bytecode[length++] = 1;
                } else if (parse_word(&text, "no")) {
                    bytecode[length++] = 2;
                } else {
                    goto XXX_ERROR_XXX;
                }
                if (! parse_word(&text, "parity")) {
                    goto XXX_ERROR_XXX;
                }

                // parse the optional loopback specifier
                if (parse_word(&text, "loopback")) {
                    bytecode[length++] = 1;
                } else {
                    bytecode[length++] = 0;
                }
            } else {
                goto XXX_ERROR_XXX;
            }
            break;

        case code_assert:
            // parse the assertion expression
            if (! parse_expression(0, &text, &length, bytecode)) {
                goto XXX_ERROR_XXX;
            }
            break;

        case code_read:
            if (! *text) {
                goto XXX_ERROR_XXX;
            }
            // while there are more variables to parse...
            while (*text) {
                if (length > 1) {
                    if (! parse_char(&text, ',')) {
                        goto XXX_ERROR_XXX;
                    }
                    bytecode[length++] = code_comma;
                }

                // parse the variable
                if (! parse_var(true, 0, &text, &length, bytecode)) {
                    goto XXX_ERROR_XXX;
                }
            }
            break;

        case code_data:
            if (! *text) {
                goto XXX_ERROR_XXX;
            }
            // while there is more data to parse...
            while (*text) {
                if (length > 1) {
                    if (! parse_char(&text, ',')) {
                        goto XXX_ERROR_XXX;
                    }
                }

                neg = false;
                if (text[0] == '0' && text[1] == 'x') {
                    bytecode[length++] = code_load_and_push_immediate_hex;
                } else {
                    if (parse_char(&text, '-')) {
                        neg = true;
                    }
                    bytecode[length++] = code_load_and_push_immediate;
                }
                // parse the (positive) constant
                if (! parse_const(&text, &length, bytecode)) {
                    goto XXX_ERROR_XXX;
                }
                // if the user requested negative...
                if (neg) {
                    // make it so
                    *(int *)(bytecode+length-sizeof(int)) = -*(int *)(bytecode+length-sizeof(int));
                }
            }
            break;

        case code_restore:
            // if the user specified a line number...
            if (*text) {
                // parse the line number to restore to
                if (! parse_const(&text, &length, bytecode)) {
                    goto XXX_ERROR_XXX;
                }
            } else {
                // restore to the beginning of the program
                *(int *)(bytecode+length) = 0;
                length += sizeof(int);
            }
            break;

        case code_dim:
            if (! *text) {
                goto XXX_ERROR_XXX;
            }
            // while there are more variables to parse...
            while (*text) {
                if (length > 1) {
                    if (! parse_char(&text, ',')) {
                        goto XXX_ERROR_XXX;
                    }
                    bytecode[length++] = code_comma;
                }

                // parse the variable
                if (! parse_var(true, 0, &text, &length, bytecode)) {
                    goto XXX_ERROR_XXX;
                }

                // if the user did not specify an "as" format...
                if (! parse_word(&text, "as")) {
                    continue;
                }

                bytecode[length++] = code_as;

                // parse the size specifier
                if (parse_word(&text, "byte")) {
                    size = sizeof(byte);
                } else if (parse_word(&text, "integer")) {
                    size = sizeof(int);
                } else {
                    size = sizeof(int);
                }
                bytecode[length++] = size;

                // parse the type specifier
                if (parse_word(&text, "ram")) {
                    bytecode[length++] = code_ram;
                } else if (parse_word(&text, "flash")) {
                    if (size != sizeof(int)) {
                        goto XXX_ERROR_XXX;
                    }
                    bytecode[length++] = code_flash;
                } else if (parse_word(&text, "pin")) {
                    bytecode[length++] = code_pin;

                    // parse the pin name
                    assert(PIN_LAST == LENGTHOF(pins));
                    for (pin_number = 0; pin_number < PIN_LAST; pin_number++) {
                        if (parse_word(&text, pins[pin_number].name)) {
                            break;
                        }
                    }
                    if (pin_number == PIN_LAST) {
                        goto XXX_ERROR_XXX;
                    }
                    assert(pin_number < 256);
                    bytecode[length++] = pin_number;

                    if (! parse_word(&text, "for")) {
                        goto XXX_ERROR_XXX;
                    }

                    // parse the pin usage
                    if (parse_word(&text, "analog")) {
                        if (parse_word(&text, "input")) {
                            bytecode[length++] = pin_type_analog_input;
                        } else if (parse_word(&text, "output")) {
                            bytecode[length++] = pin_type_analog_output;
                        } else {
                            goto XXX_ERROR_XXX;
                        }
                    } else if (parse_word(&text, "digital")) {
                        if (parse_word(&text, "input")) {
                            bytecode[length++] = pin_type_digital_input;
                        } else if (parse_word(&text, "output")) {
                            bytecode[length++] = pin_type_digital_output;
                        } else {
                            goto XXX_ERROR_XXX;
                        }
                    } else if (parse_word(&text, "uart")) {
                        if (parse_word(&text, "input")) {
                            bytecode[length++] = pin_type_uart_input;
                        } else if (parse_word(&text, "output")) {
                            bytecode[length++] = pin_type_uart_output;
                        } else {
                            goto XXX_ERROR_XXX;
                        }
                    } else if (parse_word(&text, "frequency")) {
                        if (parse_word(&text, "input")) {
                            printf("unsupported pin type\n");
                            goto XXX_ERROR_XXX;
                        } else if (parse_word(&text, "output")) {
                            bytecode[length++] = pin_type_frequency_output;
                        } else {
                            goto XXX_ERROR_XXX;
                        }
                    } else {
                        goto XXX_ERROR_XXX;
                    }

                    assert(length);
                    pin_type = bytecode[length-1];
                    assert(! (pin_type & (pin_type-1)));  // only 1 bit set
                    if (pin_type != pin_type_digital_input && pin_type != pin_type_digital_output) {
                        if (! (pins[pin_number].pin_types & pin_type)) {
                            printf("unsupported pin type\n");
                            goto XXX_ERROR_XXX;
                        }
                    }
                } else {
                    // default to ram variables
                    bytecode[length++] = code_ram;
                }
            }
            break;

        case code_let:
            // parse the variable
            if (! parse_var(true, 0, &text, &length, bytecode)) {
                goto XXX_ERROR_XXX;
            }

            // parse the assignment
            if (! parse_char(&text, '=')) {
                goto XXX_ERROR_XXX;
            }

            // parse the assignment expression
            if (! parse_expression(0, &text, &length, bytecode)) {
                goto XXX_ERROR_XXX;
            }
            break;

        case code_print:
            if (! *text) {
                goto XXX_ERROR_XXX;
            }
            // while there are more items to print...
            while (*text) {
                if (length > 1) {
                    if (! parse_char(&text, ',')) {
                        goto XXX_ERROR_XXX;
                    }
                    bytecode[length++] = code_comma;
                }

                // if the next item is a string...
                if (*text == '"') {
                    bytecode[length++] = code_string;

                    // find the matching quote
                    p = match_quote(text);
                    if (! p) {
                        goto XXX_ERROR_XXX;
                    }

                    assert(*p == '"');
                    *p = '\0';
                    assert(*text == '"');
                    text++;

                    // generate the string to bytecode, and advance length and *text past the name
                    while (*text) {
                        bytecode[length++] = *text++;
                    }
                    bytecode[length++] = *text;

                    assert(text == p);
                    text++;

                // otherwise, the next item is an expression...
                } else {
                    bytecode[length++] = code_expression;

                    // parse the expression
                    if (! parse_expression(0, &text, &length, bytecode)) {
                        goto XXX_ERROR_XXX;
                    }
                }
            }
            break;

        case code_if:
        case code_elseif:
        case code_while:
            if (keywords[i].code == code_if || keywords[i].code == code_elseif) {
                // make sure we have a "then"
                if (! parse_tail(&text, "then")) {
                    text += strlen(text);
                    goto XXX_ERROR_XXX;
                }
            } else {
                // make sure we have a "do"
                if (! parse_tail(&text, "do")) {
                    text += strlen(text);
                    goto XXX_ERROR_XXX;
                }
            }

            // parse the conditional expression
            if (! parse_expression(0, &text, &length, bytecode)) {
                goto XXX_ERROR_XXX;
            }
            break;

        case code_else:
        case code_endif:
        case code_endwhile:
            // nothing to do here
            break;

        case code_break:
            // if the user specified a break level...
            if (*text) {
                // parse the break level
                if (! parse_const(&text, &length, bytecode)) {
                    goto XXX_ERROR_XXX;
                }
                assert(length >= sizeof(int));
                if (! *(int *)(bytecode+length-sizeof(int))) {
                    goto XXX_ERROR_XXX;
                }
            } else {
                // break 1 level
                *(int *)(bytecode+length) = 1;
                length += sizeof(int);
            }
            break;

        case code_for:
            // parse the for loop variable
            if (! parse_var(true, 0, &text, &length, bytecode)) {
                goto XXX_ERROR_XXX;
            }

            // parse the assignment
            if (! parse_char(&text, '=')) {
                goto XXX_ERROR_XXX;
            }

            // find the "to" keyword
            p = find_keyword(text, "to");
            if (! p) {
                goto XXX_ERROR_XXX;
            }
            *p = 0;

            // parse initial value
            if (! parse_expression(0, &text, &length, bytecode)) {
                goto XXX_ERROR_XXX;
            }
            if (*text) {
                goto XXX_ERROR_XXX;
            }

            text = p+2;
            parse_trim(&text);

            // see if there is a "step" keyword
            p = find_keyword(text, "step");
            if (p) {
                *p = 0;
            }

            bytecode[length++] = code_comma;

            // parse final value
            if (! parse_expression(0, &text, &length, bytecode)) {
                goto XXX_ERROR_XXX;
            }
            if (*text) {
                goto XXX_ERROR_XXX;
            }

            // if there was a step keyword...
            if (p) {
                text = p+4;
                parse_trim(&text);

                bytecode[length++] = code_comma;

                // parse step value
                if (! parse_expression(0, &text, &length, bytecode)) {
                    goto XXX_ERROR_XXX;
                }
            }
            break;

        case code_next:
            // nothing to do here
            break;

        case code_gosub:
        case code_sub:
            if (! *text) {
                goto XXX_ERROR_XXX;
            }
            // generate the subname to bytecode, and advance length and *text past the name
            while (*text) {
                bytecode[length++] = *text++;
            }
            bytecode[length++] = *text;
            break;

        case code_return:
        case code_endsub:
            // nothing to do here
            break;

        case code_sleep:
            // parse the sleep time expression
            if (! parse_expression(0, &text, &length, bytecode)) {
                goto XXX_ERROR_XXX;
            }
            break;

        case code_stop:
        case code_end:
            // nothing to do here
            break;

        default:
            assert(0);
            break;
    }
    if (*text) {
        goto XXX_ERROR_XXX;
    }

    *length_out = length;
    return true;

XXX_ERROR_XXX:
    *syntax_error = text - text_in;
    assert(*syntax_error >= 0 && *syntax_error < BASIC_LINE_SIZE);
    return false;
}

// *** bytecode de-compiler ***

// this function unparses (de-compiles) a variable from bytecode.
static
int
unparse_var_lvalue(byte *bytecode_in, char **out)
{
    int blen;
    byte *bytecode;

    bytecode = bytecode_in;

    // if the bytecode is an array element...
    if ((*bytecode) == code_load_and_push_var_indexed) {
        bytecode++;

        // get the index expression length; the variable name follows the index expression
        blen = *(int *)bytecode;
        bytecode += sizeof(int);

        // decompile the variable name and index expression
        *out += sprintf(*out, "%s[", bytecode+blen);
        unparse_expression(0, bytecode, blen, out);
        *out += sprintf(*out, "]");
        bytecode += blen;
        bytecode += strlen((char *)bytecode)+1;
    } else {
        assert((*bytecode) == code_load_and_push_var);
        bytecode++;

        // decompile the variable name
        *out += sprintf(*out, "%s", bytecode);
        bytecode += strlen((char *)bytecode)+1;
    }

    // return the number of bytecodes consumed
    return bytecode - bytecode_in;
}

#define MAX_TEXTS  10

// revisit -- can we reduce memory???
static char texts[MAX_TEXTS][BASIC_LINE_SIZE];
static int precedence[MAX_TEXTS];

// this function unparses (de-compiles) an expression from bytecode.
static
int
unparse_expression(int tbase, byte *bytecode_in, int length, char **out)
{
    int i;
    int n;
    int ttop;
    byte code;
    int value;
    bool unary;
    byte *bytecode;
    char temp[BASIC_LINE_SIZE];

    ttop = tbase;
    bytecode = bytecode_in;

    // while there are more bytecodes to decompile...
    while (bytecode < bytecode_in+length) {
        code = *bytecode;
        if (code == code_comma) {
            break;
        }
        bytecode++;

        switch (code) {
            case code_load_and_push_immediate:
            case code_load_and_push_immediate_hex:
                value = *(int *)bytecode;
                bytecode += sizeof(int);
                precedence[ttop] = 100;
                // decompile the constant
                if (code == code_load_and_push_immediate_hex) {
                    sprintf(texts[ttop++], "0x%x", value);
                } else {
                    sprintf(texts[ttop++], "%d", value);
                }
                break;

            case code_load_and_push_var:
                precedence[ttop] = 100;
                // decompile the variable
                sprintf(texts[ttop++], "%s", bytecode);
                bytecode += strlen((char *)bytecode)+1;
                break;

            case code_load_and_push_var_indexed:
                // our index is already on the stack
                strcpy(temp, texts[ttop-1]);

                precedence[ttop-1] = 100;
                // decompile the indexed variable array element
                sprintf(texts[ttop-1], "%s[%s]", bytecode, temp);
                bytecode += strlen((char *)bytecode)+1;
                break;

            default:
                // this should be an operator!!!
                for (i = 0; i < LENGTHOF(ops); i++) {
                    if (ops[i].code == code) {
                        break;
                    }
                }
                assert(i != LENGTHOF(ops));

                unary = (code == code_logical_not || code == code_bitwise_not || code == code_unary_plus || code == code_unary_minus);

                n = 0;

                // if this is a binary operator...
                if (! unary) {
                    // if the lhs's operator's precedence is greater than the current precedence...
                    if (ops[i].precedence > precedence[ttop-2]) {
                        // we need to parenthesize the left hand side
                        n += sprintf(temp+n, "(");
                    }
                    // decompile the left hand side
                    n += sprintf(temp+n, "%s", texts[ttop-2]);
                    if (ops[i].precedence > precedence[ttop-2]) {
                        n += sprintf(temp+n, ")");
                    }
                }

                // decompile the operator
                n += sprintf(temp+n, "%s", ops[i].op);

                // if the rhs's operator's precedence is greater than the current precedence...
                if (ops[i].precedence >= precedence[ttop-1]+unary) {
                    // we need to parenthesize the right hand side
                    n += sprintf(temp+n, "(");
                }
                // decompile the right hand side
                n += sprintf(temp+n, "%s", texts[ttop-1]);
                if (ops[i].precedence >= precedence[ttop-1]+unary) {
                    n += sprintf(temp+n, ")");
                }

                // pop the operator's arguments off the stack
                if (! unary) {
                    ttop -= 2;
                } else {
                    ttop -= 1;
                }

                // and push the operator's result back on the stack
                precedence[ttop] = ops[i].precedence;
                sprintf(texts[ttop++], "%s", temp);
                break;
        }
    }

    // decompile the resulting expression
    assert(ttop == tbase+1);
    *out += sprintf(*out, "%s", texts[tbase]);

    // return the number of bytecodes consumed
    return bytecode - bytecode_in;
}

// this function unparses (de-compiles) a statement line from bytecode.
void
unparse_bytecode(IN byte *bytecode_in, IN int length, OUT char *text)
{
    int i;
    int n;
    int len;
    int size;
    int pin;
    int timer;
    int type;
    int uart;
    int baud;
    int data;
    int line;
    int value;
    byte parity;
    byte loopback;
    int interval;
    byte device;
    byte code;
    byte code2;
    char *out;
    byte *bytecode;
    bool output;

    bytecode = bytecode_in;

    // find the bytecode keyword
    for (i = 0; i < LENGTHOF(keywords); i++) {
        if (keywords[i].code == *bytecode_in) {
            break;
        }
    }
    assert(i != LENGTHOF(keywords));

    // decompile the bytecode keyword
    out = text;
    out += sprintf(out, keywords[i].keyword);
    out += sprintf(out, " ");

    switch ((code = *bytecode++)) {
        case code_rem:
            // decompile the comment
            len = sprintf(out, "%s", bytecode);
            out += len;
            bytecode += len+1;
            break;

        case code_on:
        case code_off:
        case code_mask:
        case code_unmask:
            // find the device type
            device = *bytecode++;
            if (device == device_timer) {
                // decompile the timer
                out += sprintf(out, "timer ");

                // and the timer number
                timer = *(int *)bytecode;
                assert(timer >= 0 && timer < MAX_TIMERS);
                bytecode += sizeof(int);
                out += sprintf(out, "%d", timer);

            } else if (device == device_uart) {
                // decompile the uart
                out += sprintf(out, "uart ");

                // and the uart number
                uart = *(int *)bytecode;
                assert(uart >= 0 && uart < MAX_UARTS);
                bytecode += sizeof(int);
                out += sprintf(out, "%d ", uart);

                // and the uart data direction
                output = *bytecode++;
                out += sprintf(out, "%s", output?"output":"input");
            } else {
                assert(0);
            }

            // if we're enabling interrupts...
            if (code == code_on) {
                out += sprintf(out, " ");

                len = *(int *)bytecode;
                bytecode += sizeof(int);

                // decompile the interrupt handler statement
                unparse_bytecode(bytecode, len, out);
                bytecode += len;
            }
            break;

        case code_configure:
            // find the device type
            device = *bytecode++;
            if (device == device_timer) {
                // decompile the timer
                out += sprintf(out, "timer ");

                // and the timer number
                timer = *(int *)bytecode;
                assert(timer >= 0 && timer < MAX_TIMERS);
                bytecode += sizeof(int);
                out += sprintf(out, "%d ", timer);

                // and the timer interval
                interval = *(int *)bytecode;
                bytecode += sizeof(int);
                out += sprintf(out, "for %d ms", interval);

            } else if (device == device_uart) {
                // decompile the uart
                out += sprintf(out, "uart ");

                // and the uart number
                uart = *(int *)bytecode;
                assert(uart >= 0 && uart < MAX_UARTS);
                bytecode += sizeof(int);
                out += sprintf(out, "%d ", uart);

                // find the uart protocol and optional loopback specifier
                baud = *(int *)bytecode;
                bytecode += sizeof(int);
                data = *(int *)bytecode;
                bytecode += sizeof(int);
                parity = *bytecode++;
                loopback = *bytecode++;

                // and decompile it
                out += sprintf(out, "for %d baud %d data %s parity%s", baud, data, parity==0?"even":(parity==1?"odd":"no"), loopback?" loopback":"");
            } else {
                assert(0);
            }
            break;

        case code_assert:
            // decompile the assertion expression
            bytecode += unparse_expression(0, bytecode, bytecode_in+length-bytecode, &out);
            break;

        case code_read:
            // while there are more variables...
            while (bytecode < bytecode_in+length) {
                if (bytecode > bytecode_in+1) {
                    // separate variables with a comma
                    out += sprintf(out, ", ");
                    assert(*bytecode == code_comma);
                    bytecode++;
                }

                // decompile the variable
                bytecode += unparse_var_lvalue(bytecode, &out);
            }
            break;

        case code_data:
            // while there is more data...
            while (bytecode < bytecode_in+length) {
                if (bytecode > bytecode_in+1) {
                    // separate datas with a comma
                    out += sprintf(out, ", ");
                }

                // decompile the constant
                code = *bytecode++;
                value = *(int *)bytecode;
                bytecode += sizeof(int);
                if (code == code_load_and_push_immediate_hex) {
                    out += sprintf(out, "0x%x", value);
                } else {
                    assert(code == code_load_and_push_immediate);
                    out += sprintf(out, "%d", value);
                }
            }
            break;

        case code_restore:
            line = *(int *)bytecode;
            bytecode += sizeof(int);
            // if the user specified a line number...
            if (line) {
                // decompile the line number
                out += sprintf(out, "%d", line);
            }
            break;

        case code_dim:
            // while there are more variables...
            while (bytecode < bytecode_in+length) {
                if (bytecode > bytecode_in+1) {
                    // separate variables with a comma
                    out += sprintf(out, ", ");
                    assert(*bytecode == code_comma);
                    bytecode++;
                }

                // decompile the variable
                bytecode += unparse_var_lvalue(bytecode, &out);

                // if the user did not specify an "as" format...
                if (*bytecode != code_as) {
                    continue;
                }

                // decompile the "as"
                out += sprintf(out, " as ");
                bytecode++;

                // decompile the size specifier
                size = *bytecode++;
                if (size == sizeof(byte)) {
                    out += sprintf(out, "byte ");
                } else {
                    assert(size == sizeof(int));
                }

                // decompile the type specifier
                code2 = *bytecode++;
                if (code2 == code_ram) {
                    out += sprintf(out, "ram");
                } else if (code2 == code_flash) {
                    out += sprintf(out, "flash");
                } else {
                    assert(code2 == code_pin);
                    out += sprintf(out, "pin ");

                    pin = *bytecode++;
                    type = *bytecode++;

                    // decompile the pin name
                    assert(pin >= 0 && pin < PIN_LAST);
                    out += sprintf(out, "%s ", pins[pin].name);

                    out += sprintf(out, "for ");

                    // decompile the pin usage
                    if (type == pin_type_frequency_output) {
                        out += sprintf(out, "%s", "frequency output");
                    } else if (type == pin_type_analog_input) {
                        out += sprintf(out, "%s", "analog input");
                    } else if (type == pin_type_analog_output) {
                        out += sprintf(out, "%s", "analog output");
                    } else if (type == pin_type_uart_input) {
                        out += sprintf(out, "%s", "uart input");
                    } else if (type == pin_type_uart_output) {
                        out += sprintf(out, "%s", "uart output");
                    } else if (type == pin_type_digital_input) {
                        out += sprintf(out, "%s", "digital input");
                    } else {
                        assert(type == pin_type_digital_output);
                        out += sprintf(out, "%s", "digital output");
                    }
                }
            }
            break;

        case code_let:
            assert(bytecode < bytecode_in+length);

            // decompile the variable
            bytecode += unparse_var_lvalue(bytecode, &out);

            // decompile the assignment
            out += sprintf(out, " = ");

            // decompile the expression
            bytecode += unparse_expression(0, bytecode, bytecode_in+length-bytecode, &out);
            break;

        case code_print:
            // while there are more items...
            while (bytecode < bytecode_in+length) {
                if (bytecode > bytecode_in+1) {
                    // separate items with a comma
                    out += sprintf(out, ", ");
                    assert(*bytecode == code_comma);
                    bytecode++;
                }

                // if the next item is a string...
                if (*bytecode == code_string) {
                    bytecode++;
                    // decompile the string
                    out += sprintf(out, "\"");
                    len = sprintf(out, "%s", bytecode);
                    out += len;
                    out += sprintf(out, "\"");
                    bytecode += len+1;
                } else {
                    assert(*bytecode == code_expression);
                    bytecode++;
                    // decompile the expression
                    bytecode += unparse_expression(0, bytecode, bytecode_in+length-bytecode, &out);
                }
            }
            break;

        case code_if:
        case code_elseif:
        case code_while:
            // decompile the conditional expression
            bytecode += unparse_expression(0, bytecode, bytecode_in+length-bytecode, &out);
            // decompile the "then" or "do"
            if (code == code_if || code == code_elseif) {
                out += sprintf(out, " then");
            } else {
                out += sprintf(out, " do");
            }
            break;

        case code_else:
        case code_endif:
        case code_endwhile:
            // nothing to do here
            break;

        case code_break:
            n = *(int *)bytecode;
            bytecode += sizeof(int);
            // if the break level is not 1...
            if (n != 1) {
                // decompile the break level
                out += sprintf(out, " %d", n);
            }
            break;

        case code_for:
            assert(bytecode < bytecode_in+length);

            // decompile the for loop variable
            bytecode += unparse_var_lvalue(bytecode, &out);

            // decompile the assignment
            out += sprintf(out, " = ");

            // decompile the initial value expression
            bytecode += unparse_expression(0, bytecode, bytecode_in+length-bytecode, &out);

            // decompile the "to"
            assert(*bytecode == code_comma);
            bytecode++;
            out += sprintf(out, " to ");

            // decompile the final value expression
            bytecode += unparse_expression(0, bytecode, bytecode_in+length-bytecode, &out);

            // if there is a "step" keyword...
            if (bytecode_in+length > bytecode) {
                assert(*bytecode == code_comma);
                bytecode++;
                // decompile the "step" and the step value expression
                out += sprintf(out, " step ");
                bytecode += unparse_expression(0, bytecode, bytecode_in+length-bytecode, &out);
            }
            break;

        case code_next:
            // nothing to do here
            break;

        case code_gosub:
        case code_sub:
            // decompile the subname
            len = sprintf(out, "%s", bytecode);
            out += len;
            bytecode += len+1;
            break;

        case code_return:
        case code_endsub:
            // nothing to do here
            break;

        case code_sleep:
            // decompile the sleep time expression
            bytecode += unparse_expression(0, bytecode, bytecode_in+length-bytecode, &out);
            break;

        case code_stop:
        case code_end:
            // nothing to do here
            break;

        default:
            assert(0);
            break;
    }

    assert(bytecode == bytecode_in+length);
}

