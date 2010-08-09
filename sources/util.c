// *** util.c *********************************************************
// this file implements generic utility functions.

#include "main.h"

#if PIC32
void
write32(byte *addr, uint32 data)
{
    *(byte *)(addr+0) = data & 0xff;
    *(byte *)(addr+1) = (data>>8) & 0xff;
    *(byte *)(addr+2) = (data>>16) & 0xff;
    *(byte *)(addr+3) = (data>>24) & 0xff;
}

uint32
read32(const byte *addr)
{
    return *(const byte *)(addr+3) << 24 |
           *(const byte *)(addr+2) << 16 |
           *(const byte *)(addr+1) << 8 |
           *(const byte *)(addr+0);
}

void
write16(byte *addr, uint16 data)
{
    *(byte *)(addr+0) = data & 0xff;
    *(byte *)(addr+1) = (data>>8) & 0xff;
}

uint16
read16(const byte *addr)
{
    return *(const byte *)(addr+1) << 8 |
           *(const byte *)(addr+0);
}
#endif

// N.B. the usb controller bdt data structures are defined to be little
// endian and the coldfire core is big endian, so we have to byteswap.

#if ! STICK_GUEST
uint32
byteswap(uint32 x, uint32 size)
{
#if PIC32
    return x;
#else
    // byteswap all bytes of x within size
    switch (size) {
        case 4:
#if GCC
            __asm__("byterev.l  %0\n"
                    : /* outputs */ "=r" (x)
                    : /* inputs */ "r" (x));
#else
            asm {
                move.l     x,D0
                byterev.l  D0
                move.l     D0,x
            }
#endif
            break;
        case 2:
#if GCC
            __asm__("move.l     %0, %%d0\n"
                    "byterev.l  %%d0\n"
                    "move.w     #0, %%d0\n"
                    "swap       %%d0\n"
                    "move.l     %%d0, %0\n"
                    : /* outputs */ "=r" (x)
                    : /* inputs */ "r" (x)
                    : /* clobber */ "d0");
#else
            asm {
                move.l     x,D0
                byterev.l  D0
                move.w     #0,D0
                swap       D0
                move.l     D0,x
            }
#endif
            break;
        case 1:
            break;
        default:
            assert(0);
            break;
    }
    return x;
#endif
}
#endif // ! STICK_GUEST

// return the current interrupt mask level
int
gpl(void)
{
    int oldlevel;

#if PIC32
    oldlevel = (_CP0_GET_STATUS() >> 10) & 7;
#else
    short csr;

    // get the sr
    csr = get_sr();
    
    oldlevel = (csr >> 8) & 7;
#endif
    return oldlevel;
}

// set the current interrupt mask level and return the old one
int
splx(int level)
{
#if PIC32
    int csr;
    int oldlevel;
    
    // get the sr
    csr = _CP0_GET_STATUS();

    oldlevel = (csr >> 10) & 7;
    if (level <= 0) {
        // we're going down
        level = -level;
    } else {
        // we're going up
        level = MAX(level, oldlevel);
    }
    assert(level >= 0 && level <= 7);
    csr = (csr & 0xffffe3ff) | (level << 10);

    // update the sr
    _CP0_SET_STATUS(csr);

    return -oldlevel;
#else
    short csr;
    int oldlevel;
    
    // get the sr
    csr = get_sr();

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
    set_sr(csr);

    return -oldlevel;
#endif
}

static volatile int g;

static int blips_per_ms;

static void
blip(void)
{
    int x;
    for (x = 0; x < 500; x++) {
        g++;
    }
}

// delay for the specified number of milliseconds
void
delay(int ms)
{
#if STICK_GUEST
    // we sleep quicker for unit tests
    ms = isatty(0)?(ms):(ms)/10;
#if _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
#else // ! STICK_GUEST
    int m;
    int x;
    int blips;
        
    // if interrupts are initialized...
    if (gpl() < SPL_PIT0) {
        // wait for the pit0 to count off the ticks
        m = msecs;
        blips = 0;
        while (msecs-m < ms+1) {
            blip();
            blips++;
        }
        if (! blips_per_ms) {
            blips_per_ms = blips/ms+1;
        }
    // otherwise; make a good guess with a busywait
    } else {
        assert(blips_per_ms);
        while (ms--) {
            for (x = 0; x < blips_per_ms; x++) {
                blip();
            }
        }
    }
#endif // ! STICK_GUEST
}

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

