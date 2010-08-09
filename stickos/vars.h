// *** vars.h *********************************************************

extern bool var_trace;

// variables at end of flash page
enum flash_var {
    FLASH_AUTORUN,
    FLASH_AUTORESET,
    FLASH_NODEID,
    FLASH_NEXT,
    // private variables here
    FLASH_LAST = 0x100
};

#define FLASH_OFFSET(var)  (BASIC_SMALL_PAGE_SIZE-sizeof(int)-(FLASH_LAST-var)*sizeof(int))


int var_open_scope();
void var_close_scope(int scope);

// called at runtime to manipulate variables
void var_declare(const char *name, int gosubs, int type, int size, int max_index, int pin_number, int pin_type, int pin_qual, int nodeid);
void var_declare_reference(const char *name, int gosubs, const char *target_name);
void var_set(const char *name, int index, int value);
int var_get(const char *name, int index);
int var_get_size(const char *name);

void var_set_flash(int var, int value);
int var_get_flash(int var);

// resets volatile variables (and flash params) prior to a run
void var_clear(bool flash);

void var_mem(void);

void var_poll(void);

void var_initialize(void);

