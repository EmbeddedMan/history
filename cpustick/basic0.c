// *** basic0.c *******************************************************
// this file implements private extensions to the stickos command
// interpreter.

#include "main.h"

enum cmdcode {
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    command_clone,  // [run]
#endif
    command_connect,  // nnn
    command_download,  // nnn
    command_help,
#if MCF52233
    command_ipaddress,  // [dhcp|<ipaddress>]
#endif
    command_nodeid,  // nnn
    command_reset,
    command_upgrade,
    command_uptime,
#if SODEBUG || MCF52259 || PIC32
    command_zigflea,
#endif
    command_dummy
};

static
const char * const commands[] = {
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    "clone",
#endif
    "connect",
    "download",
    "help",
#if MCF52233
    "ipaddress",
#endif    
    "nodeid",
    "reset",
    "upgrade",
    "uptime",
#if SODEBUG || MCF52259 || PIC32
    "zigflea",
#endif
};


// *** help ***********************************************************

#if BADGE_BOARD
char * const help_about_short =
"StickOS for MCF51JM128 v" VERSION "!";
#endif

char * const help_about =
#if MCF52233
"Welcome to StickOS for Freescale MCF52233 v" VERSION "!\n"
#elif MCF52221
"Welcome to StickOS for Freescale MCF52221 v" VERSION "!\n"
#elif DEMO_KIT && MCF52259
"Welcome to StickOS for Freescale MCF52252 DemoKit v" VERSION "!\n"
#elif MCF52259
"Welcome to StickOS for Freescale MCF52252 v" VERSION "!\n"
#elif MCF5211
"Welcome to StickOS for Freescale MCF5211 v" VERSION "!\n"
#elif BADGE_BOARD && MCF51JM128
"Welcome to StickOS for Freescale MCF51JM128 BadgeBoard v" VERSION "!\n"
#elif MCF51JM128
"Welcome to StickOS for Freescale MCF51JM128 v" VERSION "!\n"
#elif MCF51CN128
"Welcome to StickOS for Freescale MCF51CN128 v" VERSION "!\n"
#elif MCF51QE128
"Welcome to StickOS for Freescale MCF51QE128 v" VERSION "!\n"
#elif MC9S08QE128
"Welcome to StickOS for Freescale MC9S08QE128 v" VERSION "!\n"
#elif MC9S12DT256
"Welcome to StickOS for Freescale MC9S12DT256 v" VERSION "!\n"
#elif MC9S12DP512
"Welcome to StickOS for Freescale MC9S12DP512 v" VERSION "!\n"
#elif PIC32 && defined(__32MX440F256H__)
"Welcome to StickOS for Microchip PIC32MXx-F256H v" VERSION "!\n"
#elif PIC32 && defined(__32MX440F512H__) && HIDBL
"Welcome to StickOS for Microchip PIC32MXx-F512H CUI32 v" VERSION "!\n"
#elif PIC32 && defined(__32MX440F512H__)
"Welcome to StickOS for Microchip PIC32MXx-F512H v" VERSION "!\n"
#elif PIC32 && defined(__32MX460F512L__) && HIDBL
"Welcome to StickOS for Microchip PIC32MXx-F512L UBW32 v" VERSION "!\n"
#elif PIC32 && defined(__32MX460F512L__)
"Welcome to StickOS for Microchip PIC32MXx-F512L v" VERSION "!\n"
#else
#error
#endif
"Copyright (c) 2008-2010; all rights reserved.\n"
"http://www.cpustick.com\n"
"support@cpustick.com\n"
;

#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#pragma CODE_SEG HELP_CODE
#pragma STRING_SEG HELP_STRING
// N.B. for MC9S08QE128/MC9S12DT256/MC9S12DP512 we put all the help strings
// and the function that accesses them in page 7 or 38; we have to put the
// functions that copy these to RAM in unbanked memory.
#endif

#if ! SODEBUG || STICK_GUEST
static char *const help_general =
#if HELP_COMPRESS
"for more information:\n"
"  help about\n"
"  help commands\n"
"  help modes\n"
"  help statements\n"
"  help blocks\n"
"  help devices\n"
"  help expressions\n"
"  help strings\n"
"  help variables\n"
"  help pins\n"
#if MCF52259
"  help board\n"
#endif
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
"  help clone\n"
#endif
"  help zigflea\n"
"\n"
"see also:\n"
"  http://www.cpustick.com\n"
#else
"fo\262mor\245information:\n"
"\300hel\260about\n"
"\300hel\260commands\n"
"\300hel\260modes\n"
"\300hel\260statements\n"
"\300hel\260blocks\n"
"\300hel\260devices\n"
"\300hel\260expressions\n"
"\300hel\260strings\n"
"\300hel\260variables\n"
"\300hel\260pins\n"
#if MCF52259
"  help board\n"
#endif
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
"\300hel\260clone\n"
#endif
"\300hel\260zigflea\n"
"\n"
"se\245also:\n"
"\300http://www.cpustick.com\n"
#endif
;

