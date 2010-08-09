#include "main.h"

// this file meets bootloader requirements for targets with bootloaders

extern asm void _startup(void);

#if MCF51JM128
#if ! BADGE_BOARD
#error
#endif
#define USER_ENTRY_ADDRESS  0x000039C0
#elif MCF52259
#if ! DEMO_KIT
#error
#endif
#define USER_ENTRY_ADDRESS  0x00004000
#else
#error
#endif

const byte _UserEntry[] @ USER_ENTRY_ADDRESS = {
    0x4E,
    0x71,
    0x4E,
    0xF9  // asm NOP(0x4E71), asm JMP(0x4EF9)           
};

void  (* const _UserEntry2[])()@(USER_ENTRY_ADDRESS+4)=
{
    _startup,
};

extern void pre_main(void);
void pre_main(void)
{
    int i;
    uint32 **v;
    extern far uint32 _swvect[512];  // 8 bytes per entry

#if MCF51JM128
    
    asm (move.l  #0x00800000,d0);
    asm (movec   d0,vbr);

    v = (uint32 **)0x00800000;

    for (i=0; i<128; i++) {
        v[i] = _swvect+2*i;
    }
#else
    asm (move.l  #0x20000000,d0);
    asm (movec   d0,vbr);

    v = (uint32 **)0x20000000;

    for (i=0; i<256; i++) {
        v[i] = _swvect+2*i;
    }
#endif    
}
