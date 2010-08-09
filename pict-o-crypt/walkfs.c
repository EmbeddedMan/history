// *** walkfs.c *******************************************************
// this file walks the filesystem and encrypts elegible files when
// the pict-o-crypt is in run mode.

#include "main.h"

/*
128-bit key "00010203050607080A0B0C0D0F101112",
and an input "506812A45F08C889B97F5980038B8359",
the cipher should be "D8F532538289EF7D06B506A4FD5BE9C9".
*/

static params_t params;

int read_ticks;
int write_ticks;
int aes_enc_ticks;
int encrypt_ticks;
int encrypt_bytes;
int total_ticks;

int nrounds;
unsigned long rk[RKLENGTH(AESBITS)];
    
void aes_pre_enc(uint32 bits, byte *key, byte *chain)
{
    int ms;
    
    ms = ticks;
    
    nrounds = rijndaelSetupEncrypt(rk, key, bits);
    memset(chain, 0, 16);
    
    aes_enc_ticks += ticks-ms;
}

void aes_enc(byte chain[16], int iolen, byte *plain)
{
    int ms;
    byte temp[16];

    ms = ticks;
    
    assert(! (iolen%16));
    while (iolen) {
        ((int *)plain)[0] ^= ((int *)chain)[0];
        ((int *)plain)[1] ^= ((int *)chain)[1];
        ((int *)plain)[2] ^= ((int *)chain)[2];
        ((int *)plain)[3] ^= ((int *)chain)[3];

        rijndaelEncrypt(rk, nrounds, plain, temp);
        
        //memcpy(chain, temp, 16);
        ((int *)chain)[0] = ((int *)temp)[0];
        ((int *)chain)[1] = ((int *)temp)[1];
        ((int *)chain)[2] = ((int *)temp)[2];
        ((int *)chain)[3] = ((int *)temp)[3];
        //memcpy(plain, temp, 16);
        ((int *)plain)[0] = ((int *)temp)[0];
        ((int *)plain)[1] = ((int *)temp)[1];
        ((int *)plain)[2] = ((int *)temp)[2];
        ((int *)plain)[3] = ((int *)temp)[3];
        
        plain += 16;
        assert(iolen >= 16);
        iolen -= 16;
    }
    
    aes_enc_ticks += ticks-ms;
}

#if _WIN32
void aes_pre_dec(uint32 bits, byte *key, byte *chain)
{
    nrounds = rijndaelSetupDecrypt(rk, key, bits);
    memset(chain, 0, 16);
}

void aes_dec(byte chain[16], int iolen, byte *cypher)
{
    int i;
    byte temp[16];
    byte temp2[16];

    assert(! (iolen%16));
    while (iolen) {
        memcpy(temp2, cypher, 16);
        rijndaelDecrypt(rk, nrounds, cypher, temp);
        for (i =0; i < 4; i++) {
            ((int *)temp)[i] ^= ((int *)chain)[i];
        }
        memcpy(cypher, temp, 16);
        memcpy(chain, temp2, 16);
        cypher += 16;
        assert(iolen >= 16);
        iolen -= 16;
    }
}
#endif

static
void
output_file(IN char *rfilename, IN char *extension, OUT char *wfilename)
{
    char *p;
    
    p = strchr(rfilename, '.');
    assert(p && p-rfilename);
    strncpy(wfilename, rfilename, p-rfilename);
    strcpy(wfilename + (p-rfilename), extension);
}

#if ! _WIN32
static
bool
encrypt_file(PVOLINFO vi, byte *scratch, char *rfilename, int rfilesize, char *wfilename, int wfilesize)
{
    int ms;
    int ms2;
    int filesize;
    FILEINFO rfi;
    FILEINFO wfi;
    uint32 actual;
    uint32 result;
    uint32 initial;
    header_t *header;
    byte chain[16];
    
    ms = ticks;
    initial = seconds;    
    
    // open the input file, if specified
    if (rfilename && DFS_OpenFile(vi, (unsigned char *)rfilename, DFS_READ, scratch, &rfi)) {
        return false;
    }
    
    // open the output file
    if (DFS_OpenFile(vi, (unsigned char *)wfilename, DFS_WRITE, scratch, &wfi)) {
        return false;
    }
    
    filesize = 0;
    if (rfilename) {
        // first encrypt the header
        memset(big_buffer, 0, sizeof(big_buffer));
        header = (header_t *)big_buffer;
        header->filesize = rfilesize;
        strcpy(header->filename, rfilename);
        
        aes_pre_enc(params.aesbits, params.aeskey, chain);
        
        // encrypt the next block of data
        aes_enc(chain, sizeof(big_buffer), big_buffer);

        ms2 = ticks;
        if (DFS_WriteFile(&wfi, scratch, big_buffer, &actual, sizeof(big_buffer))) {
            return false;
        }
        write_ticks += ticks-ms2;
        assert(actual == sizeof(big_buffer));
        filesize += actual;
        encrypt_bytes += actual;
    
        // then read all the data of the file
        for (;;) {
            if (panic) {
                return false;
            }
            
            ms2 = ticks;
            result = DFS_ReadFile(&rfi, scratch, big_buffer, &actual, sizeof(big_buffer));
            if (result == DFS_EOF || (! result && ! actual)) {
                break;
            }
            read_ticks += ticks-ms2;
            
            if (result) {
                return false;
            }
            
            assert(actual && actual <= sizeof(big_buffer));
            memset(big_buffer+actual, 0, sizeof(big_buffer)-actual);
            
            // encrypt the next block of data
            aes_enc(chain, sizeof(big_buffer), big_buffer);
            
            ms2 = ticks;
            result = DFS_WriteFile(&wfi, scratch, big_buffer, &actual, sizeof(big_buffer));
            if (result) {
                return false;
            }
            write_ticks += ticks-ms2;
            
            assert(actual == sizeof(big_buffer));
            filesize += actual;
            encrypt_bytes += actual;
        }
    }
    
    // wipe the input file up to its original length
    memset(big_buffer, 0, sizeof(big_buffer));
    while (filesize < wfilesize) {
        if (panic) {
            return false;
        }
        
        ms2 = ticks;
        if (DFS_WriteFile(&wfi, scratch, big_buffer, &actual, sizeof(big_buffer))) {
            return false;
        }
        write_ticks += ticks-ms2;
        
        assert(actual == sizeof(big_buffer));
        filesize += actual;
    }
    
    DFS_HostFlush(0);
    
    encrypt_ticks += ticks-ms;
    
#if ! _WIN32
    led_happy_progress();
#endif
    printf("  %d seconds\n", seconds-initial);
    
    return true;
}

