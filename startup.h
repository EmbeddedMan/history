#ifndef STARTUP_INCLUDED
#if MCF52233
#define FLASH_START  0
#define FLASH_BYTES  (256*1024)
#define FLASH_PAGE_SIZE  2048
#define BASIC_RAM_PAGE_SIZE  2048
#define BASIC_VARS  100
#define BASIC_STORES  4
#elif MCF52221
#define FLASH_START  0
#define FLASH_BYTES  (128*1024)
#define FLASH_PAGE_SIZE  2048
#define BASIC_RAM_PAGE_SIZE  2048
#define BASIC_VARS  100
#define BASIC_STORES  2
#elif MCF52259
#define FLASH_START  0
#define FLASH_BYTES  (256*1024)  // the smallest part we support
#define FLASH_PAGE_SIZE  4096
#define BASIC_RAM_PAGE_SIZE  4096
#define BASIC_VARS  100
#define BASIC_STORES  4
#elif MCF5211
#define FLASH_START  0
#define FLASH_BYTES  (128*1024)
#define FLASH_PAGE_SIZE  2048
#define BASIC_RAM_PAGE_SIZE  2048
#define BASIC_VARS  100
#define BASIC_STORES  2
#elif MCF51JM128
#define FLASH_START  0
#define FLASH_BYTES  (128*1024)
#define FLASH_PAGE_SIZE  1024
#define BASIC_RAM_PAGE_SIZE  1024
#define BASIC_VARS  100
#define BASIC_STORES  2
#elif MCF51QE128
#define FLASH_START  0
#define FLASH_BYTES  (128*1024)
#define FLASH_PAGE_SIZE  1024
#define BASIC_RAM_PAGE_SIZE  512
#define BASIC_VARS  40
#define BASIC_STORES  2
#elif MC9S08QE128
#define FLASH_START  0L
#define FLASH_BYTES  (64*1024L)
#define FLASH_PAGE_SIZE  512
#define BASIC_RAM_PAGE_SIZE  512
#define BASIC_VARS  40
#define BASIC_STORES  2

#define FLASH2_PPAGE  0x06
#define FLASH2_START  0x8000  // BASIC stores, for code access
#define FLASH2_BYTES  (16*1024L)
#elif MC9S12DT256
#define FLASH_START  0L
#define FLASH_BYTES  (64*1024L)
#define FLASH_PAGE_SIZE  512
#define BASIC_RAM_PAGE_SIZE  512
#define BASIC_VARS  40
#define BASIC_STORES  2

#define FLASH2_PPAGE  0x3c  // N.B. must be in flash block 0!
#define FLASH2_START  0x8000  // BASIC stores, for code access
#define FLASH2_BYTES  (16*1024L)
#elif PIC32
#define FLASH_START  0x9D000000
#define FLASH_BYTES  (256*1024)  // the smallest part we support
#define FLASH_PAGE_SIZE  4096
#define BASIC_RAM_PAGE_SIZE  4096
#define BASIC_VARS  100
#define BASIC_STORES  4

#define FLASH2_START  0x9FC00000  // boot flash, for flash upgrade
#define FLASH2_BYTES  (12*1024)
#else
#error
#endif

#if MC9S08QE128 || MC9S12DT256
#define BASIC_LARGE_PAGE_SIZE  (7*1024)
#else
#if DEBUG && ! STICK_GUEST
#define BASIC_LARGE_PAGE_SIZE  (8*1024)
#else
#define BASIC_LARGE_PAGE_SIZE  (12*1024)
#endif
#endif

#define BASIC_SMALL_PAGE_SIZE  FLASH_PAGE_SIZE

#define INCOMPAT  0  // for testing
#define COMPAT  0  // for testing

extern byte *end_of_static;

extern uint32 cpu_frequency;
extern uint32 bus_frequency;
extern uint32 oscillator_frequency;

extern bool debugger_attached;
#if PICTOCRYPT
extern byte big_buffer[8192];
#else
extern byte big_buffer[1024];
#endif

typedef void (*flash_upgrade_ram_begin_f)(bool);

void
#if PIC32
__longramfunc__
__attribute__((nomips16))
#endif
flash_upgrade_ram_begin(bool compatible);

void
flash_upgrade_ram_end(void);

#define STARTUP_INCLUDED  1
#endif  // STARTUP_INCLUDED

