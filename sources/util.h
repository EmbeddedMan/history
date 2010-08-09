// *** util.h *********************************************************

#if ! MCF51JM128
#define SPL_PIT0  6  // pit0 isr runs at interrupt level 6
#define SPL_USB  4  // usb isr runs at interrupt level 4
#define SPL_IRQ4  4  // irq4 isr runs at interrupt level 4 (fixed, zigbee)
#define SPL_IRQ1  1  // irq1 isr runs at interrupt level 1 (fixed, sleep)
#else  // ! MCF51JM128
#define SPL_PIT0  6  // pit0 isr runs at interrupt level 6
#define SPL_USB  6  // usb isr runs at interrupt level 6
#define SPL_IRQ4  4  // irq isr runs at interrupt level 4 (zigbee)
//#define SPL_IRQ1  1  // irq1 isr runs at interrupt level 1 (fixed, sleep)
#endif

// N.B. the usb controller bdt data structures and the usb protocol
// layers are defined to be little endian and the coldfire core is
// big endian, so we have to byteswap.

#define BYTESWAP(x)  byteswap(x, sizeof(x))

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

#if ! _WIN32
void *
memcpy(void *d,  const void *s, unsigned long n);

void *
memmove(void *d,  const void *s, unsigned long n);

void *
memset(void *p,  int d, unsigned long n);

int
memcmp(const void *d,  const void *s, unsigned long n);
#endif

