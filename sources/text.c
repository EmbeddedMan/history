// *** text.c *********************************************************
// this file implements generic text handling functions.

#include "main.h"

// compression
// 0x01 - 0x1f -> add 0x60 and add 2 spaces; except \n for 0x7f
// ' '-'~'     -> ' '-'~'
// 0x80 - 0xbf -> subtract 0x40 and add 1 space
// 0xc0 - 0xff -> add 2+(c-0xc0) spaces

#if HELP_COMPRESS
void
text_compress(
    IN char *text,
    OUT byte *buffer
    )
{
    int c;

    while ((c = *text++)) {
        if (c >= 0x40 && c < 0x80 && text[0] == ' ' && text[1] != ' ') {
            c += 0x40;
            assert(c >= 0x80 && c < 0xc0);
            text++;
        } else if (c > 0x60 && c < 0x80 && text[0] == ' ' && text[1] == ' ' && text[2] != ' ') {
            c -= 0x60;
            if (c == '\n') {
                c = 0x7f;
            }
            assert((c != '\n' && (c > 0 && c < 0x20)) || c == 0x7f);
            text += 2;
        } else if (c == ' ' && text[0] == ' ') {
            text++;
            c = 0xc0;
            while (*text == ' ' && c < 0xff) {
                text++;
                c++;
            }
            assert(c >= 0xc0 && c < 0x100);
        } else {
            assert(c == '\n' || (c >= 0x20 && c < 0x7f));
        }
        *buffer++ = c;
    }
    *buffer = '\0';
}
#endif

void
text_expand(
    IN byte *buffer,
    OUT char *text
    )
{
    int c;

    while ((c = *buffer++)) {
        if (c >= 0x80 && c < 0xc0) {
            *text++ = c-0x40;
            *text++ = ' ';
        } else if ((c != '\n' && (c > 0 && c < 0x20)) || c == 0x7f) {
            if (c == 0x7f) {
                c = '\n';
            }
            *text++ = c+0x60;
            *text++ = ' ';
            *text++ = ' ';
        } else if (c >= 0xc0 && c < 0x100) {
            *text++ = ' ';
            *text++ = ' ';
            while (c > 0xc0) {
                *text++ = ' ';
                c--;
            }
        } else {
            assert(c == '\n' || (c >= 0x20 && c < 0x7f));
            *text++ = c;
        }
    }
    *text = '\0';
}