static char * const help_commands =
#if HELP_COMPRESS
"<Ctrl-C>                      -- stop program\n"
"auto <line>                   -- automatically number program lines\n"
"clear [flash]                 -- clear ram [and flash] variables\n"
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
"clone [run]                   -- clone flash to slave MCU [and run]\n"
#endif
"cls                           -- clear terminal screen\n"
"cont [<line>]                 -- continue program from stop\n"
"delete ([<line>][-][<line>]|<subname>) -- delete program lines\n"
"download <slave Hz>           -- download flash to slave MCU\n"
"dir                           -- list saved programs\n"
"edit <line>                   -- edit program line\n"
"help [<topic>]                -- online help\n"
"list ([<line>][-][<line>]|<subname>) -- list program lines\n"
"load <name>                   -- load saved program\n"
"memory                        -- print memory usage\n"
"new                           -- erase code ram and flash memories\n"
"profile ([<line>][-][<line>]|<subname>) -- display profile info\n"
"purge <name>                  -- purge saved program\n"
"renumber [<line>]             -- renumber program lines (and save)\n"
"reset                         -- reset the MCU!\n"
"run [<line>]                  -- run program\n"
"save [<name>]                 -- save code ram to flash memory\n"
"undo                          -- undo code changes since last save\n"
#if ! BADGE_BOARD && ! DEMO_KIT && ! MCF9S08QE128 && ! MC9S12DT256 && ! MC9S12DP512 && ! MC51QE128
"upgrade                       -- upgrade StickOS firmware!\n"
#endif
"uptime                        -- print time since last reset\n"
"\n"
"for more information:\n"
"  help modes\n"
#else
"<Ctrl-C>\324-- sto\260program\n"
"aut\257<line>\321-- automaticall\271numbe\262progra\255lines\n"
"clea\262[flash]\317-- clea\262ra\255[an\244flash\235variables\n"
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
"clon\245[run]\321-- clon\245flas\250t\257slav\245MC\225[an\244run]\n"
#endif
"cls\331-- clea\262termina\254screen\n"
"con\264[<line>]\317-- continu\245progra\255fro\255stop\n"
"delet\245([<line>][-][<line>]|<subname>) -- delet\245progra\255lines\n"
"downloa\244<slav\245Hz>\311-- downloa\244flas\250t\257slav\245MCU\n"
"dir\331-- lis\264save\244programs\n"
"edi\264<line>\321-- edi\264progra\255line\n"
"hel\260[<topic>]\316-- onlin\245help\n"
"lis\264([<line>][-][<line>]|<subname>) -- lis\264progra\255lines\n"
"loa\244<name>\321-- loa\244save\244program\n"
"memory\326-- prin\264memor\271usage\n"
"new\331-- eras\245cod\245ra\255an\244flas\250memories\n"
"profil\245([<line>][-][<line>]|<subname>) -- displa\271profil\245info\n"
"purg\245<name>\320-- purg\245save\244program\n"
"renumbe\262[<line>]\313-- renumbe\262progra\255line\263(an\244save)\n"
"reset\327-- rese\264th\245MCU!\n"
"ru\256[<line>]\320-- ru\256program\n"
"sav\245[<name>]\317-- sav\245cod\245ra\255t\257flas\250memory\n"
"undo\330-- und\257cod\245change\263sinc\245las\264save\n"
#if ! BADGE_BOARD && ! DEMO_KIT && ! MCF9S08QE128 && ! MC9S12DT256 && ! MC9S12DP512 && ! MC51QE128
"upgrade\325-- upgrad\245StickO\223firmware!\n"
#endif
"uptime\326-- prin\264tim\245sinc\245las\264reset\n"
"\n"
"fo\262mor\245information:\n"
"\300hel\260modes\n"
#endif
;

static char * const help_modes =
#if HELP_COMPRESS
"analog [<millivolts>]             -- set/display analog voltage scale\n"
"autorun [on|off]                  -- autorun mode (on reset)\n"
"echo [on|off]                     -- terminal echo mode\n"
"indent [on|off]                   -- listing indent mode\n"
#if MCF52233
"ipaddress [dhcp|<ipaddress>]      -- set/display ip address\n"
#endif
"nodeid [<nodeid>|none]            -- set/display zigflea nodeid\n"
"numbers [on|off]                  -- listing line numbers mode\n"
"pins [<assign> [<pinname>|none]]  -- set/display StickOS pin assignments\n"
"prompt [on|off]                   -- terminal prompt mode\n"
"servo [<Hz>]                      -- set/display servo Hz (on reset)\n"
"sleep [on|off]                    -- debugger sleep mode\n"
"step [on|off]                     -- debugger single-step mode\n"
"trace [on|off]                    -- debugger trace mode\n"
#if MCF52259 || PIC32
"usbhost [on|off]                  -- USB host mode (on reset)\n"
#endif
"watchsmart [on|off]               -- low-overhead watchpoint mode\n"
"\n"
"pin assignments:\n"
"  heartbeat  safemode*\n"
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
"  qspi_cs*  clone_rst*  zigflea_rst*  zigflea_attn*  zigflea_rxtxen\n"
#else
"  qspi_cs*  zigflea_rst*  zigflea_attn*  zigflea_rxtxen\n"
#endif
"\n"
"for more information:\n"
"  help pins\n"
#else
"analo\247[<millivolts>]\313-- set/displa\271analo\247voltag\245scale\n"
"autoru\256[on|off]\320-- autoru\256mod\245(o\256reset)\n"
"ech\257[on|off]\323-- termina\254ech\257mode\n"
"inden\264[on|off]\321-- listin\247inden\264mode\n"
#if MCF52233
"ipaddress [dhcp|<ipaddress>]      -- set/display ip address\n"
#endif
"nodei\244[<nodeid>|none]\312-- set/displa\271zigfle\241nodeid\n"
"number\263[on|off]\320-- listin\247lin\245number\263mode\n"
"pin\263[<assign> [<pinname>|none]]\300-- set/displa\271StickO\223pi\256assignments\n"
"promp\264[on|off]\321-- termina\254promp\264mode\n"
"serv\257[<Hz>]\324-- set/displa\271serv\257H\272(o\256reset)\n"
"slee\260[on|off]\322-- debugge\262slee\260mode\n"
"ste\260[on|off]\323-- debugge\262single-ste\260mode\n"
"trac\245[on|off]\322-- debugge\262trac\245mode\n"
#if MCF52259 || PIC32
"usbhost [on|off]                  -- USB host mode (on reset)\n"
#endif
"watchsmar\264[on|off]\315-- low-overhea\244watchpoin\264mode\n"
"\n"
"pi\256assignments:\n"
"\300heartbea\024safemode*\n"
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
"\300qspi_cs*\300clone_rst*\300zigflea_rst*\300zigflea_attn*\300zigflea_rxtxen\n"
#else
"  qspi_cs*  zigflea_rst*  zigflea_attn*  zigflea_rxtxen\n"
#endif
"\n"
"fo\262mor\245information:\n"
"\300hel\260pins\n"
#endif
;

