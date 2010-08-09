// *** basic2.c *******************************************************
// this file implements private extensions to the stickos command
// interpreter.

#include "main.h"

#define SHRINK  0

enum cmdcode {
    command_demo,
    command_help,
#if MCF52233
    command_ipaddress,  // [dhcp|<ipaddress>]
#endif
    command_dummy
};

static
const char * const commands[] = {
    "demo",
    "help",
#if MCF52233
    "ipaddress",
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
#elif MCF52259
"Welcome to StickOS for Freescale MCF52252 v" VERSION "!\n"
#elif MCF5211
"Welcome to StickOS for Freescale MCF5211 v" VERSION "!\n"
#elif MCF51JM128
"Welcome to StickOS for Freescale MCF51JM128 v" VERSION "!\n"
#elif MCF51QE128
"Welcome to StickOS for Freescale MCF51QE128 v" VERSION "!\n"
#elif MC9S08QE128
"Welcome to StickOS for Freescale MC9S08QE128 v" VERSION "!\n"
#elif MC9S12DT256
"Welcome to StickOS for Freescale MC9S12DT256 v" VERSION "!\n"
#elif MC9S12DP512
"Welcome to StickOS for Freescale MC9S12DP512 v" VERSION "!\n"
#elif PIC32
"Welcome to StickOS for Microchip PIC32MX v" VERSION "!\n"
#else
#error
#endif
"Copyright (c) 2008; all rights reserved.\n"
"http://www.cpustick.com\n"
"support@cpustick.com\n"
#if INCOMPAT
"incompatible\n"
#endif
#if COMPAT
"compatible\n"
#endif
;

#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#pragma CODE_SEG HELP_CODE
#pragma STRING_SEG HELP_STRING
// N.B. for MC9S08QE128/MC9S12DT256/MC9S12DP512 we put all the help strings
// and the function that accesses them in page 7 or 38; we have to put the
// functions that copy these to RAM in unbanked memory.
#endif

#if ! SHRINK
static char *const help_general =
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
#if MCF52221 || MCF52259
"  help board\n"
#endif
#if MCF52221 || MCF52233 || MCF52259
"  help clone\n"
#endif
"  help zigbee\n"
"\n"
"see also:\n"
"  http://www.cpustick.com\n"
;

static char * const help_commands =
"auto <line>                   -- automatically number program lines\n"
"clear [flash]                 -- clear ram [and flash] variables\n"
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
"clone [run]                   -- clone flash to slave MCU [and run]\n"
#endif
"cls                           -- clear terminal screen\n"
"cont [<line>]                 -- continue program from stop\n"
"delete [<line>][-][<line>]    -- delete program lines or <subname>\n"
"download <slave Hz>           -- download for relay to QSPI to EzPort\n"
"dir                           -- list saved programs\n"
"edit <line>                   -- edit program line\n"
"help [<topic>]                -- online help\n"
"list [<line>][-][<line>]      -- list program lines or <subname>\n"
"load <name>                   -- load saved program\n"
"memory                        -- print memory usage\n"
"new                           -- erase code ram and flash memories\n"
"profile [<line>][-][<line>]   -- like list, but display profile info\n"
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
;

static char * const help_modes =
"analog [<millivolts>]             -- set/display analog voltage scale\n"
"autorun [on|off]                  -- autorun (on reset) mode\n"
"echo [on|off]                     -- terminal echo mode\n"
"indent [on|off]                   -- listing indent mode\n"
#if MCF52233
"ipaddress [dhcp|<ipaddress>]      -- set/display ip address\n"
#endif
"nodeid [<nodeid>|none]            -- set/display zigbee nodeid\n"
"pins [<assign> [<pinname>|none]]  -- set/display StickOS pin assignments\n"
"prompt [on|off]                   -- terminal prompt mode\n"
"servo [<Hz>]                      -- set/display servo Hz (on reset)\n"
"sleep [on|off]                    -- debugger sleep mode\n"
"step [on|off]                     -- debugger single-step mode\n"
"trace [on|off]                    -- debugger trace mode\n"
#if MCF52259 || PIC32
"usbhost [on|off]                  -- set/display USB host mode (on reset)\n"
#endif
"\n"
"pin assignments:\n"
"  heartbeat     safemode*\n"
"  clone_rst*    zigbee_rst*   zigbee_attn*  zigbee_rxtxen\n"
"\n"
"for more information:\n"
"  help pins\n"
;

static char * const help_statements =
"<line> <statement>                     -- enter program line into code ram\n"
"\n"
"assert <expression>                    -- break if expression is false\n"
"data <n> [, ...]                       -- read-only data\n"
"dim <variable>[[n]] [as ...] [, ...]   -- dimension variables\n"
"end                                    -- end program\n"
#if BADGE_BOARD
"jm(clear|set) <r>, <c>                 -- clear/set row/column of LED matrix\n"
"jmscroll ...                           -- scroll printout to LED matrix\n"
#endif
"label <label>                          -- read/data label\n"
"let <variable> = <expression> [, ...]  -- assign variable\n"
"print (\"string\"|<expression>) [, ...]  -- print strings/expressions\n"
"qspi <variable> [, ...]                -- perform qspi I/O by reference\n"
"read <variable> [, ...]                -- read read-only data into variables\n"
"rem <remark>                           -- remark\n"
"restore [<label>]                      -- restore read-only data pointer\n"
"sleep <expression> (s|ms|us)           -- delay program execution\n"
"stop                                   -- insert breakpoint in code\n"
"\n"
"for more information:\n"
"  help blocks\n"
"  help devices\n"
"  help expressions\n"
"  help variables\n"
;

static char * const help_blocks =
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
;

static char * const help_devices =
"qspi:\n"
"  configure qspi for <n> csiv\n"
"\n"
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
"\n"
"watchpoints:\n"
"  on <expression> do <statement>             -- on expr execute statement\n"
"  off <expression>                           -- disable expr watchpoint\n"
"  mask <expression>                          -- mask/hold expr watchpoint\n"
"  unmask <expression>                        -- unmask expr watchpoint\n"
;

static char * const help_expressions =
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

static char *const help_variables =
"all variables must be dimensioned!\n"
"variables dimensioned in a sub are local to that sub\n"
"simple variables are passed to sub params by reference\n"
"array variable indices start at 0; v[0] is the same as v\n"
"\n"
"ram variables:\n"
"  dim <var>[[n]]\n"
"  dim <var>[[n]] as (byte|short)\n"
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
"  nodeid    msecs     seconds   ticks     ticks_per_msec      (read-only)\n"
"\n"
"for more information:\n"
"  help pins\n"
;

static char *const help_pins =
"pin names:\n"
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
"    0        1        2        3        4     5     6     7\n"
"  -------- -------- -------- --------- ----- ----- ----- ------+\n"
"  an0      an1      an2      an3       an4   an5   an6   an7   | PORT AN\n"
"  scl      sda                                                 | PORT AS\n"
#if MCF52233 || MCF52259 || MCF5211
"  gpt0     gpt1     gpt2     gpt3                              | PORT TA\n"
#endif
#if MCF52259
"           irq1*             irq3*           irq5*       irq7* | PORT NQ\n"
#else
"           irq1*                       irq4*             irq7* | PORT NQ\n"
#endif
#if MCF52233
"                             irq11*                            | PORT GP\n"
#endif
"  qspi_cs0 qspi_clk qspi_din qspi_dout                         | PORT QS\n"
"  dtin0    dtin1    dtin2    dtin3                             | PORT TC\n"
"  utxd0    urxd0    urts0*   ucts0*                            | PORT UA\n"
"  utxd1    urxd1    urts1*   ucts1*                            | PORT UB\n"
#if MCF52259
"  utxd2    urxd2    urts2*   ucts2*                            | PORT UC\n"
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
"  ra0     ra1     ra2     ra3     ra4     ra5     ra6     ra7     | PORT A\n"
"                                                  ra14    ra15    |      A+8\n"
"  an0     an1     an2     an3     an4     an5     an6     an7     | PORT B\n"
"  an8     an9     an10    an11    an12    an13    an14    an15    |      B+8\n"
"          rc1     rc2     rc3     rc4                             | PORT C\n"
"                                          rc13    rc14            |      C+8\n"
"  rd0     rd1     rd2     rd3     rd4     rd5     rd6     rd7     | PORT D\n"
"  rd8     rd9     rd10    rd11    rd12    rd13    rd14    rd15    |      D+8\n"
"  re0     re1     re2     re3     re4     re5     re6     re7     | PORT E\n"
"  re8     re9                                                     |      E+8\n"
"  rf0     rf1     rf2     rf3     rf4     rf5                     | PORT F\n"
"  rf8                             rf12    rf13                    |      F+8\n"
"  rg0     rg1                                     rg6     rg7     | PORT G\n"
"  rg8     rg9                     rg12    rg13    rg14            |      G+8\n"
"\n"
"all pins support general purpose digital input/output\n"
"an? = potential analog input pins (mV)\n"
"rd[0-4] = potential analog output (PWM) pins (mV)\n"
"rd[0-4] = potential servo output (PWM) pins (cms)\n"
"rd[0-4] = potential frequency output pins (Hz)\n"
"rf2 (u1), rf4 (u2) = potential uart input pins (received byte)\n"
"rf8 (u1), rf5 (u2) = potential uart output pins (transmit byte)\n"
#else
#error
#endif
;

#if MCF52221
static char *const help_board =
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
"9  qspi_cs0           d  d  d  d\n"
"10 rcon*/irq4*        t  t  t  t\n"
"                g  +  i  i  i  i  a  a  a  a  a  a  a  a\n"
"                n  3  n  n  n  n  n  n  n  n  n  n  n  n\n"
"                d  V  3  2  1  0  0  1  2  3  4  5  6  7\n"
"\n"
"                1  2  3  4  5  6  7  8  9  10 11 12 13 14\n"
;
#elif MCF52259
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

#if MCF52221 || MCF52233 || MCF52259
static char *const help_clone =
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
#endif

static char *const help_zigbee =
"connect <nodeid>              -- connect to MCU <nodeid> via zigbee\n"
"\n"
"remote node variables:\n"
"  dim <varremote>[[n]] as remote on nodeid <nodeid>\n"
"\n"
"zigbee cable:\n"
"  MCU                 MC1320X\n"
"  -------------       -----------\n"
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
"  qspi_clk            spiclk\n"
"  qspi_din            miso\n"
"  qspi_dout           mosi\n"
"  qspi_cs0            ce*\n"
#if MCF52259
"  irq1*               irq*\n"
#else
"  irq4*               irq*\n"
#endif
#elif MCF51JM128
"  spsck1 (pte6)       spiclk\n"
"  miso1 (pte4)        miso\n"
"  mosi1 (pte5)        mosi\n"
"  ss1* (pte7)         ce*\n"
"  irq*                irq*\n"
#elif MCF51QE128 || MC9S08QE128
"  spsck1 (ptb2)       spiclk\n"
"  miso1 (ptb4)        miso\n"
"  mosi1 (ptb3)        mosi\n"
"  ss1* (ptb5)         ce*\n"
"  irq*                irq*\n"
#elif MC9S12DT256 || MC9S12DP512
"  sck0 (pm5)          spiclk\n"
"  miso0 (pm2)         miso\n"
"  mosi0 (pm4)         mosi\n"
"  ss0* (pm3)          ce*\n"
"  irq* (pe1)          irq*\n"
#elif PIC32
// REVISIT -- implement zigbee on MRF24J40
"  sck1                spiclk\n"
"  sdi1                miso\n"
"  sdo1                mosi\n"
"  rg8                 ce*\n"
"  int1                irq*\n"
#else
#error
#endif
"  pins zigbee_rst*    rst*\n"
"  pins zigbee_rxtxen  rxtxen\n"
"  vss                 vss\n"
"  vdd                 vdd\n"
;
#endif

void
basic2_help(IN char *text_in)
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
#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#pragma STRING_SEG DEFAULT
#endif
        line[p-text] = '\0';
        printf("%s\n", line);
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

#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#pragma CODE_SEG DEFAULT
#endif


// *** demo ***********************************************************

#if ! SHRINK
static const char * const demo0[] = {
  "rem ### blinky ###",
  "dim i",
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
  "dim led as pin dtin2 for digital output",
#elif MCF51JM128
  "dim led as pin ptf1 for digital output inverted",
#elif MCF51QE128 || MC9S08QE128
  "dim led as pin ptc3 for digital output inverted",
#elif MC9S12DT256 || MC9S12DP512
  "dim led as pin pb6 for digital output inverted",
#elif PIC32
  "dim led as pin rd2 for digital output inverted",
#else
#error
#endif
  "while 1 do",
  "  for i = 1 to 16",
  "    let led = !led",
  "    sleep 50 ms",
  "  next",
  "  sleep 800 ms",
  "endwhile",
  "end"
};

static const char *const demo1[] = {
  "rem ### uart isr ###",
  "dim data",
  "data 1, 1, 2, 3, 5, 8, 13, 21, 0",
#if MCF52221 || MCF52233 || MCF52259 || MCF5211 || MC9S12DT256 || MC9S12DP512
  "configure uart 0 for 300 baud 8 data no parity loopback",
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
  "dim tx as pin utxd0 for uart output",
  "dim rx as pin urxd0 for uart input",
#elif MC9S12DT256 || MC9S12DP512
  "dim tx as pin ps1 for uart output",
  "dim rx as pin ps0 for uart input",
#else
#error
#endif
  "on uart 0 input do gosub receive",
  "on uart 0 output do gosub transmit",
#else
  "configure uart 2 for 300 baud 8 data no parity loopback",
#if MCF51JM128
  "dim tx as pin ptc3 for uart output",
  "dim rx as pin ptc5 for uart input",
#elif MCF51QE128 || MC9S08QE128
  "dim tx as pin ptc7 for uart output",
  "dim rx as pin ptc6 for uart input",
#elif PIC32
  "dim tx as pin rf5 for uart output",
  "dim rx as pin rf4 for uart input",
#else
#error
#endif
  "on uart 2 input do gosub receive",
  "on uart 2 output do gosub transmit",
#endif
  "sleep 1000 ms",
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
  "configure uart 1 for 9600 baud 7 data even parity loopback",
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
  "dim tx as pin utxd1 for uart output",
  "dim rx as pin urxd1 for uart input",
#elif MCF51JM128
  "dim tx as pin pte0 for uart output",
  "dim rx as pin pte1 for uart input",
#elif MCF51QE128 || MC9S08QE128
  "dim tx as pin ptb1 for uart output",
  "dim rx as pin ptb0 for uart input",
#elif MC9S12DT256 || MC9S12DP512
  "dim tx as pin ps3 for uart output",
  "dim rx as pin ps2 for uart input",
#elif PIC32
  "dim tx as pin rf8 for uart output",
  "dim rx as pin rf2 for uart input",
#else
#error
#endif
  "let tx = 3",
#if MCF51JM128 || MCF51QE128 || MC9S08QE128 || MC9S12DT256 || MC9S12DP512
  "while tx do",
  "endwhile",
  "print rx",
#endif
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
#if MCF52221 || MCF52233 || MCF52259 || MCF5211 || PIC32
  "dim thermocouple as pin an0 for analog input",
  "dim relay as pin an1 for digital output",
#elif MCF51JM128 || MCF51QE128 || MC9S08QE128
  "dim thermocouple as pin ptb0 for analog input",
  "dim relay as pin ptb1 for digital output",
#elif MC9S12DT256 || MC9S12DP512
  "dim thermocouple as pin pad00 for analog input",
  "dim relay as pin pa0 for digital output",
#else
#error
#endif
  "data 5124, 6, 7460, 9, 8940, 3, -1, -1",
  "configure timer 0 for 1000 ms",
  "on timer 0 do gosub adjust",
  "rem ---------------",
  "while target!=-1 do",
  "  sleep secs s",
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
#endif

// *** basic2_run() ***************************************************

// this function implements the stickos command interpreter.
bool
basic2_run(char *text_in)
{
#if ! SHRINK
    int i;
#endif
    int cmd;
    int len;
    char *text;
    int number1;
    int number2;
#if MCF52233
    bool boo;
#endif
#if MCF52233
    int a0, a1, a2, a3;
#endif

    text = text_in;

    parse_trim(&text);

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
        case command_demo:
#if ! SHRINK
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
#endif
            break;

        case command_help:
#if ! SHRINK
            if (! *text) {
                basic2_help(help_general);
            } else
#endif
            if (parse_word(&text, "about")) {
                basic2_help(help_about);
#if ! SHRINK
            } else if (parse_word(&text, "commands")) {
                basic2_help(help_commands);
            } else if (parse_word(&text, "modes")) {
                basic2_help(help_modes);
            } else if (parse_word(&text, "statements")) {
                basic2_help(help_statements);
            } else if (parse_word(&text, "devices")) {
                basic2_help(help_devices);
            } else if (parse_word(&text, "blocks")) {
                basic2_help(help_blocks);
            } else if (parse_word(&text, "expressions")) {
                basic2_help(help_expressions);
            } else if (parse_word(&text, "variables")) {
                basic2_help(help_variables);
            } else if (parse_word(&text, "pins")) {
                basic2_help(help_pins);
#if MCF52221 || MCF52259
            } else if (parse_word(&text, "board")) {
                basic2_help(help_board);
#endif
#if MCF52221 || MCF52233 || MCF52259
            } else if (parse_word(&text, "clone")) {
                basic2_help(help_clone);
#endif
            } else if (parse_word(&text, "zigbee")) {
                basic2_help(help_zigbee);
            } else {
                goto XXX_ERROR_XXX;
#endif
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
                    printf("%u.%u.%u.%u\n", (int)(i>>24)&0xff, (int)(i>>16)&0xff, (int)(i>>8)&0xff, (int)i&0xff);
                }
            }
            break;
#endif

        default:
            return false;
            break;
    }
    
    return true;

XXX_ERROR_XXX:
    terminal_command_error(text-text_in);
    return true;
}

