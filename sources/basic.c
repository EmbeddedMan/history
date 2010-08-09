#include "main.h"

// *** basic **************************************************************

#if BASIC

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

byte *end_of_static;
byte *start_of_dynamic;
byte *end_of_dynamic;

// phase #1

// phase #2
// allow interrupts on all digital input pins!
// sleep switch seems broken on autorun!
// re-dim pin should allow pin_type change (and reconfigure)?  or verify it has not!
// need a second catalog page for safe updates!
// figure out how to get ivt out of ram!
// can we make sub/endsub block behave more like for/next, from error perspective?
// should we attempt to make rx transfers never time out/clear feature?
// should "," be supported for *all* statements?  i.e., let a=b,c=0
// core dump -- copy ram to secondary code flash on assert/exception/halt?
// get rid of 512 byte pad for bdtbuffer!; how to shrink code?
// add ability to configure multiple I/O pins at once, and assign/read them in binary
// limit line numbers to 16 bits?
// "prompt (on|off)", "echo (on|off)" for slave data acquisition mode
// handle uart errors (interrupt as well as poll)
// allow gosub from command line
// sub stack trace?  (with sub local vars?)
// list, delete, etc., should allow sub names
// builtin vars -- ticks, rand -- or ticks(), rand()?  will we support functions?
// builtin operators -- lengthof(a) or a#
// add sub parameter mappings?
// sub return values?
// watchpoint
// add strings
// ?: operator!
// "if <expression> then <statement> [else <statement>]"
// we need a better story for preserving hex constants in listings!
// optional "let" statement
// sleeps and timers don't work with single-stepping
// multiple (main) threads?  "input" statement?
// add support for comments at the end of lines '?  //?
// follow visual basic keywords?

// phase #3
// add spi control
// add i2c control
// add usb host control
// add usb device control