static char * const help_statements =
#if HELP_COMPRESS
"<line>                                 -- delete program line from code ram\n"
"<line> <statement>                     -- enter program line into code ram\n"
"\n"
"assert <expression>                    -- break if expression is false\n"
"data <n> [, ...]                       -- read-only data\n"
"dim <variable>[$][[n]] [as ...] [, ...] -- dimension variables\n"
"end                                    -- end program\n"
#if BADGE_BOARD
"jm(clear|set) <r>, <c>                 -- clear/set row/column of LED matrix\n"
"jmscroll ...                           -- scroll printout to LED matrix\n"
#endif
"halt                                   -- loop forever\n"
"input [dec|hex|raw] <variable>[$] [, ...] -- input data\n"
"label <label>                          -- read/data label\n"
"let <variable>[$] = <expression> [, ...] -- assign variable\n"
"print [dec|hex|raw] <expression> [, ...] [;] -- print results\n"
"read <variable> [, ...]                -- read read-only data into variables\n"
"rem <remark>                           -- remark\n"
"restore [<label>]                      -- restore read-only data pointer\n"
"sleep <expression> (s|ms|us)           -- delay program execution\n"
"stop                                   -- insert breakpoint in code\n"
"vprint <variable>[$] = [dec|hex|raw] <expression> [, ...] -- print to variable\n"
"\n"
"for more information:\n"
"  help blocks\n"
"  help devices\n"
"  help expressions\n"
"  help strings\n"
"  help variables\n"
#else
"<line>\337-- delet\245progra\255lin\245fro\255cod\245ram\n"
"<line> <statement>\323-- ente\262progra\255lin\245int\257cod\245ram\n"
"\n"
"asser\264<expression>\322-- brea\253i\246expressio\256i\263false\n"
"dat\241<n> [, ...]\325-- read-onl\271data\n"
"di\255<variable>[$][[n]\235[a\263...\235[, ...\235-- dimensio\256variables\n"
"end\342-- en\244program\n"
#if BADGE_BOARD
"jm(clear|set) <r>, <c>                 -- clear/set row/column of LED matrix\n"
"jmscroll ...                           -- scroll printout to LED matrix\n"
#endif
"halt\341-- loo\260forever\n"
"inpu\264[dec|hex|raw\235<variable>[$\235[, ...\235-- inpu\264data\n"
"labe\254<label>\330-- read/dat\241label\n"
"le\264<variable>[$\235= <expression> [, ...\235-- assig\256variable\n"
"prin\264[dec|hex|raw\235<expression> [, ...\235[;\235-- prin\264results\n"
"rea\244<variable> [, ...]\316-- rea\244read-onl\271dat\241int\257variables\n"
"re\255<remark>\331-- remark\n"
"restor\245[<label>]\324-- restor\245read-onl\271dat\241pointer\n"
"slee\260<expression> (s|ms|us)\311-- dela\271progra\255execution\n"
"stop\341-- inser\264breakpoin\264i\256code\n"
"vprin\264<variable>[$\235= [dec|hex|raw\235<expression> [, ...\235-- prin\264t\257variable\n"
"\n"
"fo\262mor\245information:\n"
"\300hel\260blocks\n"
"\300hel\260devices\n"
"\300hel\260expressions\n"
"\300hel\260strings\n"
"\300hel\260variables\n"
#endif
;

static char * const help_blocks =
#if HELP_COMPRESS
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
"gosub <subname> [<expression>, ...]\n"
"\n"
"sub <subname> [<param>, ...]\n"
"  [return]\n"
"endsub\n"
#else
"i\246<expression> then\n"
"[elsei\246<expression> then]\n"
"[else]\n"
"endif\n"
"\n"
"fo\262<variable> = <expression> t\257<expression> [ste\260<expression>]\n"
"\300[(break|continue) [n]]\n"
"next\n"
"\n"
"whil\245<expression> do\n"
"\300[(break|continue) [n]]\n"
"endwhile\n"
"\n"
"do\n"
"\300[(break|continue) [n]]\n"
"unti\254<expression>\n"
"\n"
"gosu\242<subname> [<expression>, ...]\n"
"\n"
"su\242<subname> [<param>, ...]\n"
"\300[return]\n"
"endsub\n"
#endif
;

static char * const help_devices =
#if HELP_COMPRESS
"timers:\n"
"  configure timer <n> for <n> (s|ms|us)\n"
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
"  uart <n> (read|write) <variable> [, ...]   -- perform uart I/O\n"
"\n"
#if PIC32 || MCF52221 || MCF52233 || MCF52259 || MCF5211
"i2c:\n"
"  i2c (start <addr>|(read|write) <variable> [, ...]|stop) -- master i2c I/O\n"
"\n"
#endif
"qspi:\n"
"  qspi <variable> [, ...]                    -- master qspi I/O\n"
"\n"
"watchpoints:\n"
"  on <expression> do <statement>             -- on expr execute statement\n"
"  off <expression>                           -- disable expr watchpoint\n"
"  mask <expression>                          -- mask/hold expr watchpoint\n"
"  unmask <expression>                        -- unmask expr watchpoint\n"
#else
"timers:\n"
"\300configur\245time\262<n> fo\262<n> (s|ms|us)\n"
"\300o\256time\262<n> d\257<statement>\316-- o\256time\262execut\245statement\n"
"\300of\246time\262<n>\334-- disabl\245time\262interrupt\n"
"\300mas\253time\262<n>\333-- mask/hol\244time\262interrupt\n"
"\300unmas\253time\262<n>\331-- unmas\253time\262interrupt\n"
"\n"
"uarts:\n"
"\300configur\245uar\264<n> fo\262<n> bau\244<n> dat\241(even|odd|no) parit\271[loopback]\n"
"\300o\256uar\264<n> (input|output) d\257<statement>\300-- o\256uar\264execut\245statement\n"
"\300of\246uar\264<n> (input|output)\316-- disabl\245uar\264interrupt\n"
"\300mas\253uar\264<n> (input|output)\315-- mask/hol\244uar\264interrupt\n"
"\300unmas\253uar\264<n> (input|output)\313-- unmas\253uar\264interrupt\n"
"\300uar\264<n> (read|write) <variable> [, ...]\301-- perfor\255uar\264I/O\n"
"\n"
#if PIC32 || MCF52221 || MCF52233 || MCF52259 || MCF5211
"i2c:\n"
"\300i2\243(star\264<addr>|(read|write) <variable> [, ...]|stop) -- maste\262i2\243I/O\n"
"\n"
#endif
"qspi:\n"
"\300qsp\251<variable> [, ...]\322-- maste\262qsp\251I/O\n"
"\n"
"watchpoints:\n"
"\300o\256<expression> d\257<statement>\313-- o\256exp\262execut\245statement\n"
"\300of\246<expression>\331-- disabl\245exp\262watchpoint\n"
"\300mas\253<expression>\330-- mask/hol\244exp\262watchpoint\n"
"\300unmas\253<expression>\326-- unmas\253exp\262watchpoint\n"
#endif
;

static char * const help_expressions =
#if HELP_COMPRESS
"the following operators are supported as in C,\n"
"in order of decreasing precedence:\n"
"  <n>                       -- decimal constant\n"
"  0x<n>                     -- hexadecimal constant\n"
"  'c'                       -- character constant\n"
"  <variable>                -- simple variable\n"
"  <variable>[<expression>]  -- array variable element\n"
"  <variable>#               -- length of array or string\n"
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
#else
"th\245followin\247operator\263ar\245supporte\244a\263i\256C,\n"
"i\256orde\262o\246decreasin\247precedence:\n"
"\300<n>\325-- decima\254constant\n"
"\3000x<n>\323-- hexadecima\254constant\n"
"\300'c'\325-- characte\262constant\n"
"\300<variable>\316-- simpl\245variable\n"
"\300<variable>[<expression>]\300-- arra\271variabl\245element\n"
"\300<variable>#\315-- lengt\250o\246arra\271o\262string\n"
"\300(\301)\323-- grouping\n"
"\300!\301~\323-- logica\254not, bitwis\245not\n"
"\300*\301/\301%\317-- times, divide, mod\n"
"\300+\301-\323-- plus, minus\n"
"\300>>\300<<\322-- shif\264right, left\n"
"\300<=\300<\300>=\300>\314-- inequalities\n"
"\300==\300!=\322-- equal, no\264equal\n"
"\300|\301^\301&\317-- bitwis\245or, xor, and\n"
"\300|\034^^\300&&\316-- logica\254or, xor, and\n"
"fo\262mor\245information:\n"
"\300hel\260variables\n"
#endif
;

