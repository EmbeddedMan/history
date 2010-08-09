// *** basic.c ********************************************************
// this file implements the stickos command interpreter.

#include "main.h"

#if _WIN32
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

// phase #1

// phase #2
// features:
//  have nodeid and clusterid, and broadcast clusterid^0x4242 as magic number
//  save zigbee channel in nvparam!
//  get rid of on uart/on timer???
//  multiple watchpoints?
//  auto line number (for paste)
//  builtin operators -- lengthof(a) or a#
//  short circuit && and || operators
//  add character constants 'c', '\n'; do we want a way to print in character form?
//  add ability to configure multiple I/O pins at once, and assign/read them in binary (or with arrays?)
//  add sub parameter mappings? sub return values?
//  add strings
//  multiple (main) threads?  "input" statement?
//  "on usb [connect|disconnect]" statement for slave mode!
//  allow "digital input debounce" for pin type!  also "digital|analog input|output invert"!
//  ?: operator!
//  add support for comments at the end of lines '?  //?
//  switch statement (on xxx goto...?)
//  allow gosub from command line?
//  sub stack trace?  (with sub local vars?)
//  one line "if <expression> then <statement> [else <statement>]"
//  optional "let" statement
// perf:
//  mave var one slot towards "last in gosub scope" on usage rand()%16?
//  code optimization for advancing to the next program line; remember backwards loops (10)?
// user guide:
// bugs:
//  "clear" or "new" should set max_gosubs to 0!
//  re-dim pin should allow pin_type change (and reconfigure)?  or verify it has not!
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
    command_demo,  // [n [nnn]]
    command_dir,
    command_echo,  // [on|off]
    command_edit, // nnn
    command_help,
    command_indent,  // [on|off]
#if MCF52233
    command_ipaddress,  // [dhcp|<ipaddress>]
#endif
    command_list,  // ([nnn] [-] [nnn]|<subname>)
    command_load,  // <name>
    command_memory,
    command_new,
    command_nodeid,  // nnn
    command_prompt,  // [on|off]
    command_purge, // <name>
    command_renumber,
    command_reset,
    command_run,  // [nnn]
    command_save,  // [<name>]
    command_step, // [on|off]
    command_trace, // [on|off]
    command_undo,
    command_upgrade,
    command_uptime
};

static
const struct commands {
    char *command;
    enum cmdcode code;
} commands[] = {
    "autoreset", command_autoreset,
    "autorun", command_autorun,
    "clear", command_clear,
    "clone", command_clone,
    "cls", command_cls,
    "connect", command_connect,
    "cont", command_cont,
    "delete", command_cont,
    "demo", command_demo,
    "dir", command_dir,
    "echo", command_echo,
    "edit", command_edit,
    "help", command_help,
    "indent", command_indent,
#if MCF52233
    "ipaddress", command_ipaddress,
#endif    
    "list", command_list,
    "load", command_load,
    "memory", command_memory,
    "new", command_new,
    "nodeid", command_nodeid,
    "prompt", command_prompt,
    "purge", command_purge,
    "renumber", command_renumber,
    "reset", command_reset,
    "run", command_run,
    "save", command_save,
    "step", command_step,
    "trace", command_trace,
    "undo", command_undo,
    "upgrade", command_upgrade,
    "uptime", command_uptime
};

// revisit -- merge these with basic.c/parse.c???

static
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

static char *help_about =
#if MCF52233
"Welcome to StickOS for Freescale MCF52233 v" VERSION "!\n"
#elif MCF52221
"Welcome to StickOS for Freescale MCF52221 v" VERSION "!\n"
#else
#error
#endif
"Copyright (c) 2008; all rights reserved.\n"
"http://www.cpustick.com\n"
#if INCOMPAT
"incompatible\n"
#endif
#if COMPAT
"compatible\n"
#endif
;

