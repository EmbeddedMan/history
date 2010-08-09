// *** printf.c *******************************************************
// this file implements a lightweight printf utility on top of the
// console transport.

#include "main.h"
#include <stdarg.h>
extern bool debugger_attached;

#define isdigit(c)  ((c) >= '0' && (c) <= '9')

#define MAXDIGITS  32

static const char digits[] = "0123456789abcdef";
static const char zeros[] = "00000000000000000000000000000000";
static const char spaces[] = "                                ";

static
int
convert(unsigned value, unsigned radix, char *buffer)
{
    int i;
    int n;
    unsigned scale;
    unsigned lastscale;
    int digit;

    i = 0;

    if (radix == -10) {
        if ((int)value < 0) {
            buffer[i++] = '-';
            value = 0-value;
        }
        radix = 10;
    }

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
vsnprintf(char *buffer, int length, const char *format, va_list ap)
{
    int i;
    int j;
    int n;
    char c;
    bool nl;
    int zero;
    int width;
    int prec;
    const char *p;
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

            if (c == '-') {
                // revisit -- implement
                c = *++p;
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
            
            prec = 0x7fffffff;
            if (c == '.') {
                prec = 0;
                c = *++p;
                while (isdigit(c)) {
                    prec = prec*10 + c - '0';
                    c = *++p;
                }
            }
            
            if (c == 'h' || c == 'l') {
                c = *++p;
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
                    j = convert(n, -10, temp);
                    break;
                case 'u':
                    j = convert(n, 10, temp);
                    break;
                case 'o':
                    j = convert(n, 8, temp);
                    break;
                case 's':
                    j = MIN((int)strlen((char *)n), prec);
                    break;
                case 'x':
                case 'X':
                    j = convert(n, 16, temp);
                    break;
                default:
                    assert(0);
                    break;
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
                buffer[length-3] = '\r';
            }
            if (length > 1) {
                buffer[length-2] = '\n';
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
snprintf(char *buffer, unsigned long length, const char *format, ...)
{
    int n;
    va_list ap;

    va_start(ap, format);
    n = vsnprintf(buffer, length, format, ap);
    va_end(ap);
    return n;
}

int
sprintf(char *buffer, const char *format, ...)
{
    int n;
    va_list ap;

    va_start(ap, format);
    n = vsnprintf(buffer, 0x7fffffff, format, ap);
    va_end(ap);
    return n;
}

#if ! _WIN32
static char bbuffer[BASIC_LINE_SIZE+2];  // 2 for \r\n
#else
static char bbuffer[BASIC_LINE_SIZE+1];  // 1 for \n
#endif

int
printf(const char *format, ...)
{
    int n;
    va_list ap;

    va_start(ap, format);
    n = vprintf(format, ap);
    va_end(ap);

    return n;
}

int
vprintf(const char *format, va_list ap)
{
    int n;
    
    assert(gpl() == 0);

    n = vsnprintf(bbuffer, sizeof(bbuffer), format, ap);
    assert(! bbuffer[sizeof(bbuffer)-1]);
    
#if _WIN32
    write(1, bbuffer, MIN(n, sizeof(bbuffer)-1));
#else
    terminal_print((byte *)bbuffer, MIN(n, sizeof(bbuffer)-1));
#endif

    return n;
}

int
vsprintf(char *outbuf, const char *format, va_list ap)
{
    int n;
    
    n = vsnprintf(outbuf, sizeof(bbuffer), format, ap);
    assert(! outbuf[sizeof(bbuffer)-1]);

    return n;
}

