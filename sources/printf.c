// *** printf.c *******************************************************
// this file implements a lightweight printf utility on top of the
// console transport.

#include "main.h"
#include <stdarg.h>
extern bool debugger_attached;

#if BADGE_BOARD
bool printf_scroll;
#endif

#define MAXDIGITS  32

static const char digits[] = "0123456789abcdef";
static const char zeros[] = "00000000000000000000000000000000";
static const char spaces[] = "                                ";

static
int
convert(uintptr value, unsigned radix, char *buffer)
{
    int i;
    int n;
    uintptr scale;
    uintptr lastscale;
    int digit;

    i = 0;

    if (radix == -10) {
        if ((intptr)value < 0) {
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
    char c;
    char *q;
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

            switch (c) {
                case 'b':
                    j = convert(va_arg(ap, int), 2, temp);
                    break;
                case 'c':
                    temp[0] = (char)va_arg(ap, int);
                    temp[1] = '\0';
                    j = 1;
                    break;
                case 'd':
                    j = convert(va_arg(ap, int), -10, temp);
                    break;
                case 'u':
                    j = convert(va_arg(ap, int), 10, temp);
                    break;
                case 'o':
                    j = convert(va_arg(ap, int), 8, temp);
                    break;
                case 's':
                    q = va_arg(ap, char *);
                    j = MIN((int)strlen(q), prec);
                    break;
                case 'x':
                case 'X':
                    j = convert(va_arg(ap, int) & 0xffffffff, 16, temp);
                    break;
                case 'p':
                    j = convert((uintptr)va_arg(ap, void *), 16, temp);
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
                    strncpy(buffer+i, q, MIN(j, length-i));
                }
            } else {
                if (i < length) {
                    strncpy(buffer+i, temp, MIN(j, length-i));
                }
            }
            i += j;
        } else {
#if ! STICK_GUEST
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
#if ! STICK_GUEST
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

#if ! STICK_GUEST
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
    
#if STICK_GUEST
    write(1, bbuffer, MIN(n, sizeof(bbuffer)-1));
#else
#if BADGE_BOARD && ! SKELETON
    if (run_printf && run2_scroll) {
        jm_scroll(bbuffer, MIN(n, sizeof(bbuffer)-1));
    } else {
#endif
        terminal_print((byte *)bbuffer, MIN(n, sizeof(bbuffer)-1));
#if BADGE_BOARD && ! SKELETON
    }
#endif
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

#if IN_MEMORY_TRACE

enum { trace_size = 200 };

struct {
    int  cursor;
    char buffer[trace_size];
} trace_buffer;

void
trace(const char *fmt, ...)
{
    char buf[32];
    va_list ap;
    int s;

    // create a '\0' terminated message in the buf local.
    va_start(ap, fmt);
    s = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

#if TRACE_TO_SERIAL
    // send message to serial port.
    serial_send(buf, s);
    serial_send("\r\n", 2);
#endif

    // if trace_buffer has room for the message...
    if ((trace_buffer.cursor + s + 1) < trace_size) {
        // append the message into trace buffer.
        memcpy(&trace_buffer.buffer[trace_buffer.cursor], buf, s+1);
        trace_buffer.cursor += s;
        trace_buffer.buffer[trace_buffer.cursor++] = ' ';
    }
}

void
trace_print(void)
{
    if (trace_buffer.cursor <= 0) {
        return;
    }

    printf("%s\n", trace_buffer.buffer);
}

void
trace_reset(void)
{
    trace_buffer.cursor = 0;
    memset(trace_buffer.buffer, 0, trace_size);
}

#endif // IN_MEMORY_TRACE

