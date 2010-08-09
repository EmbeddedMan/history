#include <stdarg.h>

// N.B. we add buffering to the default printf since we can only process
//      a few BDM accesses per second thru the "Trap #14" mechanism... :-(

#define assert(x)  if (! (x)) { asm { halt } }

#define isdigit(c)  ((c) >= '0' && (c) <= '9')

#define MAXDIGITS  32

static char digits[] = "0123456789abcdef";
static char zeros[] = "00000000000000000000000000000000";
static char spaces[] = "                                ";

static asm __declspec(register_abi)
unsigned char
TRKAccessFile(long command, unsigned long file_handle, unsigned long *length_ptr, char *buffer_ptr)
{
    move.l    D3,-(a7)
    andi.l    #0x000000ff,D0
    move.l    A1,D3
    movea.l   A0,A1
    move.l    (A1),D2
    trap      #14
    move.l    D1,(A1)
    move.l    (A7)+,D3
    rts
}

static
void
putchar(char ch)
{
    static unsigned long i;
    static char buffer[128];
    
    buffer[i++] = ch;
    assert(i <= sizeof(buffer));
    if (ch == '\n') {
        TRKAccessFile(0xD0, 0, &i, buffer);
        i = 0;
    }
}

static
int
putchars(char *s, int n)
{
    int i;
    
    i = 0;
    if (! n) {
        n--;
    }
    while (*s && n--) {
        putchar(*s++);
        i++;
    }
    return i;
}

static
int
convert(unsigned value, unsigned radix, char *buffer)
{
    int i;
    int n;
    unsigned scale;
    unsigned lastscale;
    int digit;
    
    assert(radix >= 2 && radix < sizeof(digits));
    
    // first find our scale
    lastscale = 1;
    scale = radix;
    for (n = 0; n < MAXDIGITS; n++) {
        if (value < scale || scale/radix != lastscale) {
            break;
        }
        lastscale = scale;  // in case we overflow
        scale *= radix;
    }
    scale = lastscale;
    
    // then work our digits
    i = 0;
    do {
        assert(scale);
        digit = value/scale;
        buffer[i++] = digits[digit];
        value -= digit*scale;
        scale /= radix;
    } while (n--);
    
    // nul terminate
    buffer[i] = '\0';
    return i;
}

int
printf(char *format, ...)
{
    int i;
    int j;
    int n;
    int x;
    char c;
    char *p;
    int zero;
    int width;
    va_list ap;
    char buffer[MAXDIGITS+1];

    i = 0;
    
    va_start(ap, format);
    x = splx(7);
    
    for (p = format; *p; p++) {
        c = *p;
        if (c == '%') {
            zero = 0;
            width = 0;
            c = *++p;
            if (isdigit(c)) {
                if (c == '0') {
                    zero = 1;
                    c = *++p;
                    assert(isdigit(c));
                }
                width = c - '0';
                c = *++p;
            }
            n = va_arg(ap, int);
            switch (c) {
                case 'b':
                    j = convert(n, 2, buffer);
                    break;
                case 'c':
                    buffer[0] = (char)n;
                    buffer[1] = '\0';
                    j = 1;
                    break;
                case 'd':
                    j = convert(n, 10, buffer);
                    break;
                case 'o':
                    j = convert(n, 8, buffer);
                    break;
                case 's':
                    j = putchars((char *)n, 0);
                    buffer[0] = '\0';
                    break;
                case 'x':
                    j = convert(n, 16, buffer);
                    break;
                default:
                    assert(0);
            }
            if (j < width) {
                if (zero) {
                    putchars(zeros, width-j);
                } else {
                    putchars(spaces, width-j);
                }
            }
            i += j;
            putchars(buffer, 0);
        } else {
            i++;
            putchar(c);
        }
    }

    splx(x);
    va_end(ap);

    return i;
}

// **************************************************************************
