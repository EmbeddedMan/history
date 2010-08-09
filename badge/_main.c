#include "main.h"

typedef void (* pFun)(void);
extern asm void _startup(void);

#define USER_ENTRY_ADDRESS      0x0039C0
#define REDIRECT_VECTORS        0x003800

const byte _UserEntry[] @ USER_ENTRY_ADDRESS = {
  0x4E,
  0x71,
  0x4E,
  0xF9           //asm NOP(0x4E71), asm JMP(0x4EF9)           
};

void  (* const _UserEntry2[])()@(USER_ENTRY_ADDRESS+4)=
{
  _startup,
};


static
__interrupt void dummy_ISR() 
{
    uint32 dummy = 0;
    extern void led_line(int);
    
    led_hex(*(&dummy+7));
}


void  (* const RAM_vector[])()@REDIRECT_VECTORS= {
    
    (pFun)&dummy_ISR,              // vector_0  INITSP
    (pFun)&dummy_ISR,             // vector_1  INITPC
    (pFun)&dummy_ISR,             // vector_2  Vaccerr
    (pFun)&dummy_ISR,             // vector_3  Vadderr
    (pFun)&dummy_ISR,             // vector_4  Viinstr
    (pFun)&dummy_ISR,             // vector_5  VReserved5
    (pFun)&dummy_ISR,             // vector_6  VReserved6
    (pFun)&dummy_ISR,             // vector_7  VReserved7
    (pFun)&dummy_ISR,             // vector_8  Vprviol
    (pFun)&dummy_ISR,             // vector_9  Vtrace
    (pFun)&dummy_ISR,             // vector_10 Vunilaop
    (pFun)&dummy_ISR,             // vector_11 Vunilfop
    (pFun)&dummy_ISR,             // vector_12 Vdbgi
    (pFun)&dummy_ISR,             // vector_13 VReserved13
    (pFun)&dummy_ISR,             // vector_14 Vferror
    (pFun)&dummy_ISR,             // vector_15 VReserved15
    (pFun)&dummy_ISR,             // vector_16 VReserved16
    (pFun)&dummy_ISR,             // vector_17 VReserved17
    (pFun)&dummy_ISR,             // vector_18 VReserved18
    (pFun)&dummy_ISR,             // vector_19 VReserved19
    (pFun)&dummy_ISR,             // vector_20 VReserved20
    (pFun)&dummy_ISR,             // vector_21 VReserved21
    (pFun)&dummy_ISR,             // vector_22 VReserved22
    (pFun)&dummy_ISR,             // vector_23 VReserved23
    (pFun)&dummy_ISR,             // vector_24 Vspuri
    (pFun)&dummy_ISR,             // vector_25 VReserved25
    (pFun)&dummy_ISR,             // vector_26 VReserved26
    (pFun)&dummy_ISR,             // vector_27 VReserved27 
    (pFun)&dummy_ISR,             // vector_28 VReserved28
    (pFun)&dummy_ISR,             // vector_29 VReserved29
    (pFun)&dummy_ISR,             // vector_30 VReserved30
    (pFun)&dummy_ISR,             // vector_31 VReserved31
    (pFun)&dummy_ISR,             // vector_32 Vtrap0
    (pFun)&dummy_ISR,             // vector_33 Vtrap1
    (pFun)&dummy_ISR,             // vector_34 Vtrap2 
    (pFun)&dummy_ISR,             // vector_35 Vtrap3
    (pFun)&dummy_ISR,             // vector_36 Vtrap4
    (pFun)&dummy_ISR,             // vector_37 Vtrap5
    (pFun)&dummy_ISR,             // vector_38 Vtrap6
    (pFun)&dummy_ISR,             // vector_39 Vtrap7
    (pFun)&dummy_ISR,             // vector_40 Vtrap8
    (pFun)&dummy_ISR,             // vector_41 Vtrap9 
    (pFun)&dummy_ISR,             // vector_42 Vtrap10
    (pFun)&dummy_ISR,             // vector_43 Vtrap11
    (pFun)&dummy_ISR,             // vector_44 Vtrap12
    (pFun)&dummy_ISR,             // vector_45 Vtrap13 
    (pFun)&dummy_ISR,             // vector_46 Vtrap14
    (pFun)&dummy_ISR,             // vector_47 Vtrap15
    (pFun)&dummy_ISR,             // vector_48 VReserved48
    (pFun)&dummy_ISR,             // vector_49 VReserved49
    (pFun)&dummy_ISR,             // vector_50 VReserved50
    (pFun)&dummy_ISR,             // vector_51 VReserved51
    (pFun)&dummy_ISR,             // vector_52 VReserved52
    (pFun)&dummy_ISR,             // vector_53 VReserved53
    (pFun)&dummy_ISR,             // vector_54 VReserved54
    (pFun)&dummy_ISR,             // vector_55 VReserved55
    (pFun)&dummy_ISR,             // vector_56 VReserved56
    (pFun)&dummy_ISR,             // vector_57 VReserved57
    (pFun)&dummy_ISR,             // vector_58 VReserved58
    (pFun)&dummy_ISR,             // vector_59 VReserved59
    (pFun)&dummy_ISR,             // vector_60 VReserved60
    (pFun)&dummy_ISR,             // vector_61 Vunsinstr
    (pFun)&dummy_ISR,             // vector_62 VReserved62
    (pFun)&dummy_ISR,             // vector_63 VReserved63
    (pFun)zb_pre_isr,             // vector_64 Virq
    (pFun)&dummy_ISR,             // vector_65 Vlvd
    (pFun)&dummy_ISR,             // vector_66 Vlol
    (pFun)&dummy_ISR,             // vector_67 Vspi1
    (pFun)&dummy_ISR,             // vector_68 Vspi2 
    (pFun)usb_isr,                // vector_69 Vusb
    (pFun)&dummy_ISR,             // vector_70 VReserved70
    (pFun)&dummy_ISR,             // vector_71 Vtpm1ch0
    (pFun)&dummy_ISR,             // vector_72 Vtpm1ch1
    (pFun)&dummy_ISR,             // vector_73 Vtpm1ch2
    (pFun)&dummy_ISR,             // vector_74 Vtpm1ch3 
    (pFun)&dummy_ISR,             // vector_75 Vtpm1ch4
    (pFun)&dummy_ISR,             // vector_76 Vtpm1ch5
    (pFun)&dummy_ISR,             // vector_77 Vtpm1ovf
    (pFun)&dummy_ISR,             // vector_78 Vtpm2ch0
    (pFun)&dummy_ISR,             // vector_79 Vtpm2ch1
    (pFun)&dummy_ISR,             // vector_80 Vtpm2ovf
    (pFun)&dummy_ISR,             // vector_81 Vsci1err
    (pFun)&dummy_ISR,             // vector_82 Vsci1rx
    (pFun)&dummy_ISR,             // vector_83 Vsci1tx
    (pFun)&dummy_ISR,             // vector_84 Vsci2err
    (pFun)&dummy_ISR,             // vector_85 Vsci2rx
    (pFun)&dummy_ISR,             // vector_86 Vsci2tx
    (pFun)&dummy_ISR,             // vector_87 Vkeyboard
    (pFun)&dummy_ISR,             // vector_88 Vadc
    (pFun)&dummy_ISR,             // vector_89 Vacmpx
    (pFun)&dummy_ISR,             // vector_90 Viic1x
    (pFun)timer_isr,              // vector_91 Vrtc
    (pFun)&dummy_ISR,             // vector_92 Viic2x
    (pFun)&dummy_ISR,             // vector_93 Vcmt
    (pFun)&dummy_ISR,             // vector_94 Vcanwu
    (pFun)&dummy_ISR,             // vector_95 Vcanerr 
    (pFun)&dummy_ISR,             // vector_96 Vcanrx
    (pFun)&dummy_ISR,             // vector_97 Vcantx
    (pFun)&dummy_ISR,             // vector_98 Vrnga
    (pFun)&dummy_ISR,             // vector_99 VReserved99
    (pFun)&dummy_ISR,             // vector_100 VReserved100
    (pFun)&dummy_ISR,             // vector_101 VReserved101 
    (pFun)&dummy_ISR,             // vector_102 VReserved102
    (pFun)&dummy_ISR,             // vector_103 VReserved103
    (pFun)&dummy_ISR,             // vector_104 VL7swi 
    (pFun)&dummy_ISR,             // vector_105 VL6swi
    (pFun)&dummy_ISR,             // vector_106 VL5swi
    (pFun)zb_isr,                 // vector_107 VL4swi
    (pFun)&dummy_ISR,             // vector_108 VL3swi
    (pFun)&dummy_ISR,             // vector_109 VL2swi
    (pFun)&dummy_ISR,             // vector_110 VL1swi
};


void pre_main(void) {

  /* !! This section needs to be here to redirect interrupt vectors !! */
  dword *pdst,*psrc;
  word i;
  //asm (jmp _Entry);
   
  
  asm (move.l  #0x00800000,d0);
  asm (movec   d0,vbr);
  
  pdst=(dword*)0x00800000;
  psrc=(dword*)&RAM_vector;
  
  for (i=0;i<111;i++,pdst++,psrc++)
  {
    *pdst=*psrc;
  }
}