void *
memcpy(void *d,  const void *s, size_t n)
{
    
    if (((uintptr)d&3)+((uintptr)s&3)+(n&3) == 0) {
        uint32 *dtemp = d;
        const uint32 *stemp = s;
        
        n >>= 2;
        while (n--) {
            *(dtemp++) = *(stemp++);
        }
    } else {
        uint8 *dtemp = d;
        const uint8 *stemp = s;
        
        while (n--) {
            *(dtemp++) = *(stemp++);
        }
    }
    return d;
}

void *
memmove(void *d,  const void *s, size_t n)
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
memset(void *p,  int d, size_t n)
{
    int dd;
    
    if (((uintptr)p&3)+(n&3) == 0) {
        uint32 *ptemp = p;
    
        n >>= 2;
        d = d & 0xff;
        dd = d<<24|d<<16|d<<8|d;
        while (n--) {
            *(ptemp++) = dd;
        }
    } else {
        uint8 *ptemp = p;
        
        while (n--) {
            *(ptemp++) = d;
        }
    }
    return p;
}

int
memcmp(const void *d,  const void *s, size_t n)
{
    char c;
    const uint8 *dtemp = d;
    const uint8 *stemp = s;
    
    while (n--) {
        c = *(dtemp++) - *(stemp++);
        if (c) {
            return c;
        }
    }
    return 0;
}

size_t
strlen(const char *s)
{
    const char *stemp = s;
    
    while (*stemp) {
        stemp++;
    }
    return stemp - s;
}

char *
strcat(char *dest, const char *src)
{
    char *orig_dest = dest;
    
    while (*dest) {
        dest++;
    }
    do {
        *(dest++) = *src;
    } while (*(src++));
    
    return orig_dest;
}

char *
strncat(char *dest, const char *src, size_t n)
{
    char *orig_dest = dest;
    
    while (*dest) {
        dest++;
    }
    while ((n-- > 0) && *src) {
        *(dest++) = *(src++);
    }
    *dest = '\0';
    
    return orig_dest;
}

char *
strcpy(char *dest, const char *src)
{
    char *orig_dest = dest;
    
    do {
        *(dest++) = *src;
    } while (*(src++));
    
    return orig_dest;
}

char *
strncpy(char *dest, const char *src, size_t n)
{
    char *orig_dest = dest;
    
    while (n--) {
        *(dest++) = *src;
        if (! *(src++)) {
            break;
        }
    }
    
    return orig_dest;
}

int
strcmp(const char *s1, const char *s2)
{
    for (; *s1 == *s2; s1++, s2++) {
        if (! *s1) {
            return 0;
        }
    }

    if (*s1 < *s2) {
        return -1;
    } else {
        return 1;
    }
}

int
strncmp(const char *s1, const char *s2, size_t n)
{
    for (; (n > 0) && (*s1 == *s2); n--, s1++, s2++) {
        if (! *s1) {
            return 0;
        }
    }

    if (n <= 0) {
        return 0;
    }

    if (*s1 < *s2) {
        return -1;
    } else {
        return 1;
    }
}

char *
strchr(const char *s, int c)
{
    for (; *s; s++) {
        if (*s == c) {
            return (char *)s;
        }
    }
    return NULL;
}

int
isdigit(int c)
{
    return (c >= '0') && (c <= '9');
}

int
isspace(int c)
{
    return (c == ' ') || (c == '\f') || (c == '\n') || (c == '\r') || (c == '\t') || (c == '\v');
}

int
isupper(int c)
{
    return (c >= 'A') && (c <= 'Z');
}

int
islower(int c)
{
    return (c >= 'a') && (c <= 'z');
}

int
isalpha(int c)
{
    return isupper(c) || islower(c);
}

int
isalnum(int c)
{
    return isalpha(c) || isdigit(c);
}

int
isprint(int c)
{
    return (c >= ' ') && (c <= '~');
}

#if STICK_GUEST
static uint16 sr;
#endif

#if ! PIC32
uint16
get_sr(void)
{
    uint16 csr;
#if STICK_GUEST
    csr = sr;
#elif GCC
    __asm__("move.w  %/sr, %0\n" : /* outputs */ "=r" (csr));
#else
    asm {
        move.w     sr,d0
        move.w     d0,csr
    }
#endif
    return csr;
}

void
set_sr(uint16 csr)
{
#if STICK_GUEST
    sr = csr;
#elif GCC
    __asm__("move.w  %0, %/sr\n" :: /* inputs */ "r" (csr));
#else
    asm {
        move.w     csr,d0
        move.w     d0,sr
    }
#endif
}
#endif

