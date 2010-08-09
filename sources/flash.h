// *** flash.h ********************************************************

#define FLASH_PAGE_SIZE  2048
#if MCF52233
#define FLASH_BYTES  (256*1024)
#elif MCF52221
#define FLASH_BYTES  (128*1024)
#else
#error
#endif

void
flash_erase_pages(uint32 *addr, uint32 npages);

void
flash_write_words(uint32 *addr, uint32 *data, uint32 nwords);

void
flash_upgrade(void);

void
flash_initialize(void);

