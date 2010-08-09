// *** util.h *********************************************************

#if MCF52221 || MCF52233
#define SPL_PIT0  6  // pit0 isr runs at interrupt level 6
#define SPL_SERIAL 5 // uart0 isr runs at interrupt level 5
                     //   (should be higher than anything being debugged with serial tracing).
#define SPL_USB  4  // usb isr runs at interrupt level 4
#define SPL_IRQ4  4  // irq4 isr runs at interrupt level 4 (fixed, zigbee)
#define SPL_IRQ1  1  // irq1 isr runs at interrupt level 1 (fixed, sleep)
#elif MCF51JM128
#define SPL_PIT0  6  // pit0 isr runs at interrupt level 6
#define SPL_USB  6  // usb isr runs at interrupt level 6
#define SPL_IRQ4  4  // irq isr runs at interrupt level 4 (zigbee)
//#define SPL_IRQ1  1  // irq1 isr runs at interrupt level 1 (fixed, sleep)
#elif PIC32
#define SPL_PIT0  6  // pit0 isr runs at interrupt level 6
#define SPL_USB  6  // usb isr runs at interrupt level 6
#define SPL_IRQ4  4  // irq isr runs at interrupt level 4 (zigbee)
#else
#error
#endif

// REVISIT -- align all bytecode values and eliminate these (except for qspi)?
#if PIC32
void
write32(byte *addr, uint32 data);

uint32
read32(const byte *addr);

void
write16(byte *addr, uint16 data);

uint16
read16(const byte *addr);
#else
// N.B. believe it or not, #defining these makes a 10% difference in
//      ColdFire performance!
#define write32(a, d)  (*(uint32 *)(a) = (d))
#define read32(a)  (*(uint32 *)(a))
#define write16(a, d)  (*(uint16 *)(a) = (d))
#define read16(a)  (*(uint16 *)(a))
#endif

// N.B. the usb controller bdt data structures and the usb protocol
// layers are defined to be little endian and the coldfire core is
// big endian, so we have to byteswap.

// revisit -- rename to indicate this converts to little endian
#define BYTESWAP(x)  byteswap(x, sizeof(x))

// revisit -- rename to indicate this converts to little endian
uint32
byteswap(uint32 x, uint32 size);

// return the current interrupt mask level
int
gpl(void);

// set the current interrupt mask level and return the old one
int
splx(int level);

// delay for the specified number of milliseconds
void
delay(int ms);

int
gethex(char *p);

int
get2hex(char **p);

void
tailtrim(char *p);

void *
memcpy(void *d,  const void *s, size_t n);

void *
memmove(void *d,  const void *s, size_t n);

void *
memset(void *p,  int d, size_t n);

int
memcmp(const void *d,  const void *s, size_t n);

size_t
strlen(const char *s);

char *
strcat(char *dest, const char *src);

char *
strncat(char *dest, const char *src, size_t n);

char *
strcpy(char *dest, const char *src);

char *
strncpy(char *dest, const char *src, size_t n);

int
strcmp(const char *s1, const char *s2);

int
strncmp(const char *s1, const char *s2, size_t n);

char *
strchr(const char *s, int c);

int
isdigit(int c);

int
isspace(int c);

int
isupper(int c);

int
islower(int c);

int
isalpha(int c);

int
isalnum(int c);

int
isprint(int c);

uint16
get_sr(void);

void
set_sr(uint16 csr);