static char *help_general =
"for more information:\n"
"  help about\n"
"  help commands\n"
"  help modes\n"
"  help statements\n"
"  help blocks\n"
"  help devices\n"
"  help expressions\n"
"  help variables\n"
"  help pins\n"
"  help board\n"
"  help clone\n"
"  help zigbee\n"
"\n"
"see also:\n"
"  http://www.cpustick.com\n"
;

static char *help_commands =
"clear [flash]                 -- clear ram [and flash] variables\n"
"clone [run]                   -- clone flash to slave CPUStick [and run]\n"
"cls                           -- clear terminal screen\n"
"cont [<line>]                 -- continue program from stop\n"
"delete [<line>][-][<line>]    -- delete program lines or <subname>\n"
"dir                           -- list saved programs\n"
"edit <line>                   -- edit program line\n"
"help [<topic>]                -- online help\n"
"list [<line>][-][<line>]      -- list program lines or <subname>\n"
"load <name>                   -- load saved program\n"
"memory                        -- print memory usage\n"
"new                           -- erase code ram and flash memories\n"
"purge <name>                  -- purge saved program\n"
"renumber [<line>]             -- renumber program lines (and save)\n"
"reset                         -- reset the CPUStick!\n"
"run [<line>]                  -- run program\n"
"save [<name>]                 -- save code ram to flash memory\n"
"undo                          -- undo code changes since last save\n"
"upgrade                       -- upgrade StickOS firmware!\n"
"uptime                        -- print time since last reset\n"
"\n"
"for more information:\n"
"  help modes\n"
;

static char *help_modes =
"autoreset [on|off]            -- autoreset (on wake) mode\n"
"autorun [on|off]              -- autorun (on reset) mode\n"
"echo [on|off]                 -- terminal echo mode\n"
"indent [on|off]               -- listing indent mode\n"
#if MCF52233
"ipaddress [dhcp|<ipaddress>]  -- set/display ip address\n"
#endif
"prompt [on|off]               -- terminal prompt mode\n"
"step [on|off]                 -- debugger single-step mode\n"
"trace [on|off]                -- debugger trace mode\n"
;

static char *help_statements =
"<line> <statement>                     -- enter program line into code ram\n"
"\n"
"assert <expression>                    -- break if expression is false\n"
"data <n> [, ...]                       -- read-only data\n"
"dim <variable>[[n]] [as ...] [, ...]   -- dimension variables\n"
"end                                    -- end program\n"
"label <label>                          -- read/data label\n"
"let <variable> = <expression> [, ...]  -- assign variable\n"
"print (\"string\"|<expression>) [, ...]  -- print strings/expressions\n"
"qspi <variable> [, ...]                -- perform qspi I/O by reference\n"
"read <variable> [, ...]                -- read read-only data into variables\n"
"rem <remark>                           -- remark\n"
"restore [<label>]                      -- restore read-only data pointer\n"
"sleep <expression>                     -- delay program execution (ms)\n"
"stop                                   -- insert breakpoint in code\n"
"\n"
"for more information:\n"
"  help blocks\n"
"  help devices\n"
"  help expressions\n"
"  help variables\n"
;

static char *help_blocks =
"if <expression> then\n"
"[elseif <expression> then]\n"
"[else]\n"
"endif\n"
"\n"
"for <variable> = <expression> to <expression> [step <expression>]\n"
"  [(break|continue) [n]]\n"
"next\n"
"\n"
"while <expression> do\n"
"  [(break|continue) [n]]\n"
"endwhile\n"
"\n"
"do\n"
"  [(break|continue) [n]]\n"
"until <expression>\n"
"\n"
"gosub <subname>\n"
"\n"
"sub <subname>\n"
"  [return]\n"
"endsub\n"
;

