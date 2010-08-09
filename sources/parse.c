#include "main.h"

// *** parse ****************************************************************

#if BASIC

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

static
void
trim(IN char **p)
{
    while (isspace(**p)) {
        (*p)++;
    }
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

// revisit -- merge these with basic.c/parse.c/run.c???

static
bool
parse_char(IN OUT char **text, IN char c)
{
    if (**text != c) {
        return false;
    }

    (*text)++;

    trim(text);
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
    
    (*text) += len;
    
    trim(text);
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

    p = find_keyword(*text, tail);
    if (! p) {
        return false;
    }

    // make sure nothing follows
    *p = '\0';
    p += strlen(tail);
    trim(&p);
    if (*p) {
        return false;
    }

    // N.B. no need to trim(text);
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

    trim(text);
    return true;
}

static
bool
parse_expression(IN int obase, IN OUT char **text, IN OUT int *length, IN OUT byte *bytecode);

static
int
unparse_expression(int tbase, byte *bytecode_in, int length, char **out);

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

    i = 0;
    while (isalpha(**text) || isdigit(**text) || **text == '_') {
        name[i++] = *(*text)++;
    }
    name[i] = '\0';

    if (**text == '[') {
        if (lvalue) {
            // for lvalues we push the code first, followed by the expression length
            bytecode[(*length)++] = code_load_and_push_var_indexed;
            hole = (int *)(bytecode + *length);
            *length += sizeof(int);
            olength = *length;
        }

        p = match_paren(*text);
        if (! p) {
            return false;
        }
        assert(*p == ']');
        *p = '\0';
        assert(**text == '[');
        (*text)++;

        // first we push the index
        if (! parse_expression(obase, text, length, bytecode)) {
            return false;
        }

        *text = p;
        (*text)++;

        if (lvalue) {
            *hole = *length - olength;
        } else {
            // then we push the variable deref
            bytecode[(*length)++] = code_load_and_push_var_indexed;
        }
    } else {
        // then we push the variable deref
        bytecode[(*length)++] = code_load_and_push_var;
    }

    for (i = 0; name[i]; i++) {
        bytecode[(*length)++] = name[i];
    }
    bytecode[(*length)++] = '\0';

    trim(text);
    return true;
}

int
unparse_var_lvalue(byte *bytecode_in, char **out)
{
    int blen;
    byte *bytecode;

    bytecode = bytecode_in;

    if ((*bytecode) == code_load_and_push_var_indexed) {
        bytecode++;

        blen = *(int *)bytecode;
        bytecode += sizeof(int);

        *out += sprintf(*out, "%s[", bytecode+blen);
        unparse_expression(0, bytecode, blen, out);
        *out += sprintf(*out, "]");
        bytecode += blen;
        bytecode += strlen((char *)bytecode)+1;
    } else {
        assert((*bytecode) == code_load_and_push_var);
        bytecode++;
        *out += sprintf(*out, "%s", bytecode);
        bytecode += strlen((char *)bytecode)+1;
    }

    return bytecode - bytecode_in;
}

static
char *
match_quote(char *p)
{
    char c;

    assert(*p == '"');

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

#define MAX_OP_STACK  10

static struct op *op_stack[MAX_OP_STACK];

static
void
parse_clean(IN int obase, IN OUT int *otop, IN int precedence, IN OUT int *length, IN OUT byte *bytecode)
{
    int j;
    bool pop;

    for (j = *otop-1; j >= obase+1; j--) {
        assert(op_stack[j]->precedence >= op_stack[j-1]->precedence);
    }

    for (j = *otop-1; j >= obase; j--) {
        if (op_stack[j]->code == code_logical_not || op_stack[j]->code == code_bitwise_not) {
            // right to left associativity
            pop = op_stack[j]->precedence > precedence;
        } else {
            // left to right associativity
            pop = op_stack[j]->precedence >= precedence;
        }
        if (pop) {
            // pop it
            *otop = j;
            bytecode[(*length)++] = op_stack[j]->code;
        } else {
            break;
        }
    }
}

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
        if (isdigit(c)) {
            if (! number) {
                return false;
            }
            if ((*text)[0] == '0' && (*text)[1] == 'x') {
                bytecode[(*length)++] = code_load_and_push_immediate_hex;
            } else {
                bytecode[(*length)++] = code_load_and_push_immediate;
            }
            if (! parse_const(text, length, bytecode)) {
                return false;
            }
            number = false;
        } else if (isalpha(c)) {
            if (! number) {
                return false;
            }
            if (! parse_var(false, otop, text, length, bytecode)) {
                return false;
            }
            number = false;
        } else {
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

                if (! parse_expression(otop, text, length, bytecode)) {
                    return false;
                }

                *text = p;
                (*text)++;
                number = false;
            } else {
                // this should be an operator!!!
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
                    i++;
                    assert(ops[i].code == code_unary_plus);
                } else if (number && ops[i].code == code_subtract) {
                    i++;
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

            trim(text);
        }
    }

    // clean the entire op stack!
    parse_clean(obase, &otop, 0, length, bytecode);
    assert(otop == obase);
    return true;
}

