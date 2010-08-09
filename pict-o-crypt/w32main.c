#include <windows.h>
#include "main.h"

byte big_buffer[8192];
byte huge_buffer[65536];

void
flash_erase_pages(uint32 *addr, uint32 npages)
{
}

void
flash_write_words(uint32 *addr, uint32 *data, uint32 nwords)
{
    memcpy(addr, data, nwords*4);
}

bool error;

void terminal_command_error(int position)
{
    error = true;
}

params_t global_params;

char *source;
char *dest;

void
usage(char *argv0)
{
    printf("usage: %s [-b <aesbits>] [-k <aeskey>] <sourcedir> <destdir>\n", argv0);
    printf("v" VERSION "\n");
}

char *
getarg(int argc, char **argv, int *optind)
{
    assert(argv[*optind][0] == '-');

    if (argv[*optind][2] == '\0') {
        if (*optind+1 < argc) {
            *optind = *optind+1;
            return argv[*optind];
        }
    } else {
        return argv[*optind]+2;
    }
    return NULL;
}

bool
make_dirs(char *path)
{
    char *p;
    char *q;
    char *end;
    char *next;
    BOOL boo;

    next = path;
    for (;;) {
        p = strchr(next, '/');
        q = strchr(next, '\\');
        if (p && q) {
            end = MIN(p, q);
        } else if (p) {
            end = p;
        } else if (q) {
            end = q;
        } else {
            break;
        }
        if (end > path && *(end-1) != ':') {
            *end = '\0';
            boo = CreateDirectory(path, NULL);
            *end = '\\';
            if (! boo && GetLastError() != ERROR_ALREADY_EXISTS) {
                return false;
            }
        }
        next = end+1;
    }

    return true;
}

