// *** basic.c ********************************************************
// this file implements the stickos command interpreter.

// Copyright (c) Rich Testardi, 2008.  All rights reserved.
// Patent pending.

#include "main.h"

#if STICK_GUEST
byte FLASH_CODE1_PAGE[BASIC_LARGE_PAGE_SIZE];
byte FLASH_CODE2_PAGE[BASIC_LARGE_PAGE_SIZE];
byte FLASH_STORE_PAGES[BASIC_STORES][BASIC_LARGE_PAGE_SIZE];
byte FLASH_CATALOG_PAGE[BASIC_SMALL_PAGE_SIZE];
byte FLASH_PARAM1_PAGE[BASIC_SMALL_PAGE_SIZE];
byte FLASH_PARAM2_PAGE[BASIC_SMALL_PAGE_SIZE];
#endif

byte RAM_CODE_PAGE[BASIC_SMALL_PAGE_SIZE];
byte RAM_VARIABLE_PAGE[BASIC_SMALL_PAGE_SIZE];

byte *start_of_dynamic;
byte *end_of_dynamic;

struct timer_unit timer_units[] = {
    "us", -1000/ticks_per_msec,
    "ms", ticks_per_msec,
    "s", ticks_per_msec*1000,
};

// PC/Cell framework!!!

// phase #1
// why mcf52xxx dtim freq not off by 2x?
// figure out usb "pop" on first badge run -- is that the badge issue?

// phase #2
// features:
//  pic32: disable pullups on analog input
//  pic32: add autorun disable switch (rd6?)
//  if data area of flash looks bogus on boot, we should clear it all!
//  allow broadcast/remote nodeid setting (with switch), like pagers
//  need way for upgrade to preserve nodeid (and ipaddress, and maybe just flash param page?)
//  allow zigbee remote variable to be read as well as written
//  auto line number (for paste)
//  have nodeid and clusterid, and broadcast 0x4242 and clusterid as magic number
//  save zigbee channel in nvparam!
//  get rid of on uart/on timer???
//  multiple watchpoints?
//  builtin operators -- lengthof(a) or a#
//  short circuit && and || operators
//  add character constants 'c', '\n'; do we want a way to print in character form?
//  add ability to configure multiple I/O pins at once, and assign/read them in binary (or with arrays?)
//  add strings
//  multiple (main) threads?  "input" statement?
//  "on usb [connect|disconnect]" statement for slave mode!
//  ?: operator!
//  add support for comments at the end of lines '?  //?
//  switch statement (on xxx goto...?)
//  allow gosub from command line?
//  sub stack trace?  (with sub local vars?)
//  one line "if <expression> then <statement> [else <statement>]"
//  optional "let" statement
// perf:
//  mave var one slot towards "last in gosub scope" on usage rand()%16?
//  can we skip statement execution more fully when run_condition is false?
// user guide:
// bugs:
//  sleep switch power draw is too high!
//  need mechanism to reconfigure pins irq1* and irq7*, other than reset!
//  need a second catalog page for safe updates!
//  can we make sub/endsub block behave more like for/next, from error and listing perspective?
//  should we attempt to make rx transfers never time out/clear feature?
//  core dump -- copy ram to secondary code flash on assert/exception/halt?
//  handle uart errors (interrupt as well as poll)
//  sleeps and timers don't work with single-stepping

// phase #3
// features:
//  add i2c control
//  add usb host control
//  add usb device control

enum cmdcode {
    command_autoreset,  // [on|off]
    command_autorun,  // [on|off]
    command_clear,  // [flash]
    command_clone,  // [run]
    command_cls,
    command_connect,  // nnn
    command_cont,  // [nnn]
    command_delete,  // ([nnn] [-] [nnn]|<subname>)
    command_dir,
    command_echo,  // [on|off]
    command_edit, // nnn
    command_indent,  // [on|off]
    command_list,  // ([nnn] [-] [nnn]|<subname>)
    command_load,  // <name>
    command_memory,
    command_new,
    command_nodeid,  // nnn
    command_profile,  // ([nnn] [-] [nnn]|<subname>)
    command_prompt,  // [on|off]
    command_purge, // <name>
    command_renumber,
    command_reset,
    command_run,  // [nnn]
    command_save,  // [<name>]
    command_sleep, // [on|off]
    command_step, // [on|off]
    command_trace, // [on|off]
    command_undo,
    command_upgrade,
    command_uptime,
    command_zigbee
};

