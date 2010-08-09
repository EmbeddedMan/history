// *** basic.h ********************************************************

enum devices {
    device_timer,
    device_uart
};

// bytecodes
enum bytecode {
    code_deleted = 0x80,  // used to indicate a deleted line in ram
    code_rem,
    code_on,
      code_timer,  // used for on and configure
      code_uart,  // used for on and configure
    code_off,
    code_mask,
    code_unmask,
    code_configure,
    code_assert,
    code_read,
      code_data,
      code_restore,
    code_dim,
      code_comma,  // used for dim and print
      code_as,
      code_ram,  // page_offset in RAM_VARIABLE_PAGE (allocated internally)
      code_flash,  // page_offset in FLASH_PARAM_PAGE (allocated internally)
      code_pin,  // pin_number, pin_type
    code_let,
    code_print,
      code_string,
      code_expression,
    code_if,
      code_elseif,
      code_else,
      code_endif,
    code_while,
      code_break,
      code_endwhile,  // N.B. for loops translate into while/endwhile
    code_for,
      code_next,
    code_gosub,
      code_sub,
      code_return,
      code_endsub,
    code_sleep,
    code_stop,
    code_end,

    // exressions
    code_load_and_push_immediate,  // integer
    code_load_and_push_immediate_hex,  // hex integer
    code_load_and_push_var,  // variable name, '\0'
    code_load_and_push_var_indexed, // index on stack; variable name, '\0'
    code_logical_not, code_bitwise_not,
    code_unary_plus, code_unary_minus,
    code_add, code_subtract, code_multiply, code_divide, code_mod,
    code_shift_right, code_shift_left,
    code_bitwise_and, code_bitwise_or, code_bitwise_xor,
    code_logical_and, code_logical_or, code_logical_xor,
    code_greater, code_less, code_equal,
    code_greater_or_equal, code_less_or_equal, code_not_equal,
    code_max
};


#define BASIC_SMALL_PAGE_SIZE  2048
#define BASIC_LARGE_PAGE_SIZE  (10*1024)
#define BASIC_BYTECODE_SIZE  (4*BASIC_LINE_SIZE)

#define BASIC_STORES  3

#if ! _WIN32
#define START_DYNAMIC  (FLASH_BYTES - ((2+BASIC_STORES)*BASIC_LARGE_PAGE_SIZE+3*BASIC_SMALL_PAGE_SIZE))
#define FLASH_CODE1_PAGE     (byte *)(START_DYNAMIC)
#define FLASH_CODE2_PAGE     (byte *)(START_DYNAMIC+BASIC_LARGE_PAGE_SIZE)
#define FLASH_STORE_PAGE(x)  (byte *)(START_DYNAMIC+((2+(x))*BASIC_LARGE_PAGE_SIZE))
#define FLASH_CATALOG_PAGE   (byte *)(START_DYNAMIC+(2+BASIC_STORES)*BASIC_LARGE_PAGE_SIZE)
#define FLASH_PARAM1_PAGE    (byte *)(START_DYNAMIC+(2+BASIC_STORES)*BASIC_LARGE_PAGE_SIZE+BASIC_SMALL_PAGE_SIZE)
#define FLASH_PARAM2_PAGE    (byte *)(START_DYNAMIC+(2+BASIC_STORES)*BASIC_LARGE_PAGE_SIZE+2*BASIC_SMALL_PAGE_SIZE)
#else
extern byte FLASH_CODE1_PAGE[BASIC_LARGE_PAGE_SIZE];
extern byte FLASH_CODE2_PAGE[BASIC_LARGE_PAGE_SIZE];
extern byte FLASH_STORE_PAGES[BASIC_STORES][BASIC_LARGE_PAGE_SIZE];
#define FLASH_STORE_PAGE(x)  FLASH_STORE_PAGES[x]
extern byte FLASH_CATALOG_PAGE[BASIC_SMALL_PAGE_SIZE];
extern byte FLASH_PARAM1_PAGE[BASIC_SMALL_PAGE_SIZE];
extern byte FLASH_PARAM2_PAGE[BASIC_SMALL_PAGE_SIZE];
#endif

extern byte RAM_CODE_PAGE[BASIC_SMALL_PAGE_SIZE];
extern byte RAM_VARIABLE_PAGE[BASIC_SMALL_PAGE_SIZE];

extern byte *start_of_dynamic;
extern byte *end_of_dynamic;

void basic_run(char *line);

void basic_initialize(void);