#define MAX_TEXTS  10

// revisit -- can we reduce memory???
static char texts[MAX_TEXTS][BASIC_LINE_SIZE];
static int precedence[MAX_TEXTS];

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
                if (code == code_load_and_push_immediate_hex) {
                    sprintf(texts[ttop++], "0x%x", value);
                } else {
                    sprintf(texts[ttop++], "%d", value);
                }
                break;

            case code_load_and_push_var:
                precedence[ttop] = 100;
                sprintf(texts[ttop++], "%s", bytecode);
                bytecode += strlen((char *)bytecode)+1;
                break;

            case code_load_and_push_var_indexed:
                // our index is already on the stack
                strcpy(temp, texts[ttop-1]);

                precedence[ttop-1] = 100;
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
                if (! unary) {
                    if (ops[i].precedence > precedence[ttop-2]) {
                        n += sprintf(temp+n, "(");
                    }
                    n += sprintf(temp+n, "%s", texts[ttop-2]);
                    if (ops[i].precedence > precedence[ttop-2]) {
                        n += sprintf(temp+n, ")");
                    }
                }
                n += sprintf(temp+n, "%s", ops[i].op);
                if (ops[i].precedence >= precedence[ttop-1]+unary) {
                    n += sprintf(temp+n, "(");
                }
                n += sprintf(temp+n, "%s", texts[ttop-1]);
                if (ops[i].precedence >= precedence[ttop-1]+unary) {
                    n += sprintf(temp+n, ")");
                }
                if (! unary) {
                    ttop -= 2;
                } else {
                    ttop -= 1;
                }

                precedence[ttop] = ops[i].precedence;
                sprintf(texts[ttop++], "%s", temp);
                break;
        }
    }

    assert(ttop == tbase+1);
    *out += sprintf(*out, "%s", texts[tbase]);

    return bytecode - bytecode_in;
}

