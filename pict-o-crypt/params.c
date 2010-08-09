// *** params.c *******************************************************
// this file implements pict-o-crypt flash parameters.

#include "main.h"

#define PARAMS_PAGE1  (uint32 *)(128*1024-FLASH_PAGE_SIZE)
#define PARAMS_PAGE2  (uint32 *)(128*1024-2*FLASH_PAGE_SIZE)

#if ! _WIN32
// the last word of each flash bank is the generation number
#define GENERATION(p)  (((params_t *)(p))->generation)

// we always pick the newer flash bank
#define CURRENT_PARAM_PAGE  ((GENERATION(PARAMS_PAGE1)+1 > GENERATION(PARAMS_PAGE2)+1) ? PARAMS_PAGE1 : PARAMS_PAGE2)
#define ALTERNATE_PARAM_PAGE  ((GENERATION(PARAMS_PAGE1)+1 <= GENERATION(PARAMS_PAGE2)+1) ? PARAMS_PAGE1 : PARAMS_PAGE2)
#else
extern params_t global_params;

#define CURRENT_PARAM_PAGE  (uint32 *)&global_params
#define ALTERNATE_PARAM_PAGE  (uint32 *)&global_params
#endif


void
params_get(OUT params_t *params)
{
    assert(sizeof(*params) <= FLASH_PAGE_SIZE);
    assert(sizeof(*params)%sizeof(int) == 0);
    
    *params = *(params_t *)CURRENT_PARAM_PAGE;
}

void
params_set(IN params_t *params)
{
    uint32 *page;
    
    assert(sizeof(*params) <= FLASH_PAGE_SIZE);
    assert(sizeof(*params)%sizeof(int) == 0);
    
    params->generation++;
    
    page = ALTERNATE_PARAM_PAGE;
    flash_erase_pages(page, 1);
    flash_write_words(page, (uint32 *)params, sizeof(*params)/sizeof(int));
}

void
params_default_aeskey(IN OUT params_t *params)
{
    params->aesbits = 256;
    memset(&params->aeskey, 1, sizeof(params->aeskey));
}

void
params_default_files(IN OUT params_t *params)
{
    memset(params->extensions, 0, sizeof(params->extensions));
    strcpy(params->extensions[0], "JPG");
    strcpy(params->extensions[1], "MOV");
    strcpy(params->extensions[2], "MPG");
    strcpy(params->extensions[3], "MP4");
    strcpy(params->extensions[4], "NEF");
}

