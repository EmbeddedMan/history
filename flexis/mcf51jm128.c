/* Based on CPU DB MCF51JM128_80, version 3.00.031 (RegistersPrg V2.23) */
/* DataSheet : MCF51JM128RM Rev. 1 Draft A 1/2008 */

#include <mcf51jm128.h>

/*lint -save -esym(765, *) */


/* * * * *  8-BIT REGISTERS  * * * * * * * * * * * * * * * */
/* NVFTRIM - macro for reading non volatile register       Nonvolatile MCG Fine Trim; 0x000003FE */
/* Tip for register initialization in the user code:  const byte NVFTRIM_INIT @0x000003FE = <NVFTRIM_INITVAL>; */
/* NVMCGTRM - macro for reading non volatile register      Nonvolatile MCG Trim Register; 0x000003FF */
/* Tip for register initialization in the user code:  const byte NVMCGTRM_INIT @0x000003FF = <NVMCGTRM_INITVAL>; */
/* NVBACKKEY0 - macro for reading non volatile register    Backdoor Comparison Key 0; 0x00000400 */
/* Tip for register initialization in the user code:  const byte NVBACKKEY0_INIT @0x00000400 = <NVBACKKEY0_INITVAL>; */
/* NVBACKKEY1 - macro for reading non volatile register    Backdoor Comparison Key 1; 0x00000401 */
/* Tip for register initialization in the user code:  const byte NVBACKKEY1_INIT @0x00000401 = <NVBACKKEY1_INITVAL>; */
/* NVBACKKEY2 - macro for reading non volatile register    Backdoor Comparison Key 2; 0x00000402 */
/* Tip for register initialization in the user code:  const byte NVBACKKEY2_INIT @0x00000402 = <NVBACKKEY2_INITVAL>; */
/* NVBACKKEY3 - macro for reading non volatile register    Backdoor Comparison Key 3; 0x00000403 */
/* Tip for register initialization in the user code:  const byte NVBACKKEY3_INIT @0x00000403 = <NVBACKKEY3_INITVAL>; */
/* NVBACKKEY4 - macro for reading non volatile register    Backdoor Comparison Key 4; 0x00000404 */
/* Tip for register initialization in the user code:  const byte NVBACKKEY4_INIT @0x00000404 = <NVBACKKEY4_INITVAL>; */
/* NVBACKKEY5 - macro for reading non volatile register    Backdoor Comparison Key 5; 0x00000405 */
/* Tip for register initialization in the user code:  const byte NVBACKKEY5_INIT @0x00000405 = <NVBACKKEY5_INITVAL>; */
/* NVBACKKEY6 - macro for reading non volatile register    Backdoor Comparison Key 6; 0x00000406 */
/* Tip for register initialization in the user code:  const byte NVBACKKEY6_INIT @0x00000406 = <NVBACKKEY6_INITVAL>; */
/* NVBACKKEY7 - macro for reading non volatile register    Backdoor Comparison Key 7; 0x00000407 */
/* Tip for register initialization in the user code:  const byte NVBACKKEY7_INIT @0x00000407 = <NVBACKKEY7_INITVAL>; */
/* NVPROT - macro for reading non volatile register        Nonvolatile Flash Protection Register; 0x0000040D */
/* Tip for register initialization in the user code:  const byte NVPROT_INIT @0x0000040D = <NVPROT_INITVAL>; */
/* NVOPT - macro for reading non volatile register         Nonvolatile Flash Options Register; 0x0000040F */
/* Tip for register initialization in the user code:  const byte NVOPT_INIT @0x0000040F = <NVOPT_INITVAL>; */
volatile PTADSTR _PTAD;                                    /* Port A Data Register; 0xFFFF8000 */
volatile PTADDSTR _PTADD;                                  /* Port A Data Direction Register; 0xFFFF8001 */
volatile PTBDSTR _PTBD;                                    /* Port B Data Register; 0xFFFF8002 */
volatile PTBDDSTR _PTBDD;                                  /* Port B Data Direction Register; 0xFFFF8003 */
volatile PTCDSTR _PTCD;                                    /* Port C Data Register; 0xFFFF8004 */
volatile PTCDDSTR _PTCDD;                                  /* Port C Data Direction Register; 0xFFFF8005 */
volatile PTDDSTR _PTDD;                                    /* Port D Data Register; 0xFFFF8006 */
volatile PTDDDSTR _PTDDD;                                  /* Port D Data Direction Register; 0xFFFF8007 */
volatile PTEDSTR _PTED;                                    /* Port E Data Register; 0xFFFF8008 */
volatile PTEDDSTR _PTEDD;                                  /* Port E Data Direction Register; 0xFFFF8009 */
volatile PTFDSTR _PTFD;                                    /* Port F Data Register; 0xFFFF800A */
volatile PTFDDSTR _PTFDD;                                  /* Port F Data Direction Register; 0xFFFF800B */
volatile PTGDSTR _PTGD;                                    /* Port G Data Register; 0xFFFF800C */
volatile PTGDDSTR _PTGDD;                                  /* Port G Data Direction Register; 0xFFFF800D */
volatile ACMPSCSTR _ACMPSC;                                /* ACMP Status and Control Register; 0xFFFF800E */
volatile ADCSC1STR _ADCSC1;                                /* Status and Control Register 1; 0xFFFF8010 */
volatile ADCSC2STR _ADCSC2;                                /* Status and Control Register 2; 0xFFFF8011 */
volatile ADCCFGSTR _ADCCFG;                                /* Configuration Register; 0xFFFF8016 */
volatile APCTL1STR _APCTL1;                                /* Pin Control 1 Register; 0xFFFF8017 */
volatile APCTL2STR _APCTL2;                                /* Pin Control 2 Register; 0xFFFF8018 */
volatile IRQSCSTR _IRQSC;                                  /* Interrupt request status and control register; 0xFFFF801B */
volatile KBI1SCSTR _KBI1SC;                                /* KBI1 Status and Control Register; 0xFFFF801C */
volatile KBI1PESTR _KBI1PE;                                /* KBI1 Pin Enable Register; 0xFFFF801D */
volatile KBI1ESSTR _KBI1ES;                                /* KBI1 Edge Select Register; 0xFFFF801E */
volatile TPM1SCSTR _TPM1SC;                                /* TPM1 Status and Control Register; 0xFFFF8020 */
volatile TPM1C0SCSTR _TPM1C0SC;                            /* TPM1 Timer Channel 0 Status and Control Register; 0xFFFF8025 */
volatile TPM1C1SCSTR _TPM1C1SC;                            /* TPM1 Timer Channel 1 Status and Control Register; 0xFFFF8028 */
volatile TPM1C2SCSTR _TPM1C2SC;                            /* TPM1 Timer Channel 2 Status and Control Register; 0xFFFF802B */
volatile TPM1C3SCSTR _TPM1C3SC;                            /* TPM1 Timer Channel 3 Status and Control Register; 0xFFFF802E */
volatile TPM1C4SCSTR _TPM1C4SC;                            /* TPM1 Timer Channel 4 Status and Control Register; 0xFFFF8031 */
volatile TPM1C5SCSTR _TPM1C5SC;                            /* TPM1 Timer Channel 5 Status and Control Register; 0xFFFF8034 */
volatile SCI1C1STR _SCI1C1;                                /* SCI1 Control Register 1; 0xFFFF803A */
volatile SCI1C2STR _SCI1C2;                                /* SCI1 Control Register 2; 0xFFFF803B */
volatile SCI1S1STR _SCI1S1;                                /* SCI1 Status Register 1; 0xFFFF803C */
volatile SCI1S2STR _SCI1S2;                                /* SCI1 Status Register 2; 0xFFFF803D */
volatile SCI1C3STR _SCI1C3;                                /* SCI1 Control Register 3; 0xFFFF803E */
volatile SCI1DSTR _SCI1D;                                  /* SCI1 Data Register; 0xFFFF803F */
volatile SCI2C1STR _SCI2C1;                                /* SCI2 Control Register 1; 0xFFFF8042 */
volatile SCI2C2STR _SCI2C2;                                /* SCI2 Control Register 2; 0xFFFF8043 */
volatile SCI2S1STR _SCI2S1;                                /* SCI2 Status Register 1; 0xFFFF8044 */
volatile SCI2S2STR _SCI2S2;                                /* SCI2 Status Register 2; 0xFFFF8045 */
volatile SCI2C3STR _SCI2C3;                                /* SCI2 Control Register 3; 0xFFFF8046 */
volatile SCI2DSTR _SCI2D;                                  /* SCI2 Data Register; 0xFFFF8047 */
volatile MCGC1STR _MCGC1;                                  /* MCG Control Register 1; 0xFFFF8048 */
volatile MCGC2STR _MCGC2;                                  /* MCG Control Register 2; 0xFFFF8049 */
volatile MCGTRMSTR _MCGTRM;                                /* MCG Trim Register; 0xFFFF804A */
volatile MCGSCSTR _MCGSC;                                  /* MCG Status and Control Register; 0xFFFF804B */
volatile MCGC3STR _MCGC3;                                  /* MCG Control Register 3; 0xFFFF804C */
volatile MCGC4STR _MCGC4;                                  /* MCG Control Register 4; 0xFFFF804D */
volatile SPI1C1STR _SPI1C1;                                /* SPI1 Control Register 1; 0xFFFF8050 */
volatile SPI1C2STR _SPI1C2;                                /* SPI1 Control Register 2; 0xFFFF8051 */
volatile SPI1BRSTR _SPI1BR;                                /* SPI1 Baud Rate Register; 0xFFFF8052 */
volatile SPI1SSTR _SPI1S;                                  /* SPI1 Status Register; 0xFFFF8053 */
volatile IIC1ASTR _IIC1A;                                  /* IIC Address Register; 0xFFFF8058 */
volatile IIC1FSTR _IIC1F;                                  /* IIC Frequency Divider Register; 0xFFFF8059 */
volatile IIC1C1STR _IIC1C1;                                /* IIC Control Register 1; 0xFFFF805A */
volatile IIC1SSTR _IIC1S;                                  /* IIC Status Register; 0xFFFF805B */
volatile IIC1DSTR _IIC1D;                                  /* IIC Data I/O Register; 0xFFFF805C */
volatile IIC1C2STR _IIC1C2;                                /* IIC Control Register 2; 0xFFFF805D */
volatile TPM2SCSTR _TPM2SC;                                /* TPM2 Status and Control Register; 0xFFFF8060 */
volatile TPM2C0SCSTR _TPM2C0SC;                            /* TPM2 Timer Channel 0 Status and Control Register; 0xFFFF8065 */
volatile TPM2C1SCSTR _TPM2C1SC;                            /* TPM2 Timer Channel 1 Status and Control Register; 0xFFFF8068 */
volatile RTCSCSTR _RTCSC;                                  /* RTC Status and Control Register; 0xFFFF806C */
volatile RTCCNTSTR _RTCCNT;                                /* RTC Counter Register; 0xFFFF806D */
volatile RTCMODSTR _RTCMOD;                                /* RTC Modulo Register; 0xFFFF806E */
volatile SPI2C1STR _SPI2C1;                                /* SPI2 Control Register 1; 0xFFFF8070 */
volatile SPI2C2STR _SPI2C2;                                /* SPI2 Control Register 2; 0xFFFF8071 */
volatile SPI2BRSTR _SPI2BR;                                /* SPI2 Baud Rate Register; 0xFFFF8072 */
volatile SPI2SSTR _SPI2S;                                  /* SPI2 Status Register; 0xFFFF8073 */
volatile SPI2C3STR _SPI2C3;                                /* SPI2 Control Register 3; 0xFFFF8078 */
volatile SRSSTR _SRS;                                      /* System Reset Status Register; 0xFFFF9800 */
volatile SOPT1STR _SOPT1;                                  /* System Options Register 1; 0xFFFF9802 */
volatile SOPT2STR _SOPT2;                                  /* System Options Register 2; 0xFFFF9803 */
volatile SPMSC1STR _SPMSC1;                                /* System Power Management Status and Control 1 Register; 0xFFFF9809 */
volatile SPMSC2STR _SPMSC2;                                /* System Power Management Status and Control 2 Register; 0xFFFF980A */
volatile SCGC1STR _SCGC1;                                  /* System Clock Gating Control 1 Register; 0xFFFF980B */
volatile SCGC2STR _SCGC2;                                  /* System Clock Gating Control 2 Register; 0xFFFF980C */
volatile SCGC3STR _SCGC3;                                  /* System Clock Gating Control 3 Register; 0xFFFF980D */
volatile SOPT3STR _SOPT3;                                  /* System Options 3 Register; 0xFFFF980E */
volatile SOPT4STR _SOPT4;                                  /* System Options 4 Register; 0xFFFF980F */
volatile FCDIVSTR _FCDIV;                                  /* FLASH Clock Divider Register; 0xFFFF9820 */
volatile FOPTSTR _FOPT;                                    /* Flash Options Register; 0xFFFF9821 */
volatile FCNFGSTR _FCNFG;                                  /* Flash Configuration Register; 0xFFFF9823 */
volatile FPROTSTR _FPROT;                                  /* Flash Protection Register; 0xFFFF9824 */
volatile FSTATSTR _FSTAT;                                  /* Flash Status Register; 0xFFFF9825 */
volatile FCMDSTR _FCMD;                                    /* Flash Command Register; 0xFFFF9826 */
volatile IIC2ASTR _IIC2A;                                  /* IIC Address Register; 0xFFFF9838 */
volatile IIC2FSTR _IIC2F;                                  /* IIC Frequency Divider Register; 0xFFFF9839 */
volatile IIC2C1STR _IIC2C1;                                /* IIC Control Register 1; 0xFFFF983A */
volatile IIC2SSTR _IIC2S;                                  /* IIC Status Register; 0xFFFF983B */
volatile IIC2DSTR _IIC2D;                                  /* IIC Data I/O Register; 0xFFFF983C */
volatile IIC2C2STR _IIC2C2;                                /* IIC Control Register 2; 0xFFFF983D */
volatile PTAPESTR _PTAPE;                                  /* Port A Pull Enable Register; 0xFFFF9840 */
volatile PTASESTR _PTASE;                                  /* Port A Slew Rate Enable Register; 0xFFFF9841 */
volatile PTADSSTR _PTADS;                                  /* Port A Drive Strength Selection Register; 0xFFFF9842 */
volatile PTAIFESTR _PTAIFE;                                /* Port A Input Filter Enable Register; 0xFFFF9843 */
volatile PTBPESTR _PTBPE;                                  /* Port B Pull Enable Register; 0xFFFF9844 */
volatile PTBSESTR _PTBSE;                                  /* Port B Slew Rate Enable Register; 0xFFFF9845 */
volatile PTBDSSTR _PTBDS;                                  /* Port B Drive Strength Selection Register; 0xFFFF9846 */
volatile PTBIFESTR _PTBIFE;                                /* Port B Input Filter Enable Register; 0xFFFF9847 */
volatile PTCPESTR _PTCPE;                                  /* Port C Pull Enable Register; 0xFFFF9848 */
volatile PTCSESTR _PTCSE;                                  /* Port C Slew Rate Enable Register; 0xFFFF9849 */
volatile PTCDSSTR _PTCDS;                                  /* Port C Drive Strength Selection Register; 0xFFFF984A */
volatile PTCIFESTR _PTCIFE;                                /* Port C Input Filter Enable Register; 0xFFFF984B */
volatile PTDPESTR _PTDPE;                                  /* Port D Pull Enable Register; 0xFFFF984C */
volatile PTDSESTR _PTDSE;                                  /* Port D Slew Rate Enable Register; 0xFFFF984D */
volatile PTDDSSTR _PTDDS;                                  /* Port D Drive Strength Selection Register; 0xFFFF984E */
volatile PTDIFESTR _PTDIFE;                                /* Port D Input Filter Enable Register; 0xFFFF984F */
volatile PTEPESTR _PTEPE;                                  /* Port E Pull Enable Register; 0xFFFF9850 */
volatile PTESESTR _PTESE;                                  /* Port E Slew Rate Enable Register; 0xFFFF9851 */
volatile PTEDSSTR _PTEDS;                                  /* Port E Drive Strength Selection Register; 0xFFFF9852 */
volatile PTEIFESTR _PTEIFE;                                /* Port E Input Filter Enable Register; 0xFFFF9853 */
volatile PTFPESTR _PTFPE;                                  /* Port F Pull Enable Register; 0xFFFF9854 */
volatile PTFSESTR _PTFSE;                                  /* Port F Slew Rate Enable Register; 0xFFFF9855 */
volatile PTFDSSTR _PTFDS;                                  /* Port F Drive Strength Selection Register; 0xFFFF9856 */
volatile PTFIFESTR _PTFIFE;                                /* Port F Input Filter Enable Register; 0xFFFF9857 */
volatile PTGPESTR _PTGPE;                                  /* Port G Pull Enable Register; 0xFFFF9858 */
volatile PTGSESTR _PTGSE;                                  /* Port G Slew Rate Enable Register; 0xFFFF9859 */
volatile PTGDSSTR _PTGDS;                                  /* Port G Drive Strength Selection Register; 0xFFFF985A */
volatile PTGIFESTR _PTGIFE;                                /* Port G Input Filter Enable Register; 0xFFFF985B */
volatile PTHPESTR _PTHPE;                                  /* Port H Pull Enable Register; 0xFFFF985C */
volatile PTHSESTR _PTHSE;                                  /* Port H Slew Rate Enable Register; 0xFFFF985D */
volatile PTHDSSTR _PTHDS;                                  /* Port H Drive Strength Selection Register; 0xFFFF985E */
volatile PTHIFESTR _PTHIFE;                                /* Port H Input Filter Enable Register; 0xFFFF985F */
volatile PTJPESTR _PTJPE;                                  /* Port J Pull Enable Register; 0xFFFF9860 */
volatile PTJSESTR _PTJSE;                                  /* Port J Slew Rate Enable Register; 0xFFFF9861 */
volatile PTJDSSTR _PTJDS;                                  /* Port J Drive Strength Selection Register; 0xFFFF9862 */
volatile PTJIFESTR _PTJIFE;                                /* Port J Input Filter Enable Register; 0xFFFF9863 */
volatile CMTOCSTR _CMTOC;                                  /* CMT Output Control Register; 0xFFFF986C */
volatile CMTMSCSTR _CMTMSC;                                /* CMT Modulator Status and Control Register; 0xFFFF986D */
volatile PTHDSTR _PTHD;                                    /* Port H Data Register; 0xFFFF9874 */
volatile PTHDDSTR _PTHDD;                                  /* Port H Data Direction Register; 0xFFFF9875 */
volatile PTJDSTR _PTJD;                                    /* Port J Data Register; 0xFFFF9876 */
volatile PTJDDSTR _PTJDD;                                  /* Port J Data Direction Register; 0xFFFF9877 */
volatile CANCTL0STR _CANCTL0;                              /* MSCAN Control 0 Register; 0xFFFF9880 */
volatile CANCTL1STR _CANCTL1;                              /* MSCAN Control 1 Register; 0xFFFF9881 */
volatile CANBTR0STR _CANBTR0;                              /* MSCAN Bus Timing Register 0; 0xFFFF9882 */
volatile CANBTR1STR _CANBTR1;                              /* MSCAN Bus Timing Register 1; 0xFFFF9883 */
volatile CANRFLGSTR _CANRFLG;                              /* MSCAN Receiver Flag Register; 0xFFFF9884 */
volatile CANRIERSTR _CANRIER;                              /* MSCAN Receiver Interrupt Enable Register; 0xFFFF9885 */
volatile CANTFLGSTR _CANTFLG;                              /* MSCAN Transmitter Flag Register; 0xFFFF9886 */
volatile CANTIERSTR _CANTIER;                              /* MSCAN Transmitter Interrupt Enable Register; 0xFFFF9887 */
volatile CANTARQSTR _CANTARQ;                              /* MSCAN Transmitter Message Abort Request; 0xFFFF9888 */
volatile CANTAAKSTR _CANTAAK;                              /* MSCAN Transmitter Message Abort Acknowledge; 0xFFFF9889 */
volatile CANTBSELSTR _CANTBSEL;                            /* MSCAN Transmit Buffer Selection; 0xFFFF988A */
volatile CANIDACSTR _CANIDAC;                              /* MSCAN Identifier Acceptance Control Register; 0xFFFF988B */
volatile CANMISCSTR _CANMISC;                              /* MSCAN Miscellaneous Register; 0xFFFF988D */
volatile CANRXERRSTR _CANRXERR;                            /* MSCAN Receive Error Counter Register; 0xFFFF988E */
volatile CANTXERRSTR _CANTXERR;                            /* MSCAN Transmit Error Counter Register; 0xFFFF988F */
volatile CANIDAR0STR _CANIDAR0;                            /* MSCAN Identifier Acceptance Register 0; 0xFFFF9890 */
volatile CANIDAR1STR _CANIDAR1;                            /* MSCAN Identifier Acceptance Register 1; 0xFFFF9891 */
volatile CANIDAR2STR _CANIDAR2;                            /* MSCAN Identifier Acceptance Register 2; 0xFFFF9892 */
volatile CANIDAR3STR _CANIDAR3;                            /* MSCAN Identifier Acceptance Register 3; 0xFFFF9893 */
volatile CANIDMR0STR _CANIDMR0;                            /* MSCAN Identifier Mask Register 0; 0xFFFF9894 */
volatile CANIDMR1STR _CANIDMR1;                            /* MSCAN Identifier Mask Register 1; 0xFFFF9895 */
volatile CANIDMR2STR _CANIDMR2;                            /* MSCAN Identifier Mask Register 2; 0xFFFF9896 */
volatile CANIDMR3STR _CANIDMR3;                            /* MSCAN Identifier Mask Register 3; 0xFFFF9897 */
volatile CANIDAR4STR _CANIDAR4;                            /* MSCAN Identifier Acceptance Register 4; 0xFFFF9898 */
volatile CANIDAR5STR _CANIDAR5;                            /* MSCAN Identifier Acceptance Register 5; 0xFFFF9899 */
volatile CANIDAR6STR _CANIDAR6;                            /* MSCAN Identifier Acceptance Register 6; 0xFFFF989A */
volatile CANIDAR7STR _CANIDAR7;                            /* MSCAN Identifier Acceptance Register 7; 0xFFFF989B */
volatile CANIDMR4STR _CANIDMR4;                            /* MSCAN Identifier Mask Register 4; 0xFFFF989C */
volatile CANIDMR5STR _CANIDMR5;                            /* MSCAN Identifier Mask Register 5; 0xFFFF989D */
volatile CANIDMR6STR _CANIDMR6;                            /* MSCAN Identifier Mask Register 6; 0xFFFF989E */
volatile CANIDMR7STR _CANIDMR7;                            /* MSCAN Identifier Mask Register 7; 0xFFFF989F */
volatile CANRIDR0STR _CANRIDR0;                            /* MSCAN 0 Receive Identifier Register 0; 0xFFFF98A0 */
volatile CANRIDR1STR _CANRIDR1;                            /* MSCAN 0 Receive Identifier Register 1; 0xFFFF98A1 */
volatile CANRIDR2STR _CANRIDR2;                            /* MSCAN 0 Receive Identifier Register 2; 0xFFFF98A2 */
volatile CANRIDR3STR _CANRIDR3;                            /* MSCAN 0 Receive Identifier Register 3; 0xFFFF98A3 */
volatile CANRDSR0STR _CANRDSR0;                            /* MSCAN Receive Data Segment Register 0; 0xFFFF98A4 */
volatile CANRDSR1STR _CANRDSR1;                            /* MSCAN Receive Data Segment Register 1; 0xFFFF98A5 */
volatile CANRDSR2STR _CANRDSR2;                            /* MSCAN Receive Data Segment Register 2; 0xFFFF98A6 */
volatile CANRDSR3STR _CANRDSR3;                            /* MSCAN Receive Data Segment Register 3; 0xFFFF98A7 */
volatile CANRDSR4STR _CANRDSR4;                            /* MSCAN Receive Data Segment Register 4; 0xFFFF98A8 */
volatile CANRDSR5STR _CANRDSR5;                            /* MSCAN Receive Data Segment Register 5; 0xFFFF98A9 */
volatile CANRDSR6STR _CANRDSR6;                            /* MSCAN Receive Data Segment Register 6; 0xFFFF98AA */
volatile CANRDSR7STR _CANRDSR7;                            /* MSCAN Receive Data Segment Register 7; 0xFFFF98AB */
volatile CANRDLRSTR _CANRDLR;                              /* MSCAN Receive Data Length Register; 0xFFFF98AC */
volatile CANTIDR0STR _CANTIDR0;                            /* MSCAN 0 Transmit Identifier Register 0; 0xFFFF98B0 */
volatile CANTIDR1STR _CANTIDR1;                            /* MSCAN 0 Transmit Identifier Register 1; 0xFFFF98B1 */
volatile CANTIDR2STR _CANTIDR2;                            /* MSCAN 0 Transmit Identifier Register 2; 0xFFFF98B2 */
volatile CANTIDR3STR _CANTIDR3;                            /* MSCAN 0 Transmit Identifier Register 3; 0xFFFF98B3 */
volatile CANTDSR0STR _CANTDSR0;                            /* MSCAN Transmit Data Segment Register 0; 0xFFFF98B4 */
volatile CANTDSR1STR _CANTDSR1;                            /* MSCAN Transmit Data Segment Register 1; 0xFFFF98B5 */
volatile CANTDSR2STR _CANTDSR2;                            /* MSCAN Transmit Data Segment Register 2; 0xFFFF98B6 */
volatile CANTDSR3STR _CANTDSR3;                            /* MSCAN Transmit Data Segment Register 3; 0xFFFF98B7 */
volatile CANTDSR4STR _CANTDSR4;                            /* MSCAN Transmit Data Segment Register 4; 0xFFFF98B8 */
volatile CANTDSR5STR _CANTDSR5;                            /* MSCAN Transmit Data Segment Register 5; 0xFFFF98B9 */
volatile CANTDSR6STR _CANTDSR6;                            /* MSCAN Transmit Data Segment Register 6; 0xFFFF98BA */
volatile CANTDSR7STR _CANTDSR7;                            /* MSCAN Transmit Data Segment Register 7; 0xFFFF98BB */
volatile CANTDLRSTR _CANTDLR;                              /* MSCAN Transmit Data Length Register; 0xFFFF98BC */
volatile CANTTBPRSTR _CANTTBPR;                            /* MSCAN Transmit Buffer Priority; 0xFFFF98BD */
volatile PER_IDSTR _PER_ID;                                /* Peripherial ID Register; 0xFFFF9A00 */
volatile ID_COMPSTR _ID_COMP;                              /* Peripheral ID Complement Register; 0xFFFF9A04 */
volatile REVSTR _REV;                                      /* Peripheral Revision Register; 0xFFFF9A08 */
volatile ADD_INFOSTR _ADD_INFO;                            /* Peripheral Additional Info Register; 0xFFFF9A0C */
volatile OTG_INT_STATSTR _OTG_INT_STAT;                    /* OTG Interrupt Status Register; 0xFFFF9A10 */
volatile OTG_INT_ENSTR _OTG_INT_EN;                        /* OTG Interrupt Control Register; 0xFFFF9A14 */
volatile OTG_STATSTR _OTG_STAT;                            /* Interrupt Status Register; 0xFFFF9A18 */
volatile OTG_CTRLSTR _OTG_CTRL;                            /* OTG Control Register; 0xFFFF9A1C */
volatile INT_STATSTR _INT_STAT;                            /* Interrupt Status Register; 0xFFFF9A80 */
volatile INT_ENBSTR _INT_ENB;                              /* Interrupt Enable Register; 0xFFFF9A84 */
volatile ERR_STATSTR _ERR_STAT;                            /* Error Interrupt Status Register; 0xFFFF9A88 */
volatile ERR_ENBSTR _ERR_ENB;                              /* Error Interrupt Enable Register; 0xFFFF9A8C */
volatile STATSTR _STAT;                                    /* Status Register; 0xFFFF9A90 */
volatile CTLSTR _CTL;                                      /* Control Register; 0xFFFF9A94 */
volatile ADDRSTR _ADDR;                                    /* Address Register; 0xFFFF9A98 */
volatile BDT_PAGE_01STR _BDT_PAGE_01;                      /* BDT Page Register 1; 0xFFFF9A9C */
volatile FRM_NUMLSTR _FRM_NUML;                            /* Frame Number Register Low; 0xFFFF9AA0 */
volatile FRM_NUMHSTR _FRM_NUMH;                            /* Frame Number Register High; 0xFFFF9AA4 */
volatile TOKENSTR _TOKEN;                                  /* Token Register; 0xFFFF9AA8 */
volatile SOF_THLDSTR _SOF_THLD;                            /* SOF Threshold Register; 0xFFFF9AAC */
volatile BDT_PAGE_02STR _BDT_PAGE_02;                      /* BDT Page Register 2; 0xFFFF9AB0 */
volatile BDT_PAGE_03STR _BDT_PAGE_03;                      /* BDT Page Register 3; 0xFFFF9AB4 */
volatile ENDPT0STR _ENDPT0;                                /* Endpoint Control Register 0; 0xFFFF9AC0 */
volatile ENDPT1STR _ENDPT1;                                /* Endpoint Control Register 1; 0xFFFF9AC4 */
volatile ENDPT2STR _ENDPT2;                                /* Endpoint Control Register 2; 0xFFFF9AC8 */
volatile ENDPT3STR _ENDPT3;                                /* Endpoint Control Register 3; 0xFFFF9ACC */
volatile ENDPT4STR _ENDPT4;                                /* Endpoint Control Register 4; 0xFFFF9AD0 */
volatile ENDPT5STR _ENDPT5;                                /* Endpoint Control Register 5; 0xFFFF9AD4 */
volatile ENDPT6STR _ENDPT6;                                /* Endpoint Control Register 6; 0xFFFF9AD8 */
volatile ENDPT7STR _ENDPT7;                                /* Endpoint Control Register 7; 0xFFFF9ADC */
volatile ENDPT8STR _ENDPT8;                                /* Endpoint Control Register 8; 0xFFFF9AE0 */
volatile ENDPT9STR _ENDPT9;                                /* Endpoint Control Register 9; 0xFFFF9AE4 */
volatile ENDPT10STR _ENDPT10;                              /* Endpoint Control Register 10; 0xFFFF9AE8 */
volatile ENDPT11STR _ENDPT11;                              /* Endpoint Control Register 11; 0xFFFF9AEC */
volatile ENDPT12STR _ENDPT12;                              /* Endpoint Control Register 12; 0xFFFF9AF0 */
volatile ENDPT13STR _ENDPT13;                              /* Endpoint Control Register 13; 0xFFFF9AF4 */
volatile ENDPT14STR _ENDPT14;                              /* Endpoint Control Register 14; 0xFFFF9AF8 */
volatile ENDPT15STR _ENDPT15;                              /* Endpoint Control Register 15; 0xFFFF9AFC */
volatile USB_CTRLSTR _USB_CTRL;                            /* USB Control Register; 0xFFFF9B00 */
volatile USB_OTG_OBSERVESTR _USB_OTG_OBSERVE;              /* USB OTG Observe Register; 0xFFFF9B04 */
volatile USB_OTG_CONTROLSTR _USB_OTG_CONTROL;              /* USB OTG Control Register; 0xFFFF9B08 */
volatile USBTRC0STR _USBTRC0;                              /* USB Transceiver and Regulator Control Register 0; 0xFFFF9B0C */
volatile OTGPINSTR _OTGPIN;                                /* USB OTG Pin Control Register; 0xFFFF9B10 */
volatile INTC_FRCSTR _INTC_FRC;                            /* INTC Force Interrupt Register; 0xFFFFFFD0 */
volatile INTC_PL6P7STR _INTC_PL6P7;                        /* INTC Programmable Level 6, Priority 7 Register; 0xFFFFFFD8 */
volatile INTC_PL6P6STR _INTC_PL6P6;                        /* INTC Programmable Level 6, Priority 6 Register; 0xFFFFFFD9 */
volatile INTC_WCRSTR _INTC_WCR;                            /* INTC Wake-up Control Register; 0xFFFFFFDB */
volatile INTC_SFRCSTR _INTC_SFRC;                          /* INTC Set Interrupt Force Register; 0xFFFFFFDE */
volatile INTC_CFRCSTR _INTC_CFRC;                          /* INTC Clear Interrupt Force Register; 0xFFFFFFDF */
volatile INTC_SWIACKSTR _INTC_SWIACK;                      /* INTC Software IACK Register; 0xFFFFFFE0 */
volatile INTC_LVL1IACKSTR _INTC_LVL1IACK;                  /* INTC Level 1 IACK Register; 0xFFFFFFE4 */
volatile INTC_LVL2IACKSTR _INTC_LVL2IACK;                  /* INTC Level 2 IACK Register; 0xFFFFFFE8 */
volatile INTC_LVL3IACKSTR _INTC_LVL3IACK;                  /* INTC Level 3 IACK Register; 0xFFFFFFEC */
volatile INTC_LVL4IACKSTR _INTC_LVL4IACK;                  /* INTC Level 4 IACK Register; 0xFFFFFFF0 */
volatile INTC_LVL5IACKSTR _INTC_LVL5IACK;                  /* INTC Level 5 IACK Register; 0xFFFFFFF4 */
volatile INTC_LVL6IACKSTR _INTC_LVL6IACK;                  /* INTC Level 6 IACK Register; 0xFFFFFFF8 */
volatile INTC_LVL7IACKSTR _INTC_LVL7IACK;                  /* INTC Level 7 IACK Register; 0xFFFFFFFC */