static
const char *commands[] = {
    "autoreset",
    "autorun",
    "clear",
    "clone",
    "cls",
    "connect",
    "cont",
    "delete",
    "dir",
    "echo",
    "edit",
    "indent",
    "list",
    "load",
    "memory",
    "new",
    "nodeid",
    "profile",
    "prompt",
    "purge",
    "renumber",
    "reset",
    "run",
    "save",
    "sleep",
    "step",
    "trace",
    "undo",
    "upgrade",
    "uptime",
    "zigbee"
};

// revisit -- merge this with basic.c/parse.c???

bool
basic_const(IN OUT char **text, OUT int *value_out)
{
    int value;

    if (! isdigit((*text)[0])) {
        return false;
    }

    // parse constant value and advance *text past constant
    value = 0;
    while (isdigit((*text)[0])) {
        value = value*10 + (*text)[0] - '0';
        (*text)++;
    }

    *value_out = value;
    parse_trim(text);
    return true;
}


// this function implements the stickos command interpreter.
void
basic_run(char *text_in)
{
    int i;
    int t;
    int d;
    int h;
    int m;
    int cmd;
    int len;
    bool boo;
    bool reset;
    bool init;
    char *text;
    int length;
    int number1;
    int number2;
    bool *boolp;
    struct line *line;
    int syntax_error;
    byte bytecode[BASIC_BYTECODE_SIZE];

    if (run_step && ! *text_in) {
        text = "cont";
    } else {
        text = text_in;
    }

    parse_trim(&text);

    for (cmd = 0; cmd < LENGTHOF(commands); cmd++) {
        len = strlen(commands[cmd]);
        if (! strncmp(text, commands[cmd], len)) {
            break;
        }
    }
    // N.B. cmd == LENGTHOF(commands) OK below!!!

    if (cmd != LENGTHOF(commands)) {
        text += len;
        parse_trim(&text);
    }

#if STICK_GUEST
    // let timer ticks arrive; align ticks on run
    timer_ticks(cmd == command_run);
#endif

    number1 = 0;
    number2 = 0;

    switch (cmd) {
        case command_autoreset:
        case command_autorun:
            if (*text) {
                if (parse_word(&text, "on")) {
                    boo = true;
                } else if (parse_word(&text, "off")) {
                    boo = false;
                } else {
                    goto XXX_ERROR_XXX;
                }
                if (*text) {
                    goto XXX_ERROR_XXX;
                }
                var_set_flash(cmd==command_autorun?FLASH_AUTORUN:FLASH_AUTORESET, boo);
            } else {
                if (var_get_flash(cmd==command_autorun?FLASH_AUTORUN:FLASH_AUTORESET) == 1) {
                    printf("on\n");
                } else {
                    printf("off\n");
                }
            }
            break;

        case command_nodeid:
            if (*text) {
                if (parse_word(&text, "none")) {
                    number1 = -1;
                } else {
                    if (! basic_const(&text, &number1) || number1 == -1) {
                        goto XXX_ERROR_XXX;
                    }
                }
                if (*text) {
                    goto XXX_ERROR_XXX;
                }
                var_set_flash(FLASH_NODEID, number1);
#if ! STICK_GUEST
                zb_nodeid = number1;
#endif
            } else {
                i = var_get_flash(FLASH_NODEID);
                if (i == -1) {
                    printf("none\n");
                } else {
                    printf("%u\n", i);
                }
            }
            break;
        
        case command_clear:
            boo = false;
            if (parse_word(&text, "flash")) {
                boo = true;
            }
            if (*text) {
                goto XXX_ERROR_XXX;
            }

            run_clear(boo);
            break;

        case command_clone:
            boo = false;
            if (parse_word(&text, "run")) {
                boo = true;
            }
            if (*text) {
                goto XXX_ERROR_XXX;
            }

            basic2_help(help_about);
            printf("cloning...\n");
            clone(boo);
            break;

        case command_cls:
            if (*text) {
                goto XXX_ERROR_XXX;
            }
            printf("%c[2J\n", '\033');
            break;
            
        case command_connect:
            if (! zb_present) {
                printf("zigbee not present\n");
#if ! STICK_GUEST
            } else if (zb_nodeid == -1) {
                printf("zigbee nodeid not set\n");
#endif
            } else {
                if (! basic_const(&text, &number1) || number1 == -1) {
                    goto XXX_ERROR_XXX;
                }
                if (*text) {
                    goto XXX_ERROR_XXX;
                }
                
                printf("press Ctrl-D to disconnect...\n");

#if ! STICK_GUEST
                assert(main_command);
                main_command = NULL;
                terminal_command_ack(false);

                terminal_rxid = number1;
                
                while (terminal_rxid != -1) {
                    basic_poll();
                }
#endif

                printf("...disconnected\n");
            }
            break;

        case command_cont:
        case command_run:
            if (*text) {
                (void)basic_const(&text, &number1);
                if (*text) {
                    goto XXX_ERROR_XXX;
                }
            }

#if ! STICK_GUEST
            terminal_command_discard(true);
            if (main_command) {
                main_command = NULL;
                terminal_command_ack(false);
            }
#endif

            run(cmd == command_cont, number1);

#if ! STICK_GUEST
            terminal_command_discard(false);
#endif
            break;

        case command_delete:
        case command_list:
        case command_profile:
            if (*text) {
                boo = basic_const(&text, &number1);
                number2 = number1;
                if (*text) {
                    boo |= parse_char(&text, '-');
                    number2 = 0;
                    boo |= basic_const(&text, &number2);
                }
                if (! boo) {
                    line = code_line(code_sub, (byte *)text);
                    if (line == NULL) {
                        goto XXX_ERROR_XXX;
                    }
                    number1 = line->line_number;
                    text += strlen(text);
                    number2 = 0x7fffffff;  // endsub
                }
            }
            if (*text) {
                goto XXX_ERROR_XXX;
            }

            if (cmd == command_delete) {
                code_delete(number1, number2);
            } else {
                code_list(cmd == command_profile, number1, number2);
            }
            break;

        case command_dir:
            if (*text) {
                goto XXX_ERROR_XXX;
            }
            code_dir();
            break;

        case command_edit:
            (void)basic_const(&text, &number1);
            if (! number1 || *text) {
                goto XXX_ERROR_XXX;
            }
            code_edit(number1);
            break;

        case command_load:
        case command_purge:
        case command_save:
            if (cmd != command_save && ! *text) {
                goto XXX_ERROR_XXX;
            }
            if (cmd == command_load) {
                code_load(text);
            } else if (cmd == command_purge) {
                code_purge(text);
            } else if (cmd == command_save) {
                code_store(text);
            } else {
                assert(0);
            }
            break;

        case command_memory:
            if (*text) {
                goto XXX_ERROR_XXX;
            }

            code_mem();
            var_mem();
            break;

        case command_new:
            if (*text) {
                goto XXX_ERROR_XXX;
            }

            code_new();
            break;

        case command_renumber:
            number1 = 10;
            (void)basic_const(&text, &number1);
            if (! number1 || *text) {
                goto XXX_ERROR_XXX;
            }

            code_save(number1);
            break;

        case command_reset:
            if (*text) {
                goto XXX_ERROR_XXX;
            }

#if ! STICK_GUEST
            (void)splx(7);
#if MCF52221 || MCF52233
            MCF_RCM_RCR = MCF_RCM_RCR_SOFTRST;
#elif MCF51JM128
            asm {
                move.l  #0x00000000,d0
                movec   d0,CPUCR
                trap    #0
            };
#elif PIC32
            SYSKEY = 0;
            SYSKEY = 0xAA996655;
            SYSKEY = 0x556699AA;
            RSWRSTSET = _RSWRST_SWRST_MASK;
            while (RSWRST, true) {
                // NULL
            }
#else
#endif
            ASSERT(0);
#endif
            break;

        case command_echo:
        case command_indent:
        case command_prompt:
        case command_sleep:
        case command_step:
        case command_trace:
            // *** interactive debugger ***
            if (cmd == command_echo) {
                boolp = &terminal_echo;
            } else if (cmd == command_indent) {
                boolp = &code_indent;
            } else if (cmd == command_prompt) {
                boolp = &main_prompt;
            } else if (cmd == command_sleep) {
                boolp = &run_sleep;
            } else if (cmd == command_step) {
                boolp = &run_step;
            } else {
                assert(cmd == command_trace);
                boolp = &var_trace;
            }

            if (*text) {
                if (parse_word(&text, "on")) {
                    *boolp = true;
                } else if (parse_word(&text, "off")) {
                    *boolp = false;
                } else {
                    goto XXX_ERROR_XXX;
                }
                if (*text) {
                    goto XXX_ERROR_XXX;
                }
            } else {
                if (*boolp) {
                    printf("on\n");
                } else {
                    printf("off\n");
                }
            }
            break;

        case command_undo:
            if (*text) {
                goto XXX_ERROR_XXX;
            }

            code_undo();
            break;

        case command_upgrade:
            // upgrade StickOS!
            flash_upgrade();
            break;

        case command_uptime:
            if (*text) {
                goto XXX_ERROR_XXX;
            }

            t = seconds;
            d = t/(24*60*60);
            t = t%(24*60*60);
            h = t/(60*60);
            t = t%(60*60);
            m = t/(60);
            printf("%dd %dh %dm\n", d, h, m);
            break;
            
        case command_zigbee:
            reset = parse_word(&text, "reset");
            init = parse_word(&text, "init");
            if (*text) {
                goto XXX_ERROR_XXX;
            }
#if ! STICK_GUEST
            zb_diag(reset, init);
#endif
            break;

        case LENGTHOF(commands):
            // if the line begins with a line number
            if (isdigit(*text)) {
                (void)basic_const(&text, &number1);
                if (! number1) {
                    goto XXX_ERROR_XXX;
                }
                if (*text) {
                    code_insert(number1, text, text-text_in);
                } else {
                    code_insert(number1, NULL, text-text_in);
                }
            } else if (*text) {
                // if this is not a private command...
                if (! basic2_run(text_in)) {
                    // *** interactive debugger ***
                    // see if this might be a basic line executing directly
                    if (parse_line(text, &length, bytecode, &syntax_error)) {
                        // run the bytecode
                        run_bytecode(true, bytecode, length);
                    } else {
                        terminal_command_error(text-text_in + syntax_error);
                    }
                }
            }
            break;
            
        default:
            assert(0);
            break;
    }
    return;

XXX_ERROR_XXX:
    terminal_command_error(text-text_in);
}

#if ! STICK_GUEST
void
basic_poll(void)
{
    terminal_poll();
    var_poll();
}
#endif

// this function initializes the basic module.
void
basic_initialize(void)
{
#if ! STICK_GUEST
    start_of_dynamic = FLASH_CODE1_PAGE;
    ASSERT(end_of_static < start_of_dynamic);

    end_of_dynamic = FLASH_PARAM2_PAGE+BASIC_SMALL_PAGE_SIZE;
    ASSERT(end_of_dynamic <= (byte *)(FLASH_START+FLASH_BYTES));
#endif

    code_initialize();
    var_initialize();
}