static char *help_devices =
"qspi:\n"
"  configure qspi for <n> csiv\n"
"\n"
"timers:\n"
"  configure timer <n> for <n> ms\n"
"  on timer <n> do <statement>                -- on timer execute statement\n"
"  off timer <n>                              -- disable timer interrupt\n"
"  mask timer <n>                             -- mask/hold timer interrupt\n"
"  unmask timer <n>                           -- unmask timer interrupt\n"
"\n"
"uarts:\n"
"  configure uart <n> for <n> baud <n> data (even|odd|no) parity [loopback]\n"
"  on uart <n> (input|output) do <statement>  -- on uart execute statement\n"
"  off uart <n> (input|output)                -- disable uart interrupt\n"
"  mask uart <n> (input|output)               -- mask/hold uart interrupt\n"
"  unmask uart <n> (input|output)             -- unmask uart interrupt\n"
"\n"
"watchpoints:\n"
"  on <expression> do <statement>             -- on expr execute statement\n"
"  off <expression>                           -- disable expr watchpoint\n"
"  mask <expression>                          -- mask/hold expr watchpoint\n"
"  unmask <expression>                        -- unmask expr watchpoint\n"
;

static char *help_expressions =
"the following operators are supported as in C,\n"
"in order of decreasing precedence:\n"
"  <n>                       -- decimal constant\n"
"  0x<n>                     -- hexadecimal constant\n"
"  <variable>                -- simple variable\n"
"  <variable>[<expression>]  -- array variable element\n"
"  (   )                     -- grouping\n"
"  !   ~                     -- logical not, bitwise not\n"
"  *   /   %                 -- times, divide, mod\n"
"  +   -                     -- plus, minus\n"
"  >>  <<                    -- shift right, left\n"
"  <=  <  >=  >              -- inequalities\n"
"  ==  !=                    -- equal, not equal\n"
"  |   ^   &                 -- bitwise or, xor, and\n"
"  ||  ^^  &&                -- logical or, xor, and\n"
"for more information:\n"
"  help variables\n"
;

static char *help_variables =
"all variables must be dimensioned!\n"
"variables dimensioned in a sub are local to that sub\n"
"array variable indices start at 0; v[0] is the same as v\n"
"\n"
"ram variables:\n"
"  dim <var>[[n]]\n"
"  dim <var>[[n]] as (byte|short)\n"
"\n"
"flash parameter variables:\n"
"  dim <varflash>[[n]] as flash\n"
"\n"
"pin alias variables\n"
"  dim <varpin> as pin <pinname> for (digital|analog|frequency|uart) \\\n"
"                                      (input|output)\n"
"\n"
"system variables (read-only)\n"
"  drops     failures  nodeid    receives\n"
"  retries   seconds   ticks     transmits\n"
"\n"
"for more information:\n"
"  help pins\n"
;

static char *help_pins =
"pin names:\n"
"  dtin3     dtin2     dtin1     dtin0\n"
"  an0       an1       an2       an3\n"
"  an4       an5       an6       an7\n"
"  ucts0*    urts0*    urxd0     utxd0\n"
"  ucts1*    urts1*    urxd1     utxd1\n"
#if MCF52233
"  irq11*    irq7*     irq4*     irq1*\n"
#elif MCF52221
"  irq7*     irq4*     irq1*\n"
#else
#error
#endif
"  qspi_cs0  qspi_clk  qspi_din  qspi_dout\n"
#if MCF52233
"  gpt0      gpt1      gpt2      gpt3\n"
#endif
"  scl       sda\n"
"\n"
"all pins support general purpose digital input/output\n"
"an? = potential analog input pins (mV)\n"
"dtin? = potential analog output (PWM actually) pins (mV)\n"
"dtin? = potential frequency output pins (Hz)\n"
"urxd? = potential uart input pins (received byte)\n"
"utxd? = potential uart output pins (transmit byte)\n"
;