static char * const help_strings =
#if HELP_COMPRESS
"v$ is a nul-terminated view into a byte array v[]\n"
"\n"
"string statements:\n"
"  dim, input, let, print, vprint\n"
"  if <expression> <relation> <expression> then\n"
"  while <expression> <relation> <expression> do\n"
"  until <expression> <relation> <expression> do\n"
"\n"
"string expressions:\n"
"  \"literal\"                      -- literal string\n"
"  <variable>$                    -- variable string\n"
"  <variable>$[<start>:<length>]  -- variable substring\n"
"  +                              -- concatenates strings\n"
"\n"
"string relations:\n"
"  <=  <  >=  >                   -- inequalities\n"
"  ==  !=                         -- equal, not equal\n"
"  ~  !~                          -- contains, does not contain\n"
"for more information:\n"
"  help variables\n"
#else
"v$ i\263\241nul-terminate\244vie\267int\257\241byt\245arra\271v[]\n"
"\n"
"strin\247statements:\n"
"\300dim, input, let, print, vprint\n"
"\300i\246<expression> <relation> <expression> then\n"
"\300whil\245<expression> <relation> <expression> do\n"
"\300unti\254<expression> <relation> <expression> do\n"
"\n"
"strin\247expressions:\n"
"\300\"literal\"\324-- litera\254string\n"
"\300<variable>$\322-- variabl\245string\n"
"\300<variable>$[<start>:<length>]\300-- variabl\245substring\n"
"\300+\334-- concatenate\263strings\n"
"\n"
"strin\247relations:\n"
"\300<=\300<\300>=\300>\321-- inequalities\n"
"\300==\300!=\327-- equal, no\264equal\n"
"\300\036!~\330-- contains, doe\263no\264contain\n"
"fo\262mor\245information:\n"
"\300hel\260variables\n"
#endif
;

static char *const help_variables =
#if HELP_COMPRESS
"all variables must be dimensioned!\n"
"variables dimensioned in a sub are local to that sub\n"
"simple variables are passed to sub params by reference; otherwise, by value\n"
"array variable indices start at 0\n"
#if PIC32 || MCF52221 || MCF52233 || MCF52259 || MCF5211
"v is the same as v[0], except for input/print/i2c/qspi/uart statements\n"
#else
"v is the same as v[0], except for input/print/qspi/uart statements\n"
#endif
"\n"
"ram variables:\n"
"  dim <var>[$][[n]]\n"
"  dim <var>[[n]] as (byte|short)\n"
"\n"
"absolute variables:\n"
"  dim <var>[[n]] [as (byte|short)] at address <addr>\n"
"\n"
"flash parameter variables:\n"
"  dim <varflash>[[n]] as flash\n"
"\n"
"pin alias variables:\n"
"  dim <varpin> as pin <pinname> for (digital|analog|servo|frequency|uart) \\\n"
"                                      (input|output) \\\n"
"                                      [debounced] [inverted] [open_drain]\n"
"\n"
"system variables:\n"
"  getchar  nodeid  msecs  seconds  ticks  ticks_per_msec  (read-only)\n"
"\n"
"for more information:\n"
"  help pins\n"
#else
"al\254variable\263mus\264b\245dimensioned!\n"
"variable\263dimensione\244i\256\241su\242ar\245loca\254t\257tha\264sub\n"
"simpl\245variable\263ar\245passe\244t\257su\242param\263b\271reference; otherwise, b\271value\n"
"arra\271variabl\245indice\263star\264a\2640\n"
#if PIC32 || MCF52221 || MCF52233 || MCF52259 || MCF5211
"\266i\263th\245sam\245a\263v[0], excep\264fo\262input/print/i2c/qspi/uar\264statements\n"
#else
"v is the same as v[0], except for input/print/qspi/uart statements\n"
#endif
"\n"
"ra\255variables:\n"
"\300di\255<var>[$][[n]]\n"
"\300di\255<var>[[n]\235a\263(byte|short)\n"
"\n"
"absolut\245variables:\n"
"\300di\255<var>[[n]\235[a\263(byte|short)\235a\264addres\263<addr>\n"
"\n"
"flas\250paramete\262variables:\n"
"\300di\255<varflash>[[n]\235a\263flash\n"
"\n"
"pi\256alia\263variables:\n"
"\300di\255<varpin> a\263pi\256<pinname> fo\262(digital|analog|servo|frequency|uart) \\\n"
"\344(input|output) \\\n"
"\344[debounced\235[inverted\235[open_drain]\n"
"\n"
"syste\255variables:\n"
"\300getcha\022nodei\004msec\023second\023tick\023ticks_per_mse\003(read-only)\n"
"\n"
"fo\262mor\245information:\n"
"\300hel\260pins\n"
#endif
;

