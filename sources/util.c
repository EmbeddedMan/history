// *** util.c *********************************************************
// this file implements generic utility functions.

#include "main.h"

// N.B. the usb controller bdt data structures are defined to be little
// endian and the coldfire core is big endian, so we have to byteswap.

#if ! _WIN32
uint32
byteswap(uint32 x, uint32 size)
{
    // byteswap all bytes of x within size
    switch (size) {
        case 4:
            asm {
                move.l     x,D0
                byterev.l  D0
                move.l     D0,x
            }
            break;
        case 2:
            asm {
                move.l     x,D0
                byterev.l  D0
                move.w     #0,D0
                swap       D0
                move.l     D0,x
            }
            break;
        case 1:
            break;
        default:
            assert(0);
            break;
    }
    return x;
}

// return the current interrupt mask level
int
gpl(void)
{
    short csr;
    int oldlevel;
    
    // get the sr
    asm {
        move.w     sr,d0
        move.w     d0,csr
    }

    oldlevel = (csr >> 8) & 7;
    
    return oldlevel;
}

// set the current interrupt mask level and return the old one
int
splx(int level)
{
    short csr;
    int oldlevel;
    
    // get the sr
    asm {
        move.w     sr,d0
        move.w     d0,csr
    }

    oldlevel = (csr >> 8) & 7;
    if (level <= 0) {
        // we're going down
        level = -level;
    } else {
        // we're going up
        level = MAX(level, oldlevel);
    }
    assert(level >= 0 && level <= 7);
    csr = (csr & 0xf8ff) | (level << 8);

    // update the sr
    asm {
        move.w     csr,d0
        move.w     d0,sr
    }

    return -oldlevel;
}

static volatile int g;

// delay for the specified number of milliseconds
void
delay(int ms)
{
    int t;
    int x;
    int y;

    // if interrupts are initialized...
    if (initialized) {
        // wait for the pit0 to count off the ticks
        t = ticks;
        while (ticks-t < ms+1) {
            // NULL
        }
    // otherwise; make a good guess with a busywait
    } else {
        y = fsys_frequency/6000;
        while (ms--) {
            for (x = 0; x < y; x++) {
                g++;
            }
        }
    }
}
#else
extern int isatty(int);

void
delay(int ms)
{
    Sleep(isatty(0)?(ms):(ms)/10);
}
#endif

int
gethex(char *p)
{
    char c;

    c = *p;
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'A' && c <= 'F') {
        return 10 + c - 'A';
    } else if (c >= 'a' && c <= 'f') {
        return 10 + c - 'a';
    } else {
        return -1;
    }
}

int
get2hex(char **p)
{
    int v1, v0;

    v1 = gethex(*p);
    if (v1 == -1) {
        return -1;
    }
    v0 = gethex(*p+1);
    if (v0 == -1) {
        return -1;
    }

    (*p) += 2;
    return v1*16+v0;
}

void
tailtrim(char *text)
{
    char *p;
    
    p = strchr(text, '\0');
    while (p > text && isspace(*(p-1))) {
        p--;
    }
    *p = '\0';
}

#if ! _WIN32
void *
memcpy(void *d,  const void *s, uint32 n)
{
    void *dd;
    
    dd = d;
    if (((int)d&3)+((int)s&3)+((int)n&3) == 0) {
        n >>= 2;
        while (n--) {
            *(((int *)d)++) = *(((int *)s)++);
        }
    } else {
        while (n--) {
            *(((char *)d)++) = *(((char *)s)++);
        }
    }
    return dd;
}

void *
memmove(void *d,  const void *s, uint32 n)
{
    void *dd;
    
    if ((char *)d > (char *)s && (char *)d < (char *)s+n) {
        dd = d;
        while (n--) {
            *((char *)d+n) = *((char *)s+n);
        }
        return dd;
    } else {
        return memcpy(d, s, n);
    }
}

void *
memset(void *p,  int d, uint32 n)
{
    int dd;
    void *pp;
    
    pp = p;
    if (((int)p&3)+((int)n&3) == 0) {
        n >>= 2;
        d = d & 0xff;
        dd = d<<24|d<<16|d<<8|d;
        while (n--) {
            *(((int *)p)++) = dd;
        }
    } else {
        while (n--) {
            *(((char *)p)++) = d;
        }
    }
    return pp;
}

int
memcmp(const void *d,  const void *s, uint32 n)
{
    char c;
    
    while (n--) {
        c = *(((char *)d)++) - *(((char *)s)++);
        if (c) {
            return c;
        }
    }
    return 0;
}
#endif

