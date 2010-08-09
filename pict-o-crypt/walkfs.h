// *** run.h **********************************************************

typedef struct header {
    int filesize;
    char filename[128];
} header_t;

bool
walkfs(int pass);

void aes_pre_enc(uint32 bits, byte *key, byte *chain);

void aes_enc(byte chain[16], int iolen, byte *plain);

void aes_pre_dec(uint32 bits, byte *key, byte *chain);

void aes_dec(byte chain[16], int iolen, byte *cypher);

