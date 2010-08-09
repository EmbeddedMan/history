#define VERSION  "1.42b"

#if PIC32
// REVISIT -- we take debug info from the pic32 environment for now
#ifdef __DEBUG
#define DEBUG  1
#else
#define DEBUG  0
#endif
#endif

#if MCF52221 || MCF52233
#define DEMO  1  // 1 enables DEMO board USB power and dtin3 LED
#endif

#if PIC32
#define STARTER  1  // 1 enables STARTER board rd0 LED
#endif

#define FAST  1  // 1 disables expensive check functions
#if PICTOCRYPT
#define SECURE  1  // 1 sets flash security
#else
#define SECURE  0  // 1 sets flash security
#endif
#ifndef EXTRACT
#define EXTRACT  0  // 1 uses extracted headers rather than Freescale
#endif

#if ! STICK_GUEST
#define SHRINK  0  // turn on for building debug code
#endif

// Enable in-memory trace buffer for debugging with the trace() macro.
#define IN_MEMORY_TRACE 0

// Use UART0 as a terminal.  Warning: resources used by the serial
// terminal conflict with uart basic primitives.  This conflict could
// be resolved by ifdef-ing out those primitives from the basic
// interpreter.
#define SERIAL_TERMINAL 0

#if SERIAL_TERMINAL
// Enable UART0 serial driver.
#define SERIAL_DRIVER 1
#endif

// Send trace() message to serial driver in addition to in-memory trace buffer.
#define TRACE_TO_SERIAL 0