static char *const help_pins =
#if HELP_COMPRESS
"pin names:\n"
#else
"pi\256names:\n"
#endif
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
#if HELP_COMPRESS
"    0         1         2         3        4     5        6        7\n"
"  --------  --------- --------- -------- ----- -------- -------- ------+\n"
"  an0       an1       an2       an3      an4   an5      an6      an7   | AN\n"
"  scl       sda                                                        | AS\n"
#if MCF52233 || MCF52259 || MCF5211
"  gpt0      gpt1      gpt2      gpt3                                   | TA\n"
#endif
#if MCF52259
"            irq1*               irq3*          irq5*             irq7* | NQ\n"
#else
"            irq1*                        irq4*                   irq7* | NQ\n"
#endif
#if MCF52233
"                                irq11*                                 | GP\n"
#endif
"  qspi_dout qspi_din  qspi_clk  qspi_cs0       qspi_cs2 qspi_cs3       | QS\n"
"  dtin0     dtin1     dtin2     dtin3                                  | TC\n"
"  utxd0     urxd0     urts0*    ucts0*                                 | UA\n"
"  utxd1     urxd1     urts1*    ucts1*                                 | UB\n"
#if MCF52259
"  fec_col   fec_crs   fec_rxclk fec_rxd[0-3]                  fec_rxdv | TI\n"
"  fec_rxer  fec_txclk fec_txd[0-3]                   fec_txen fec_txer | TJ\n"
"  utxd2     urxd2     urts2*    ucts2*                                 | UC\n"
#endif
"\n"
"all pins support general purpose digital input/output\n"
"an? = potential analog input pins (mV)\n"
#if MCF52233 || MCF52259 || MCF5211
"dtin?, gpt? = potential analog output (PWM) pins (mV)\n"
"dtin?, gpt? = potential servo output (PWM) pins (cms)\n"
#else
"dtin? = potential analog output (PWM) pins (mV)\n"
"dtin? = potential servo output (PWM) pins (cms)\n"
#endif
"dtin? = potential frequency output pins (Hz)\n"
"urxd? = potential uart input pins (received byte)\n"
"utxd? = potential uart output pins (transmit byte)\n"
#else
"\3020\3071\3072\3073\3064\3035\3066\3067\n"
"\300--------\300--------- --------- -------- ----- -------- -------- ------+\n"
"\300an0\305an1\305an2\305an3\304an4\301an5\304an6\304an7\301\274AN\n"
"\300scl\305sda\366\274AS\n"
#if MCF52233 || MCF52259 || MCF5211
"  gpt0      gpt1      gpt2      gpt3                                   | TA\n"
#endif
#if MCF52259
"            irq1*               irq3*          irq5*             irq7* | NQ\n"
#else
"\312irq1*\326irq4*\321irq7* \274NQ\n"
#endif
#if MCF52233
"                                irq11*                                 | GP\n"
#endif
"\300qspi_dou\264qspi_di\016qspi_cl\013qspi_cs0\305qspi_cs2 qspi_cs3\305\274QS\n"
"\300dtin0\303dtin1\303dtin2\303dtin3\340\274TC\n"
"\300utxd0\303urxd0\303urts0*\302ucts0*\337\274UA\n"
"\300utxd1\303urxd1\303urts1*\302ucts1*\337\274UB\n"
#if MCF52259
"  fec_col   fec_crs   fec_rxclk fec_rxd[0-3]                  fec_rxdv | TI\n"
"  fec_rxer  fec_txclk fec_txd[0-3]                   fec_txen fec_txer | TJ\n"
"  utxd2     urxd2     urts2*    ucts2*                                 | UC\n"
#endif
"\n"
"al\254pin\263suppor\264genera\254purpos\245digita\254input/output\n"
"an? = potentia\254analo\247inpu\264pin\263(mV)\n"
#if MCF52233 || MCF52259 || MCF5211
"dtin?, gpt? = potential analog output (PWM) pins (mV)\n"
"dtin?, gpt? = potential servo output (PWM) pins (cms)\n"
#else
"dtin? = potentia\254analo\247outpu\264(PWM) pin\263(mV)\n"
"dtin? = potentia\254serv\257outpu\264(PWM) pin\263(cms)\n"
#endif
"dtin? = potentia\254frequenc\271outpu\264pin\263(Hz)\n"
"urxd? = potentia\254uar\264inpu\264pin\263(receive\244byte)\n"
"utxd? = potentia\254uar\264outpu\264pin\263(transmi\264byte)\n"
#endif
#elif MCF51JM128
"    0       1       2       3       4       5       6       7\n"
"  ------- ------- ------- ------- ------- ------- ------- --------+\n"
"  pta0    pta1    pta2    pta3    pta4    pta5                    | PORT A\n"
"  ptb0    ptb1    ptb2    ptb3    ptb4    ptb5    ptb6    ptb7    | PORT B\n"
"  ptc0    ptc1    ptc2    ptc3    ptc4    ptc5    ptc6            | PORT C\n"
"  ptd0    ptd1    ptd2    ptd3    ptd4    ptd5    ptd6    ptd7    | PORT D\n"
"  pte0    pte1    pte2    pte3    pte4    pte5    pte6    pte7    | PORT E\n"
"  ptf0    ptf1    ptf2    ptf3    ptf4    ptf5    ptf6    ptf7    | PORT F\n"
"  ptg0    ptg1    ptg2    ptg3                                    | PORT G\n"
"\n"
"all pins support general purpose digital input/output\n"
"ptb?, ptd[0134] = potential analog input pins (mV)\n"
"pte[23], ptf[0-5] = potential analog output (PWM) pins (mV)\n"
"pte[23], ptf[0-5] = potential servo output (PWM) pins (cms)\n"
"pte[23], ptf[0-5] = potential frequency output pins (Hz)\n"
"pte1 (u1), ptc5 (u2) = potential uart input pins (received byte)\n"
"pte0 (u1), ptc3 (u2) = potential uart output pins (transmit byte)\n"
#elif MCF51CN128
"    0       1       2       3       4       5       6       7\n"
"  ------- ------- ------- ------- ------- ------- ------- --------+\n"
"  pta0    pta1    pta2    pta3    pta4    pta5    pta6    pta7    | PORT A\n"
"  ptb0    ptb1    ptb2    ptb3    ptb4    ptb5    ptb6    ptb7    | PORT B\n"
"  ptc0    ptc1    ptc2            ptc4    ptc5    ptc6    ptc7    | PORT C\n"
"  ptd0    ptd1    ptd2    ptd3                    ptd6    ptd7    | PORT D\n"
"  pte0    pte1    pte2    pte3    pte4    pte5    pte6    pte7    | PORT E\n"
"  ptf0    ptf1    ptf2    ptf3    ptf4    ptf5    ptf6    ptf7    | PORT F\n"
"  ptg0    ptg1    ptg2    ptg3    ptg4    ptg5    ptg6    ptg7    | PORT G\n"
"  pth0    pth1    pth2    pth3    pth4    pth5    pth6    pth7    | PORT H\n"
"  ptj0    ptj1    ptj2    ptj3    ptj4    ptj5                    | PORT J\n"
"\n"
"all pins support general purpose digital input/output\n"
"ptc[4-7], ptd[0-37], pte[0-2] = potential analog input pins (mV)\n"
"ptb[67], ptc0, pte[3-5] = potential analog output (PWM) pins (mV)\n"
"ptb[67], ptc0, pte[3-5] = potential servo output (PWM) pins (cms)\n"
"ptb[67], ptc0, pte[3-5] = potential frequency output pins (Hz)\n"
"ptd1 (u1), ptd3 (u2), pta4 (u3) = potential uart input pins (received byte)\n"
"ptd0 (u1), ptd2 (u2), pta3 (u3) = potential uart output pins (transmit byte)\n"
#elif MCF51QE128 || MC9S08QE128
"    0       1       2       3       4       5       6       7\n"
"  ------- ------- ------- ------- ------- ------- ------- --------+\n"
"  pta0    pta1    pta2    pta3    pta4    pta5    pta6    pta7    | PORT A\n"
"  ptb0    ptb1    ptb2    ptb3    ptb4    ptb5    ptb6    ptb7    | PORT B\n"
"  ptc0    ptc1    ptc2    ptc3    ptc4    ptc5    ptc6    ptc7    | PORT C\n"
"  ptd0    ptd1    ptd2    ptd3    ptd4    ptd5    ptd6    ptd7    | PORT D\n"
"  pte0    pte1    pte2    pte3    pte4    pte5    pte6    pte7    | PORT E\n"
"  ptf0    ptf1    ptf2    ptf3    ptf4    ptf5    ptf6    ptf7    | PORT F\n"
"  ptg0    ptg1    ptg2    ptg3                                    | PORT G\n"
"\n"
"all pins support general purpose digital input/output\n"
"pta[0-367], ptb[0-3], ptf[0-7], ptg[23] = potential analog input pins (mV)\n"
"pta[0167], ptb[45], ptc[0-5] = potential analog output (PWM) pins (mV)\n"
"pta[0167], ptb[45], ptc[0-5] = potential servo output (PWM) pins (cms)\n"
"pta[0167], ptb[45], ptc[0-5] = potential frequency output pins (Hz)\n"
"ptb0 (u1), ptc6 (u2) = potential uart input pins (received byte)\n"
"ptb1 (u1), ptc7 (u2) = potential uart output pins (transmit byte)\n"
#elif MC9S12DT256 || MC9S12DP512
"    0       1       2       3       4       5       6       7\n"
"  ------- ------- ------- ------- ------- ------- ------- --------+\n"
"  pad00   pad01   pad02   pad03   pad04   pad05   pad06   pad07   | PORT AD0\n"
"  pad08   pad09   pad10   pad11   pad12   pad13   pad14   pad15   | PORT AD1\n"
"  pa0     pa1     pa2     pa3     pa4     pa5     pa6     pa7     | PORT A\n"
"  pb0     pb1     pb2     pb3     pb4     pb5     pb6     pb7     | PORT B\n"
"  pe0     pe1     pe2     pe3     pe4     pe5     pe6     pe7     | PORT E\n"
"  ph0     ph1     ph2     ph3     ph4     ph5     ph6     ph7     | PORT H\n"
"  pj0     pj1                                     pj6     pj7     | PORT J\n"
"  pk0     pk1     pk2     pk3     pk4     pk5     pk6     pk7     | PORT K\n"
"  pm0     pm1     pm2     pm3     pm4     pm5     pm6     pm7     | PORT M\n"
"  pp0     pp1     pp2     pp3     pp4     pp5     pp6     pp7     | PORT P\n"
"  ps0     ps1     ps2     ps3     ps4     ps5     ps6     ps7     | PORT S\n"
"  pt0     pt1     pt2     pt3     pt4     pt5     pt6     pt7     | PORT T\n"
"\n"
"all pins support general purpose digital input\n"
"all pins except pad?? and pe[01] support general purpose digital output\n"
"pad?? = potential analog input pins (mV)\n"
"pp? = potential analog output (PWM) pins (mV)\n"
"pp? = potential servo output (PWM) pins (cms)\n"
"pt? = potential frequency output pins (Hz)\n"
"ps0 (u0), ps2 (u1) = potential uart input pins (received byte)\n"
"ps1 (u0), ps3 (u1) = potential uart output pins (transmit byte)\n"
#elif PIC32
"  0/8     1/9     2/10    3/11    4/12    5/13    6/14    7/15\n"
"  ------- ------- ------- ------- ------- ------- ------- --------+\n"
#if _PORTA_RA0_MASK
"  ra0     ra1     ra2     ra3     ra4     ra5     ra6     ra7     | PORT A\n"
"          ra9     ra10                            ra14    ra15    |      A+8\n"
#endif
"  an0     an1     an2     an3     an4     an5     an6     an7     | PORT B\n"
"  an8     an9     an10    an11    an12    an13    an14    an15    |      B+8\n"
"          rc1     rc2     rc3     rc4                             | PORT C\n"
"                                  rc12    rc13    rc14    rc15    |      C+8\n"
"  rd0     rd1     rd2     rd3     rd4     rd5     rd6     rd7     | PORT D\n"
"  rd8     rd9     rd10    rd11    rd12    rd13    rd14    rd15    |      D+8\n"
"  re0     re1     re2     re3     re4     re5     re6     re7     | PORT E\n"
"  re8     re9                                                     |      E+8\n"
"  rf0     rf1     rf2     rf3     rf4     rf5                     | PORT F\n"
"  rf8                             rf12    rf13                    |      F+8\n"
"  rg0     rg1     rg2     rg3                     rg6     rg7     | PORT G\n"
"  rg8     rg9                     rg12    rg13    rg14    rg15    |      G+8\n"
"\n"
"all pins support general purpose digital input/output\n"
"an? = potential analog input pins (mV)\n"
"rd[0-4] = potential analog output (PWM) pins (mV)\n"
"rd[0-4] = potential servo output (PWM) pins (cms)\n"
"rd[0-4] = potential frequency output pins (Hz)\n"
"rf4 (u2) = potential uart input pins (received byte)\n"
"rf5 (u2) = potential uart output pins (transmit byte)\n"
#else
#error
#endif
;