static char *help_board =
"                1  2  3  4  5  6  7  8  9  10 11 12 13 14\n"
"\n"
"                g  +  u  u  u  u  u  u  u  u  i  i  i  +\n"
"                n  3  c  r  r  t  c  r  r  t  r  r  r  5\n"
"                d  V  t  t  x  x  t  t  x  x  q  q  q  V\n"
"                      s  s  d  d  s  s  d  d  7  4  1\n"
"1  gnd                0  0  0  0  1  1  1  1  *  *  *\n"
"2  +3V                *  *        *  *\n"
"3  rsti*\n"
"4  scl\n"
"5  sda\n"
"6  qspi_din\n"
"7  qspi_dout\n"
"8  qspi_clk\n"
"9  qspi_cs0         d  d  d  d\n"
"10 rcon*/irq4*      t  t  t  t\n"
"                g  +  i  i  i  i  a  a  a  a  a  a  a  a\n"
"                n  3  n  n  n  n  n  n  n  n  n  n  n  n\n"
"                d  V  3  2  1  0  0  1  2  3  4  5  6  7\n"
"\n"
"                1  2  3  4  5  6  7  8  9  10 11 12 13 14\n"
;

static char *help_clone =
"clone cable:\n"
"  master     slave\n"
"  ---------  ----------------\n"
"  qspi_clk   qspi_clk (ezpck)\n"
"  qspi_din   qspi_dout (ezpq)\n"
"  qspi_dout  qspi_din (ezpd)\n"
"  qspi_cs0   rcon* (ezpcs*)\n"
"  scl        rsti*\n"
"  vss        vss\n"
"  vdd        vdd\n"
;

static char *help_zigbee =
"nodeid (<nodeid>|none)        -- set/display zigbee nodeid\n"
"\n"
"connect <nodeid>              -- connect to CPUStick <nodeid> via zigbee\n"
"\n"
"remote node variables\n"
"  dim <varremote>[[n]] as remote on nodeid <nodeid>\n"
"\n"
"zigbee cable:\n"
"  MCU        MC1320X\n"
"  ---------  -------\n"
"  qspi_clk   spiclk\n"
"  qspi_din   miso\n"
"  qspi_dout  mosi\n"
"  qspi_cs0   ce*\n"
"  irq4*      irq*\n"
"  scl        rst*\n"
"  sda        rxtxen\n"
"  vss        vss\n"
"  vdd        vdd\n"
;

static
void
help(IN char *text_in)
{
    char *p;
    char *text;
    char line[BASIC_LINE_SIZE];

    text = text_in;

    // while there is more help to print...
    while (*text) {
        // print the next line of help
        p = strchr(text, '\n');
        assert(p);
        assert(p-text < BASIC_LINE_SIZE);
        memcpy(line, text, p-text);
        line[p-text] = '\0';
        printf("%s\n", line);
        text = p+1;
    }

#if ! _WIN32
    if (text_in == help_about) {
        printf("(checksum 0x%x)\n", flash_checksum);
#if 0
        printf("MCF_RCM_RSR = 0x%x\n", MCF_RCM_RSR);
#endif
    }
#endif
}

static const char * const demo0[] = {
  "rem ### blinky ###",
  "dim i",
  "dim led as pin dtin2 for digital output",
  "while 1 do",
  "  for i = 1 to 16",
  "    let led = !led",
  "    sleep 50",
  "  next",
  "  sleep 800",
  "endwhile",
  "end"
};


static const char *const demo1[] = {
  "rem ### uart isr ###",
  "dim data",
  "data 1, 1, 2, 3, 5, 8, 13, 21, 0",
  "configure uart 0 for 300 baud 8 data no parity loopback",
  "dim tx as pin utxd0 for uart output",
  "dim rx as pin urxd0 for uart input",
  "on uart 0 input do gosub receive",
  "on uart 0 output do gosub transmit",
  "sleep 1000",
  "end",
  "sub receive",
  "  print \"received\", rx",
  "endsub",
  "sub transmit",
  "  read data",
  "  if ! data then",
  "    return",
  "  endif",
  "  assert !tx",
  "  print \"sending\", data",
  "  let tx = data",
  "endsub"
};

