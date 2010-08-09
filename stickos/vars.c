// *** vars.c *********************************************************
// this file implements the variable access module, including ram,
// pin, and flash variables, as well as the external pin control and
// access module.

// Copyright (c) Rich Testardi, 2008.  All rights reserved.
// Patent pending.

#include "main.h"

// the last word of each flash bank is the generation number
#define SGENERATION(p)  *(int *)((p)+BASIC_SMALL_PAGE_SIZE-sizeof(int))

// we always pick the newer flash bank
#define FLASH_PARAM_PAGE  ((SGENERATION(FLASH_PARAM1_PAGE)+1 > SGENERATION(FLASH_PARAM2_PAGE)+1) ? FLASH_PARAM1_PAGE : FLASH_PARAM2_PAGE)

static byte *alternate_flash_param_page;

// *** pin variables ***

static struct system_var {
    char *name;
    int *integer;  // if NULL, then variable is constant.
    int constant;
    void (*set_cbfn)(int value);  // if NULL, then the var is read-only.
} systems[] = {
#if ! STICK_GUEST
    "nodeid", &zb_nodeid, 0, NULL,
#endif
    "msecs", (int *)&msecs, 0, NULL,
    "seconds", (int *)&seconds, 0, NULL,
    "ticks", (int *)&ticks, 0, NULL,
    "ticks_per_msec", NULL, ticks_per_msec, NULL
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
        struct {
            byte number;
            uint8 type;
            uint8 qual;
        } pin;
        struct var *target_var; // type=code_var_reference
    } u;
    int nodeid;  // for remote variable sets
} vars[MAX_VARS];

static int max_vars;  // allocated

static int ram_offset;  // allocated in RAM_VARIABLE_PAGE
static int param_offset;  // allocated in FLASH_PARAM_PAGE

// *** system variable access routines ***

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
    generation = SGENERATION(FLASH_PARAM_PAGE)+1;
    flash_write_words((uint32 *)(alternate_flash_param_page+BASIC_SMALL_PAGE_SIZE-sizeof(int)), (uint32 *)&generation, 1);

    assert(FLASH_PARAM_PAGE == alternate_flash_param_page);
    assert(SGENERATION(FLASH_PARAM1_PAGE) != SGENERATION(FLASH_PARAM2_PAGE));

    delay(500);  // this always takes a while!
}