enum cmdcode {
    command_autoreset,  // [on|off]
    command_autorun,  // [on|off]
    command_clear,  // [flash]
    command_clone,  // [run]
    command_cont,  // [nnn]
    command_delete,  // [nnn] [-] [nnn]
    command_demo,  // [n [nnn]]
    command_dir,
    command_edit, // nnn
    command_help,
    command_list,  // [nnn] [-] [nnn]
    command_load,  // <name>
    command_memory,
    command_new,
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
struct commands {
    char *command;
    enum cmdcode code;
} commands[] = {
    "autoreset", command_autoreset,
    "autorun", command_autorun,
    "clear", command_clear,
    "clone", command_clone,
    "cont", command_cont,
    "delete", command_cont,
    "demo", command_demo,
    "dir", command_dir,
    "edit", command_edit,
    "help", command_help,
    "list", command_list,
    "load", command_load,
    "memory", command_memory,
    "new", command_new,
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

// revisit -- merge these with basic.c/parse.c/run.c???

static
void
trim(IN char **p)
{
    while (isspace(**p)) {
        (*p)++;
    }
}

static
bool
basic_const(char **text, int *value_out)
{
    int value;

    if (! isdigit((*text)[0])) {
        return false;
    }

    value = 0;
    while (isdigit((*text)[0])) {
        value = value*10 + (*text)[0] - '0';
        (*text)++;
    }

    *value_out = value;
    trim(text);
    return true;
}

static
bool
basic_char(char **text, char c)
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
basic_word(char **text, char *word)
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

static char *help_about =
"Welcome to StickOS for Freescale MCF52221 v1.0!\n"
"Copyright (c) CPUStick.com, 2008; all rights reserved.\n"
;

static char *help_general =
"for more information:\n"
"  help about\n"
"  help commands\n"
"  help statements\n"
"  help blocks\n"
"  help devices\n"
"  help expressions\n"
"  help variables\n"
"  help pins\n"
"  help board\n"
"  help clone\n"
;

static char *help_commands =
"auto(reset|run) [on|off]    -- autoreset or autorun mode\n"
"clear [flash]               -- clear ram [and flash] variables\n"
"clone [run]                 -- clone flash to slave CPUStick\n"
"cont [<line>]               -- continue program from stop\n"
"delete [<line>][-][<line>]  -- delete program lines\n"
"dir                         -- list saved programs\n"
"edit <line>                 -- edit program line\n"
"help [<topic>]              -- online help\n"
"list [<line>][-][<line>]    -- list program lines\n"
"load <name>                 -- load saved program\n"
"memory                      -- print memory usage\n"
"new                         -- erase code ram and flash memories\n"
"purge <name>                -- purge saved program\n"
"renumber [<line>]           -- renumber program lines (and save)\n"
"reset                       -- reset the CPUStick!\n"
"run [<line>]                -- run program\n"
"save [<name>]               -- save code ram to flash memory\n"
"step [on|off]               -- single-step mode\n"
"trace [on|off]              -- trace mode\n"
"undo                        -- undo code changes since last save\n"
"upgrade                     -- upgrade StickOS firmware!\n"
"uptime                      -- print time since last reset\n"
;

static char *help_statements =
"<line> <statement>                     -- enter program line into code ram\n"
"\n"
"assert <expression>                    -- break if expression is false\n"
"data <n> [, ...]                       -- read-only data\n"
"dim <variable>[[n]] [as ...] [, ...]   -- dimension variables\n"
"end                                    -- end program\n"
"let <variable> = <expression>          -- assign variable\n"
"print (\"string\"|<expression>) [, ...]  -- print strings/expressions\n"
"read <variable> [, ...]                -- read read-only data into variables\n"
"rem <remark>                           -- remark\n"
"restore [<line>]                       -- restore read-only data pointer\n"
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
"  [break [n]]\n"
"next\n"
"\n"
"while <expression> do\n"
"  [break [n]]\n"
"endwhile\n"
"\n"
"gosub <sub name>[(params, ...)]\n"
"\n"
"sub <sub name>[(params, ...)]\n"
"  [return]\n"
"endsub\n"
;

static char *help_devices =
"timers:\n"
"  configure timer <n> for <n> ms\n"
"  on timer <n> <statement>                -- on timer execute <statement>\n"
"  off timer <n>                           -- disable timer interrupt\n"
"  [un]mask timer <n>                      -- mask or unmask timer interrupt\n"
"\n"
"uarts:\n"
"  configure uart <n> for <n> baud <n> data (even|odd|no) parity [loopback]\n"
"  on uart <n> (input|output) <statement>  -- on uart execute <statement>\n"
"  off uart <n> (input|output)             -- disable uart interrupt\n"
"  [un]mask uart <n> (input|output)        -- mask or unmask uart interrupt\n"
;

static char *help_expressions =
"the following operators are supported as in C,\n"
"in order of increasing precedence:\n"
"  ||  ^^  &&                -- logical or, xor, and\n"
"  |   ^   &                 -- bitwise or, xor, and\n"
"  ==  !=                    -- equal, not equal\n"
"  <=  <  >=  >              -- inequalities\n"
"  >>  <<                    -- shift right, left\n"
"  +   -                     -- plus, minus\n"
"  *   /   %                 -- times, divide, mod\n"
"  !   ~                     -- logical not, bitwise not\n"
"  (   )                     -- grouping\n"
"  <variable>                -- simple variable\n"
"  <variable>[<expression>]  -- array variable element\n"
"  <n>                       -- decimal constant\n"
"  0x<n>                     -- hexadecimal constant\n"
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
"  dim <var>[[n]] as (byte|integer)\n"
"\n"
"flash parameter variables:\n"
"  dim <varflash>[[n]] as flash\n"
"\n"
"pin alias variables\n"
"  dim <varpin> as pin <pinname> (analog|digital|uart) (input|output)\n"
"\n"
"for more information:\n"
"  help pins\n"
;

static char *help_pins =
"pin names:\n"
"  qspi_cs0  qspi_clk  qspi_din  qspi_dout dtin3   dtin2   dtin1   dtin0\n"
"  ucts0*    urts0*    urxd0     utxd0     ucts1*  urts1*  urxd1   utxd1\n"
"  an0       an1       an2       an3       an4     an5     an6     an7\n"
"  irq7*     irq4*     irq1*\n"
"            scl       sda\n"
"\n"
"all pins support general purpose digital input/output\n"
"an? = potential analog input pins (scale 0..32767)\n"
"dtin? = potential analog output (PWM actually) pins (scale 0..32767)\n"
"urxd? = potential uart input pins (received byte)\n"
"utxd? = potential uart output pins (transmit byte)\n"
;

static char *help_board =
"              1 2 3 4 5 6 7 8 9 10   1 2 3 4 5 6 7 8 9 10\n"
"\n"
"              g + u u u u u u u u    g + i i i       a p\n"
"              n 3 c r r t c r r t    n 3 r r r       l s\n"
"              d V t t x x t t x x    d V q q q       l t\n"
"1  gnd            s s d d s s d d        7 4 1       p c\n"
"2  +3V            0 0 0 0 1 1 1 1        * * *       s l\n"
"                  * *     * *                        t k\n"
"1  gnd\n"
"2  +3V                                                          gnd  1\n"
"3  rsti*                                                        +5V  2\n"
"4  scl\n"
"5  sda                  q\n"
"6  qspi_din       q q q s\n"
"7  qspi_dout      s s s p\n"
"8  qspi_clk       p p p i\n"
"9  qspi_cs0       i i i _ d d d d\n"
"10 rcon*          _ _ _ d t t t t\n"
"              g + c c d o i i i i    g + a a a a a a a a\n"
"              n 3 s l i u n n n n    n 3 n n n n n n n n\n"
"              d V 0 k n t 3 2 1 0    d V 0 1 2 3 4 5 6 7\n"
"\n"
"              1 2 3 4 5 6 7 8 9 10   1 2 3 4 5 6 7 8 9 10\n"
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

static
void
help(char *text)
{
    char *p;
    char line[BASIC_LINE_SIZE];

    while (*text) {
        p = strchr(text, '\n');
        assert(p);
        assert(p-text < BASIC_LINE_SIZE);
        memcpy(line, text, p-text);
        line[p-text] = '\0';
        printf("%s\n", line);
        text = p+1;
    }
}

static const char *demo0[] = {
  " rem ### blinky ###",
  "dim i",
  "dim led as pin irq4* for digital output",
  "while 1 do",
  "  for i = 1 to 16",
  "    let led = !led",
  "    sleep 50",
  "  next",
  "  sleep 800",
  "endwhile",
  "end"
};


static const char *demo1[] = {
  "rem ### uart isr ###",
  "dim data",
  "data 1, 1, 2, 3, 5, 8, 13, 21, 0",
  "configure uart 0 for 300 baud 8 data no parity loopback",
  "dim tx as pin utxd0 for uart output",
  "dim rx as pin urxd0 for uart input",
  "on uart 0 input gosub receive",
  "on uart 0 output gosub transmit",
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

static const char *demo2[] = {
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

static const char *demo3[] = {
  "rem ### toaster ###",
  "dim target, secs",
  "dim thermocouple as pin an0 for analog input",
  "dim relay as pin an1 for digital output",
  "data 5124, 6, 7460, 9, 8940, 3, 0, 0",
  "configure timer 0 for 1000 ms",
  "on timer 0 gosub adjust",
  "read target, secs",
  "while target do",
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

static
int
gethex(char **p)
{
    char c;
    
    c = **p;
    if (c >= '0' && c <= '9') {
        (*p)++;
        return c - '0';
    } else if (c >= 'A' && c <= 'F') {
        (*p)++;
        return 10 + c - 'A';
    } else {
        return -1;
    }
}

static
int
get2hex(char **p)
{
    int v1, v0;
    
    v1 = gethex(p);
    if (v1 == -1) {
        return -1;
    }
    v0 = gethex(p);
    if (v0 == -1) {
        return -1;
    }
    
    return v1*16+v0;
}

static
void
upgrade()
{
#if ! _WIN32
    int i;
    int n;
    int x;
    int y;
    char c;
    int sum;
    int addr;
    int count;
    char *s19;
    bool done;
    uint32 data;
    bool s0_found;
    void (*fn)(void);
    
    if ((int)end_of_static > FLASH_BYTES/2) {
        printf("code exceeds half of flash\n");
        return;
    }

    // erase the staging area
    flash_upgrade_prepare();

    printf("paste S19 upgrade file now...\n");
    ftdi_echo = false;
    
    y = 0;
    done = false;
    s0_found = false;
    
    do {
        if (cpustick_ready) {
            cpustick_ready = NULL;
            ftdi_command_ack(false);
        }
        while (! cpustick_ready) {
            // NULL
        }
        
        // we have an S19 line to parse
        s19 = cpustick_ready;
        while (isspace(*s19)) {
            s19++;
        }
        
        if (! *s19) {
            continue;
        }

        sum = 0;
        
        // header        
        if (*s19++ != 'S') {
            printf("\nbad record\n");
            break;
        }
        c = *s19++;
        
        if (c == '0') {
            s0_found = true;
        } else if (c == '3') {
            // 1 byte of count
            n = get2hex(&s19);
            if (n == -1) {
                printf("\nbad count\n");
                break;
            }
            sum += n;
            count = n;
            
            // we flash 4 bytes at a time!
            if ((count-1) % 4) {
                printf("\nbad count\n");
                break;
            }
            
            // 4 bytes of address
            addr = 0;
            for (i = 0; i < 4; i++) {
                n = get2hex(&s19);
                if (n == -1) {
                    printf("\nbad address\n");
                    break;
                }
                sum += n;
                addr = addr*256+n;
            }
            if (i != 4) {
                break;
            }
            
            assert(count > 4);
            count -= 4;
            
            while (count > 1) {
                assert((count-1) % 4 == 0);
                
                // get 4 bytes of data
                data = 0;
                for (i = 0; i < 4; i++) {
                    n = get2hex(&s19);
                    if (n == -1) {
                        printf("\nbad data\n");
                        break;
                    }
                    sum += n;
                    data = data*256+n;
                }
                if (i != 4) {
                    break;
                }
                
                // program the word
                flash_write_words((uint32 *)(FLASH_BYTES/2+addr), &data, 1);
                
                assert(count > 4);
                count -= 4;
                addr += 4;
            }
            if (count > 1) {
                break;
            }
            
            assert(count == 1);
            n = get2hex(&s19);
            if (n == -1) {
                printf("\nbad checksum\n");
                break;
            }
            sum += n;
            if ((sum & 0xff) != 0xff) {
                printf("\nbad checksum 0x%x\n", sum & 0xff);
                break;
            }
            
            if (y++%2) {
                printf(".");
            }
        } else if (c == '7') {
            done = true;
        } else {
            // unrecognized record
            break;
        }
    } while (! done);
    
    if (! s0_found || ! done) {
        if (cpustick_ready) {
            cpustick_ready = NULL;
            ftdi_command_ack(false);
        }
        
        // we're in trouble!
        if (! s0_found) {
            printf("s0 record not found\n");
        }
        if (! done) {
            printf("s7 record not found\n");
        }
        printf("upgrade failed\n");
        ftdi_echo = true;

        // erase the staging area
        code_new();
    } else {
        printf("\npaste done!\n");
        printf("programming flash...\n");
        printf("wait for CPUStick LED e1 to blink!\n");
        delay(100);
        
        // disable interrupts
        x = splx(7);
    
        // copy our flash upgrade routine to RAM
        assert((int)flash_upgrade_end - (int)flash_upgrade <= BASIC_SMALL_PAGE_SIZE);
        memcpy(RAM_CODE_PAGE, flash_upgrade, (int)flash_upgrade_end - (int)flash_upgrade);
        
        // and run it!
        fn = (void *)RAM_CODE_PAGE;
        fn();
        
        // we should not come back!
        assert(0);
        memset(RAM_CODE_PAGE, 0, BASIC_SMALL_PAGE_SIZE);
        splx(x);
    }
#endif
}

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
    bool flash;
    bool boo;
    char *text;
    int length;
    int number1;
    int number2;
    bool *boolp;
    int syntax_error;
    byte bytecode[BASIC_BYTECODE_SIZE];

    if (run_step && ! *text_in) {
        text = "cont";
    } else {
        text = text_in;
    }

    trim(&text);

    for (cmd = 0; cmd < LENGTHOF(commands); cmd++) {
        len = strlen(commands[cmd].command);
        if (! strncmp(text, commands[cmd].command, len)) {
            break;
        }
    }

    if (cmd != LENGTHOF(commands)) {
        text += len;
    }
    trim(&text);

    number1 = 0;
    number2 = 0;

    switch (cmd) {
        case command_autoreset:
        case command_autorun:
            if (*text) {
                if (basic_word(&text, "on")) {
                    boo = true;
                } else if (basic_word(&text, "off")) {
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

        case command_clear:
            flash = false;
            if (basic_word(&text, "flash")) {
                flash = true;
            }
            if (*text) {
                goto XXX_ERROR_XXX;
            }

            var_clear(flash);
            break;
            
    	case command_clone:
    		boo = false;
            if (*text) {
            	if (basic_word(&text, "run")) {
            		boo = true;
            	}
	            if (*text) {
                	goto XXX_ERROR_XXX;
	            }
            }
            help(help_about);
            printf("cloning...\n");
    		clone(boo);
    		break;

        case command_cont:
        case command_run:
            if (*text) {
                if (! basic_const(&text, &number1)) {
                    goto XXX_ERROR_XXX;
                }
                if (*text) {
                    goto XXX_ERROR_XXX;
                }
            }
            
#if ! _WIN32
            ftdi_command_discard(true);
            if (cpustick_ready) {
                cpustick_ready = NULL;
                ftdi_command_ack(false);
            }
#endif

            run(cmd == command_cont, number1);
            
#if ! _WIN32
            ftdi_command_discard(false);
#endif
            break;

        case command_delete:
        case command_list:
            if (*text) {
                if (basic_const(&text, &number1)) {
                    number2 = number1;
                }
                if (*text) {
                    if (! basic_char(&text, '-')) {
                        goto XXX_ERROR_XXX;
                    }
                    number2 = 0;
                    (void)basic_const(&text, &number2);
                    if (*text) {
                        goto XXX_ERROR_XXX;
                    }
                }
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
            if (! basic_const(&text, &number1) || ! number1) {
                goto XXX_ERROR_XXX;
            }
            if (*text) {
                goto XXX_ERROR_XXX;
            }
            code_edit(number1);
            break;
            
        case command_help:
            if (! *text) {
                help(help_general);
            } else if (basic_word(&text, "about")) {
                help(help_about);
            } else if (basic_word(&text, "commands")) {
                help(help_commands);
            } else if (basic_word(&text, "statements")) {
                help(help_statements);
            } else if (basic_word(&text, "devices")) {
                help(help_devices);
            } else if (basic_word(&text, "blocks")) {
                help(help_blocks);
            } else if (basic_word(&text, "expressions")) {
                help(help_expressions);
            } else if (basic_word(&text, "variables")) {
                help(help_variables);
            } else if (basic_word(&text, "pins")) {
                help(help_pins);
            } else if (basic_word(&text, "board")) {
                help(help_board);
            } else if (basic_word(&text, "clone")) {
                help(help_clone);
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
            if (*text) {
                if (! basic_const(&text, &number1) || ! number1) {
                    goto XXX_ERROR_XXX;
                }
            }
            if (*text) {
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

        case command_step:
        case command_trace:
            if (cmd == command_step) {
                boolp = &run_step;
            } else {
                assert(cmd == command_trace);
                boolp = &var_trace;
            }
            // revisit -- duplicate code
            if (*text) {
                if (basic_word(&text, "on")) {
                    *boolp = true;
                } else if (basic_word(&text, "off")) {
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
            upgrade();
            
            // if we're back, we're (probably) in trouble!
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
                if (! basic_const(&text, &number1) || ! number1) {
                    goto XXX_ERROR_XXX;
                }
                if (*text) {
                    code_insert(number1, text, text-text_in);
                } else {
                    code_insert(number1, NULL, text-text_in);
                }
            } else if (*text) {
                // see if this might be a basic line executing directly
                boo = parse_line(text, &length, bytecode, &syntax_error);
                if (! boo) {
                	ftdi_command_error(text-text_in + syntax_error);
                } else {
                    // run the bytecode
                    (void)run_bytecode(true, bytecode, length);
                }
            }
    }
    return;

XXX_ERROR_XXX:
    ftdi_command_error(text-text_in);
}

void
basic_initialize(void)
{
#if ! _WIN32
    extern unsigned char far __DATA_RAM[], __DATA_ROM[], __DATA_END[];

    end_of_static = __DATA_ROM + (__DATA_END - __DATA_RAM);
    start_of_dynamic = FLASH_CODE1_PAGE;
    assert(end_of_static < start_of_dynamic);

    end_of_dynamic = FLASH_PARAM2_PAGE+BASIC_SMALL_PAGE_SIZE;
    assert(end_of_dynamic <= (byte *)0x20000);
#endif
    
    code_initialize();
}

#endif
