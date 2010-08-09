#define VERSION  "1.80"

#if PIC32
// REVISIT -- we take debug info from the pic32 environment for now
#ifdef __DEBUG
#define DEBUG  1
#else
#define DEBUG  0
#endif
#elif ! FLASHER
#endif

// uncomment for HID bootloader and cp procdefs.ld.ubw32 procdefs.ld
//#define HIDBL  1

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
#define DEMO  1  // 1 enables DEMO board USB power
#endif

#if PIC32
#define STARTER  1  // 1 enables STARTER board USB power
#endif

#if PICTOCRYPT
#define SECURE  0  // 1 sets flash security
#else
#define SECURE  0  // 1 sets flash security
#endif

#ifndef EXTRACT
#define EXTRACT  0  // 1 uses extracted headers rather than Freescale
#endif

// Enable in-memory trace buffer for debugging with the trace() macro.
#define IN_MEMORY_TRACE 0

