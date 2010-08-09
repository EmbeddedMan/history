// *** main.h *********************************************************

#if ! _WIN32

#include "MCF52221.h"

#else

#include <windows.h>
#include <assert.h>
extern int write(int, void *, int);
extern char *gets(char *);

typedef unsigned int uint32;

#define inline

#endif

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

#include "basic.h"
#include "printf.h"
#include <string.h>
#include <ctype.h>

#include "flash.h"
#include "util.h"
#include "ftdi.h"
#include "clone.h"
#include "cpustick.h"

#if ! _WIN32

#include "adc.h"
#include "led.h"
#include "sleep.h"
#include "timer.h"
#include "uart.h"
#include "usb.h"

extern __declspec(system) uint32 __VECTOR_RAM[];

#ifdef INTERNAL_FLASH
#else  // INTERNAL_RAM
#endif

#define DEMO  1
#define RELEASE  1

#if RELEASE
#define assert(x)  // for release bits
#else
#if DEMO
#define assert(x)  if (! (x)) { asm { halt } }
#else
#define assert(x)  if (! (x)) { led_line(__LINE__); }
#endif
#endif

#else

#define ticks (int)(GetTickCount())
#define seconds  (ticks/1000)

#endif