bool
decrypt_file(char *source, char *file, char *dest)
{
    int n;
    char *p;
    BOOL boo;
    HANDLE r;
    HANDLE w;
    DWORD actual;
    DWORD remain;
    DWORD partial;
    DWORD wsize;
    char *wname;
    byte chain[16];
    char rpath[1024];
    char wpath[1024];

    strcpy(rpath, source);
    n = strlen(rpath);
    if (! n || rpath[n-1] != '\\') {
        strcat(rpath, "\\");
    }
    strcat(rpath, file);

    // open the input file
    r = CreateFile(rpath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    assert(r != INVALID_HANDLE_VALUE);

    aes_pre_dec(global_params.aesbits, global_params.aeskey, chain);

    // first decrypt the header
    boo = ReadFile(r, big_buffer, sizeof(big_buffer), &actual, NULL);
    assert(boo);

    if (actual < sizeof(big_buffer)) {
        printf("Decryption failed (size check)\n");
        return false;
    }

    aes_dec(chain, sizeof(big_buffer), big_buffer);

    wsize = W32BYTESWAP(*(int *)big_buffer);
    wname = big_buffer+4;

    n = strlen(wname);
    for (p = big_buffer+4+n; p < big_buffer+sizeof(big_buffer); p++) {
        if (*p) {
            break;
        }
    }

    if (p != big_buffer+sizeof(big_buffer)) {
        printf("Decryption failed (zero check)\n");
        return false;
    }

    printf("decrypting %s (%d bytes)\r\n", wname, wsize);
    strcpy(wpath, dest);
    n = strlen(wpath);
    if (! n || wpath[n-1] != '\\') {
        strcat(wpath, "\\");
    }
    strcat(wpath, wname);

    boo = make_dirs(wpath);
    if (! boo) {
        printf("Cannot create directory (0x%x)\n", GetLastError());
        return false;
    }

    // open the output file
    w = CreateFile(wpath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (w == INVALID_HANDLE_VALUE) {
        printf("Cannot create file (0x%x)\n", GetLastError());
        return false;
    }

    remain = wsize;

    while (remain) {
        partial = MIN(remain, sizeof(huge_buffer));
        assert(((partial+15)&~15) < sizeof(huge_buffer));

        boo = ReadFile(r, huge_buffer, (partial+15)&~15, &actual, NULL);
        assert(boo);
        if (actual < ((partial+15)&~15)) {
            printf("Decryption failed (size check)\n");
            return false;
        }

        aes_dec(chain, (partial+15)&~15, huge_buffer);

        boo = WriteFile(w, huge_buffer, partial, &actual, NULL);
        if (! boo) {
            printf("Cannot write file (0x%x)\n", GetLastError());
            return false;
        }
        if (actual != partial) {
            printf("Cannot write file\n");
            return false;
        }

        assert(partial <= remain);
        remain -= partial;
    }

    CloseHandle(w);
    CloseHandle(r);

    (void)DeleteFile(rpath);

    return true;
}

bool
decrypt_dir(char *source, char *dest)
{
    int n;
    bool boo;
    HANDLE h;
    WIN32_FIND_DATA ffd;
    char subsource[1024];
    char pattern[1024];

    strcpy(pattern, source);
    strcat(pattern, "\\*");

    h = FindFirstFile(pattern, &ffd);
    if (h == INVALID_HANDLE_VALUE) {
        printf("Cannot open source directory (0x%x)\n", GetLastError());
        return false;
    }

    do {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (ffd.cFileName[0] != '.') {
                strcpy(subsource, source);
                strcat(subsource, "\\");
                strcat(subsource, ffd.cFileName);
                boo = decrypt_dir(subsource, dest);
                if (! boo) {
                    return boo;
                }
            }
        } else {
            n = strlen(ffd.cFileName);
            if (n >= 4 && ! strcmp(ffd.cFileName+n-4, ".REC")) {
                if (! decrypt_file(source, ffd.cFileName, dest)) {
                    return false;
                }
            }
        }
    } while (FindNextFile(h, &ffd));
 
    if (GetLastError() != ERROR_NO_MORE_FILES) {
        return false;
    }

    FindClose(h);

    return true;
}

void
memxor(byte *p1, byte *p2, byte *p3, int n)
{
    while (n--) {
        *p1++ = *p2++ ^ *p3++;
    }
}

int aestest(void)
{
    int j;
    char *text;
    bool hexkey;
    byte key[32];
    byte chain[32];  // only 16 bytes used
    byte plain[32];  // only 16 bytes used
    byte cipher[32];  // only 16 bytes used
    byte cv[32];  // only 16 bytes used
    byte ib[32];  // only 16 bytes used
    byte old[32];  // only 16 bytes used
    byte temp[32];  // only 16 bytes used
    char buffer[65];

    // wikipedia

    strcpy(buffer, "00010203050607080A0B0C0D0F101112");
    text = buffer;
    getaeskey(&text, &hexkey, key);
    assert(! *text && hexkey);

    strcpy(buffer, "00000000000000000000000000000000");
    text = buffer;
    getaeskey(&text, &hexkey, chain);
    assert(! *text && hexkey);

    strcpy(buffer, "506812A45F08C889B97F5980038B8359");
    text = buffer;
    getaeskey(&text, &hexkey, plain);
    assert(! *text && hexkey);

    strcpy(buffer, "D8F532538289EF7D06B506A4FD5BE9C9");
    text = buffer;
    getaeskey(&text, &hexkey, cipher);
    assert(! *text && hexkey);

    aes_pre_enc(128, key, chain);

    assert(memcmp(plain, cipher, 16));
    aes_enc(chain, 16, plain);
    assert(! memcmp(plain, cipher, 16));
    if (memcmp(plain, cipher, 16)) {
        printf("128 bit failed (wiki)\n");
    } else {
        printf("128 bit succeeded (wiki)\n");
    }

    // nist 128

    strcpy(buffer, "000102030405060708090A0B0C0D0E0F");
    text = buffer;
    getaeskey(&text, &hexkey, key);
    assert(! *text && hexkey);

    strcpy(buffer, "00000000000000000000000000000000");
    text = buffer;
    getaeskey(&text, &hexkey, chain);
    assert(! *text && hexkey);

    strcpy(buffer, "000102030405060708090A0B0C0D0E0F");
    text = buffer;
    getaeskey(&text, &hexkey, plain);
    assert(! *text && hexkey);

    strcpy(buffer, "0A940BB5416EF045F1C39458C653EA5A");
    text = buffer;
    getaeskey(&text, &hexkey, cipher);
    assert(! *text && hexkey);

    aes_pre_enc(128, key, chain);

    assert(memcmp(plain, cipher, 16));
    aes_enc(chain, 16, plain);
    assert(! memcmp(plain, cipher, 16));
    if (memcmp(plain, cipher, 16)) {
        printf("128 bit failed\n");
    } else {
        printf("128 bit succeeded\n");
    }

    // nist 192

    strcpy(buffer, "000102030405060708090A0B0C0D0E0F1011121314151617");
    text = buffer;
    getaeskey(&text, &hexkey, key);
    assert(! *text && hexkey);

    strcpy(buffer, "00000000000000000000000000000000");
    text = buffer;
    getaeskey(&text, &hexkey, chain);
    assert(! *text && hexkey);

    strcpy(buffer, "000102030405060708090A0B0C0D0E0F");
    text = buffer;
    getaeskey(&text, &hexkey, plain);
    assert(! *text && hexkey);

    strcpy(buffer, "0060BFFE46834BB8DA5CF9A61FF220AE");
    text = buffer;
    getaeskey(&text, &hexkey, cipher);
    assert(! *text && hexkey);

    aes_pre_enc(192, key, chain);

    assert(memcmp(plain, cipher, 16));
    aes_enc(chain, 16, plain);
    assert(! memcmp(plain, cipher, 16));
    if (memcmp(plain, cipher, 16)) {
        printf("192 bit failed\n");
    } else {
        printf("192 bit succeeded\n");
    }

    // nist 256

    strcpy(buffer, "000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F");
    text = buffer;
    getaeskey(&text, &hexkey, key);
    assert(! *text && hexkey);

    strcpy(buffer, "00000000000000000000000000000000");
    text = buffer;
    getaeskey(&text, &hexkey, chain);
    assert(! *text && hexkey);

    strcpy(buffer, "000102030405060708090A0B0C0D0E0F");
    text = buffer;
    getaeskey(&text, &hexkey, plain);
    assert(! *text && hexkey);

    strcpy(buffer, "5A6E045708FB7196F02E553D02C3A692");
    text = buffer;
    getaeskey(&text, &hexkey, cipher);
    assert(! *text && hexkey);

    aes_pre_enc(256, key, chain);

    assert(memcmp(plain, cipher, 16));
    aes_enc(chain, 16, plain);
    assert(! memcmp(plain, cipher, 16));
    if (memcmp(plain, cipher, 16)) {
        printf("256 bit failed\n");
    } else {
        printf("256 bit succeeded\n");
    }

    // nist 256 monticarlo

/*
KEYSIZE=256

I=0
KEY=0000000000000000000000000000000000000000000000000000000000000000
IV=00000000000000000000000000000000
PT=00000000000000000000000000000000
CT=FE3C53653E2F45B56FCD88B2CC898FF0

I=1
KEY=B2493DE29713367D9FAA93469F8EF596FE3C53653E2F45B56FCD88B2CC898FF0
IV=FE3C53653E2F45B56FCD88B2CC898FF0
PT=B2493DE29713367D9FAA93469F8EF596
CT=7CE2ABAF8BEF23C4816DC8CE842048A7

I=2
KEY=33A36646FE56F70DC0C51A3117E639F182DEF8CAB5C06671EEA0407C48A9C757
IV=7CE2ABAF8BEF23C4816DC8CE842048A7
PT=81EA5BA46945C1705F6F89778868CC67
CT=50CD14A12C6852D39654C816BFAF9AC2
*/

    strcpy(buffer, "0000000000000000000000000000000000000000000000000000000000000000");
    text = buffer;
    getaeskey(&text, &hexkey, key);
    assert(! *text && hexkey);

    strcpy(buffer, "00000000000000000000000000000000");
    text = buffer;
    getaeskey(&text, &hexkey, chain);
    assert(! *text && hexkey);

    strcpy(buffer, "00000000000000000000000000000000");
    text = buffer;
    getaeskey(&text, &hexkey, plain);
    assert(! *text && hexkey);

    strcpy(buffer, "FE3C53653E2F45B56FCD88B2CC898FF0");
    text = buffer;
    getaeskey(&text, &hexkey, cipher);
    assert(! *text && hexkey);

    aes_pre_enc(256, key, chain);

    assert(memcmp(plain, cipher, 16));

    memcpy(cv, chain, sizeof(cv));
    for (j = 0; j < 10000; j++) {
        memxor(ib, plain, cv, sizeof(ib));
        aes_enc(chain, 16, plain);
        memcpy(temp, plain, sizeof(temp));
        if (j == 0) {
            memcpy(plain, cv, sizeof(plain));
        } else {
            memcpy(plain, old, sizeof(old));
        }
        memcpy(old, temp, sizeof(old));
        memcpy(cv, temp, sizeof(cv));
    }

    assert(! memcmp(old, cipher, 16));
    if (memcmp(old, cipher, 16)) {
        printf("256 bit monticarlo failed\n");
    } else {
        printf("256 bit monticarlo succeeded\n");
    }

    return 0;
}

int
main(int argc, char **argv)
{
    int optind;
    static char command[BASIC_LINE_SIZE];

    params_default_aeskey(&global_params);
    params_default_files(&global_params);

    // N.B. this code goes thru the admin_run() interface to reuse
    // as much of the PoC code as possible, to help test it.

    if (argc == 2 && ! strcmp(argv[1], "-t")) {
        return aestest();
    }

    optind = 1;
    while (argc > optind && argv[optind][0] == '-') {
        switch (argv[optind][1]) {
            case 'b':
                sprintf(command, "aesbits %s", getarg(argc, argv, &optind));
                admin_run(command);
                if (error) {
                    usage(argv[0]);
                    return 1;
                }
                break;
            case 'k':
                sprintf(command, "aeskey %s", getarg(argc, argv, &optind));
                admin_run(command);
                if (error) {
                    usage(argv[0]);
                    return 1;
                }
                break;
            default:
                usage(argv[0]);
                return 1;
        }
        optind++;
    }

    if (optind < argc) {
        source = argv[optind++];
    } else {
        source = ".";
    }

    if (optind < argc) {
        dest = argv[optind++];
    } else {
        dest = ".";
    }

    if (optind < argc) {
        usage(argv[0]);
        return 1;
    }

    if (decrypt_dir(source, dest)) {
        return 0;
    }
    return 1;
}

