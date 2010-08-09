// *** main.h *********************************************************

#ifndef MAIN_INCLUDED
#define SLEEP_DELAY  60

#include "config.h"

#if ! _WIN32

#if EXTRACT && ! MCF51JM128
#include "extract.h"
#else
#if MCF52233
#include "MCF52235.h"
#elif MCF52221
#include "MCF52221.h"
#elif MCF51JM128
#include "MCF51JM128.h"
#include "compat.h"
#else
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
#endif
#endif

#if ! MCF51JM128
#define INTERRUPT  __declspec(interrupt)
#else
#define INTERRUPT  interrupt
#endif

#else  // _WIN32

#include <windows.h>
#include <assert.h>
extern int write(int, void *, int);
extern char *gets(char *);

#if ! NO_UINT_TYPEDEFS
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
#define NO_UINT_TYPEDEFS  1
#endif

#define inline
#undef MAX_PATH
#define W32BYTESWAP(x) ((x)&0xff)<<24|((x)&0xff00)<<8|((x)&0xff0000)>>8|((x)&0xff000000)>>24;
#define FLASH_PAGE_SIZE  2048

#endif  // ! _WIN32

typedef unsigned char bool;
#if ! MCF51JM128
typedef unsigned char byte;
#endif

enum {
    false,
    true
};

#define IN
#define OUT
#define OPTIONAL
#define VARIABLE  1
#define MIN(a, b)  ((a) < (b) ? (a) : (b))
#define MAX(a, b)  ((a) > (b) ? (a) : (b))
#define ROUNDUP(n, s)  (((n)+(s)-1)&~((s)-1))  // N.B. s must be power of 2!
#define ROUNDDOWN(n, s)  ((n)&~((s)-1))  // N.B. s must be power of 2!
#define LENGTHOF(a)  (sizeof(a)/sizeof(a[0]))
#define OFFSETOF(t, f)  ((int)(&((t *)0)->f))

#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#include "clone.h"
#include "flash.h"
#include "pin.h"
#include "printf.h"
#include "qspi.h"
#include "zigbee.h"
#include "terminal.h"
#include "util.h"

#if ! _WIN32

#include "startup.h"
#include "init.h"
#include "vectors.h"

#include "adc.h"
#include "led.h"
#include "sleep.h"
#include "timer.h"

#if MCF52221 || MCF51JM128
#include "ftdi.h"
#include "scsi.h"
#include "usb.h"
#endif

#if MCF52233
#undef MCF_EPORT_EPPDR
#undef MCF_EPORT_EPPAR
#undef MCF_EPORT_EPFR
#undef MCF_EPORT_EPIER
#define MCF_EPORT_EPPDR  MCF_EPORT0_EPPDR
#define MCF_EPORT_EPPAR  MCF_EPORT0_EPPAR
#define MCF_EPORT_EPFR  MCF_EPORT0_EPFR
#define MCF_EPORT_EPIER  MCF_EPORT0_EPIER
#endif

#if BADGE_BOARD
#include "jm.h"
#endif

#endif  // ! _WIN32

#if PICTOCRYPT
#include "pict-o-crypt.h"
#include "admin.h"
#include "aes.h"
#include "block.h"
#include "fat32.h"
#include "params.h"
#include "walkfs.h"

#elif STICKOS
#include "cpustick.h"
#include "basic.h"
#include "code.h"
#include "parse.h"
#include "run.h"
#include "vars.h"
#include "basic2.h"
#include "parse2.h"
#include "run2.h"

#elif SKELETON
#include "skeleton.h"

#elif FLASHER
#include "flasher.h"

#endif  // PICTOCRYPT

#if MCF52233
extern void os_yield(void);
#else
#define os_yield()  // NULL
#endif

#if ! _WIN32

#if DEBUG
#define assert(x)  if (! (x)) { led_line(__LINE__); }
#else
#define assert(x)
#endif
#define ASSERT(x)  if (! (x)) { led_line(__LINE__); }

#else  // _WIN32

#define ticks (int)(GetTickCount())
#define seconds  (ticks/1000)

#if PICTOCRYPT
extern byte big_buffer[8192];
#else
extern byte big_buffer[1024];
#endif

#endif  // ! _WIN32

#define BASIC_LINE_SIZE  79

#define MAIN_INCLUDED  1
#endif  // MAIN_INCLUDED

