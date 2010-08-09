#include "main.h"

#if MCF52233
extern void *rich_so;
#endif

bool terminal_echo = true;

static terminal_command_cbfn command_cbfn;
static terminal_ctrlc_cbfn ctrlc_cbfn;

#define TERMINAL_INPUT_LINE_SIZE  72

static bool discard;
static char command[TERMINAL_INPUT_LINE_SIZE];

static byte hist_in;
static byte hist_out;
static bool hist_first = true;

#if PICTOCRYPT
#define NHIST  2
#else
#define NHIST  8
#endif
#define HISTWRAP(x)  (((unsigned)(x))%NHIST)
static char history[NHIST][TERMINAL_INPUT_LINE_SIZE];

static int ki;
static char keys[8];
static int cursor;
static char echo[TERMINAL_INPUT_LINE_SIZE*2];

static bool ack;

static asm __declspec(register_abi)
unsigned char
TRKAccessFile(long command, unsigned long file_handle, int *length_ptr, char *buffer_ptr)
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

void
terminal_print(byte *buffer, int length)
{
#pragma unused(length)
#if MCF52221
#if ! FLASHER
    if (ftdi_attached) {
        ftdi_print((char *)buffer);
    } else
#endif
    if (debugger_attached) {
        TRKAccessFile(0xD0, 0, &length, (char *)buffer);
    }
#elif MCF52233
    if (rich_so) {
        m_send(rich_so, buffer, length);
    } else if (debugger_attached) {
        TRKAccessFile(0xD0, 0, &length, (char *)buffer);
    }
#else
#error
#endif
}

static
void
terminal_send(byte *buffer, int length)
{
#pragma unused(length)
#if MCF52221
    ftdi_send(buffer, length);
#elif MCF52233
    if (rich_so) {
        m_send(rich_so, buffer, length);
    } else if (debugger_attached) {
        TRKAccessFile(0xD0, 0, &length, (char *)buffer);
    }
#else
#error
#endif
}

// *** line editor ***

enum keys {
    KEY_RIGHT,
    KEY_LEFT,
    KEY_UP,
    KEY_DOWN,
    KEY_HOME,
    KEY_END,
    KEY_BS,
    KEY_BS_DEL,
    KEY_DEL
};

struct keycode {
    char *keys;
    byte code;
} const keycodes[] = {
    "\033[C", KEY_RIGHT,
    "\033[D", KEY_LEFT,
    "\033[A", KEY_UP,
    "\033[B", KEY_DOWN,
    "\033[H", KEY_HOME,
    "\033[1~", KEY_HOME,
    "\033[K", KEY_END,
    "\033[4~", KEY_END,
    "\010", KEY_BS,
    "\177", KEY_BS_DEL,  // ambiguous
    "\033[3~", KEY_DEL
};

#define KEYS_RIGHT  "\033[%dC"
#define KEYS_LEFT  "\033[%dD"
#define KEY_DELETE  "\033[P"
#define KEY_CLEAR  "\033[K"