bool
parse_line(IN char *text_in, OUT int *length_out, OUT byte *bytecode, OUT int *syntax_error)
{
    int i;
    char *p;
    int len;
    int size;
    int length;
    char *text;
    int pin_type;
    int pin_number;

    text = text_in;
    trim(&text);

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

    trim(&text);

    length = 0;
    bytecode[length++] = keywords[i].code;

    switch (keywords[i].code) {
        case code_rem:
            while (*text) {
                bytecode[length++] = *text++;
            }
            bytecode[length++] = *text;
            break;

        case code_on:
        case code_off:
        case code_mask:
        case code_unmask:
            if (parse_word(&text, "timer")) {
                bytecode[length++] = device_timer;
            
                if (! parse_const(&text, &length, bytecode)) {
                    goto XXX_ERROR_XXX;
                }
                assert(length >= sizeof(int));
                if (*(int *)(bytecode+length-sizeof(int)) < 0 || *(int *)(bytecode+length-sizeof(int)) >= MAX_TIMERS) {
                    goto XXX_ERROR_XXX;
                }
            } else if (parse_word(&text, "uart")) {
                bytecode[length++] = device_uart;
            
                if (! parse_const(&text, &length, bytecode)) {
                    goto XXX_ERROR_XXX;
                }
                assert(length >= sizeof(int));
                if (*(int *)(bytecode+length-sizeof(int)) < 0 || *(int *)(bytecode+length-sizeof(int)) >= MAX_UARTS) {
                    goto XXX_ERROR_XXX;
                }
                
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
            if (keywords[i].code == code_on) {
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
            if (parse_word(&text, "timer")) {
                bytecode[length++] = device_timer;

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

                // interval
                if (! parse_const(&text, &length, bytecode)) {
                    goto XXX_ERROR_XXX;
                }
                if (! parse_word(&text, "ms")) {
                    goto XXX_ERROR_XXX;
                }
            } else if (parse_word(&text, "uart")) {
                bytecode[length++] = device_uart;
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

                // baud rate
                if (! parse_const(&text, &length, bytecode)) {
                    goto XXX_ERROR_XXX;
                }
                if (! parse_word(&text, "baud")) {
                    goto XXX_ERROR_XXX;
                }

                // data bits
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
            if (! parse_expression(0, &text, &length, bytecode)) {
                goto XXX_ERROR_XXX;
            }
            break;

        case code_read:
            if (! *text) {
                goto XXX_ERROR_XXX;
            }
            while (*text) {
                if (length > 1) {
                    if (! parse_char(&text, ',')) {
                        goto XXX_ERROR_XXX;
                    }
                    bytecode[length++] = code_comma;
                }
                
                if (! parse_var(true, 0, &text, &length, bytecode)) {
                    goto XXX_ERROR_XXX;
                }
            }
            break;

        case code_data:
            if (! *text) {
                goto XXX_ERROR_XXX;
            }
            while (*text) {
                if (length > 1) {
                    if (! parse_char(&text, ',')) {
                        goto XXX_ERROR_XXX;
                    }
                }
                
                if (text[0] == '0' && text[1] == 'x') {
                    bytecode[length++] = code_load_and_push_immediate_hex;
                } else {
                    bytecode[length++] = code_load_and_push_immediate;
                }
                if (! parse_const(&text, &length, bytecode)) {
                    goto XXX_ERROR_XXX;
                }
            }
            break;

        case code_restore:
            if (*text) {
                if (! parse_const(&text, &length, bytecode)) {
                    goto XXX_ERROR_XXX;
                }
            } else {
                *(int *)(bytecode+length) = 0;
                length += sizeof(int);
            }
            break;

        case code_dim:
            if (! *text) {
                goto XXX_ERROR_XXX;
            }
            while (*text) {
                if (length > 1) {
                    if (! parse_char(&text, ',')) {
                        goto XXX_ERROR_XXX;
                    }
                    bytecode[length++] = code_comma;
                }
                
                if (! parse_var(true, 0, &text, &length, bytecode)) {
                    goto XXX_ERROR_XXX;
                }
                
                if (! parse_word(&text, "as")) {
                    continue;
                }
                bytecode[length++] = code_as;
                
                if (parse_word(&text, "byte")) {
                    size = sizeof(byte);
                } else if (parse_word(&text, "integer")) {
                    size = sizeof(int);
                } else {
                    size = sizeof(int);
                }
                bytecode[length++] = size;
                
                if (parse_word(&text, "ram")) {
                    bytecode[length++] = code_ram;
                } else if (parse_word(&text, "flash")) {
                    if (size != sizeof(int)) {  // integer only
                        goto XXX_ERROR_XXX;
                    }
                    bytecode[length++] = code_flash;
                } else if (parse_word(&text, "pin")) {
                    bytecode[length++] = code_pin;
          
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
                    
                    if (parse_word(&text, "analog")) {
                        if (parse_word(&text, "input")) {
                            bytecode[length++] = pin_type_analog_input;
                        } else {
                            printf("unsupported pin type\n");
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
                    bytecode[length++] = code_ram;
                }
            }
            break;

        case code_let:
            if (! parse_var(true, 0, &text, &length, bytecode)) {
                goto XXX_ERROR_XXX;
            }
            if (! parse_char(&text, '=')) {
                goto XXX_ERROR_XXX;
            }

            if (! parse_expression(0, &text, &length, bytecode)) {
                goto XXX_ERROR_XXX;
            }
            break;

        case code_print:
            if (! *text) {
                goto XXX_ERROR_XXX;
            }
            while (*text) {
                if (length > 1) {
                    if (! parse_char(&text, ',')) {
                        goto XXX_ERROR_XXX;
                    }
                    bytecode[length++] = code_comma;
                }
                if (*text == '"') {
                    p = match_quote(text);
                    if (! p) {
                        goto XXX_ERROR_XXX;
                    }

                    assert(*p == '"');
                    *p = '\0';
                    assert(*text == '"');
                    text++;

                    bytecode[length++] = code_string;
                    while (*text) {
                        bytecode[length++] = *text++;
                    }
                    bytecode[length++] = *text;

                    assert(text == p);
                    text++;
                } else {
                    bytecode[length++] = code_expression;
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
                if (! parse_tail(&text, "then")) {
                    text += strlen(text);
                    goto XXX_ERROR_XXX;
                }
            } else {
                if (! parse_tail(&text, "do")) {
                    text += strlen(text);
                    goto XXX_ERROR_XXX;
                }
            }
            if (! parse_expression(0, &text, &length, bytecode)) {
                goto XXX_ERROR_XXX;
            }
            break;

        case code_else:
        case code_endif:
        case code_endwhile:
            break;

        case code_break:
            if (*text) {
                if (! parse_const(&text, &length, bytecode)) {
                    goto XXX_ERROR_XXX;
                }
                assert(length >= sizeof(int));
                if (! *(int *)(bytecode+length-sizeof(int))) {
                    goto XXX_ERROR_XXX;
                }
            } else {
                *(int *)(bytecode+length) = 1;
                length += sizeof(int);
            }
            break;

        case code_for:
            if (! parse_var(true, 0, &text, &length, bytecode)) {
                goto XXX_ERROR_XXX;
            }
            if (! parse_char(&text, '=')) {
                goto XXX_ERROR_XXX;
            }

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
            trim(&text);

            p = find_keyword(text, "step");
            if (p) {
                *p = 0;
            }

            bytecode[length++] = code_comma;

            // parse final valie
            if (! parse_expression(0, &text, &length, bytecode)) {
                goto XXX_ERROR_XXX;
            }
            if (*text) {
                goto XXX_ERROR_XXX;
            }

            if (p) {
                text = p+4;
                trim(&text);

                bytecode[length++] = code_comma;

                // parse step valie
                if (! parse_expression(0, &text, &length, bytecode)) {
                    goto XXX_ERROR_XXX;
                }
            }
            break;

        case code_next:
            break;

        case code_gosub:
        case code_sub:
            if (! *text) {
                goto XXX_ERROR_XXX;
            }
            while (*text) {
                bytecode[length++] = *text++;
            }
            bytecode[length++] = *text;
            break;

        case code_return:
        case code_endsub:
            break;

        case code_sleep:
            if (! parse_expression(0, &text, &length, bytecode)) {
                goto XXX_ERROR_XXX;
            }
            break;
            
        case code_stop:
        case code_end:
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

    for (i = 0; i < LENGTHOF(keywords); i++) {
        if (keywords[i].code == *bytecode_in) {
            break;
        }
    }
    assert(i != LENGTHOF(keywords));

    out = text;
    out += sprintf(out, keywords[i].keyword);
    out += sprintf(out, " ");

    switch ((code = *bytecode++)) {
        case code_rem:
            len = sprintf(out, "%s", bytecode);
            out += len;
            bytecode += len+1;
            break;

        case code_on:
        case code_off:
        case code_mask:
        case code_unmask:
            device = *bytecode++;
            if (device == device_timer) {
                out += sprintf(out, "timer ");
                
                timer = *(int *)bytecode;
                assert(timer >= 0 && timer < MAX_TIMERS);
                bytecode += sizeof(int);
                out += sprintf(out, "%d", timer);
                
                if (code == code_on) {
                    out += sprintf(out, " ");

                    len = *(int *)bytecode;
                    bytecode += sizeof(int);

                    unparse_bytecode(bytecode, len, out);
                    bytecode += len;
                }
            } else if (device == device_uart) {
                out += sprintf(out, "uart ");
                
                uart = *(int *)bytecode;
                assert(uart >= 0 && uart < MAX_UARTS);
                bytecode += sizeof(int);
                out += sprintf(out, "%d ", uart);

                output = *bytecode++;
                out += sprintf(out, "%s", output?"output":"input");
                
                if (code == code_on) {
                    out += sprintf(out, " ");

                    len = *(int *)bytecode;
                    bytecode += sizeof(int);

                    unparse_bytecode(bytecode, len, out);
                    bytecode += len;
                }
            } else {
                assert(0);
            }
            break;

        case code_configure:
            device = *bytecode++;
            if (device == device_timer) {
                out += sprintf(out, "timer ");
                
                timer = *(int *)bytecode;
                assert(timer >= 0 && timer < MAX_TIMERS);
                bytecode += sizeof(int);
                out += sprintf(out, "%d ", timer);

                interval = *(int *)bytecode;
                bytecode += sizeof(int);

                out += sprintf(out, "for %d ms", interval);
            } else if (device == device_uart) {
                out += sprintf(out, "uart ");
                
                uart = *(int *)bytecode;
                assert(uart >= 0 && uart < MAX_UARTS);
                bytecode += sizeof(int);
                out += sprintf(out, "%d ", uart);

                baud = *(int *)bytecode;
                bytecode += sizeof(int);
                data = *(int *)bytecode;
                bytecode += sizeof(int);
                parity = *bytecode++;
                loopback = *bytecode++;

                out += sprintf(out, "for %d baud %d data %s parity%s", baud, data, parity==0?"even":(parity==1?"odd":"no"), loopback?" loopback":"");
            } else {
                assert(0);
            }
            break;

        case code_assert:
            bytecode += unparse_expression(0, bytecode, bytecode_in+length-bytecode, &out);
            break;

        case code_read:
            while (bytecode < bytecode_in+length) {
                if (bytecode > bytecode_in+1) {
                    out += sprintf(out, ", ");
                    assert(*bytecode == code_comma);
                    bytecode++;
                }

                bytecode += unparse_var_lvalue(bytecode, &out);
            }
            break;

        case code_data:
            while (bytecode < bytecode_in+length) {
                if (bytecode > bytecode_in+1) {
                    out += sprintf(out, ", ");
                }

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
            if (line) {
                out += sprintf(out, "%d", line);
            }
            break;

        case code_dim:
            while (bytecode < bytecode_in+length) {
                if (bytecode > bytecode_in+1) {
                    out += sprintf(out, ", ");
                    assert(*bytecode == code_comma);
                    bytecode++;
                }

                bytecode += unparse_var_lvalue(bytecode, &out);
                
                if (*bytecode != code_as) {
                    continue;
                }
                
                out += sprintf(out, " as ");
                bytecode++;
                
                size = *bytecode++;
                if (size == sizeof(byte)) {
                    out += sprintf(out, "byte ");
                } else {
                    assert(size == sizeof(int));
                    //out += sprintf(out, "integer ");
                }
                
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
                    
                    assert(pin >= 0 && pin < PIN_LAST);
                    out += sprintf(out, "%s ", pins[pin].name);
                    
                    out += sprintf(out, "for ");

                    if (type == pin_type_analog_input) {
                        out += sprintf(out, "%s", "analog input");
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

            bytecode += unparse_var_lvalue(bytecode, &out);

            out += sprintf(out, " = ");

            bytecode += unparse_expression(0, bytecode, bytecode_in+length-bytecode, &out);
            break;

        case code_print:
            while (bytecode < bytecode_in+length) {
                if (bytecode > bytecode_in+1) {
                    out += sprintf(out, ", ");
                    assert(*bytecode == code_comma);
                    bytecode++;
                }
                if (*bytecode == code_string) {
                    bytecode++;
                    out += sprintf(out, "\"");
                    len = sprintf(out, "%s", bytecode);
                    out += len;
                    out += sprintf(out, "\"");
                    bytecode += len+1;
                } else {
                    assert(*bytecode == code_expression);
                    bytecode++;
                    bytecode += unparse_expression(0, bytecode, bytecode_in+length-bytecode, &out);
                }
            }
            break;

        case code_if:
        case code_elseif:
        case code_while:
            bytecode += unparse_expression(0, bytecode, bytecode_in+length-bytecode, &out);
            if (code == code_if || code == code_elseif) {
                out += sprintf(out, " then");
            } else {
                out += sprintf(out, " do");
            }
            break;

        case code_else:
        case code_endif:
        case code_endwhile:
            break;

        case code_break:
            n = *(int *)bytecode;
            bytecode += sizeof(int);
            if (n != 1) {
                out += sprintf(out, " %d", n);
            }
            break;

        case code_for:
            assert(bytecode < bytecode_in+length);

            bytecode += unparse_var_lvalue(bytecode, &out);

            out += sprintf(out, " = ");
            bytecode += unparse_expression(0, bytecode, bytecode_in+length-bytecode, &out);

            assert(*bytecode == code_comma);
            bytecode++;
            out += sprintf(out, " to ");
            bytecode += unparse_expression(0, bytecode, bytecode_in+length-bytecode, &out);

            if (bytecode_in+length > bytecode) {
                assert(*bytecode == code_comma);
                bytecode++;
                out += sprintf(out, " step ");
                bytecode += unparse_expression(0, bytecode, bytecode_in+length-bytecode, &out);
            }
            break;

        case code_next:
            break;

        case code_gosub:
            len = sprintf(out, "%s", bytecode);
            out += len;
            bytecode += len+1;
            break;

        case code_sub:
            len = sprintf(out, "%s", bytecode);
            out += len;
            bytecode += len+1;
            break;

        case code_return:
        case code_endsub:
            break;

        case code_sleep:
            bytecode += unparse_expression(0, bytecode, bytecode_in+length-bytecode, &out);
            break;
            
        case code_stop:
        case code_end:
            break;

        default:
            assert(0);
            break;
    }

    assert(bytecode == bytecode_in+length);
}

#endif
