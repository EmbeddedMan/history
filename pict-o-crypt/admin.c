// *** pict-o-crypt.c *************************************************
// this file implements the main loop of pict-o-crypt, where we either
// wait for and then process administrative commands, or walk the
// filesystem and encrypt files.

#include "main.h"

// *** command processing ***

enum cmdcode {
    command_aesbits,  // [(128|192|256)]
    command_aeskey,  // [<hexkey>|<passphrase>]
    command_files,  // [clear|default|+<extension>|-<extension>]
    command_help,
    command_reset,
    command_select,  // [ro|rw]
    command_upgrade,
    command_uptime
};

static
const struct commands {
    char *command;
    enum cmdcode code;
} commands[] = {
    "aesbits", command_aesbits,
    "aeskey", command_aeskey,
    "files", command_files,
    "help", command_help,
    "reset", command_reset,
    "select", command_select,
    "upgrade", command_upgrade,
    "uptime", command_uptime
};

static
void
basic_trim(IN OUT char **p)
{
    // advance *p past any leading spaces
    while (isspace(**p)) {
        (*p)++;
    }
}

static
bool
basic_char(IN OUT char **text, IN char c)
{
    if (**text != c) {
        return false;
    }

    // advance *text past c
    (*text)++;

    basic_trim(text);
    return true;
}

static
bool
basic_word(IN OUT char **text, IN char *word)
{
    int len;

    len = strlen(word);

    if (strncmp(*text, word, len)) {
        return false;
    }

    // advance *text past word
    (*text) += len;

    basic_trim(text);
    return true;
}

static
bool
basic_const(IN OUT char **text, OUT int *value_out)
{
    int value;

    if (! isdigit((*text)[0])) {
        return false;
    }

    // parse constant value and advance *text past constant
    value = 0;
    while (isdigit((*text)[0])) {
        value = value*10 + (*text)[0] - '0';
        (*text)++;
    }

    *value_out = value;
    basic_trim(text);
    return true;
}

static char *help_general =
"commands:\n"
"  aesbits [(128|192|256)]\n"
"  aeskey (<hexkey>|<passphrase>)\n"
"  files [clear|default|+<extension>|-<extension>]\n"
"  help\n"
"  reset\n"
"  select [ro|rw]\n"
"  upgrade\n"
"  uptime\n"
"\n"
"for more information:\n"
"  help about\n"
"\n"
"or contact:\n"
"  pict.o.crypt@gmail.com\n"
;

static char *help_about =
"Welcome to Pict-o-Crypt v1.00!\n"
"Copyright (c) 2008; all rights reserved.\n"
"pict.o.crypt@gmail.com\n"
;

static
void
help(IN char *text_in)
{
    char *p;
    char *text;
    char line[BASIC_LINE_SIZE];
    
    text = text_in;

    // while there is more help to print...
    while (*text) {
        // print the next line of help
        p = strchr(text, '\n');
        assert(p);
        assert(p-text < BASIC_LINE_SIZE);
        memcpy(line, text, p-text);
        line[p-text] = '\0';
        printf("%s\n", line);
        text = p+1;
    }
    
#if ! _WIN32
    if (text_in == help_about) {
        printf("(checksum 0x%x)\n", flash_checksum);
        if (! SECURE) {
            printf("NOT SECURE\n");
        }
    }
#endif
}

void
getaeskey(IN OUT char **text, OUT bool *hexkey, OUT byte aeskey[AESBITS/8])
{
    int i;
    int hex;
    char *p;
    
    i = 0;
    memset(aeskey, 0, KEYLENGTH(AESBITS));

    // trim trailing spaces
    tailtrim(*text);
    
    // see if all characters are hex ascii
    p = *text;
    while (get2hex(&p) != -1) {
        // NULL
    }
    
    if (*p) {
        // passphrase
        *hexkey = false;
        while (**text) {
            aeskey[i++%KEYLENGTH(AESBITS)] ^= **text;
            (*text)++;
        }
    } else {
        // hexkey
        *hexkey = true;
        while (**text) {
            hex = get2hex(text);
            assert(hex != -1);
            aeskey[i++%KEYLENGTH(AESBITS)] ^= hex;
        }
    }
}

static
bool
getfiles(IN OUT char **text, OUT char extension[4])
{
    int i;
    
    i = 0;
    memset(extension, 0, 4);
    
    // trim trailing spaces
    tailtrim(*text);
    
    if (**text == '*') {
        (*text)++;
    }
    
    if (**text == '.') {
        (*text)++;
    }
    
    if (strlen(*text) < 1 || strlen(*text) > 3) {
        return false;
    }
    
    while (**text) {
        if (! isprint(**text) || ! (isalnum(**text) || **text == '_')) {
            return false;
        }
        if (**text >= 'a' && **text <= 'z') {
            extension[i++] = **text - 'a' + 'A';
        } else {
            extension[i++] = **text;
        }
        (*text)++;
    }
    assert(i < 4);
    while (i < 3) {
        extension[i++] = ' ';
    }
    assert(i < 4);
    extension[i] = '\0';
    return true;
}

static int vsbug;

