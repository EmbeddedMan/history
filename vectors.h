// *** vectors.h ******************************************************

#define VECTOR_NEW_FLASH_UPGRADE_RAM_BEGIN  (*(uint32 *)(FLASH_BYTES/2+0x800))
#define VECTOR_NEW_FLASH_UPGRADE_RAM_END    (*(uint32 *)(FLASH_BYTES/2+0x804))

#define VECTOR_OLD_INIT                     (X+0x808)
#define VECTOR_OLD_UNUSED                   (X+0x80c)

