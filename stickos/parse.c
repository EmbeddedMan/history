// *** parse.c ********************************************************
// this file implements the bytecode compiler and de-compiler for
// stickos.

// Copyright (c) Rich Testardi, 2008.  All rights reserved.
// Patent pending.

#include "main.h"


static
const struct op {
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

static
const struct keyword {
    char *keyword;
    enum bytecode code;
} keywords[] = {
    "assert", code_assert,
    "break", code_break,
    "configure", code_configure,
    "continue", code_continue,
    "data", code_data,
    "dim", code_dim,
    "do", code_do,
    "elseif", code_elseif,
    "else", code_else,  // N.B. must follow code_elseif
    "endif", code_endif,
    "endsub", code_endsub,
    "endwhile", code_endwhile,
    "end", code_end,  // N.B. must follow code_endif, code_endsub, and code_endwhile
    "for", code_for,
    "gosub", code_gosub,
    "if", code_if,
    "label", code_label,
    "let", code_let,
    "mask", code_mask,
    "next", code_next,
    "off", code_off,
    "on", code_on,
    "print", code_print,
    "qspi", code_qspi,
    "read", code_read,
    "rem", code_rem,
    "restore", code_restore,
    "return", code_return,
    "sleep", code_sleep,
    "stop", code_stop,
    "sub", code_sub,
    "unmask", code_unmask,
    "until", code_until,
    "while", code_while
};


void
parse_trim(IN char **p)
{
    // advance *p past any leading spaces
    while (isspace(**p)) {
        (*p)++;
    }
}

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

bool
parse_word(IN OUT char **text, IN const char *word)
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
bool
parse_wordn(IN OUT char **text, IN const char *word)
{
    int len;

    len = strlen(word);

    if (strncmp(*text, word, len) || isdigit(*(*text+len))) {
        return false;
    }

    // advance *text past word
    (*text) += len;

    parse_trim(text);
    return true;
}

bool
parse_words(IN OUT char **text_in, IN const char *words)
{
    int len;
    char *p;
    char *text;

    text = *text_in;
    while (words[0]) {
        len = strlen(words);
        p = strchr(words, ' ');
        if (p) {
            len = MIN(len, p-words);
        }

        if (strncmp(text, words, len)) {
            return false;
        }
        
        text += len;
        words += len;
        parse_trim(&text);
        if (words[0]) {
            assert(words[0] == ' ');
            words++;  // N.B. we assume a single space
        }
    }
    
    // advance *text_in past words
    *text_in = text;
    return true;
}

char *
parse_find_keyword(IN OUT char *text, IN char *word)
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
parse_find_tail(IN OUT char *text, IN char *tail)
{
    char *p;
    char *q;

    // find final delimited keyword in text
    p = text-1;
    do {
        q = parse_find_keyword(p+1, tail);
        if (q) {
            p = q;
        }
    } while (q);
    if (p == text-1) {
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

// revisit -- merge this with basic.c/parse.c???
bool
parse_const(IN OUT char **text, IN OUT int *length, IN OUT byte *bytecode)
{
    char c;
    int32 value;

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

    write32(bytecode+*length, value);
    *length += sizeof(uint32);

    parse_trim(text);
    return true;
}

char *
parse_match_paren(char *p)
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

char *
parse_match_quote(char *p)
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

static bool
parse_is_legal_var_name_char(const char *text) 
{
    return isalpha(*text) || isdigit(*text) || *text == '_';
}

// this function parses (compiles) a variable to bytecode.
bool
parse_var(IN bool lvalue, IN int obase, IN bool allow_array_index, IN OUT char **text, IN OUT int *length, IN OUT byte *bytecode)
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
    while (parse_is_legal_var_name_char(*text)) {
        name[i++] = *(*text)++;
    }
    name[i] = '\0';

    // if the variable is an array element...
    if (**text == '[') {
        if (! allow_array_index) {
            return false;
        }
        
        if (lvalue) {
            // for lvalues we generate the code first, followed by the index expression length
            bytecode[(*length)++] = code_load_and_push_var_indexed;
            hole = (int *)(bytecode + *length);
            *length += sizeof(uint32);
            olength = *length;
        }

        // find the end of the array index expression
        p = parse_match_paren(*text);
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
            write32((byte *)hole, *length - olength);
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

static const struct op *op_stack[MAX_OP_STACK];

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
            if (! parse_var(false, otop, true, text, length, bytecode)) {
                return false;
            }
            number = false;

        } else {
            // if this is the start of a parenthetical expression...
            if (c == '(') {
                if (! number) {
                    return false;
                }
                p = parse_match_paren(*text);
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

// this function parses (compiles) a public statement line to bytecode,
// excluding the keyword.
bool
parse_line_code(IN byte code, IN char *text_in, OUT int *length_out, OUT byte *bytecode, OUT int *syntax_error)
{
    int i;
    char *d;
    char *p;
    int len;
    bool neg;
    int length;
    char *text;
    int uart;
    int pin_type;
    int pin_qual;
    int pin_number;

    text = text_in;
    parse_trim(&text);

    length = 0;

    switch (code) {
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
                assert(length >= sizeof(uint32));
                if (read32(bytecode+length-sizeof(uint32)) < 0 || read32(bytecode+length-sizeof(uint32)) >= MAX_TIMERS) {
                    goto XXX_ERROR_XXX;
                }

            } else if (parse_word(&text, "uart")) {
                bytecode[length++] = device_uart;

                // parse the uart name
                for (uart = 0; uart < MAX_UARTS; uart++) {
                    if (parse_word(&text, uart_names[uart])) {
                        break;
                    }
                }
                if (uart == MAX_UARTS) {
                    goto XXX_ERROR_XXX;
                }
                bytecode[length++] = uart;

                // parse the uart data direction
                if (parse_word(&text, "output")) {
                    bytecode[length++] = 1;
                } else if (parse_word(&text, "input")) {
                    bytecode[length++] = 0;
                } else {
                    goto XXX_ERROR_XXX;
                }
            } else {
                // this should be an expression
                
                // find the "do"
                d = parse_find_keyword(text, "do");

                bytecode[length++] = device_watch;
                
                // parse the expression
                if (d) {
                    assert(*d == 'd');
                    *d = '\0';
                }
                len = length+sizeof(uint32);
                if (! parse_expression(0, &text, &len, bytecode)) {
                    goto XXX_ERROR_XXX;
                }
                write32(bytecode+length, len-(length+sizeof(uint32)));
                length = len;
                text += strlen(text);
                if (d) {
                    *d = 'd';
                }

                bytecode[length++] = code_comma;
            }
            
            // if we're enabling interrupts...
            if (code == code_on) {
                // parse the "do"
                if (! parse_word(&text, "do")) {
                    goto XXX_ERROR_XXX;
                }

                // parse the interrupt handler statement
                if (! parse_line(text, &len, bytecode+length+sizeof(uint32), syntax_error)) {
                    goto XXX_ERROR_XXX;
                }
                write32(bytecode+length, len);
                length += sizeof(uint32);
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
                assert(length >= sizeof(uint32));
                if (read32(bytecode+length-sizeof(uint32)) < 0 || read32(bytecode+length-sizeof(uint32)) >= MAX_TIMERS) {
                    goto XXX_ERROR_XXX;
                }

                if (! parse_word(&text, "for")) {
                    goto XXX_ERROR_XXX;
                }

                // parse the timer interval
                if (! parse_const(&text, &length, bytecode)) {
                    goto XXX_ERROR_XXX;
                }

                // parse the timer interval unit specifier
                for (i = 0; i < timer_unit_max; i++) {
                    if (parse_word(&text, timer_units[i].name)) {
                        bytecode[length++] = i;
                        break;
                    }
                }
                if (i == timer_unit_max) {
                    goto XXX_ERROR_XXX;
                }

            } else if (parse_word(&text, "uart")) {
                bytecode[length++] = device_uart;

                // parse the uart name
                for (uart = 0; uart < MAX_UARTS; uart++) {
                    if (parse_word(&text, uart_names[uart])) {
                        break;
                    }
                }
                if (uart == MAX_UARTS) {
                    goto XXX_ERROR_XXX;
                }
                bytecode[length++] = uart;

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
                assert(length >= sizeof(uint32));
                if (read32(bytecode+length-sizeof(uint32)) < 5 || read32(bytecode+length-sizeof(uint32)) > 8) {
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
            } else if (parse_word(&text, "qspi")) {
                bytecode[length++] = device_qspi;
            
                if (! parse_word(&text, "for")) {
                    goto XXX_ERROR_XXX;
                }

                // parse the chip select initial value
                if (! parse_const(&text, &length, bytecode)) {
                    goto XXX_ERROR_XXX;
                }
                if (! parse_word(&text, "csiv")) {
                    goto XXX_ERROR_XXX;
                }
            } else {
                goto XXX_ERROR_XXX;
            }
            if (*text) {
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
                if (! parse_var(true, 0, true, &text, &length, bytecode)) {
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
                    write32(bytecode+length-sizeof(uint32), 0-read32(bytecode+length-sizeof(uint32)));
                }
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
                if (! parse_var(true, 0, true, &text, &length, bytecode)) {
                    goto XXX_ERROR_XXX;
                }

                // if the user specified "as"...
                if (parse_word(&text, "as")) {
                    // parse the size specifier
                    if (parse_word(&text, "byte")) {
                        bytecode[length++] = sizeof(byte);
                        bytecode[length++] = code_ram;
                    } else if (parse_word(&text, "short")) {
                        bytecode[length++] = sizeof(short);
                        bytecode[length++] = code_ram;
                    } else {
                        bytecode[length++] = sizeof(uint32);

                        // parse the type specifier
                        if (parse_word(&text, "flash")) {
                            bytecode[length++] = code_flash;
                        } else if (parse_word(&text, "pin")) {
                            bytecode[length++] = code_pin;

                            // parse the pin name
                            for (pin_number = 0; pin_number < PIN_LAST; pin_number++) {
                                if (parse_wordn(&text, pins[pin_number].name)) {
                                    break;
                                }
                            }
                            if (pin_number == PIN_LAST) {
                                goto XXX_ERROR_XXX;
                            }
                            assert(pin_number < PIN_LAST);
                            bytecode[length++] = pin_number;

                            if (! parse_word(&text, "for")) {
                                goto XXX_ERROR_XXX;
                            }

                            // parse the pin usage
                            for (pin_type = 0; pin_type < pin_type_last; pin_type++) {
                                if (parse_words(&text, pin_type_names[pin_type])) {
                                    break;
                                }
                            }
                            if (pin_type == pin_type_last) {
                                goto XXX_ERROR_XXX;
                            }
                            bytecode[length++] = pin_type;
                            
                            // parse the pin qualifier(s)
                            pin_qual = 0;
                            for (i = 0; i < pin_qual_last; i++) {
                                if (parse_word(&text, pin_qual_names[i])) {
                                    pin_qual |= 1<<i;
                                }
                            }
                            bytecode[length++] = pin_qual;
                            
                            // see if the pin usage is legal
                            if (! (pins[pin_number].pin_type_mask & (1<<pin_type))) {
                                printf("unsupported pin type\n");
                                goto XXX_ERROR_XXX;
                            }
                            
                            // see if the pin qualifier usage is legal
                            if (pin_qual &~ pin_qual_mask[pin_type]) {
                                printf("unsupported pin qualifier\n");
                                goto XXX_ERROR_XXX;
                            }
                        } else if (parse_word(&text, "remote") && parse_word(&text, "on") && parse_word(&text, "nodeid")) {
                            bytecode[length++] = code_nodeid;
                            
                            // parse the nodeid
                            if (! parse_const(&text, &length, bytecode)) {
                                goto XXX_ERROR_XXX;
                            }
                        } else {
                            goto XXX_ERROR_XXX;
                        }
                    }
                } else {
                    bytecode[length++] = sizeof(uint32);
                    bytecode[length++] = code_ram;
                }
            }
            break;

        case code_let:
            if (! *text) {
                goto XXX_ERROR_XXX;
            }
            // while there are more items to assign...
            while (*text) {
                if (length > 1) {
                    if (! parse_char(&text, ',')) {
                        goto XXX_ERROR_XXX;
                    }
                    bytecode[length++] = code_comma;
                }

                // parse the variable
                if (! parse_var(true, 0, true, &text, &length, bytecode)) {
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

                if (parse_word(&text, "hex")) {
                    bytecode[length++] = code_hex;
                } else if (parse_word(&text, "dec")) {
                    bytecode[length++] = code_dec;
                }
                
                // if the next item is a string...
                if (*text == '"') {
                    bytecode[length++] = code_string;

                    // find the matching quote
                    p = parse_match_quote(text);
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

                    parse_trim(&text);
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

        case code_qspi:
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
                if (! parse_var(true, 0, true, &text, &length, bytecode)) {
                    goto XXX_ERROR_XXX;
                }
            }
            break;

        case code_if:
        case code_elseif:
        case code_while:
        case code_until:
            if (code == code_if || code == code_elseif) {
                // make sure we have a "then"
                if (! parse_find_tail(text, "then")) {
                    text += strlen(text);
                    goto XXX_ERROR_XXX;
                }
            } else if (code == code_while) {
                // make sure we have a "do"
                if (! parse_find_tail(text, "do")) {
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
        case code_do:
            // nothing to do here
            break;

        case code_break:
        case code_continue:
            // if the user specified a break/continue level...
            if (*text) {
                // parse the break/continue level
                if (! parse_const(&text, &length, bytecode)) {
                    goto XXX_ERROR_XXX;
                }
                assert(length >= sizeof(uint32));
                if (! read32(bytecode+length-sizeof(uint32))) {
                    goto XXX_ERROR_XXX;
                }
            } else {
                // break/continue 1 level
                write32(bytecode+length, 1);
                length += sizeof(uint32);
            }
            break;

        case code_for:
            // parse the for loop variable
            if (! parse_var(true, 0, true, &text, &length, bytecode)) {
                goto XXX_ERROR_XXX;
            }

            // parse the assignment
            if (! parse_char(&text, '=')) {
                goto XXX_ERROR_XXX;
            }

            // find the "to" keyword
            p = parse_find_keyword(text, "to");
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
            p = parse_find_keyword(text, "step");
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

        case code_label:
        case code_restore:
        case code_gosub:
        case code_sub:
            if (! *text && code != code_restore) {
                goto XXX_ERROR_XXX;
            }
            // generate the label/subname to bytecode
            while (*text && parse_is_legal_var_name_char(text)) {
                bytecode[length++] = *text++;
            }
            bytecode[length++] = '\0';

            // if there's more text after the sub name...
            if (*text) {
                parse_trim(&text);
                len = 0;

                if ((code != code_sub) && (code != code_gosub)) {
                    goto XXX_ERROR_XXX;
                }
                
                while (*text) {
                    if (len) {
                        if (! parse_char(&text, ',')) {
                            goto XXX_ERROR_XXX;
                        }
                        bytecode[length++] = code_comma;
                    }
                    len = 1;
                    if (code == code_sub) {
                        // parse the next parameter name
                        if (! parse_var(true, 0, false /* ! allow_array_index */, &text, &length, bytecode)) {
                            goto XXX_ERROR_XXX;
                        }
                    } else if (code == code_gosub) {
                        // parse the next parameter to pass
                        if (! parse_expression(0, &text, &length, bytecode)) {
                            goto XXX_ERROR_XXX;
                        }
                    }
                }
            }
            break;

        case code_return:
        case code_endsub:
            // nothing to do here
            break;

        case code_sleep:
            // parse and push the trailing timer interval unit specifier
            for (i = 0; i < timer_unit_max; i++) {
                if (parse_find_tail(text, timer_units[i].name)) {
                    bytecode[length++] = i;
                    break;
                }
            }
            if (i == timer_unit_max) {
                text += strlen(text);
                goto XXX_ERROR_XXX;
            }
            
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

// this function parses (compiles) a statement line to bytecode.
bool
parse_line(IN char *text_in, OUT int *length_out, OUT byte *bytecode, OUT int *syntax_error_in)
{
    int i;
    int len;
    bool boo;
    int length;
    char *text;
    int syntax_error;

    text = text_in;
    parse_trim(&text);

    // check for public commands
    for (i = 0; i < LENGTHOF(keywords); i++) {
        len = strlen(keywords[i].keyword);
        if (! strncmp(text, keywords[i].keyword, len)) {
            text += len;
            break;
        }
    }
    if (i == LENGTHOF(keywords)) {
        // check for private commands
        if (parse2_line(text_in, length_out, bytecode, &syntax_error)) {
            return true;
        }

        *syntax_error_in = text - text_in + syntax_error;
        assert(*syntax_error_in >= 0 && *syntax_error_in < BASIC_LINE_SIZE);
        return false;
    }

    *bytecode = keywords[i].code;

    // parse the public command
    boo = parse_line_code(keywords[i].code, text, &length, bytecode+1, &syntax_error);
    if (! boo) {
        *syntax_error_in = text - text_in + syntax_error;
        assert(*syntax_error_in >= 0 && *syntax_error_in < BASIC_LINE_SIZE);
        return boo;
    }
    
    *length_out = 1+length;
    return true;
}


// *** bytecode de-compiler ***

// this function unparses (de-compiles) a variable from bytecode.
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
        blen = read32(bytecode);
        bytecode += sizeof(uint32);

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
int
unparse_expression(int tbase, byte *bytecode_in, int length, char **out)
{
    int i;
    int n;
    int ttop;
    byte code;
    int32 value;
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
                value = read32(bytecode);
                bytecode += sizeof(uint32);
                precedence[ttop] = 100;
                // decompile the constant
                if (code == code_load_and_push_immediate_hex) {
                    sprintf(texts[ttop++], "0x%lx", value);
                } else {
                    sprintf(texts[ttop++], "%ld", value);
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

// this function unparses (de-compiles) a public statement line from bytecode,
// excluding the keyword.
void
unparse_bytecode_code(IN byte code, IN byte *bytecode_in, IN int length, OUT char *out)
{
    int i;
    int n;
    int len;
    int size;
    int pin;
    int timer;
    int type;
    int qual;
    int uart;
    int32 baud;
    int data;
    int32 value;
    byte parity;
    byte loopback;
    int32 interval;
    enum timer_unit_type timer_unit;
    int csiv;
    byte device;
    byte code2;
    byte *bytecode;
    bool output;
    int nodeid;

    bytecode = bytecode_in;

    switch (code) {
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
                timer = read32(bytecode);
                assert(timer >= 0 && timer < MAX_TIMERS);
                bytecode += sizeof(uint32);
                out += sprintf(out, "%d", timer);

            } else if (device == device_uart) {
                // decompile the uart
                out += sprintf(out, "uart ");

                // and the uart name
                uart = *bytecode++;
                assert(uart >= 0 && uart < MAX_UARTS);
                out += sprintf(out, "%s ", uart_names[uart]);

                // and the uart data direction
                output = *bytecode++;
                out += sprintf(out, "%s", output?"output":"input");
            } else if (device == device_watch) {

                // this is an expression
                len = read32(bytecode);
                bytecode += sizeof(uint32);

                bytecode += unparse_expression(0, bytecode, len, &out);

                assert(*bytecode == code_comma);
                bytecode++;
            } else {
                assert(0);
            }

            // if we're enabling interrupts...
            if (code == code_on) {
                out += sprintf(out, " do ");

                len = read32(bytecode);
                bytecode += sizeof(uint32);

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
                timer = read32(bytecode);
                assert(timer >= 0 && timer < MAX_TIMERS);
                bytecode += sizeof(uint32);
                out += sprintf(out, "%d ", timer);

                // and the timer interval
                interval = read32(bytecode);
                bytecode += sizeof(uint32);
                out += sprintf(out, "for %ld ", interval);

                // and the timer interval unit specifier
                timer_unit = (enum timer_unit_type)*(bytecode++);
                out += sprintf(out, "%s", timer_units[timer_unit].name);

            } else if (device == device_uart) {
                // decompile the uart
                out += sprintf(out, "uart ");

                // and the uart name
                uart = *bytecode++;
                assert(uart >= 0 && uart < MAX_UARTS);
                out += sprintf(out, "%s ", uart_names[uart]);

                // find the uart protocol and optional loopback specifier
                baud = read32(bytecode);
                bytecode += sizeof(uint32);
                data = read32(bytecode);
                bytecode += sizeof(uint32);
                parity = *bytecode++;
                loopback = *bytecode++;

                // and decompile it
                out += sprintf(out, "for %ld baud %d data %s parity%s", baud, data, parity==0?"even":(parity==1?"odd":"no"), loopback?" loopback":"");
            } else if (device == device_qspi) {
                // decompile the qspi
                out += sprintf(out, "qspi ");

                // and the chip select initial value
                csiv = read32(bytecode);
                bytecode += sizeof(uint32);
                out += sprintf(out, "for %d csiv", csiv);

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
                code2 = *bytecode++;
                value = read32(bytecode);
                bytecode += sizeof(uint32);
                if (code2 == code_load_and_push_immediate_hex) {
                    out += sprintf(out, "0x%lx", value);
                } else {
                    assert(code2 == code_load_and_push_immediate);
                    out += sprintf(out, "%ld", value);
                }
            }
            break;

        case code_dim:
#if ! GCC
            cw7bug++;
#endif
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

                size = *bytecode++;
                code2 = *bytecode++;

                // decompile the "as"
                if (size != sizeof(uint32) || code2 != code_ram) {
                    out += sprintf(out, " as ");

                    // decompile the size specifier
                    if (size == sizeof(byte)) {
                        assert(code2 == code_ram);
                        out += sprintf(out, "byte");
                    } else if (size == sizeof(short)) {
                        assert(code2 == code_ram);
                        out += sprintf(out, "short");
                    } else {
                        assert(size == sizeof(uint32));
                        
                        // decompile the type specifier
                        if (code2 == code_flash) {
                            out += sprintf(out, "flash");
                        } else if (code2 == code_pin) {
                            out += sprintf(out, "pin ");

                            pin = *bytecode++;
                            type = *bytecode++;
                            qual = *bytecode++;

                            // decompile the pin name
                            assert(pin >= 0 && pin < PIN_LAST);
                            out += sprintf(out, "%s ", pins[pin].name);

                            out += sprintf(out, "for ");

                            // decompile the pin usage
                            assert(type >= 0 && type < pin_type_last);
                            out += sprintf(out, "%s", pin_type_names[type]);
                            
                            // decompile the pin qualifier(s)
                            for (i = 0; i < pin_qual_last; i++) {
                                if (qual & (1<<i)) {
                                    out += sprintf(out, " %s", pin_qual_names[i]);
                                }
                            }
                        } else {
                            assert(code2 == code_nodeid);

                            nodeid = read32(bytecode);
                            bytecode += sizeof(uint32);
                            
                            out += sprintf(out, "remote on nodeid %u", nodeid);
                        }
                    }
                }
            }
            break;

        case code_let:
            // while there are more items...
            while (bytecode < bytecode_in+length) {
                if (bytecode > bytecode_in+1) {
                    // separate items with a comma
                    out += sprintf(out, ", ");
                    assert(*bytecode == code_comma);
                    bytecode++;
                }

                // decompile the variable
                bytecode += unparse_var_lvalue(bytecode, &out);

                // decompile the assignment
                out += sprintf(out, " = ");

                // decompile the expression
                bytecode += unparse_expression(0, bytecode, bytecode_in+length-bytecode, &out);
            }
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

                if (*bytecode == code_hex) {
                    bytecode++;
                    out += sprintf(out, "hex ");
                } else if (*bytecode == code_dec) {
                    bytecode++;
                    out += sprintf(out, "dec ");
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

        case code_qspi:
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

        case code_if:
        case code_elseif:
        case code_while:
        case code_until:
            // decompile the conditional expression
            bytecode += unparse_expression(0, bytecode, bytecode_in+length-bytecode, &out);
            // decompile the "then" or "do"
            if (code == code_if || code == code_elseif) {
                out += sprintf(out, " then");
            } else if (code == code_while) {
                out += sprintf(out, " do");
            }
            break;

        case code_else:
        case code_endif:
        case code_endwhile:
        case code_do:
            // nothing to do here
            break;

        case code_break:
        case code_continue:
            n = read32(bytecode);
            bytecode += sizeof(uint32);
            // if the break/continue level is not 1...
            if (n != 1) {
                // decompile the break/continue level
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

        case code_label:
        case code_restore:
        case code_gosub:
        case code_sub:
            // decompile the label/sub name
            len = sprintf(out, "%s", bytecode);
            out += len;
            bytecode += len+1;
            n = 0;

            if ((code == code_sub) || (code == code_gosub)) {
                // decompile the parameter names separating parameter names with a comma
                while (bytecode < bytecode_in+length) {
                    if (n) {
                        assert(*bytecode == code_comma);
                        bytecode++;
                        *(out++) = ',';
                    }
                    *(out++) = ' ';
                    n = 1;
                    
                    bytecode += unparse_expression(0, bytecode, bytecode_in+length-bytecode, &out);
                }
            }
            break;

        case code_return:
        case code_endsub:
            // nothing to do here
            break;

        case code_sleep:
            // decompile the sleep time units
            timer_unit = (enum timer_unit_type)*(bytecode++);

            // and the sleep time expression
            bytecode += unparse_expression(0, bytecode, bytecode_in+length-bytecode, &out);

            out += sprintf(out, " %s", timer_units[timer_unit].name);
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

// this function unparses (de-compiles) a statement line from bytecode.
void
unparse_bytecode(IN byte *bytecode, IN int length, OUT char *text)
{
    int i;
    char *out;
    byte code;

    out = text;

    code = *bytecode;

    // find the bytecode keyword
    for (i = 0; i < LENGTHOF(keywords); i++) {
        if (keywords[i].code == code) {
            break;
        }
    }
    if (i == LENGTHOF(keywords)) {
        unparse2_bytecode(bytecode, length, out);
        return;
    }

    // decompile the bytecode keyword
    out += sprintf(out, keywords[i].keyword);
    out += sprintf(out, " ");

    assert(length);
    unparse_bytecode_code(code, bytecode+1, length-1, out);
}

