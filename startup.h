#ifndef STARTUP_INCLUDED
#if MCF52233
#define FLASH_START  0
#define FLASH_BYTES  (256*1024)
#define FLASH_PAGE_SIZE  2048
#elif MCF52221
#define FLASH_START  0
#define FLASH_BYTES  (128*1024)
#define FLASH_PAGE_SIZE  2048
#elif MCF51JM128
#define FLASH_START  0
#define FLASH_BYTES  (128*1024)
#define FLASH_PAGE_SIZE  1024
#elif PIC32
#define FLASH_START  0x9D000000
#define FLASH_BYTES  (256*1024)  // the smallest part we support
#define FLASH_PAGE_SIZE  4096

#define FLASH2_START  0x9FC00000  // boot flash, for upgrade
#define FLASH2_BYTES  (12*1024)
#else
#error
#endif

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

