// *** main.h *********************************************************

#if ! _WIN32

#define DEMO  1  // 1 enables DEMO board USB power and dtin3 LED
#define DEBUG  1  // 0 is fastest but riskiest (removes all assert()s)
#define RELEASE  1  // 1 disables expensive check functions
#define SECURE  0  // 1 sets flash security
#define EXTRACT  0  // 1 uses extracted headers rather than Freescale

#if EXTRACT
#include "extract.h"
#else
#if MCF52233
#include "MCF52235.h"
#elif MCF52221
#include "MCF52221.h"
#else
#error
#endif
#endif

#else  // _WIN32

#include <windows.h>
#include <assert.h>
extern int write(int, void *, int);
extern char *gets(char *);

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

#define inline
#undef MAX_PATH
#define W32BYTESWAP(x) ((x)&0xff)<<24|((x)&0xff00)<<8|((x)&0xff0000)>>8|((x)&0xff000000)>>24;

#endif  // ! _WIN32

typedef unsigned char bool;
typedef unsigned char byte;

#define IN
#define OUT
#define VARIABLE  1
#define MIN(a, b)  ((a) < (b) ? (a) : (b))
#define MAX(a, b)  ((a) > (b) ? (a) : (b))
#define ROUNDUP(n, s)  (((n)+(s)-1)&~((s)-1))  // N.B. s must be power of 2!
#define ROUNDDOWN(n, s)  ((n)&~((s)-1))  // N.B. s must be power of 2!
#define LENGTHOF(a)  (sizeof(a)/sizeof(a[0]))
#define OFFSETOF(t, f)  ((int)(&((t *)0)->f))

enum {
    false,
    true
};

#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#include "clone.h"
#include "flash.h"
#include "printf.h"
#include "terminal.h"
#include "util.h"

#if ! _WIN32

#include "startup.h"

#include "adc.h"
#include "led.h"
#include "sleep.h"
#include "timer.h"

#if MCF52221
#include "ftdi.h"
#include "scsi.h"
#include "usb.h"
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
#include "basic.h"
#include "code.h"
#include "cpustick.h"
#include "parse.h"
#include "run.h"
#include "vars.h"

#elif SKELETON
#include "skeleton.h"

#elif FLASHER
#include "flasher.h"

#endif  // PICTOCRYPT

#if MCF52221
#define os_yield()  // NULL
#elif MCF52233
extern void tk_yield(void);
#define os_yield()  tk_yield()
#else
#error
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

#endif  // ! _WIN32

#define BASIC_LINE_SIZE  79