#if MCF52259
static char *const help_board =
"1  2  3  4  5  6  7  8  9  10 11 12    1  2  3  4  5  6  7  8  9  10 11 12\n"
"\n"
"g  +  q  q  q  q  r  r  q  q  r  v     u  u  u  u  g  g  g  g  i  i  i  i\n"
"n  3  s  s  s  s  c  s  s  s  s  b     t  r  r  c  p  p  p  p  r  r  r  r\n"
"d  V  p  p  p  p  o  t  p  p  t  u     x  x  t  t  t  t  t  t  q  q  q  q\n"
"      i  i  i  i  n  i  i  i  o  s     d  d  s  s  0  1  2  3  1  3  5  7\n"
"      _  _  _  _  *  n  _  _  u        2  2  2  2              *  *  *  *\n"
"      c  d  d  c     *  c  c  t              *  *                        \n"
"      l  o  i  s        s  s  *                                          \n"
"      k  u  n  0        2  3                                             \n"
"         t                                                               \n"
"\n"
"antenna                            MCU                                USB\n"
"\n"
"\n"
"      u  u        u  u                                                   \n"
"u  u  r  c  u  u  r  c  d  d  d  d                                       \n"
"t  r  t  t  t  r  t  t  t  t  t  t                                       \n"
"x  x  s  s  x  x  s  s  i  i  i  i     g  +  s  s  a  a  a  a  a  a  a  a\n"
"d  d  0  0  d  d  1  1  n  n  n  n     n  3  c  d  n  n  n  n  n  n  n  n\n"
"0  0  *  *  1  1  *  *  0  1  2  3     d  V  l  a  0  1  2  3  4  5  6  7\n"
"\n"
"1  2  3  4  5  6  7  8  9  10 11 12    1  2  3  4  5  6  7  8  9  10 11 12\n"
;
#endif

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
static char *const help_clone =
#if HELP_COMPRESS
"clone cable:\n"
"  master           slave\n"
"  ---------        ----------------\n"
"  qspi_clk         qspi_clk (ezpck)\n"
"  qspi_din         qspi_dout (ezpq)\n"
"  qspi_dout        qspi_din (ezpd)\n"
"  qspi_cs0         rcon* (ezpcs*)\n"
"  pins clone_rst*  rsti*\n"
"  vss              vss\n"
"  vdd              vdd\n"
#else
"clon\245cable:\n"
"\300master\311slave\n"
"\300---------\306----------------\n"
"\300qspi_clk\307qspi_cl\253(ezpck)\n"
"\300qspi_din\307qspi_dou\264(ezpq)\n"
"\300qspi_dout\306qspi_di\256(ezpd)\n"
"\300qspi_cs0\307rcon* (ezpcs*)\n"
"\300pin\263clone_rst*\300rsti*\n"
"\300vss\314vss\n"
"\300vdd\314vdd\n"
#endif
;
#endif

