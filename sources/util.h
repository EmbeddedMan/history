// *** util.h *********************************************************

#define SPL_PIT0  6  // pit0 isr runs at interrupt level 6
#define SPL_USB  2  // usb isr runs at interrupt level 2
#define SPL_IRQ1  1  // irq1 isr runs at interrupt level 1

// N.B. the usb controller bdt data structures and the usb protocol
// layers are defined to be little endian and the coldfire core is
// big endian, so we have to byteswap.

#define BYTESWAP(x)  byteswap(x, sizeof(x))

uint32
byteswap(uint32 x, uint32 size);

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
memcpy(void *d,  const void *s, uint32 n);

void *
memmove(void *d,  const void *s, uint32 n);

void *
memset(void *p,  int d, uint32 n);

int
memcmp(const void *d,  const void *s, uint32 n);
#endif

