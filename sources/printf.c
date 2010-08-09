#include "main.h"
#include <stdarg.h>

// *** printf ***************************************************************

// N.B. we add buffering to the default printf since we can only process
//      a few BDM accesses per second thru the "Trap #14" mechanism... :-(

#define isdigit(c)  ((c) >= '0' && (c) <= '9')

#define MAXDIGITS  32

static char digits[] = "0123456789abcdef";
static char zeros[] = "00000000000000000000000000000000";
static char spaces[] = "                                ";

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

    i = 0;

    if (radix == 10 && (int)value < 0) {
        buffer[i++] = '-';
        value = 0-value;
    }

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

static
int
vsnprintf(char *buffer, int length, char *format, va_list ap)
{
    int i;
    int j;
    int n;
    char c;
    char *p;
    bool nl;
    int zero;
    int width;
    char temp[1+MAXDIGITS+1];

    i = 0;
    
    nl = false;
    for (p = format; *p; p++) {
        c = *p;
        nl = c == '\n';
        if (c == '%') {
            zero = 0;
            width = 0;
            c = *++p;
            if (c == '%') {
                if (i < length) {
                    buffer[i] = c;
                }
                i++;
                continue;
            }

            if (isdigit(c)) {
                if (c == '0') {
                    zero = 1;
                    c = *++p;
                    assert(isdigit(c));
                }
                width = c - '0';
                c = *++p;
                if (isdigit(c)) {
                    width = width*10 + c - '0';
                    c = *++p;
                }
            }

            n = va_arg(ap, int);
            switch (c) {
                case 'b':
                    j = convert(n, 2, temp);
                    break;
                case 'c':
                    temp[0] = (char)n;
                    temp[1] = '\0';
                    j = 1;
                    break;
                case 'd':
                    j = convert(n, 10, temp);
                    break;
                case 'o':
                    j = convert(n, 8, temp);
                    break;
                case 's':
                    j = strlen((char *)n);
                    break;
                case 'x':
                    j = convert(n, 16, temp);
                    break;
                default:
                    assert(0);
            }

            if (j < width) {
                if (zero) {
                    strncpy(buffer+i, zeros, width-j);
                } else {
                    strncpy(buffer+i, spaces, width-j);
                }
                i += width-j;
            }

            if (c == 's') {
                if (i < length) {
                    strncpy(buffer+i, (char *)n, MIN(j, length-i));
                }
            } else {
                if (i < length) {
                    strncpy(buffer+i, temp, MIN(j, length-i));
                }
            }
            i += j;
        } else {
#if ! _WIN32
            if (c == '\n') {
                if (i < length) {
                    buffer[i] = '\r';
                }
                i++;
            }
#endif
            if (i < length) {
                buffer[i] = c;
            }
            i++;
        }
    }
    
    if (i < length) {
        buffer[i] = '\0';
    } else {
        if (nl) {
#if ! _WIN32
            if (length > 2) {
                buffer[length-3] = '\n';
            }
            if (length > 1) {
                buffer[length-2] = '\r';
            }
#else
            if (length > 1) {
                buffer[length-2] = '\n';
            }
#endif
        }
        if (length > 0) {
            buffer[length-1] = '\0';
        }
    }

    return i;
}

int
snprintf(char *buffer, int length, char *format, ...)
{
    int n;
    va_list ap;

    va_start(ap, format);
    n = vsnprintf(buffer, length, format, ap);
    va_end(ap);
    return n;
}

int
sprintf(char *buffer, char *format, ...)
{
    int n;
    va_list ap;

    va_start(ap, format);
    n = vsnprintf(buffer, 0x7fffffff, format, ap);
    va_end(ap);
    return n;
}

int
printf(char *format, ...)
{
    int n;
    va_list ap;
#if ! _WIN32
    static char buffer[BASIC_LINE_SIZE+2];  // 2 for \r\n

    assert(! usb_in_isr && ! timer_in_isr);
#else
    static char buffer[BASIC_LINE_SIZE+1];  // 1 for \n
#endif

    va_start(ap, format);
    n = vsnprintf(buffer, sizeof(buffer), format, ap);
    va_end(ap);

    assert(! buffer[sizeof(buffer)-1]);

#if ! _WIN32
    ftdi_print(buffer);
#else
    write(1, buffer, strlen(buffer));
#endif
    return n;
}

// **************************************************************************