// this function implements the command line editing functionality
// of the console, by accumulating one character at a time from the
// console.
static
void
accumulate(char c)
{
    int i;
    int n;
    int orig;
    int again;

    echo[0] = '\0';

    if (c == '\003') {
        if (cursor) {
            sprintf(echo+strlen(echo), KEYS_LEFT, cursor);
            assert(strlen(echo) < sizeof(echo));
            cursor = 0;
        }
        strcat(echo, KEY_CLEAR);
        command[0] = '\0';
        ki = 0;
        terminal_send((byte *)echo, strlen(echo));
        return;
    }

    do {
        again = false;

        keys[ki++] = c;
        keys[ki] = '\0';
        assert(ki < sizeof(keys));

        for (i = 0; i < LENGTHOF(keycodes); i++) {
            if (! strncmp(keycodes[i].keys, keys, ki)) {
                // potential match
                if (keycodes[i].keys[ki]) {
                    // partial match
                    return;
                }

                // full match

                switch (keycodes[i].code) {
                    case KEY_RIGHT:
                        if (cursor < strlen(command)) {
                            strcat(echo, keycodes[i].keys);
                            assert(strlen(echo) < sizeof(echo));
                            cursor++;
                        }
                        break;
                    case KEY_LEFT:
                        if (cursor) {
                            strcat(echo, keycodes[i].keys);
                            assert(strlen(echo) < sizeof(echo));
                            cursor--;
                        }
                        break;

                    case KEY_UP:
                    case KEY_DOWN:
                        if (keycodes[i].code == KEY_UP) {
                            if (hist_first) {
                                hist_out = HISTWRAP(hist_in-1);
                            } else {
                                hist_out = HISTWRAP(hist_out-1);
                            }
                        } else {
                            hist_out = HISTWRAP(hist_out+1);
                        }
                        hist_first = false;
                        for (n = 0; n < NHIST; n++) {
                            if (history[hist_out][0]) {
                                break;
                            }
                            if (keycodes[i].code == KEY_UP) {
                                hist_out = HISTWRAP(hist_out-1);
                            } else {
                                hist_out = HISTWRAP(hist_out+1);
                            }
                        }
                        if (n != NHIST) {
                            if (cursor) {
                                sprintf(echo+strlen(echo), KEYS_LEFT, cursor);
                                assert(strlen(echo) < sizeof(echo));
                                cursor = 0;
                            }
                            strcat(echo, KEY_CLEAR);
                            strcpy(command, history[hist_out]);

                            // reprint the line
                            strcat(echo, command);
                            cursor = strlen(command);
                        }
                        break;
                    case KEY_HOME:
                        if (cursor) {
                            sprintf(echo+strlen(echo), KEYS_LEFT, cursor);
                            assert(strlen(echo) < sizeof(echo));
                            cursor = 0;
                        }
                        break;
                    case KEY_END:
                        if (strlen(command)-cursor) {
                            sprintf(echo+strlen(echo), KEYS_RIGHT, strlen(command)-cursor);
                            assert(strlen(echo) < sizeof(echo));
                            cursor = strlen(command);
                        }
                        break;
                    case KEY_BS_DEL:
                    case KEY_BS:
                        if (cursor) {
                            strcat(echo, keycodes[KEY_LEFT].keys);
                            strcat(echo, KEY_DELETE);
                            cursor--;
                            memmove(command+cursor, command+cursor+1, sizeof(command)-cursor-1);
                        }
                        break;
                    case KEY_DEL:
                        if (command[cursor]) {
                            strcat(echo, KEY_DELETE);
                            memmove(command+cursor, command+cursor+1, sizeof(command)-cursor-1);
                        }
                        break;
                    default:
                        assert(0);
                        break;
                }
                ki = 0;
                terminal_send((byte *)echo, strlen(echo));
                return;
            }
        }

        // no match

        // if we had already accumulated characters...
        if (ki > 1) {
            // we'll have to go around again
            ki--;
            again = true;
        }

        // process printable characters
        orig = cursor;
        for (i = 0; i < ki; i++) {
            if (isprint(keys[i])) {
                if (strlen(command) < sizeof(command)-1) {
                    memmove(command+cursor+1, command+cursor, sizeof(command)-cursor-1);
                    command[cursor] = keys[i];
                    cursor++;
                    assert(cursor <= sizeof(command)-1);
                }
            }
        }

        if (cursor > orig) {
            // reprint the line
            strcat(echo, command+orig);

            // and back the cursor up
            assert(strlen(command+orig));
            if (strlen(command+orig)-1) {
                sprintf(echo+strlen(echo), KEYS_LEFT, strlen(command+orig)-1);
            }
        }

        ki = 0;
    } while (again);

    if (terminal_echo) {
        terminal_send((byte *)echo, strlen(echo));
    }
}

bool
terminal_receive(byte *buffer, int length)
{
    int j;

    led_unknown_progress();
    
    // accumulate commands
    for (j = 0; j < length; j++) {
        if (buffer[j] == '\003') {
            assert(ctrlc_cbfn);
            ctrlc_cbfn();
        }

        if (! discard) {
            if (buffer[j] == '\r') {
                if (strcmp(history[HISTWRAP(hist_in-1)], command)) {
                    strcpy(history[hist_in], command);
                    hist_in = HISTWRAP(hist_in+1);
                }

                assert(command_cbfn);
                command_cbfn(command);
                
                // wait for terminal_command_ack();
#if MCF52221
                return false;
#elif MCF52233
                while (! ack) {
                    os_yield();
                }
                ack = false;
#else
#error
#endif
            } else {
                accumulate(buffer[j]);
            }
        }
    }
    return true;
}

// this function allows the user to edit a recalled history line.
void
terminal_edit(char *line)
{
    // put an unmodified copy of the line in history
    strncpy(history[hist_in], line, sizeof(history[hist_in])-1);
    hist_in = HISTWRAP(hist_in+1);

    // and then allow the user to edit it
    strncpy(command, line, sizeof(command)-1);
}

// this function causes us to discard typed commands while the
// BASIC program is running, except Ctrl-C.
void
terminal_command_discard(bool discard_in)
{
    discard = discard_in;
}

// this function acknowledges receipt of an FTDI command from upper
// level code.
void
terminal_command_ack(bool edit)
{
    ki = 0;
    hist_first = true;

    if (! edit) {
        memset(command, 0, sizeof(command));
        cursor = 0;
    } else {
        printf("%s", command);
        cursor = strlen(command);
    }

#if MCF52221
    ftdi_command_ack();
#endif
    
    ack = true;
}

// this function is called by upper level code in response to an
// FTDI command error.
void
terminal_command_error(int offset)
{
    int i;
    char buffer[2+TERMINAL_INPUT_LINE_SIZE+1];

    assert(offset < TERMINAL_INPUT_LINE_SIZE);

    offset += 2;  // prompt -- revisit, this is decided elsewhere!

    if (offset >= 10) {
        strcpy(buffer, "error -");
        for (i = 7; i < offset; i++) {
            buffer[i] = ' ';
        }
        buffer[i++] = '^';
        assert(i < sizeof(buffer));
        buffer[i] = '\0';
    } else {
        for (i = 0; i < offset; i++) {
            buffer[i] = ' ';
        }
        buffer[i++] = '^';
        assert(i < sizeof(buffer));
        buffer[i] = '\0';
        strcat(buffer, " - error");
    }
    printf("%s\n", buffer);
}

void
terminal_register(terminal_command_cbfn command, terminal_ctrlc_cbfn ctrlc)
{
    command_cbfn = command;
    ctrlc_cbfn = ctrlc;
}

