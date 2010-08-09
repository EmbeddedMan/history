// *** params.h *******************************************************

#define NEXTS  16

typedef struct params {
    uint32 roflip;
    uint32 aesbits;
    byte aeskey[AESBITS/8];
    char extensions[NEXTS][4];
    uint32 generation;
} params_t;

void
params_get(OUT params_t *params);

void
params_set(IN params_t *params);

void
params_default_aeskey(IN OUT params_t *params);

void
params_default_files(IN OUT params_t *params);