// this function implements the pict-o-crypt command interpreter.
void
admin_run(char *text_in)
{
    int i;
    int j;
    int t;
    int d;
    int h;
    int m;
    int cmd;
    int len;
    int bits;
    bool roflip;
    bool boo;
    bool add;
    char *text;
    char *confirm;
    bool hexkey;
    bool hexkey2;
    params_t params;
    char extension[4];
    byte aeskey[AESBITS/8];

    text = text_in;
    basic_trim(&text);
    
    if (! *text) {
        return;
    }
    
    for (cmd = 0; cmd < LENGTHOF(commands); cmd++) {
        len = strlen(commands[cmd].command);
        if (! strncmp(text, commands[cmd].command, len)) {
            break;
        }
    }

    if (cmd != LENGTHOF(commands)) {
        text += len;
    }
    basic_trim(&text);

    params_get(&params);
    
    switch (cmd) {
        case command_aesbits:
            if (! *text) {
                printf("files will be encrypted with:\n");
                printf("  %d bits\n", params.aesbits);
            } else {
                boo = basic_const(&text, &bits);
                if (! boo || (bits != 128 && bits != 192 && bits != 256)) {
                    goto XXX_ERROR_XXX;
                }
                basic_word(&text, "bits");
                if (*text) {
                    goto XXX_ERROR_XXX;
                }
                params.aesbits = bits;
                params_set(&params);
            }
            break;
            
        case command_aeskey:
            if (! *text) {
                params_default_aeskey(&params);
                params_set(&params);
                params_set(&params);  // twice to wipe alternate page
                printf("default key updated.\n");

            } else {
                // get the original key
                getaeskey(&text, &hexkey, params.aeskey);
                
#if ! _WIN32
                if (hexkey) {
                    printf("confirm hexkey:\n? ");
                } else {
                    printf("confirm passphrase:\n? ");
                }
                
                // wait for confirmation
                if (main_command) {
                    main_command = NULL;
                    terminal_command_ack(false);
                }

                while (! main_command) {
                    // NULL
                }

                // get the confirmed key
                confirm = main_command;
#else
                goto XXX_CONFIRMED_XXX;
#endif
                getaeskey(&confirm, &hexkey2, aeskey);
                
                printf("\n");
                if (*confirm || hexkey != hexkey2 || memcmp(params.aeskey, aeskey, sizeof(aeskey))) {
                    printf("confirmation failed; key not updated.\n");
                } else {
XXX_CONFIRMED_XXX:
                    params_set(&params);
                    if (hexkey) {
                        printf("hexkey updated\n");
                    } else {
                        printf("passphrase updated\n");
                    }
                }
            }
            break;
            
        case command_files:
            params_get(&params);
            if (basic_word(&text, "clear")) {
                // clear settings
                memset(params.extensions, 0, sizeof(params.extensions));
                params_set(&params);
                
            } else if (basic_word(&text, "default")) {
                // default settings
                params_default_files(&params);
                params_set(&params);
                
            } else if (! *text) {
                // print extension list
                printf("the following files will be encrypted:\n");
                for (i = 0; i < NEXTS; i++) {
                    if (params.extensions[i][0] != 0 && params.extensions[i][0] != (char)-1) {
                        printf("  *.%s\n", params.extensions[i]);
                    }
                }
                
            } else {
                // add or remove extensions
                if (*text == '+') {
                    add = true;
                } else if (*text == '-') {
                    add = false;
                } else {
                    goto XXX_ERROR_XXX;
                }
                text++;
                
                // get the extension from the command line
                if (! getfiles(&text, extension)) {
                    goto XXX_ERROR_XXX;
                }
                
                if (! strcmp(extension, "REC") || ! strcmp(extension, "TMP")) {
                    goto XXX_ERROR_XXX;
                }
                
                // find it in the current extension list
                j = NEXTS;
                for (i = 0; i < NEXTS; i++) {
                    if (! strcmp(extension, params.extensions[i])) {
                        break;
                    }
                    if (params.extensions[i][0] == 0 || params.extensions[i][0] == (char)-1) {
                        j = i;
                    }
                }
                
                // modify the list accordingly
                if (add && i == NEXTS) {
                    if (j == NEXTS) {
                        printf("too many file extensions\n");
                    } else {
                        strcpy(params.extensions[j], extension);
                        params_set(&params);
                    }
                } else if (add) {
                    printf("extension already exists\n");
                } else if (i == NEXTS) {
                    printf("extension does not exist\n");
                } else {
                    params.extensions[i][0] = '\0';
                    params_set(&params);
                }
            }
            break;

        case command_help:
            if (! *text) {
                help(help_general);
            } else if (basic_word(&text, "about")) {
                help(help_about);
            } else {
                goto XXX_ERROR_XXX;
            }
            break;

        case command_reset:
            if (*text) {
                goto XXX_ERROR_XXX;
            }

#if ! _WIN32
            MCF_RCM_RCR = MCF_RCM_RCR_SOFTRST;
            asm { halt }
#endif
            break;

        case command_select:
            if (! *text) {
                printf("files will be encrypted if:\n");
                printf("  %s\n", params.roflip?"ro (read-only)":"rw (read/write)");
            } else {
                if (basic_word(&text, "ro")) {
                    roflip = true;
                } else if (basic_word(&text, "rw")) {
                    roflip = false;
                } else {
                    goto XXX_ERROR_XXX;
                }
                if (*text) {
                    goto XXX_ERROR_XXX;
                }
                params.roflip = roflip;
                params_set(&params);
            }
            break;
            
        case command_upgrade:
            // upgrade pict-o-crypt!
#if ! _WIN32
            flash_upgrade();
#endif
            break;

        case command_uptime:
            if (*text) {
                goto XXX_ERROR_XXX;
            }

            t = seconds;
            d = t/(24*60*60);
            t = t%(24*60*60);
            h = t/(60*60);
            t = t%(60*60);
            m = t/(60);
            printf("%dd %dh %dm\n", d, h, m);
            break;

        default:
            goto XXX_ERROR_XXX;
            break;
    }

    vsbug++;
    return;

XXX_ERROR_XXX:
    terminal_command_error(text-text_in);
}