static const char *const demo2[] = {
  "rem ### uart pio ###",
  "configure uart 0 for 9600 baud 7 data even parity loopback",
  "dim tx as pin utxd0 for uart output",
  "dim rx as pin urxd0 for uart input",
  "let tx = 3",
  "let tx = 4",
  "while tx do",
  "endwhile",
  "print rx",
  "print rx",
  "print rx",
  "end"
};

static const char *const demo3[] = {
  "rem ### toaster ###",
  "dim target, secs",
  "dim thermocouple as pin an0 for analog input",
  "dim relay as pin an1 for digital output",
  "data 5124, 6, 7460, 9, 8940, 3, -1, -1",
  "configure timer 0 for 1000 ms",
  "on timer 0 do gosub adjust",
  "rem ----------",
  "while target!=-1 do",
  "  sleep secs*1000",
  "  read target, secs",
  "endwhile",
  "let relay = 0",
  "end",
  "sub adjust",
  "  if thermocouple>=target then",
  "    let relay = 0",
  "  else",
  "    let relay = 1",
  "  endif",
  "endsub",
};


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
    char *text;
    int length;
    int number1;
    int number2;
    bool *boolp;
    int syntax_error;
#if MCF52233
    int a0, a1, a2, a3;
#endif
    byte bytecode[BASIC_BYTECODE_SIZE];

    if (run_step && ! *text_in) {
        text = "cont";
    } else {
        text = text_in;
    }

    parse_trim(&text);

    for (cmd = 0; cmd < LENGTHOF(commands); cmd++) {
        len = strlen(commands[cmd].command);
        if (! strncmp(text, commands[cmd].command, len)) {
            break;
        }
    }

    if (cmd != LENGTHOF(commands)) {
        text += len;
    }
    parse_trim(&text);

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

#if MCF52233
        case command_ipaddress:
            if (*text) {
                if (parse_word(&text, "dhcp")) {
                    a0 = a1 = a2 = a3 = 0;
                } else {
                    (void)basic_const(&text, &a0);
                    (void)parse_char(&text, '.');
                    (void)basic_const(&text, &a1);
                    (void)parse_char(&text, '.');
                    (void)basic_const(&text, &a2);
                    (void)parse_char(&text, '.');
                    boo = basic_const(&text, &a3);
                    if (! boo || (a0|a1|a2|a3)>>8) {
                        goto XXX_ERROR_XXX;
                    }
                }
                if (*text) {
                    goto XXX_ERROR_XXX;
                }
                var_set_flash(FLASH_IPADDRESS, a0<<24|a1<<16|a2<<8|a3);
            } else {
                i = var_get_flash(FLASH_IPADDRESS);
                if (! i || i == -1) {
                    printf("dhcp\n");
                } else {
                    printf("%u.%u.%u.%u\n", (unsigned)i>>24, (i>>16)&0xff, (i>>8)&0xff, i&0xff);
                }
            }
            break;
#endif

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
#if ! _WIN32
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

            var_clear(boo);
            break;

        case command_clone:
            boo = false;
            if (parse_word(&text, "run")) {
                boo = true;
            }
            if (*text) {
                goto XXX_ERROR_XXX;
            }

            help(help_about);
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
#if ! _WIN32
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
                
                printf("press Ctrl-D to disconnect\n");

#if ! _WIN32
                assert(main_command);
                main_command = NULL;
                terminal_command_ack(false);

                terminal_rxid = number1;
                terminal_txid = -1;
                
                while (terminal_rxid != -1) {
                    basic_poll();
                }
#endif
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

#if ! _WIN32
            terminal_command_discard(true);
            if (main_command) {
                main_command = NULL;
                terminal_command_ack(false);
            }
#endif

            run(cmd == command_cont, number1);

#if ! _WIN32
            terminal_command_discard(false);
