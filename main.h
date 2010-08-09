// *** main.h *********************************************************

#ifndef MAIN_INCLUDED
#define SLEEP_DELAY  60

#include "config.h"

#if ! _WIN32
#define NULL ((void*)0)
#endif

#if ! STICK_GUEST

#if EXTRACT && ! MCF51JM128
#include "extract.h"
#else
#if MCF52233
#include "MCF52235.h"
#elif MCF52221
#include "MCF52221.h"
#elif MCF52259
#include "MCF52259.h"
#elif MCF5211
#include "MCF5211.h"
#define MCF_INTC0_ICR01  MCF_INTC_ICR01
#define MCF_INTC0_ICR13  MCF_INTC_ICR13
#define MCF_INTC0_ICR55  MCF_INTC_ICR55
#define MCF_INTC0_IMRH  MCF_INTC_IMRH
#define MCF_INTC0_IMRL  MCF_INTC_IMRL
#elif MCF51JM128
#include "MCF51JM128.h"
#include "compat.h"
#elif MCF51QE128
#include "MCF51QE128.h"
#include "compat.h"
#elif MC9S08QE128
#include "MC9S08QE128.h"
#include "compat.h"
#elif MC9S12DT256
#include "MC9S12DT256.h"
#include "compat.h"
#elif PIC32
#include <plib.h>
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
#else
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
#endif
#endif

#if MC9S08QE128 || MC9S12DT256
typedef long intptr;
typedef unsigned long uintptr;
typedef uint16 size_t;
#else
typedef int intptr;
typedef unsigned int uintptr;
typedef uint32 size_t;
#endif

#if GCC
#define INTERRUPT __attribute__((interrupt))
#define far
#define __IPSBAR ((volatile uint8 *)0x40000000)
#define RAMBAR_ADDRESS ((uintptr)__RAMBAR)
#define DECLSPEC_PAGE0_CODE __attribute__((section(".page0_code")))
#define DECLSPEC_PAGE0_DATA __attribute__((section(".page0_data")))
#define DECLSPEC_PAGE1 __attribute__((section(".page1")))
#define FLASH_UPGRADE_RAM_BEGIN __attribute__((section(".text_flash_upgrade_ram_begin")))
#define FLASH_UPGRADE_RAM_END __attribute__((section(".text_flash_upgrade_ram_end")))

#define asm_halt() __asm__("halt")
#define asm_stop_2000(x) __asm__("stop #0x2000")
#define asm_stop_2700(x) __asm__("stop #0x2700")

#elif MCF52221 || MCF52233 || MCF52259 || MCF5211 || MCF51JM128 || MCF51QE128
#if MCF51JM128 || MCF51QE128
#define INTERRUPT  interrupt
#else
#define INTERRUPT  __declspec(interrupt)
#endif
#define DECLSPEC_PAGE1 __declspec(page1)
#define asm_halt() asm { halt }
#define asm_stop_2000() asm { stop #0x2000 }
#define asm_stop_2700() asm { stop #0x2700 }
#elif MC9S08QE128
#define INTERRUPT
#define asm_halt()  asm("bgnd");
#define DECLSPEC_PAGE1
#elif MC9S12DT256
#define INTERRUPT
#define asm_halt()  asm("bgnd");
#define DECLSPEC_PAGE1
#elif PIC32
#define INTERRUPT
#define asm_halt()  asm("SDBBP");
#define DECLSPEC_PAGE1
#else
#error
#endif

#else  // STICK_GUEST

#define INTERRUPT

#if WIN32
// _DEBUG/NDEBUG win
#if _DEBUG
#define DEBUG  1
#else
#if NDEBUG
#define DEBUG  0
#else
#error _DEBUG/NDEBUG?
#endif
#endif  // _DEBUG
#else  // WIN32
// DEBUG wins
#if DEBUG
#define _DEBUG
#undef NDEBUG
#else
#define NDEBUG
#undef _DEBUG
#endif  // DEBUG
#endif  // WIN32

#if GCC
#include <inttypes.h>
#include <bits/wordsize.h>
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef uintptr_t uintptr;
typedef intptr_t intptr;
#if __WORDSIZE == 64
typedef uint64_t size_t;
#else
typedef uint32 size_t;
#endif
#else // ! GCC
#define _WIN32_WINNT 0x0500
#include <windows.h>
extern int isatty(int);
#if ! NO_UINT_TYPEDEFS
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef  int int32;
typedef unsigned int uint32;
typedef int intptr;
typedef unsigned int uintptr;
#endif
#define NO_UINT_TYPEDEFS  1
#endif // ! GCC
#include <assert.h>
#define ASSERT(x)  assert(x)
#define assert_ram(x)  assert(x)

extern void write(int, const void *, size_t);
extern char *gets(char *);

#define inline
#undef MAX_PATH
#define W32BYTESWAP(x) ((x)&0xff)<<24|((x)&0xff00)<<8|((x)&0xff0000)>>8|((x)&0xff000000)>>24;

#endif  // ! STICK_GUEST

typedef unsigned char bool;
#if (! MCF51JM128 && ! MCF51QE128) || GCC
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
#define OFFSETOF(t, f)  ((int)(intptr)(&((t *)0)->f))
#define IS_POWER_OF_2(x) ((((x)-1)&(x))==0)

#include <stdarg.h>

#include "clone.h"
#include "flash.h"
#include "pin.h"
#include "printf.h"
#include "qspi.h"
#include "zigbee.h"
#include "terminal.h"
#include "timer.h"
#include "util.h"
#include "adc.h"
#include "led.h"

#include "startup.h"

#if ! STICK_GUEST

#include "init.h"
#include "vectors.h"

#include "serial.h"
#include "sleep.h"

#if MCF52221 || MCF52259 || MCF51JM128 || PIC32
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

#endif  // ! STICK_GUEST

#if PICTOCRYPT && ! MCF51JM128
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

#if ! STICK_GUEST

#if DEBUG
#define assert(x)  if (! (x)) { led_line(__LINE__); }
#else
#define assert(x)
#endif
#if DEBUG || PIC32
#define assert_ram(x)  if (! (x)) { asm_halt(); }
#else
#define assert_ram(x)
#endif
#define ASSERT(x)  if (! (x)) { led_line(__LINE__); }
#define ASSERT_RAM(x)  if (! (x)) { asm_halt(); }

#else  // STICK_GUEST

#if PICTOCRYPT
extern byte big_buffer[8192];
#else
extern byte big_buffer[1024];
#endif

#endif  // ! STICK_GUEST

// If the platform doesn't provide FLASH_UPGRADE_RAM_{BEGIN,END} then provide default no-ops.
#ifndef FLASH_UPGRADE_RAM_BEGIN
#define FLASH_UPGRADE_RAM_BEGIN
#endif
#ifndef FLASH_UPGRADE_RAM_END
#define FLASH_UPGRADE_RAM_END
#endif

#define BASIC_LINE_SIZE  79

#define MAIN_INCLUDED  1
#endif  // MAIN_INCLUDED