/* * * * *  16-BIT REGISTERS  * * * * * * * * * * * * * * * */
volatile RGPIO_DIRSTR _RGPIO_DIR;                          /* RGPIO Data Direction Register; 0x00C00000 */
volatile RGPIO_DATASTR _RGPIO_DATA;                        /* RGPIO Data Register; 0x00C00002 */
volatile RGPIO_ENBSTR _RGPIO_ENB;                          /* RGPIO Pin Enable Register; 0x00C00004 */
volatile RGPIO_CLRSTR _RGPIO_CLR;                          /* RGPIO Clear Data Register; 0x00C00006 */
volatile RGPIO_SETSTR _RGPIO_SET;                          /* RGPIO Set Data Register; 0x00C0000A */
volatile RGPIO_TOGSTR _RGPIO_TOG;                          /* RGPIO Toggle Data Register; 0x00C0000E */
volatile ADCRSTR _ADCR;                                    /* Data Result Register; 0xFFFF8012 */
volatile ADCCVSTR _ADCCV;                                  /* Compare Value Register; 0xFFFF8014 */
volatile TPM1CNTSTR _TPM1CNT;                              /* TPM1 Timer Counter Register; 0xFFFF8021 */
volatile TPM1MODSTR _TPM1MOD;                              /* TPM1 Timer Counter Modulo Register; 0xFFFF8023 */
volatile TPM1C0VSTR _TPM1C0V;                              /* TPM1 Timer Channel 0 Value Register; 0xFFFF8026 */
volatile TPM1C1VSTR _TPM1C1V;                              /* TPM1 Timer Channel 1 Value Register; 0xFFFF8029 */
volatile TPM1C2VSTR _TPM1C2V;                              /* TPM1 Timer Channel 2 Value Register; 0xFFFF802C */
volatile TPM1C3VSTR _TPM1C3V;                              /* TPM1 Timer Channel 3 Value Register; 0xFFFF802F */
volatile TPM1C4VSTR _TPM1C4V;                              /* TPM1 Timer Channel 4 Value Register; 0xFFFF8032 */
volatile TPM1C5VSTR _TPM1C5V;                              /* TPM1 Timer Channel 5 Value Register; 0xFFFF8035 */
volatile SCI1BDSTR _SCI1BD;                                /* SCI1 Baud Rate Register; 0xFFFF8038 */
volatile SCI2BDSTR _SCI2BD;                                /* SCI2 Baud Rate Register; 0xFFFF8040 */
volatile SPI1D16STR _SPI1D16;                              /* SPI1 Data Register; 0xFFFF8054 */
volatile SPI1MSTR _SPI1M;                                  /* SPI1 Match Register; 0xFFFF8056 */
volatile TPM2CNTSTR _TPM2CNT;                              /* TPM2 Timer Counter Register; 0xFFFF8061 */
volatile TPM2MODSTR _TPM2MOD;                              /* TPM2 Timer Counter Modulo Register; 0xFFFF8063 */
volatile TPM2C0VSTR _TPM2C0V;                              /* TPM2 Timer Channel 0 Value Register; 0xFFFF8066 */
volatile TPM2C1VSTR _TPM2C1V;                              /* TPM2 Timer Channel 1 Value Register; 0xFFFF8069 */
volatile SPI2D16STR _SPI2D16;                              /* SPI2 Data Register; 0xFFFF8074 */
volatile SPI2MSTR _SPI2M;                                  /* SPI2 Match Register; 0xFFFF8076 */
volatile SDIDSTR _SDID;                                    /* System Device Identification Register; 0xFFFF9806 */
volatile CMTCG1STR _CMTCG1;                                /* Carrier Generator Data Register 1; 0xFFFF9868 */
volatile CMTCG2STR _CMTCG2;                                /* Carrier Generator Data Register 2; 0xFFFF986A */
volatile CMTCMD12STR _CMTCMD12;                            /* CMT Modulator Data Register 12; 0xFFFF986E */
volatile CMTCMD34STR _CMTCMD34;                            /* CMT Modulator Data Register 34; 0xFFFF9870 */
volatile CANRTSRSTR _CANRTSR;                              /* MSCAN Receive Time Stamp Register; 0xFFFF98AE */
volatile CANTTSRSTR _CANTTSR;                              /* MSCAN Transmit Time Stamp Register; 0xFFFF98BE */


/* * * * *  32-BIT REGISTERS  * * * * * * * * * * * * * * * */
volatile RNGCRSTR _RNGCR;                                  /* Control Register; 0xFFFF98C0 */
volatile RNGSRSTR _RNGSR;                                  /* Status Register; 0xFFFF98C4 */
volatile RNGERSTR _RNGER;                                  /* Entropy Register; 0xFFFF98C8 */
volatile RNGOUTSTR _RNGOUT;                                /* Output Register; 0xFFFF98CC */

/*lint -restore */

/* EOF */
