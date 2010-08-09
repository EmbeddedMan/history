#define SPL_USB  1  // usb isr runs at interrupt level 1
#define SPL_PIT0  5  // pit0 isr runs at interrupt level 5
#define SPL_IRQ1  6  // irq1 isr runs at interrupt level 6

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
