#define FLASH_PAGE_SIZE  2048
#if MCF52233
#define FLASH_BYTES  (256*1024)
#elif MCF52221
#define FLASH_BYTES  (128*1024)
#else
#error
#endif

#define INCOMPAT  0  // for testing
#define COMPAT  0  // for testing

extern byte *end_of_static;
extern uint32 fsys_frequency;
extern uint32 oscillator_frequency;
extern bool debugger_attached;
#if PICTOCRYPT
extern byte big_buffer[8192];
#else
extern byte big_buffer[2048];
#endif

typedef void (*flash_upgrade_ram_begin_f)(bool);

void
flash_upgrade_ram_begin(bool compatible);

void
flash_upgrade_ram_end(void);