static char *const help_zigflea =
#if HELP_COMPRESS
"connect <nodeid>              -- connect to MCU <nodeid> via zigflea\n"
"<Ctrl-D>                      -- disconnect from zigflea\n"
"\n"
"remote node variables:\n"
"  dim <varremote>[[n]] as remote on nodeid <nodeid>\n"
"\n"
"zigflea cable:\n"
"  MCU                  MC1320X\n"
"  -------------        -----------\n"
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
"  qspi_clk             spiclk\n"
"  qspi_din             miso\n"
"  qspi_dout            mosi\n"
#if MCF52259
"  irq1*                irq*\n"
#else
"  irq4*                irq*\n"
#endif
#elif MCF51JM128
"  spsck1 (pte6)        spiclk\n"
"  miso1 (pte4)         miso\n"
"  mosi1 (pte5)         mosi\n"
"  irq*                 irq*\n"
#elif MCF51CN128
"  spsck1 (ptb5)        spiclk\n"
"  miso1 (ptb4)         miso\n"
"  mosi1 (ptb3)         mosi\n"
"  irq* (ptc4)          irq*\n"
#elif MCF51QE128 || MC9S08QE128
"  spsck1 (ptb2)        spiclk\n"
"  miso1 (ptb4)         miso\n"
"  mosi1 (ptb3)         mosi\n"
"  irq*                 irq*\n"
#elif MC9S12DT256 || MC9S12DP512
"  sck0 (pm5)           spiclk\n"
"  miso0 (pm2)          miso\n"
"  mosi0 (pm4)          mosi\n"
"  irq* (pe1)           irq*\n"
#elif PIC32
// REVISIT -- implement zigflea on MRF24J40
"  sck1                 spiclk\n"
"  sdi1                 miso\n"
"  sdo1                 mosi\n"
"  int1                 irq*\n"
#else
#error
#endif
"  pins qspi_cs*        ce*\n"
"  pins zigflea_rst*    rst*\n"
"  pins zigflea_rxtxen  rxtxen\n"
"  vss                  vss\n"
"  vdd                  vdd\n"
#else
"connec\264<nodeid>\314-- connec\264t\257MC\225<nodeid> vi\241zigflea\n"
"<Ctrl-D>\324-- disconnec\264fro\255zigflea\n"
"\n"
"remot\245nod\245variables:\n"
"\300di\255<varremote>[[n]\235a\263remot\245o\256nodei\244<nodeid>\n"
"\n"
"zigfle\241cable:\n"
"\300MCU\320MC1320X\n"
"\300-------------\306-----------\n"
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
"\300qspi_clk\313spiclk\n"
"\300qspi_din\313miso\n"
"\300qspi_dout\312mosi\n"
#if MCF52259
"  irq1*                irq*\n"
#else
"\300irq4*\316irq*\n"
#endif
#elif MCF51JM128
"  spsck1 (pte6)        spiclk\n"
"  miso1 (pte4)         miso\n"
"  mosi1 (pte5)         mosi\n"
"  irq*                 irq*\n"
#elif MCF51CN128
"  spsck1 (ptb5)        spiclk\n"
"  miso1 (ptb4)         miso\n"
"  mosi1 (ptb3)         mosi\n"
"  irq* (ptc4)          irq*\n"
#elif MCF51QE128 || MC9S08QE128
"  spsck1 (ptb2)        spiclk\n"
"  miso1 (ptb4)         miso\n"
"  mosi1 (ptb3)         mosi\n"
"  irq*                 irq*\n"
#elif MC9S12DT256 || MC9S12DP512
"  sck0 (pm5)           spiclk\n"
"  miso0 (pm2)          miso\n"
"  mosi0 (pm4)          mosi\n"
"  irq* (pe1)           irq*\n"
#elif PIC32
// REVISIT -- implement zigflea on MRF24J40
"  sck1                 spiclk\n"
"  sdi1                 miso\n"
"  sdo1                 mosi\n"
"  int1                 irq*\n"
#else
#error
#endif
"\300pin\263qspi_cs*\306ce*\n"
"\300pin\263zigflea_rst*\302rst*\n"
"\300pin\263zigflea_rxtxe\016rxtxen\n"
"\300vss\320vss\n"
"\300vdd\320vdd\n"
#endif
;
#endif

void
basic0_help(IN char *text_in)
{
    char *p;
    char *text;
    byte line[BASIC_OUTPUT_LINE_SIZE];
    char line2[BASIC_OUTPUT_LINE_SIZE];

    text = text_in;

    // while there is more help to print...
    while (*text) {
        // print the next line of help
        p = strchr(text, '\n');
        assert(p);
        assert(p-text < BASIC_OUTPUT_LINE_SIZE);
        memcpy(line, text, p-text);
#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#pragma STRING_SEG DEFAULT
#endif
        line[p-text] = '\0';
        text_expand(line, line2);
        printf("%s\n", line2);
        text = p+1;
    }

#if ! STICK_GUEST
    if (text_in == help_about) {
        printf("(checksum 0x%x)\n", flash_checksum);
#if 0
        printf("MCF_RCM_RSR = 0x%x\n", MCF_RCM_RSR);
#endif
    }
#endif
}

#if HELP_COMPRESS
int total_in;
int total_out;