// this function finds the specified variable in our variable table.
//
// *gosubs is set to the gosub level in which the variable or reference is found.  This may be different than the
// gosub level of the returned variable if a reference was used to locate the returned variable.
static
struct var *
var_find(IN const char *name, OUT int *gosubs)
{
    struct var *var;

    // for all declared variables...
    for (var = vars+(max_vars-1); var >= vars; var--) {
        assert(var->type);
        // if the variable name matches...
        // N.B. we use strncmp so the user can use a longer variable name and still match
        if (var->name[0] == name[0] && ! strncmp(var->name, name, sizeof(var->name)-1)) {
            // if var is a reference, return the referred to variable, making note of the gosub level and
            // any element restriction the reference imposes.
            *gosubs = var->gosubs;
            if (var->type == code_var_reference) {
                // assert that the referent is in an outer scope.
                assert(var > var->u.target_var);

                var = var->u.target_var;
                assert(var->type != code_var_reference);
            }
            return var;
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
    int i;
    
    assert(scope >= 0);
    
    // reclaim ram space, if any was allocated by this scope.
    if (max_vars > scope) {
        
        // find the first (if any exist) non-reference variable in this scope by skipping any references.
        for (i = scope; (i < max_vars) && (vars[i].type == code_var_reference); i++) {
        }

        // release any ram allocate by this scope's variables.
        if (i < max_vars) {
            assert(vars[i].type == code_ram || vars[i].type == code_nodeid);
            ram_offset = vars[i].u.page_offset;
            memset(RAM_VARIABLE_PAGE+ram_offset, 0, sizeof(RAM_VARIABLE_PAGE)-ram_offset);
        }
        
        max_vars = scope;
    }
}

static void
var_declare_internal(IN const char *name, IN int gosubs, IN int type, IN int size, IN int max_index, IN int pin_number, IN int pin_type, IN int pin_qual, IN int nodeid, IN struct var *target)
{
    struct var *var;
    int var_gosubs;

    assert(name);
    assert(type >= code_deleted && type < code_max);

    if (type == code_var_reference) {
        assert(pin_number == -1);
        assert(pin_type == -1);
        assert(pin_qual == -1);
        assert(nodeid == -1);
        
        assert(gosubs > 0); // cannot currently create reference outside of a gosub

        // otherwise, we're declaring a reference to a non-system variable
        assert(target != NULL);
    }
    
    if (! run_condition) {
        return;
    }

    if ((type == code_flash || type == code_pin) && gosubs) {
        printf("flash or pin variable declared in sub\n");
        stop();
        return;
    }

    if (! max_index) {
        printf("declared 0 length array\n");
        stop();
        return;
    }

    var = var_find(name, &var_gosubs);
    // if the var already exists...
    if (var) {
        // if the var exists at the same scope...
        if (var_gosubs == gosubs) {
            // this is a repeat dimension
            printf("var '%s' already defined at this scope\n", name);
            stop();
            return;
        } else {
            assert(gosubs > var_gosubs);
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
    assert(vars[max_vars].name[sizeof(vars[max_vars].name)-1] == '\0');
    vars[max_vars].gosubs = gosubs;
    vars[max_vars].type = type;
    assert(size == sizeof(byte) || size == sizeof(short) || size == sizeof(int));
    vars[max_vars].size = size;
    assert(max_index > 0);
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
            // N.B. this was checked when we parsed
            assert(pins[pin_number].pin_type_mask & (1<<pin_type));

            vars[max_vars].u.pin.number = pin_number;
            vars[max_vars].u.pin.type = pin_type;
            vars[max_vars].u.pin.qual = pin_qual;

#if ! STICK_GUEST
            pin_declare(pin_number, pin_type, pin_qual);
#endif
            break;

        case code_var_reference:
            vars[max_vars].u.target_var = target;
            break;
            
        default:
            assert(0);
            break;
    }

    max_vars++;
}

// this function declares a ram, flash, or pin variable!
void
var_declare(IN const char *name, IN int gosubs, IN int type, IN int size, IN int max_index, IN int pin_number, IN int pin_type, IN int pin_qual, IN int nodeid)
{
    assert(type != code_var_reference);

    var_declare_internal(name, gosubs, type, size, max_index, pin_number, pin_type, pin_qual, nodeid, NULL);
}

void
var_declare_reference(const char *name, int gosubs, const char *target_name)
{
    struct var *target;
    int target_gosubs;
    
    // see if the referent is a normal variable or another reference...
    target = var_find(target_name, &target_gosubs);
    if (! target) {
        printf("unable to find referent '%s'\n", target_name);
        stop();
        return;
    }

    var_declare_internal(name, gosubs, code_var_reference, target->size, target->max_index, -1, -1, -1, -1, target);
}

typedef struct remote_set {
    char name[VAR_NAME_SIZE];  // 14 char max variable name
    int index;
    int value;
} remote_set_t;

static remote_set_t set;

static bool remote;

#if ! STICK_GUEST
static void
class_remote_set(int nodeid, int length, byte *buffer)
{
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

// this function sets the value of a ram, flash, or pin variable!
void
var_set(IN const char *name, IN int index, IN int value)
{
#if 0  // unused for now
    int i;
#endif
    struct var *var;
    int var_gosubs;
#if ! STICK_GUEST
    remote_set_t set;
#endif

    if (! run_condition) {
        return;
    }

    var = var_find(name, &var_gosubs);
    if (! var) {
#if 0  // unused for now
        if (! index) {
            // see if this could be a special system variable
            for (i = 0; i < LENGTHOF(systems); i++) {
                if (! strcmp(name, systems[i].name)) {
                    if (systems[i].set_cbfn == NULL) {
                        printf("var '%s' is read-only\n", name);
                        stop();
                    } else {
                        systems[i].set_cbfn(value);
                    }
                    return;
                }
            }
        }
#endif
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
#if ! STICK_GUEST
                } else if (zb_nodeid == -1) {
                    printf("zigbee nodeid not set\n");
                    stop();
                    break;
#endif
                }
                
#if ! STICK_GUEST
                // if we're not being set from a remote node...
                if (! remote) {
                    // forward the variable set request to the remote node
                    strcpy(set.name, var->name);
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
                    write32(RAM_VARIABLE_PAGE+var->u.page_offset+index*sizeof(int), value);
                } else if (var->size == sizeof(short)) {
                    write16(RAM_VARIABLE_PAGE+var->u.page_offset+index*sizeof(short), value);
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

                }
                
#if ! STICK_GUEST
                pin_set(var->u.pin.number, var->u.pin.type, var->u.pin.qual, value);
#endif
                break;

            default:
                assert(0);
                break;
        }
    }
}

// this function gets the value of a ram, flash, or pin variable!
int
var_get(IN const char *name, IN int index)
{
    int i;
    int value;
    int var_gosubs;
    struct var *var;

    if (! run_condition) {
        return 0;
    }

    value = 0;

    var = var_find(name, &var_gosubs);
    if (! var) {
        if (! index) {
            // see if this could be a special system variable
            for (i = 0; i < LENGTHOF(systems); i++) {
                if (! strcmp(name, systems[i].name)) {
                    return systems[i].integer ? *systems[i].integer : systems[i].constant;
                }
            }
        }
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
                    value = read32(RAM_VARIABLE_PAGE+var->u.page_offset+index*sizeof(int));
                } else if (var->size == sizeof(short)) {
                    value = read16(RAM_VARIABLE_PAGE+var->u.page_offset+index*sizeof(short));
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
#if ! STICK_GUEST
                value = pin_get(var->u.pin.number, var->u.pin.type, var->u.pin.qual);
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
var_get_size(IN const char *name)
{
    int size;
    int var_gosubs;
    struct var *var;
    
    size = 1;
    
    var = var_find(name, &var_gosubs);
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
var_set_flash(IN int var, IN int value)
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
var_get_flash(IN int var)
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
    
    pin_clear();
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
#if ! STICK_GUEST
    zb_register(zb_class_remote_set, class_remote_set);
#endif
}
