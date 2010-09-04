// *** jm.c ***********************************************************

#include "main.h"

#define SCROLL_MS  50

static uint16 rows[5];
static uint16 scroll[5];
static byte row;

static char scroll_buf[BASIC_OUTPUT_LINE_SIZE];
static int scroll_col;

extern const unsigned char font[][5];

static
bool
jm_letter(int c, char letter)
{
    int r;
    static bool last;
    
    assert(c >= 0 && c < 6);
    
    if (c == 5 && ! last) {
        return true;
    }
    
    last = false;
    for (r = 0; r < 5; r++) {
        scroll[r] >>= 1;
        if (c == 5) {
            continue;
        }
        if (font[letter - ' '][c] & (1<<r)) {
            scroll[r] |= (1<<15);
            last = true;
        }
    }
    
    return false;
}

void
jm_timer_poll()
{
    bool again;
    char letter;
    static int calls;
    
    // display the next row
    row = (++row%5);
    
    PTAD = (PTAD & ~0x1f) | (1<<row);
    if (! zb_present) {
        PTED = (byte)~(rows[row]^scroll[row]);
    }
    PTDD = (byte)~((rows[row]^scroll[row])>>8);
    
    if (++calls == SCROLL_MS) {
        // scroll the text
        calls = 0;
        
        do {
            letter = scroll_buf[0];
            if (letter < ' ' || letter >= 0x80) {
                letter = ' ';
            }
        
            again = jm_letter(scroll_col, letter);
            
            if (scroll_buf[0]) {
                scroll_col = ++scroll_col%6;
            }
            
            if (! scroll_col) {
                memmove(scroll_buf, scroll_buf+1, sizeof(scroll_buf)-1);
                scroll_buf[sizeof(scroll_buf)-1] = '\0';
            }
        } while (again);
    }
}

void
jm_set(int r_in, int c_in)
{
    int r;
    int c;
    
    for (r = 0; r < 5; r++) {
        for (c = 0; c < 16; c++) {
            if ((r == r_in || r_in == -1) && (c == c_in || c_in == -1)) {
                rows[r] |= 1<<c;
            }
        }
    }
}

void
jm_clear(int r_in, int c_in)
{
    int r;
    int c;
    
    for (r = 0; r < 5; r++) {
        for (c = 0; c < 16; c++) {
            if ((r == r_in || r_in == -1) && (c == c_in || c_in == -1)) {
                rows[r] &= ~(1<<c);
            }
        }
    }
}

void
jm_scroll(char *text, int length)
{
    strncat(scroll_buf, text, length);
    assert(strlen(scroll_buf) < sizeof(scroll_buf));
}

bool
jm_scroll_ready()
{
    return ! scroll_buf[0];
}

void
jm_initialize(void)
{
    // set the led matrix pins to digital output
    // N.B. we don't use PTE4-7 if zigbee is present on qspi pins
    PTADD = 0x1f;
    PTADS = 0x1f;
    if (! zb_present) {
        PTEDD = 0xff;
        PTEDS = 0xff;
        SPI1C1 = 0;  // qspi off
    }
    PTDDD = 0xff;
    PTDDS = 0xff;
    
    // we start with a message...
#if STICKOS
    if (var_get_flash(FLASH_AUTORUN) != 1) {
        jm_scroll(help_about_short, strlen(help_about_short));
    }
#endif
}