#define DEPTH  5

bool
walkfs(int pass)
{
    int a;
    int e;
    int ms;
    uint32 i;
    uint32 j;
    uint32 rv;
    uint32 pstart;
    uint32 rfilesize;
    VOLINFO vi;
    DIRINFO di;
    DIRENT de;
    bool skip;
    bool first;
    int wfilesize;
    uint32 namei[DEPTH];
    uint32 skips[DEPTH];
    static char dirname[MAX_PATH];
    static char rfilename[MAX_PATH];
    static char wfilename[MAX_PATH];
    static char tfilename[MAX_PATH];
    static byte scratch[SECTOR_SIZE];
    
    ms = ticks;
    
    printf("walkfs\n");

#if ! _WIN32
    while (adc_result[0] < 20000) {
        DFS_HostFlush(0);
        led_sad(code_battery);
        delay(1000);
    }
    led_unknown();
#endif
    
    params_get(&params);
    assert(params.roflip == true || params.roflip == false);

    memset(scratch, -1, sizeof(scratch));  // remove
    pstart = DFS_GetPtnStart(0, scratch, 0, NULL, NULL, NULL);
    if (pstart == DFS_ERRMISC) {
        return 0;
    }

    memset(scratch, -1, sizeof(scratch));  // remove
    if (DFS_GetVolInfo(0, scratch, pstart, &vi)) {
        return 0;
    }
    
    if (! vi.secperclus || (vi.secperclus & (vi.secperclus-1))) {
        printf("invalid fs cluster size %d\n", vi.secperclus);
        return 0;
    }
    
    printf("fs cluster size %d\n", vi.secperclus);

    i = 0;
    namei[i] = 0;
    skips[i] = 0;
    strcpy(dirname, "");
    
    first = 1;
    for (;;) {
XXX_LOOP_XXX:        
        if (panic) {
            return 0;
        }
        
        // open a new directory
        memset(scratch, -1, sizeof(scratch));  // remove
        di.scratch = scratch;
        if (DFS_OpenDir(&vi, (unsigned char *)dirname, &di)) {
            return 0;
        }

        memset(&de, 0, sizeof(de));

        skip = 0;
        for (j = 0; j < skips[i]; j++) {
            if (DFS_GetNext(&vi, &di, &de)) {
                skip = 1;
            }
        }

        // enumerate the directory
        if (! skip && ! DFS_GetNext(&vi, &di, &de)) {
            skips[i]++;
         
            if (! de.name[0] || de.name[0] == '.') {
                continue;
            }

#if ! _WIN32
            while (adc_result[0] < 20000) {
                DFS_HostFlush(0);
                led_sad(code_battery);
                delay(1000);
            }
            led_unknown();

            while ((MCF_GPIO_SETNQ & 0x10) == 0) {
                // we're paused
                DFS_HostFlush(0);
            }
#endif
            
            if (de.attr & ATTR_DIRECTORY) {
                if (i+1 < DEPTH) {
                    namei[i] = strlen(dirname);
                    if (namei[i]) {
                        strcat(dirname, "/");
                    }
                    strncat(dirname, (char *)de.name, 8);
                    tailtrim(dirname);
                    if (strncmp((char *)de.name+8, "   ", 3)) {
                        strcat(dirname, ".");
                        strncat(dirname, (char *)de.name+8, 3);
                        tailtrim(dirname);
                    }
                    i++;
                    skips[i] = 0;
                    printf("dir: '%s'\n", dirname);
                    goto XXX_LOOP_XXX;
                }
            } else if (! (de.attr & (ATTR_HIDDEN|ATTR_SYSTEM|ATTR_VOLUME_ID)) && !!(de.attr & ATTR_READ_ONLY) == params.roflip) {
                // force upper case
                for (a = 0; a < sizeof(de.name); a++) {
                    if (de.name[a] >= 'a' && de.name[a] <= 'z') {
                        de.name[a] = de.name[a] - 'a' + 'A';
                    }
                }
                
                if (pass == 0) {
                    // pass 0: check for extension match for tmp file
                    if (! strncmp("TMP", (char *)de.name+8, 3)) {
                        strcpy(rfilename, dirname);
                        if (strlen(rfilename)) {
                            strcat(rfilename, "/");
                        }
                        strncat(rfilename, (char *)de.name, 8);
                        tailtrim(rfilename);
                        if (strncmp((char *)de.name+8, "   ", 3)) {
                            strcat(rfilename, ".");
                            strncat(rfilename, (char *)de.name+8, 3);
                            tailtrim(rfilename);
                        }

                        // rfilename needs to be purged!
                        
                        rfilesize = (de.filesize_3<<24)|(de.filesize_2<<16)|(de.filesize_1<<8)|de.filesize_0;
                        printf("temp: '%s (%d bytes)'\n", rfilename, rfilesize);

                        // we have to rewrite rfilename
                        rv = encrypt_file(&vi, scratch, NULL, 0, rfilename, rfilesize);
                        if (! rv) {
                            printf("rewrite failed\n");
                            return 0;
                        }
                        
                        // and unlink it
                        rv = DFS_UnlinkFile(&vi, (unsigned char *)rfilename, scratch);
                        if (rv) {
                            printf("unlink failed\n");
                            return 0;
                        }

                        DFS_HostFlush(0);
                    }
                } else {
                    assert(pass == 1);
                    
                    // pass 1: check for extension match for image file
                    for (e = 0; e < NEXTS; e++) {
                        if (params.extensions[e][0] != 0 && params.extensions[e][0] != (char)-1) {
                            if (! strncmp(params.extensions[e], (char *)de.name+8, 3)) {
                                break;
                            }
                        }
                    }
                    
                    if (e != NEXTS) {
                        strcpy(rfilename, dirname);
                        if (strlen(rfilename)) {
                            strcat(rfilename, "/");
                        }
                        strncat(rfilename, (char *)de.name, 8);
                        tailtrim(rfilename);
                        if (strncmp((char *)de.name+8, "   ", 3)) {
                            strcat(rfilename, ".");
                            strncat(rfilename, (char *)de.name+8, 3);
                            tailtrim(rfilename);
                        }
                        
                        // rfilename needs to be encrypted!
                        
                        rfilesize = (de.filesize_3<<24)|(de.filesize_2<<16)|(de.filesize_1<<8)|de.filesize_0;
                        printf("encrypt: '%s (%d bytes)'\n", rfilename, rfilesize);
                        
                        if (first) {
                            // for the first rfilename we create a new (unallocated) .REC file directly
                            output_file(rfilename, ".REC", wfilename);
                            wfilesize = 0;
                        }
                        
                        // we'll encrypt to wfilename
                        
                        rv = encrypt_file(&vi, scratch, rfilename, rfilesize, wfilename, wfilesize);
                        if (! rv) {
                            printf("encryption failed\n");
                            return 0;
                        }
                        
                        // and then we rename wfilename to rfilename with .REC
                        if (! first) {
                            // and then rename the old file
                            output_file(rfilename, ".REC", tfilename);
                            rv = DFS_RenameFile(&vi, (unsigned char *)wfilename, (unsigned char *)tfilename, scratch);
                            if (rv) {
                                printf("rename failed\n");
                                return 0;
                            }
                        }
                        
                        // and rename rfilename to rfilename with .TMP
                        output_file(rfilename, ".TMP", wfilename);
                        wfilesize = rfilesize;
                        rv = DFS_RenameFile(&vi, (unsigned char *)rfilename, (unsigned char *)wfilename, scratch);
                        if (rv) {
                            printf("rename failed\n");
                            return 0;
                        }
                        
                        // we reuse an old (allocated) file
                        first = 0;
                    }
                }
            }
        } else {
            skip = 1;
        }

        if (! skip) {
            // we have more files in this directory
            goto XXX_LOOP_XXX;
        }

        // we're leaving this directory        
        if (pass == 1 && ! first) {
            // lastly we have to rewrite wfilename
            rv = encrypt_file(&vi, scratch, NULL, 0, wfilename, wfilesize);
            if (! rv) {
                printf("rewrite failed\n");
                return 0;
            }
            
            // and unlink it
            rv = DFS_UnlinkFile(&vi, (unsigned char *)wfilename, scratch);
            if (rv) {
                printf("unlink failed\n");
                return 0;
            }

            DFS_HostFlush(0);
            
            first = 1;
        }

        // if this is the root dirtectory...        
        if (! i) {
            // we're done
            break;
        }

        // continue a previous directory
        i--;
        dirname[namei[i]] = '\0';
    }
    
    DFS_HostFlush(0);
    total_ticks += ticks-ms;
    
    return 1;
}
#endif