void
generate_help_code(char *name, char *string)
{
    int i;
    int c;
    static byte buffer1[65536];
    static char buffer2[65536];

    extern char * const help_commands;

    memset(buffer1, 0, sizeof(buffer1));
    memset(buffer2, 0, sizeof(buffer2));

    text_compress(string, buffer1);
    text_expand(buffer1, buffer2);

    total_in += strlen(string);
    total_out += strlen(buffer1);

    for (i = 0; string[i] || buffer2[i]; i++) {
        assert(string[i] == buffer2[i]);
    }

    printf("static char *const %s =\n", name);
    printf("\"");
    for (i = 0; (c = buffer1[i]); i++) {
        if (isprint(c)) {
            printf("%c", c);
        } else if (c == '"') {
            printf("\\\"");
        } else if (c == '\\') {
            printf("\\\\");
        } else if (c == '\n') {
            printf("\\n");
        } else {
            printf("\\%03o", c);
        }
        if (c == '\n') {
            if (buffer1[i+1]) {
                printf("\"\n");
                printf("\"");
            } else {
                printf("\"\n");
                printf(";\n");
            }
        }
    }
    printf("\n");
}

#define generate_help_macro(s)  generate_help_code(#s, s)

int
generate_help()
{
    generate_help_macro(help_general);
    generate_help_macro(help_about);
    generate_help_macro(help_commands);
    generate_help_macro(help_modes);
    generate_help_macro(help_statements);
    generate_help_macro(help_devices);
    generate_help_macro(help_blocks);
    generate_help_macro(help_expressions);
    generate_help_macro(help_strings);
    generate_help_macro(help_variables);
    generate_help_macro(help_pins);
#if MCF52259
    generate_help_macro(help_board);
#endif
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    generate_help_macro(help_clone);
#endif
    generate_help_macro(help_zigflea);

    printf("total_in = %d; total_out = %d; comp = %d%%\n", total_in, total_out, 100*total_out/total_in);
    delay(20000);

    return 0;
}
#endif

#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#pragma CODE_SEG DEFAULT
#endif


// *** basic0_run() ***************************************************

// this function implements the stickos command interpreter.
void
basic0_run(char *text_in)
{
    int i;
    int d;
    int h;
    int m;
    int t;
    int cmd;
    int len;
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    bool boo;
#endif
#if SODEBUG || MCF52259 || PIC32
    bool reset;
    bool init;
#endif
    const char *p;
    char *text;
    int number1;
    int number2;
#if MCF52233
    int a0, a1, a2, a3;
#endif

    text = text_in;

    parse_trim(&text);

    // parse private commands
    for (cmd = 0; cmd < LENGTHOF(commands); cmd++) {
        len = strlen(commands[cmd]);
        if (! strncmp(text, commands[cmd], len)) {
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
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
        case command_clone:
            boo = false;
            if (parse_word(&text, "run")) {
                boo = true;
            }
            if (*text) {
                goto XXX_ERROR_XXX;
            }

            basic0_help(help_about);
            printf("cloning...\n");
            clone(boo);
            break;
#endif

        case command_connect:
            if (! zb_present) {
                printf("zigflea not present\n");
#if ! STICK_GUEST
            } else if (zb_nodeid == -1) {
                printf("zigflea nodeid not set\n");
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
                    basic0_poll();
                }
#endif

                printf("...disconnected\n");
            }
            break;

        case command_help:
#if ! SODEBUG || STICK_GUEST
            if (! *text) {
                p = help_general;
            } else
#endif
            if (parse_word(&text, "about")) {
                p = help_about;
#if ! SODEBUG || STICK_GUEST
            } else if (parse_word(&text, "commands")) {
                p = help_commands;
            } else if (parse_word(&text, "modes")) {
                p = help_modes;
            } else if (parse_word(&text, "statements")) {
                p = help_statements;
            } else if (parse_word(&text, "devices")) {
                p = help_devices;
            } else if (parse_word(&text, "blocks")) {
                p = help_blocks;
            } else if (parse_word(&text, "expressions")) {
                p = help_expressions;
            } else if (parse_word(&text, "strings")) {
                p = help_strings;
            } else if (parse_word(&text, "variables")) {
                p = help_variables;
            } else if (parse_word(&text, "pins")) {
                p = help_pins;
#if MCF52259
            } else if (parse_word(&text, "board")) {
                p = help_board;
#endif
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
            } else if (parse_word(&text, "clone")) {
                p = help_clone;
#endif
            } else if (parse_word(&text, "zigflea")) {
                p = help_zigflea;
#endif
            } else {
                goto XXX_ERROR_XXX;
            }
            if (*text) {
                goto XXX_ERROR_XXX;
            }
            basic0_help((char *)p);
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
                    printf("%u.%u.%u.%u\n", (int)(i>>24)&0xff, (int)(i>>16)&0xff, (int)(i>>8)&0xff, (int)i&0xff);
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
        
        case command_reset:
            if (*text) {
                goto XXX_ERROR_XXX;
            }

#if ! STICK_GUEST
            (void)splx(7);
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
            MCF_RCM_RCR = MCF_RCM_RCR_SOFTRST;
#elif MCF51JM128 || MCF51CN128 || MCF51QE128
            asm {
                move.l  #0x00000000,d0
                movec   d0,CPUCR
                trap    #0
            };
#elif MC9S08QE128
            asm(stop);
#elif MC9S12DT256 || MC9S12DP512
            COPCTL = 0x01; // cop activated with shortest timeout 
            ARMCOP = 0x47; // here we will get kicked by the dog
#elif PIC32
            SYSKEY = 0;
            SYSKEY = 0xAA996655;
            SYSKEY = 0x556699AA;
            RSWRSTSET = _RSWRST_SWRST_MASK;
            while (RSWRST, true) {
                // NULL
            }
#else
#error
#endif
            ASSERT(0);
#endif
            break;

        case command_upgrade:  // upgrade StickOS S19 file
        case command_download:  // relay S19 file to QSPI to EzPort
            number1 = 0;
            if (cmd == command_download) {
                // get fsys_frequency
                (void)basic_const(&text, &number1);
                if (! number1 || *text) {
                    goto XXX_ERROR_XXX;
                }
                // validate fsys_frequency
                if (number1 < 1000000 || number1 > 80000000) {
                    goto XXX_ERROR_XXX;
                }
            }
            flash_upgrade(number1);
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
            
#if SODEBUG || MCF52259 || PIC32
        case command_zigflea:
            reset = parse_word(&text, "reset");
            init = parse_word(&text, "init");
            if (*text) {
                goto XXX_ERROR_XXX;
            }
#if ! STICK_GUEST
            zb_diag(reset, init);
#endif
            break;
#endif

        case LENGTHOF(commands):
            // this is not a private command; process a public command...
            basic_run(text_in);
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
basic0_poll(void)
{
    terminal_poll();
    var_poll();

#if STICKOSPLUS
    // flush dirty lbas to the filesystem
    flush_log_file();
#endif
}
#endif