/*! ASCII Font Table for 5x5 Character Set */
const unsigned char font[][5] = 
{ 
  { 0b00000, 0b00000, 0b00000, 0b00000, 0b00000 }, // Space
  { 0b00000, 0b00000, 0b10111, 0b00000, 0b00000 }, // !
  { 0b00100, 0b00011, 0b00100, 0b00011, 0b00000 }, // "
  { 0b01010, 0b11111, 0b01010, 0b11111, 0b01010 }, // #
  { 0b10010, 0b10101, 0b11111, 0b10101, 0b01001 }, // $
  { 0b10011, 0b01011, 0b00100, 0b11010, 0b11001 }, // %
  { 0b01010, 0b10101, 0b10111, 0b01000, 0b11000 }, // &
  { 0b00000, 0b00100, 0b00011, 0b00000, 0b00000 }, // '
  { 0b00000, 0b01110, 0b10001, 0b00000, 0b00000 }, // (
  { 0b00000, 0b10001, 0b01110, 0b00000, 0b00000 }, // )
  { 0b01010, 0b00100, 0b11111, 0b00100, 0b01010 }, // *
  { 0b00000, 0b00100, 0b01110, 0b00100, 0b00000 }, // +
  { 0b00000, 0b10000, 0b01000, 0b00000, 0b00000 }, // ,
  { 0b00000, 0b00100, 0b00100, 0b00100, 0b00000 }, // -
  { 0b00000, 0b00000, 0b10000, 0b00000, 0b00000 }, // .
  { 0b10000, 0b01000, 0b00100, 0b00010, 0b00001 }, // /
  { 0b01110, 0b11001, 0b10101, 0b01110, 0b00000 }, // 0
  { 0b00000, 0b10010, 0b11111, 0b10000, 0b00000 }, // 1
  { 0b11001, 0b10101, 0b10101, 0b10010, 0b00000 }, // 2
  { 0b10001, 0b10101, 0b10101, 0b01010, 0b00000 }, // 3
  { 0b01100, 0b01010, 0b11111, 0b01000, 0b00000 }, // 4
  { 0b10011, 0b10101, 0b10101, 0b01001, 0b00000 }, // 5
  { 0b01110, 0b10101, 0b10101, 0b01000, 0b00000 }, // 6
  { 0b00001, 0b11001, 0b00101, 0b00011, 0b00000 }, // 7
  { 0b01010, 0b10101, 0b10101, 0b01010, 0b00000 }, // 8
  { 0b00010, 0b10101, 0b10101, 0b11110, 0b00000 }, // 9
  { 0b00000, 0b00000, 0b01010, 0b00000, 0b00000 }, // :
  { 0b00000, 0b10000, 0b01010, 0b00000, 0b00000 }, // ;
  { 0b00000, 0b00100, 0b01010, 0b10001, 0b00000 }, // <
  { 0b00000, 0b01010, 0b01010, 0b01010, 0b00000 }, // =
  { 0b00000, 0b10001, 0b01010, 0b00100, 0b00000 }, // >
  { 0b00010, 0b00001, 0b10101, 0b00101, 0b00010 }, // ?
  { 0b01110, 0b10001, 0b10101, 0b10101, 0b10110 }, // @
  { 0b11110, 0b00101, 0b00101, 0b11110, 0b00000 }, // A
  { 0b11111, 0b10101, 0b10101, 0b01010, 0b00000 }, // B
  { 0b01110, 0b10001, 0b10001, 0b10001, 0b00000 }, // C
  { 0b11111, 0b10001, 0b10001, 0b01110, 0b00000 }, // D
  { 0b11111, 0b10101, 0b10101, 0b10001, 0b00000 }, // E
  { 0b11111, 0b00101, 0b00101, 0b00001, 0b00000 }, // F
  { 0b01110, 0b10001, 0b10101, 0b11100, 0b00000 }, // G
  { 0b11111, 0b00100, 0b00100, 0b11111, 0b00000 }, // H
  { 0b00000, 0b10001, 0b11111, 0b10001, 0b00000 }, // I
  { 0b01000, 0b10001, 0b01111, 0b00001, 0b00000 }, // J
  { 0b11111, 0b00100, 0b01010, 0b10001, 0b00000 }, // K
  { 0b11111, 0b10000, 0b10000, 0b10000, 0b00000 }, // L
  { 0b11111, 0b00010, 0b00100, 0b00010, 0b11111 }, // M
  { 0b11111, 0b00010, 0b00100, 0b11111, 0b00000 }, // N
  { 0b01110, 0b10001, 0b10001, 0b01110, 0b00000 }, // O
  { 0b11111, 0b00101, 0b00101, 0b00010, 0b00000 }, // P
  { 0b01110, 0b10001, 0b01001, 0b10110, 0b00000 }, // Q
  { 0b11111, 0b00101, 0b01101, 0b10010, 0b00000 }, // R
  { 0b10010, 0b10101, 0b10101, 0b01001, 0b00000 }, // S
  { 0b00001, 0b00001, 0b11111, 0b00001, 0b00001 }, // T
  { 0b01111, 0b10000, 0b10000, 0b01111, 0b00000 }, // U
  { 0b00011, 0b01100, 0b10000, 0b01100, 0b00011 }, // V
  { 0b11111, 0b01000, 0b00100, 0b01000, 0b11111 }, // W
  { 0b10001, 0b01010, 0b00100, 0b01010, 0b10001 }, // X
  { 0b00001, 0b00010, 0b11100, 0b00010, 0b00001 }, // Y
  { 0b10001, 0b11001, 0b10101, 0b10011, 0b10001 }, // Z
  { 0b00000, 0b11111, 0b10001, 0b10001, 0b00000 }, // [
  { 0b00001, 0b00010, 0b00100, 0b01000, 0b10000 }, // (
  { 0b00000, 0b10001, 0b10001, 0b11111, 0b00000 }, // ]
  { 0b00000, 0b00010, 0b00001, 0b00010, 0b00000 }, // ^
  { 0b10000, 0b10000, 0b10000, 0b10000, 0b10000 }, // _
  { 0b00000, 0b00011, 0b00100, 0b00000, 0b00000 }, // '
  { 0b01000, 0b10101, 0b10101, 0b10101, 0b11110 }, // a
  { 0b11111, 0b10100, 0b10100, 0b10100, 0b01000 }, // b
  { 0b01100, 0b10010, 0b10010, 0b10010, 0b00000 }, // c
  { 0b01000, 0b10100, 0b10100, 0b10100, 0b11111 }, // d
  { 0b01110, 0b10101, 0b10101, 0b10101, 0b10110 }, // e
  { 0b00100, 0b11110, 0b00101, 0b00001, 0b00010 }, // f
  { 0b10010, 0b10101, 0b10101, 0b10101, 0b01110 }, // g
  { 0b11111, 0b00100, 0b00100, 0b11000, 0b00000 }, // h
  { 0b00000, 0b00000, 0b11101, 0b00000, 0b00000 }, // i
  { 0b00000, 0b10000, 0b01101, 0b00000, 0b00000 }, // j
  { 0b11111, 0b00100, 0b01010, 0b10001, 0b00000 }, // k
  { 0b00000, 0b10001, 0b11111, 0b10000, 0b00000 }, // l
  { 0b11110, 0b00001, 0b00110, 0b00001, 0b11110 }, // m
  { 0b11111, 0b00001, 0b00001, 0b00001, 0b11110 }, // n
  { 0b01110, 0b10001, 0b10001, 0b01110, 0b00000 }, // o
  { 0b11111, 0b00101, 0b00101, 0b00010, 0b00000 }, // p
  { 0b00010, 0b00101, 0b00101, 0b11111, 0b10000 }, // q
  { 0b11111, 0b00010, 0b00001, 0b00001, 0b00010 }, // r
  { 0b10010, 0b10101, 0b10101, 0b01001, 0b00000 }, // s
  { 0b00010, 0b01111, 0b10010, 0b10010, 0b00000 }, // t
  { 0b01111, 0b10000, 0b10000, 0b01111, 0b00000 }, // u
  { 0b00011, 0b01100, 0b10000, 0b01100, 0b00011 }, // v
  { 0b01111, 0b10000, 0b01100, 0b10000, 0b01111 }, // w
  { 0b10001, 0b01010, 0b00100, 0b01010, 0b10001 }, // x
  { 0b10011, 0b10100, 0b10100, 0b01111, 0b00000 }, // y
  { 0b10010, 0b11010, 0b10110, 0b10010, 0b00000 }, // z
  { 0b00110, 0b01111, 0b11110, 0b01111, 0b00110 }, // {
  { 0b10010, 0b01000, 0b00111, 0b01000, 0b10010 }, // |
  { 0b00100, 0b01110, 0b10101, 0b00100, 0b00100 }, // }
  { 0b11111, 0b11111, 0b11111, 0b11111, 0b11111 }, // 
};