#endif
            break;

        case command_delete:
        case command_list:
            if (*text) {
                boo = basic_const(&text, &number1);
                number2 = number1;
                if (*text) {
                    boo |= parse_char(&text, '-');
                    number2 = 0;
                    boo |= basic_const(&text, &number2);
                }
                if (! boo) {
                    number1 = code_line(code_sub, (byte *)text);
                    if (! number1) {
                        goto XXX_ERROR_XXX;
                    }
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
                code_list(number1, number2);
            }
            break;

        case command_demo:
            number1 = 0;
            if (*text) {
                if (! basic_const(&text, &number1) || number1 < 0 || number1 >= 4) {
                    goto XXX_ERROR_XXX;
                }
            }

            number2 = 0;
            if (*text) {
                if (! basic_const(&text, &number2) || ! number2) {
                    goto XXX_ERROR_XXX;
                }
                if (*text) {
                    goto XXX_ERROR_XXX;
                }
            }

            if (! number2) {
                number2 = 10;
                code_new();
            }

            if (number1 == 0) {
                for (i = 0; i < LENGTHOF(demo0); i++) {
                    code_insert(number2+i*10, (char *)demo0[i], 0);
                }
            } else if (number1 == 1) {
                for (i = 0; i < LENGTHOF(demo1); i++) {
                    code_insert(number2+i*10, (char *)demo1[i], 0);
                }
            } else if (number1 == 2) {
                for (i = 0; i < LENGTHOF(demo2); i++) {
                    code_insert(number2+i*10, (char *)demo2[i], 0);
                }
            } else if (number1 == 3) {
                for (i = 0; i < LENGTHOF(demo3); i++) {
                    code_insert(number2+i*10, (char *)demo3[i], 0);
                }
            } else {
                assert(0);
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

        case command_help:
            if (! *text) {
                help(help_general);
            } else if (parse_word(&text, "about")) {
                help(help_about);
            } else if (parse_word(&text, "commands")) {
                help(help_commands);
            } else if (parse_word(&text, "modes")) {
                help(help_modes);
            } else if (parse_word(&text, "statements")) {
                help(help_statements);
            } else if (parse_word(&text, "devices")) {
                help(help_devices);
            } else if (parse_word(&text, "blocks")) {
                help(help_blocks);
            } else if (parse_word(&text, "expressions")) {
                help(help_expressions);
            } else if (parse_word(&text, "variables")) {
                help(help_variables);
            } else if (parse_word(&text, "pins")) {
                help(help_pins);
            } else if (parse_word(&text, "board")) {
                help(help_board);
            } else if (parse_word(&text, "clone")) {
                help(help_clone);
            } else if (parse_word(&text, "zigbee")) {
                help(help_zigbee);
            } else {
                goto XXX_ERROR_XXX;
            }
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

#if ! _WIN32
            MCF_RCM_RCR = MCF_RCM_RCR_SOFTRST;
            asm { halt }
#endif
            break;

        case command_echo:
        case command_indent:
        case command_prompt:
        case command_step:
        case command_trace:
            // *** interactive debugger ***
            if (cmd == command_echo) {
                boolp = &terminal_echo;
            } else if (cmd == command_indent) {
                boolp = &code_indent;
            } else if (cmd == command_prompt) {
                boolp = &main_prompt;
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

        default:
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
    return;

XXX_ERROR_XXX:
    terminal_command_error(text-text_in);
}

#if ! _WIN32
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
#if ! _WIN32
    extern unsigned char far __DATA_RAM[], __DATA_ROM[], __DATA_END[];

    start_of_dynamic = FLASH_CODE1_PAGE;
    assert(end_of_static < start_of_dynamic);

    end_of_dynamic = FLASH_PARAM2_PAGE+BASIC_SMALL_PAGE_SIZE;
    assert(end_of_dynamic <= (byte *)FLASH_BYTES);
#endif

    code_initialize();
    var_initialize();
}

