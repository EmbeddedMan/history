/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2007/03/19 Revision: 0.91
 */

#ifndef __MCF52221_GPIO_H__
#define __MCF52221_GPIO_H__


/*********************************************************************
*
* General Purpose I/O (GPIO)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_GPIO_PORTNQ                      (*(vuint8 *)(&__IPSBAR[0x100008]))
#define MCF_GPIO_DDRNQ                       (*(vuint8 *)(&__IPSBAR[0x100020]))
#define MCF_GPIO_SETNQ                       (*(vuint8 *)(&__IPSBAR[0x100038]))
#define MCF_GPIO_CLRNQ                       (*(vuint8 *)(&__IPSBAR[0x100050]))
#define MCF_GPIO_PNQPAR                      (*(vuint16*)(&__IPSBAR[0x100068]))

#define MCF_GPIO_PORTAN                      (*(vuint8 *)(&__IPSBAR[0x10000A]))
#define MCF_GPIO_DDRAN                       (*(vuint8 *)(&__IPSBAR[0x100022]))
#define MCF_GPIO_SETAN                       (*(vuint8 *)(&__IPSBAR[0x10003A]))
#define MCF_GPIO_CLRAN                       (*(vuint8 *)(&__IPSBAR[0x100052]))
#define MCF_GPIO_PANPAR                      (*(vuint8 *)(&__IPSBAR[0x10006A]))

#define MCF_GPIO_PORTAS                      (*(vuint8 *)(&__IPSBAR[0x10000B]))
#define MCF_GPIO_DDRAS                       (*(vuint8 *)(&__IPSBAR[0x100023]))
#define MCF_GPIO_SETAS                       (*(vuint8 *)(&__IPSBAR[0x10003B]))
#define MCF_GPIO_CLRAS                       (*(vuint8 *)(&__IPSBAR[0x100053]))
#define MCF_GPIO_PASPAR                      (*(vuint8 *)(&__IPSBAR[0x10006B]))

#define MCF_GPIO_PORTQS                      (*(vuint8 *)(&__IPSBAR[0x10000C]))
#define MCF_GPIO_DDRQS                       (*(vuint8 *)(&__IPSBAR[0x100024]))
#define MCF_GPIO_SETQS                       (*(vuint8 *)(&__IPSBAR[0x10003C]))
#define MCF_GPIO_CLRQS                       (*(vuint8 *)(&__IPSBAR[0x100054]))
#define MCF_GPIO_PQSPAR                      (*(vuint16*)(&__IPSBAR[0x10006C]))

#define MCF_GPIO_PORTTA                      (*(vuint8 *)(&__IPSBAR[0x10000E]))
#define MCF_GPIO_DDRTA                       (*(vuint8 *)(&__IPSBAR[0x100026]))
#define MCF_GPIO_SETTA                       (*(vuint8 *)(&__IPSBAR[0x10003E]))
#define MCF_GPIO_CLRTA                       (*(vuint8 *)(&__IPSBAR[0x100056]))
#define MCF_GPIO_PTAPAR                      (*(vuint8 *)(&__IPSBAR[0x10006E]))

#define MCF_GPIO_PORTTC                      (*(vuint8 *)(&__IPSBAR[0x10000F]))
#define MCF_GPIO_DDRTC                       (*(vuint8 *)(&__IPSBAR[0x100027]))
#define MCF_GPIO_SETTC                       (*(vuint8 *)(&__IPSBAR[0x10003F]))
#define MCF_GPIO_CLRTC                       (*(vuint8 *)(&__IPSBAR[0x100057]))
#define MCF_GPIO_PTCPAR                      (*(vuint8 *)(&__IPSBAR[0x10006F]))

#define MCF_GPIO_PORTUA                      (*(vuint8 *)(&__IPSBAR[0x100011]))
#define MCF_GPIO_DDRUA                       (*(vuint8 *)(&__IPSBAR[0x100029]))
#define MCF_GPIO_SETUA                       (*(vuint8 *)(&__IPSBAR[0x100041]))
#define MCF_GPIO_CLRUA                       (*(vuint8 *)(&__IPSBAR[0x100059]))
#define MCF_GPIO_PUAPAR                      (*(vuint8 *)(&__IPSBAR[0x100071]))

#define MCF_GPIO_PORTUB                      (*(vuint8 *)(&__IPSBAR[0x100012]))
#define MCF_GPIO_DDRUB                       (*(vuint8 *)(&__IPSBAR[0x10002A]))
#define MCF_GPIO_SETUB                       (*(vuint8 *)(&__IPSBAR[0x100042]))
#define MCF_GPIO_CLRUB                       (*(vuint8 *)(&__IPSBAR[0x10005A]))
#define MCF_GPIO_PUBPAR                      (*(vuint8 *)(&__IPSBAR[0x100072]))

#define MCF_GPIO_PORTUC                      (*(vuint8 *)(&__IPSBAR[0x100013]))
#define MCF_GPIO_DDRUC                       (*(vuint8 *)(&__IPSBAR[0x10002B]))
#define MCF_GPIO_SETUC                       (*(vuint8 *)(&__IPSBAR[0x100043]))
#define MCF_GPIO_CLRUC                       (*(vuint8 *)(&__IPSBAR[0x10005B]))
#define MCF_GPIO_PUCPAR                      (*(vuint8 *)(&__IPSBAR[0x100073]))

#define MCF_GPIO_PORTDD                      (*(vuint8 *)(&__IPSBAR[0x100014]))
#define MCF_GPIO_DDRDD                       (*(vuint8 *)(&__IPSBAR[0x10002C]))
#define MCF_GPIO_SETDD                       (*(vuint8 *)(&__IPSBAR[0x100044]))
#define MCF_GPIO_CLRDD                       (*(vuint8 *)(&__IPSBAR[0x10005C]))
#define MCF_GPIO_PDDPAR                      (*(vuint8 *)(&__IPSBAR[0x100074]))



/* Bit definitions and macros for MCF_GPIO_PORTNQ */
#define MCF_GPIO_PORTNQ_PORTNQ1              (0x2)
#define MCF_GPIO_PORTNQ_PORTNQ2              (0x4)
#define MCF_GPIO_PORTNQ_PORTNQ3              (0x8)
#define MCF_GPIO_PORTNQ_PORTNQ4              (0x10)
#define MCF_GPIO_PORTNQ_PORTNQ5              (0x20)
#define MCF_GPIO_PORTNQ_PORTNQ6              (0x40)
#define MCF_GPIO_PORTNQ_PORTNQ7              (0x80)

/* Bit definitions and macros for MCF_GPIO_DDRNQ */
#define MCF_GPIO_DDRNQ_DDRNQ1                (0x2)
#define MCF_GPIO_DDRNQ_DDRNQ2                (0x4)
#define MCF_GPIO_DDRNQ_DDRNQ3                (0x8)
#define MCF_GPIO_DDRNQ_DDRNQ4                (0x10)
#define MCF_GPIO_DDRNQ_DDRNQ5                (0x20)
#define MCF_GPIO_DDRNQ_DDRNQ6                (0x40)
#define MCF_GPIO_DDRNQ_DDRNQ7                (0x80)

/* Bit definitions and macros for MCF_GPIO_SETNQ */
#define MCF_GPIO_SETNQ_SETNQ1                (0x2)
#define MCF_GPIO_SETNQ_SETNQ2                (0x4)
#define MCF_GPIO_SETNQ_SETNQ3                (0x8)
#define MCF_GPIO_SETNQ_SETNQ4                (0x10)
#define MCF_GPIO_SETNQ_SETNQ5                (0x20)
#define MCF_GPIO_SETNQ_SETNQ6                (0x40)
#define MCF_GPIO_SETNQ_SETNQ7                (0x80)

/* Bit definitions and macros for MCF_GPIO_CLRNQ */
#define MCF_GPIO_CLRNQ_CLRNQ1                (0x2)
#define MCF_GPIO_CLRNQ_CLRNQ2                (0x4)
#define MCF_GPIO_CLRNQ_CLRNQ3                (0x8)
#define MCF_GPIO_CLRNQ_CLRNQ4                (0x10)
#define MCF_GPIO_CLRNQ_CLRNQ5                (0x20)
#define MCF_GPIO_CLRNQ_CLRNQ6                (0x40)
#define MCF_GPIO_CLRNQ_CLRNQ7                (0x80)

/* Bit definitions and macros for MCF_GPIO_PNQPAR */
#define MCF_GPIO_PNQPAR_PNQPAR1(x)           (((x)&0x3)<<0x2)
#define MCF_GPIO_PNQPAR_IRQ1_GPIO            (0)
#define MCF_GPIO_PNQPAR_IRQ1_IRQ1            (0x4)
#define MCF_GPIO_PNQPAR_IRQ1_SYNCA           (0x8)
#define MCF_GPIO_PNQPAR_IRQ1_USB_ALT_CLK     (0xC)
#define MCF_GPIO_PNQPAR_PNQPAR2(x)           (((x)&0x3)<<0x4)
#define MCF_GPIO_PNQPAR_IRQ2_GPIO            (0)
#define MCF_GPIO_PNQPAR_IRQ2_IRQ2            (0x10)
#define MCF_GPIO_PNQPAR_PNQPAR3(x)           (((x)&0x3)<<0x6)
#define MCF_GPIO_PNQPAR_IRQ3_GPIO            (0)
#define MCF_GPIO_PNQPAR_IRQ3_IRQ3            (0x40)
#define MCF_GPIO_PNQPAR_PNQPAR4(x)           (((x)&0x3)<<0x8)
#define MCF_GPIO_PNQPAR_IRQ4_GPIO            (0)
#define MCF_GPIO_PNQPAR_IRQ4_IRQ4            (0x100)
#define MCF_GPIO_PNQPAR_PNQPAR5(x)           (((x)&0x3)<<0xA)
#define MCF_GPIO_PNQPAR_IRQ5_GPIO            (0)
#define MCF_GPIO_PNQPAR_IRQ5_IRQ5            (0x400)
#define MCF_GPIO_PNQPAR_PNQPAR6(x)           (((x)&0x3)<<0xC)
#define MCF_GPIO_PNQPAR_IRQ6_GPIO            (0)
#define MCF_GPIO_PNQPAR_IRQ6_IRQ6            (0x1000)
#define MCF_GPIO_PNQPAR_PNQPAR7(x)           (((x)&0x3)<<0xE)
#define MCF_GPIO_PNQPAR_IRQ7_GPIO            (0)
#define MCF_GPIO_PNQPAR_IRQ7_IRQ7            (0x4000)

/* Bit definitions and macros for MCF_GPIO_PORTAN */
#define MCF_GPIO_PORTAN_PORTAN0              (0x1)
#define MCF_GPIO_PORTAN_PORTAN1              (0x2)
#define MCF_GPIO_PORTAN_PORTAN2              (0x4)
#define MCF_GPIO_PORTAN_PORTAN3              (0x8)
#define MCF_GPIO_PORTAN_PORTAN4              (0x10)
#define MCF_GPIO_PORTAN_PORTAN5              (0x20)
#define MCF_GPIO_PORTAN_PORTAN6              (0x40)
#define MCF_GPIO_PORTAN_PORTAN7              (0x80)

/* Bit definitions and macros for MCF_GPIO_DDRAN */
#define MCF_GPIO_DDRAN_DDRAN0                (0x1)
#define MCF_GPIO_DDRAN_DDRAN1                (0x2)
#define MCF_GPIO_DDRAN_DDRAN2                (0x4)
#define MCF_GPIO_DDRAN_DDRAN3                (0x8)
#define MCF_GPIO_DDRAN_DDRAN4                (0x10)
#define MCF_GPIO_DDRAN_DDRAN5                (0x20)
#define MCF_GPIO_DDRAN_DDRAN6                (0x40)
#define MCF_GPIO_DDRAN_DDRAN7                (0x80)

/* Bit definitions and macros for MCF_GPIO_SETAN */
#define MCF_GPIO_SETAN_SETAN0                (0x1)
#define MCF_GPIO_SETAN_SETAN1                (0x2)
#define MCF_GPIO_SETAN_SETAN2                (0x4)
#define MCF_GPIO_SETAN_SETAN3                (0x8)
#define MCF_GPIO_SETAN_SETAN4                (0x10)
#define MCF_GPIO_SETAN_SETAN5                (0x20)
#define MCF_GPIO_SETAN_SETAN6                (0x40)
#define MCF_GPIO_SETAN_SETAN7                (0x80)

/* Bit definitions and macros for MCF_GPIO_CLRAN */
#define MCF_GPIO_CLRAN_CLRAN0                (0x1)
#define MCF_GPIO_CLRAN_CLRAN1                (0x2)
#define MCF_GPIO_CLRAN_CLRAN2                (0x4)
#define MCF_GPIO_CLRAN_CLRAN3                (0x8)
#define MCF_GPIO_CLRAN_CLRAN4                (0x10)
#define MCF_GPIO_CLRAN_CLRAN5                (0x20)
#define MCF_GPIO_CLRAN_CLRAN6                (0x40)
#define MCF_GPIO_CLRAN_CLRAN7                (0x80)

/* Bit definitions and macros for MCF_GPIO_PANPAR */
#define MCF_GPIO_PANPAR_PANPAR0              (0x1)
#define MCF_GPIO_PANPAR_AN0_GPIO             (0)
#define MCF_GPIO_PANPAR_AN0_AN0              (0x1)
#define MCF_GPIO_PANPAR_PANPAR1              (0x2)
#define MCF_GPIO_PANPAR_AN1_GPIO             (0)
#define MCF_GPIO_PANPAR_AN1_AN1              (0x2)
#define MCF_GPIO_PANPAR_PANPAR2              (0x4)
#define MCF_GPIO_PANPAR_AN2_GPIO             (0)
#define MCF_GPIO_PANPAR_AN2_AN2              (0x4)
#define MCF_GPIO_PANPAR_PANPAR3              (0x8)
#define MCF_GPIO_PANPAR_AN3_GPIO             (0)
#define MCF_GPIO_PANPAR_AN3_AN3              (0x8)
#define MCF_GPIO_PANPAR_PANPAR4              (0x10)
#define MCF_GPIO_PANPAR_AN4_GPIO             (0)
#define MCF_GPIO_PANPAR_AN4_AN4              (0x10)
#define MCF_GPIO_PANPAR_PANPAR5              (0x20)
#define MCF_GPIO_PANPAR_AN5_GPIO             (0)
#define MCF_GPIO_PANPAR_AN5_AN5              (0x20)
#define MCF_GPIO_PANPAR_PANPAR6              (0x40)
#define MCF_GPIO_PANPAR_AN6_GPIO             (0)
#define MCF_GPIO_PANPAR_AN6_AN6              (0x40)
#define MCF_GPIO_PANPAR_PANPAR7              (0x80)
#define MCF_GPIO_PANPAR_AN7_GPIO             (0)
#define MCF_GPIO_PANPAR_AN7_AN7              (0x80)

/* Bit definitions and macros for MCF_GPIO_PORTAS */
#define MCF_GPIO_PORTAS_PORTAS0              (0x1)
#define MCF_GPIO_PORTAS_PORTAS1              (0x2)

/* Bit definitions and macros for MCF_GPIO_DDRAS */
#define MCF_GPIO_DDRAS_DDRAS0                (0x1)
#define MCF_GPIO_DDRAS_DDRAS1                (0x2)

/* Bit definitions and macros for MCF_GPIO_SETAS */
#define MCF_GPIO_SETAS_SETAS0                (0x1)
#define MCF_GPIO_SETAS_SETAS1                (0x2)

/* Bit definitions and macros for MCF_GPIO_CLRAS */
#define MCF_GPIO_CLRAS_CLRAS0                (0x1)
#define MCF_GPIO_CLRAS_CLRAS1                (0x2)

/* Bit definitions and macros for MCF_GPIO_PASPAR */
#define MCF_GPIO_PASPAR_PASPAR0(x)           (((x)&0x3)<<0)
#define MCF_GPIO_PASPAR_SCL_GPIO             (0)
#define MCF_GPIO_PASPAR_SCL_SCL              (0x1)
#define MCF_GPIO_PASPAR_SCL_USB_DMI          (0x2)
#define MCF_GPIO_PASPAR_SCL_UTXD2            (0x3)
#define MCF_GPIO_PASPAR_PASPAR1(x)           (((x)&0x3)<<0x2)
#define MCF_GPIO_PASPAR_SDA_GPIO             (0)
#define MCF_GPIO_PASPAR_SDA_SDA              (0x4)
#define MCF_GPIO_PASPAR_SDA_USB_DPI          (0x8)
#define MCF_GPIO_PASPAR_SDA_URXD2            (0xC)

/* Bit definitions and macros for MCF_GPIO_PORTQS */
#define MCF_GPIO_PORTQS_PORTQS0              (0x1)
#define MCF_GPIO_PORTQS_PORTQS1              (0x2)
#define MCF_GPIO_PORTQS_PORTQS2              (0x4)
#define MCF_GPIO_PORTQS_PORTQS3              (0x8)
#define MCF_GPIO_PORTQS_PORTQS4              (0x10)
#define MCF_GPIO_PORTQS_PORTQS5              (0x20)
#define MCF_GPIO_PORTQS_PORTQS6              (0x40)

/* Bit definitions and macros for MCF_GPIO_DDRQS */
#define MCF_GPIO_DDRQS_DDRQS0                (0x1)
#define MCF_GPIO_DDRQS_DDRQS1                (0x2)
#define MCF_GPIO_DDRQS_DDRQS2                (0x4)
#define MCF_GPIO_DDRQS_DDRQS3                (0x8)
#define MCF_GPIO_DDRQS_DDRQS4                (0x10)
#define MCF_GPIO_DDRQS_DDRQS5                (0x20)
#define MCF_GPIO_DDRQS_DDRQS6                (0x40)

/* Bit definitions and macros for MCF_GPIO_SETQS */
#define MCF_GPIO_SETQS_SETQS0                (0x1)
#define MCF_GPIO_SETQS_SETQS1                (0x2)
#define MCF_GPIO_SETQS_SETQS2                (0x4)
#define MCF_GPIO_SETQS_SETQS3                (0x8)
#define MCF_GPIO_SETQS_SETQS4                (0x10)
#define MCF_GPIO_SETQS_SETQS5                (0x20)
#define MCF_GPIO_SETQS_SETQS6                (0x40)

/* Bit definitions and macros for MCF_GPIO_CLRQS */
#define MCF_GPIO_CLRQS_CLRQS0                (0x1)
#define MCF_GPIO_CLRQS_CLRQS1                (0x2)
#define MCF_GPIO_CLRQS_CLRQS2                (0x4)
#define MCF_GPIO_CLRQS_CLRQS3                (0x8)
#define MCF_GPIO_CLRQS_CLRQS4                (0x10)
#define MCF_GPIO_CLRQS_CLRQS5                (0x20)
#define MCF_GPIO_CLRQS_CLRQS6                (0x40)

/* Bit definitions and macros for MCF_GPIO_PQSPAR */
#define MCF_GPIO_PQSPAR_PQSPAR0(x)           (((x)&0x3)<<0)
#define MCF_GPIO_PQSPAR_QSPI_DOUT_GPIO       (0)
#define MCF_GPIO_PQSPAR_QSPI_DOUT_DOUT       (0x1)
#define MCF_GPIO_PQSPAR_QSPI_DOUT_UTXD1      (0x3)
#define MCF_GPIO_PQSPAR_PQSPAR1(x)           (((x)&0x3)<<0x2)
#define MCF_GPIO_PQSPAR_QSPI_DIN_GPIO        (0)
#define MCF_GPIO_PQSPAR_QSPI_DIN_DIN         (0x4)
#define MCF_GPIO_PQSPAR_QSPI_DIN_URXD1       (0xC)
#define MCF_GPIO_PQSPAR_PQSPAR2(x)           (((x)&0x3)<<0x4)
#define MCF_GPIO_PQSPAR_QSPI_CLK_GPIO        (0)
#define MCF_GPIO_PQSPAR_QSPI_CLK_CLK         (0x10)
#define MCF_GPIO_PQSPAR_QSPI_CLK_SCL         (0x20)
#define MCF_GPIO_PQSPAR_QSPI_CLK_URTS1       (0x30)
#define MCF_GPIO_PQSPAR_PQSPAR3(x)           (((x)&0x3)<<0x6)
#define MCF_GPIO_PQSPAR_QSPI_CS0_GPIO        (0)
#define MCF_GPIO_PQSPAR_QSPI_CS0_CS0         (0x40)
#define MCF_GPIO_PQSPAR_QSPI_CS0_UCTS1       (0xC0)
#define MCF_GPIO_PQSPAR_PQSPAR4(x)           (((x)&0x3)<<0x8)
#define MCF_GPIO_PQSPAR_QSPI_CS1_GPIO        (0)
#define MCF_GPIO_PQSPAR_QSPI_CS1_CS1         (0x100)
#define MCF_GPIO_PQSPAR_QSPI_CS1_USB_PULLUP  (0x300)
#define MCF_GPIO_PQSPAR_PQSPAR5(x)           (((x)&0x3)<<0xA)
#define MCF_GPIO_PQSPAR_QSPI_CS2_GPIO        (0)
#define MCF_GPIO_PQSPAR_QSPI_CS2_CS2         (0x400)
#define MCF_GPIO_PQSPAR_QSPI_CS2_USB_DM_PD   (0xC00)
#define MCF_GPIO_PQSPAR_PQSPAR6(x)           (((x)&0x3)<<0xC)
#define MCF_GPIO_PQSPAR_QSPI_CS3_GPIO        (0)
#define MCF_GPIO_PQSPAR_QSPI_CS3_CS3         (0x1000)
#define MCF_GPIO_PQSPAR_QSPI_CS3_SYNCA       (0x2000)
#define MCF_GPIO_PQSPAR_QSPI_CS3_USB_DP_PD   (0x3000)

/* Bit definitions and macros for MCF_GPIO_PORTTA */
#define MCF_GPIO_PORTTA_PORTTA0              (0x1)
#define MCF_GPIO_PORTTA_PORTTA1              (0x2)
#define MCF_GPIO_PORTTA_PORTTA2              (0x4)
#define MCF_GPIO_PORTTA_PORTTA3              (0x8)

/* Bit definitions and macros for MCF_GPIO_DDRTA */
#define MCF_GPIO_DDRTA_DDRTA0                (0x1)
#define MCF_GPIO_DDRTA_DDRTA1                (0x2)
#define MCF_GPIO_DDRTA_DDRTA2                (0x4)
#define MCF_GPIO_DDRTA_DDRTA3                (0x8)

/* Bit definitions and macros for MCF_GPIO_SETTA */
#define MCF_GPIO_SETTA_SETTA0                (0x1)
#define MCF_GPIO_SETTA_SETTA1                (0x2)
#define MCF_GPIO_SETTA_SETTA2                (0x4)
#define MCF_GPIO_SETTA_SETTA3                (0x8)

/* Bit definitions and macros for MCF_GPIO_CLRTA */
#define MCF_GPIO_CLRTA_CLRTA0                (0x1)
#define MCF_GPIO_CLRTA_CLRTA1                (0x2)
#define MCF_GPIO_CLRTA_CLRTA2                (0x4)
#define MCF_GPIO_CLRTA_CLRTA3                (0x8)

/* Bit definitions and macros for MCF_GPIO_PTAPAR */
#define MCF_GPIO_PTAPAR_PTAPAR0(x)           (((x)&0x3)<<0)
#define MCF_GPIO_PTAPAR_GPT0_GPIO            (0)
#define MCF_GPIO_PTAPAR_GPT0_GPT0            (0x1)
#define MCF_GPIO_PTAPAR_GPT0_PWM1            (0x3)
#define MCF_GPIO_PTAPAR_PTAPAR1(x)           (((x)&0x3)<<0x2)
#define MCF_GPIO_PTAPAR_GPT1_GPIO            (0)
#define MCF_GPIO_PTAPAR_GPT1_GPT1            (0x4)
#define MCF_GPIO_PTAPAR_GPT1_PWM3            (0xC)
#define MCF_GPIO_PTAPAR_PTAPAR2(x)           (((x)&0x3)<<0x4)
#define MCF_GPIO_PTAPAR_GPT2_GPIO            (0)
#define MCF_GPIO_PTAPAR_GPT2_GPT2            (0x10)
#define MCF_GPIO_PTAPAR_GPT2_PWM5            (0x30)
#define MCF_GPIO_PTAPAR_PTAPAR3(x)           (((x)&0x3)<<0x6)
#define MCF_GPIO_PTAPAR_GPT3_GPIO            (0)
#define MCF_GPIO_PTAPAR_GPT3_GPT3            (0x40)
#define MCF_GPIO_PTAPAR_GPT3_PWM7            (0xC0)

/* Bit definitions and macros for MCF_GPIO_PORTTC */
#define MCF_GPIO_PORTTC_PORTTC0              (0x1)
#define MCF_GPIO_PORTTC_PORTTC1              (0x2)
#define MCF_GPIO_PORTTC_PORTTC2              (0x4)
#define MCF_GPIO_PORTTC_PORTTC3              (0x8)

/* Bit definitions and macros for MCF_GPIO_DDRTC */
#define MCF_GPIO_DDRTC_DDRTC0                (0x1)
#define MCF_GPIO_DDRTC_DDRTC1                (0x2)
#define MCF_GPIO_DDRTC_DDRTC2                (0x4)
#define MCF_GPIO_DDRTC_DDRTC3                (0x8)

/* Bit definitions and macros for MCF_GPIO_SETTC */
#define MCF_GPIO_SETTC_SETTC0                (0x1)
#define MCF_GPIO_SETTC_SETTC1                (0x2)
#define MCF_GPIO_SETTC_SETTC2                (0x4)
#define MCF_GPIO_SETTC_SETTC3                (0x8)

/* Bit definitions and macros for MCF_GPIO_CLRTC */
#define MCF_GPIO_CLRTC_CLRTC0                (0x1)
#define MCF_GPIO_CLRTC_CLRTC1                (0x2)
#define MCF_GPIO_CLRTC_CLRTC2                (0x4)
#define MCF_GPIO_CLRTC_CLRTC3                (0x8)

/* Bit definitions and macros for MCF_GPIO_PTCPAR */
#define MCF_GPIO_PTCPAR_PTCPAR0(x)           (((x)&0x3)<<0)
#define MCF_GPIO_PTCPAR_DTIN0_GPIO           (0)
#define MCF_GPIO_PTCPAR_DTIN0_DTIN0          (0x1)
#define MCF_GPIO_PTCPAR_DTIN0_DTOUT0         (0x2)
#define MCF_GPIO_PTCPAR_DTIN0_PWM0           (0x3)
#define MCF_GPIO_PTCPAR_PTCPAR1(x)           (((x)&0x3)<<0x2)
#define MCF_GPIO_PTCPAR_DTIN1_GPIO           (0)
#define MCF_GPIO_PTCPAR_DTIN1_DTIN1          (0x4)
#define MCF_GPIO_PTCPAR_DTIN1_DTOUT1         (0x8)
#define MCF_GPIO_PTCPAR_DTIN1_PWM2           (0xC)
#define MCF_GPIO_PTCPAR_PTCPAR2(x)           (((x)&0x3)<<0x4)
#define MCF_GPIO_PTCPAR_DTIN2_GPIO           (0)
#define MCF_GPIO_PTCPAR_DTIN2_DTIN2          (0x10)
#define MCF_GPIO_PTCPAR_DTIN2_DTOUT2         (0x20)
#define MCF_GPIO_PTCPAR_DTIN2_PWM4           (0x30)
#define MCF_GPIO_PTCPAR_PTCPAR3(x)           (((x)&0x3)<<0x6)
#define MCF_GPIO_PTCPAR_DTIN3_GPIO           (0)
#define MCF_GPIO_PTCPAR_DTIN3_DTIN3          (0x40)
#define MCF_GPIO_PTCPAR_DTIN3_DTOUT3         (0x80)
#define MCF_GPIO_PTCPAR_DTIN3_PWM6           (0xC0)

/* Bit definitions and macros for MCF_GPIO_PORTUA */
#define MCF_GPIO_PORTUA_PORTUA0              (0x1)
#define MCF_GPIO_PORTUA_PORTUA1              (0x2)
#define MCF_GPIO_PORTUA_PORTUA2              (0x4)
#define MCF_GPIO_PORTUA_PORTUA3              (0x8)

/* Bit definitions and macros for MCF_GPIO_DDRUA */
#define MCF_GPIO_DDRUA_DDRUA0                (0x1)
#define MCF_GPIO_DDRUA_DDRUA1                (0x2)
#define MCF_GPIO_DDRUA_DDRUA2                (0x4)
#define MCF_GPIO_DDRUA_DDRUA3                (0x8)

/* Bit definitions and macros for MCF_GPIO_SETUA */
#define MCF_GPIO_SETUA_SETUA0                (0x1)
#define MCF_GPIO_SETUA_SETUA1                (0x2)
#define MCF_GPIO_SETUA_SETUA2                (0x4)
#define MCF_GPIO_SETUA_SETUA3                (0x8)

/* Bit definitions and macros for MCF_GPIO_CLRUA */
#define MCF_GPIO_CLRUA_CLRUA0                (0x1)
#define MCF_GPIO_CLRUA_CLRUA1                (0x2)
#define MCF_GPIO_CLRUA_CLRUA2                (0x4)
#define MCF_GPIO_CLRUA_CLRUA3                (0x8)

/* Bit definitions and macros for MCF_GPIO_PUAPAR */
#define MCF_GPIO_PUAPAR_PUAPAR0(x)           (((x)&0x3)<<0)
#define MCF_GPIO_PUAPAR_UTXD0_GPIO           (0)
#define MCF_GPIO_PUAPAR_UTXD0_UTXD0          (0x1)
#define MCF_GPIO_PUAPAR_UTXD0_USB_SUSPEND    (0x3)
#define MCF_GPIO_PUAPAR_PUAPAR1(x)           (((x)&0x3)<<0x2)
#define MCF_GPIO_PUAPAR_URXD0_GPIO           (0)
#define MCF_GPIO_PUAPAR_URXD0_URXD0          (0x4)
#define MCF_GPIO_PUAPAR_URXD0_USB_RCV        (0xC)
#define MCF_GPIO_PUAPAR_PUAPAR2(x)           (((x)&0x3)<<0x4)
#define MCF_GPIO_PUAPAR_URTS0_GPIO           (0)
#define MCF_GPIO_PUAPAR_URTS0_URTS0          (0x10)
#define MCF_GPIO_PUAPAR_URTS0_USB_VBUSD      (0x30)
#define MCF_GPIO_PUAPAR_PUAPAR3(x)           (((x)&0x3)<<0x6)
#define MCF_GPIO_PUAPAR_UCTS0_GPIO           (0)
#define MCF_GPIO_PUAPAR_UCTS0_UCTS0          (0x40)
#define MCF_GPIO_PUAPAR_UCTS0_USB_VBUSE      (0xC0)

/* Bit definitions and macros for MCF_GPIO_PORTUB */
#define MCF_GPIO_PORTUB_PORTUB0              (0x1)
#define MCF_GPIO_PORTUB_PORTUB1              (0x2)
#define MCF_GPIO_PORTUB_PORTUB2              (0x4)
#define MCF_GPIO_PORTUB_PORTUB3              (0x8)

/* Bit definitions and macros for MCF_GPIO_DDRUB */
#define MCF_GPIO_DDRUB_DDRUB0                (0x1)
#define MCF_GPIO_DDRUB_DDRUB1                (0x2)
#define MCF_GPIO_DDRUB_DDRUB2                (0x4)
#define MCF_GPIO_DDRUB_DDRUB3                (0x8)

/* Bit definitions and macros for MCF_GPIO_SETUB */
#define MCF_GPIO_SETUB_SETUB0                (0x1)
#define MCF_GPIO_SETUB_SETUB1                (0x2)
#define MCF_GPIO_SETUB_SETUB2                (0x4)
#define MCF_GPIO_SETUB_SETUB3                (0x8)

/* Bit definitions and macros for MCF_GPIO_CLRUB */
#define MCF_GPIO_CLRUB_CLRUB0                (0x1)
#define MCF_GPIO_CLRUB_CLRUB1                (0x2)
#define MCF_GPIO_CLRUB_CLRUB2                (0x4)
#define MCF_GPIO_CLRUB_CLRUB3                (0x8)

/* Bit definitions and macros for MCF_GPIO_PUBPAR */
#define MCF_GPIO_PUBPAR_PUBPAR0(x)           (((x)&0x3)<<0)
#define MCF_GPIO_PUBPAR_UTXD1_GPIO           (0)
#define MCF_GPIO_PUBPAR_UTXD1_UTXD1          (0x1)
#define MCF_GPIO_PUBPAR_UTXD1_USB_SPEED      (0x3)
#define MCF_GPIO_PUBPAR_PUBPAR1(x)           (((x)&0x3)<<0x2)
#define MCF_GPIO_PUBPAR_URXD1_GPIO           (0)
#define MCF_GPIO_PUBPAR_URXD1_URXD1          (0x4)
#define MCF_GPIO_PUBPAR_URXD1_USB_OE         (0xC)
#define MCF_GPIO_PUBPAR_PUBPAR2(x)           (((x)&0x3)<<0x4)
#define MCF_GPIO_PUBPAR_URTS1_GPIO           (0)
#define MCF_GPIO_PUBPAR_URTS1_URTS1          (0x10)
#define MCF_GPIO_PUBPAR_URTS1_SYNCB          (0x20)
#define MCF_GPIO_PUBPAR_URTS1_UTXD2          (0x30)
#define MCF_GPIO_PUBPAR_PUBPAR3(x)           (((x)&0x3)<<0x6)
#define MCF_GPIO_PUBPAR_UCTS1_GPIO           (0)
#define MCF_GPIO_PUBPAR_UCTS1_UCTS1          (0x40)
#define MCF_GPIO_PUBPAR_UCTS1_SYNCA          (0x80)
#define MCF_GPIO_PUBPAR_UCTS1_URXD2          (0xC0)

/* Bit definitions and macros for MCF_GPIO_PORTUC */
#define MCF_GPIO_PORTUC_PORTUC0              (0x1)
#define MCF_GPIO_PORTUC_PORTUC1              (0x2)
#define MCF_GPIO_PORTUC_PORTUC2              (0x4)
#define MCF_GPIO_PORTUC_PORTUC3              (0x8)

/* Bit definitions and macros for MCF_GPIO_DDRUC */
#define MCF_GPIO_DDRUC_DDRUC0                (0x1)
#define MCF_GPIO_DDRUC_DDRUC1                (0x2)
#define MCF_GPIO_DDRUC_DDRUC2                (0x4)
#define MCF_GPIO_DDRUC_DDRUC3                (0x8)

/* Bit definitions and macros for MCF_GPIO_SETUC */
#define MCF_GPIO_SETUC_SETUC0                (0x1)
#define MCF_GPIO_SETUC_SETUC1                (0x2)
#define MCF_GPIO_SETUC_SETUC2                (0x4)
#define MCF_GPIO_SETUC_SETUC3                (0x8)

/* Bit definitions and macros for MCF_GPIO_CLRUC */
#define MCF_GPIO_CLRUC_CLRUC0                (0x1)
#define MCF_GPIO_CLRUC_CLRUC1                (0x2)
#define MCF_GPIO_CLRUC_CLRUC2                (0x4)
#define MCF_GPIO_CLRUC_CLRUC3                (0x8)

/* Bit definitions and macros for MCF_GPIO_PUCPAR */
#define MCF_GPIO_PUCPAR_PUCPAR0(x)           (((x)&0x3)<<0)
#define MCF_GPIO_PUCPAR_UTXD2_GPIO           (0)
#define MCF_GPIO_PUCPAR_UTXD2_UTXD2          (0x1)
#define MCF_GPIO_PUCPAR_UTXD2_USB_SE0        (0x3)
#define MCF_GPIO_PUCPAR_PUCPAR1(x)           (((x)&0x3)<<0x2)
#define MCF_GPIO_PUCPAR_URXD2_GPIO           (0)
#define MCF_GPIO_PUCPAR_URXD2_URXD2          (0x4)
#define MCF_GPIO_PUCPAR_URXD2_USB_DATA       (0xC)
#define MCF_GPIO_PUCPAR_PUCPAR2(x)           (((x)&0x3)<<0x4)
#define MCF_GPIO_PUCPAR_URTS2_GPIO           (0)
#define MCF_GPIO_PUCPAR_URTS2_URTS2          (0x10)
#define MCF_GPIO_PUCPAR_URTS2_USB_VBUSDIS    (0x30)
#define MCF_GPIO_PUCPAR_PUCPAR3(x)           (((x)&0x3)<<0x6)
#define MCF_GPIO_PUCPAR_UCTS2_GPIO           (0)
#define MCF_GPIO_PUCPAR_UCTS2_UCTS2          (0x40)
#define MCF_GPIO_PUCPAR_UCTS2_USB_VBUSCHG    (0xC0)

/* Bit definitions and macros for MCF_GPIO_PORTDD */
#define MCF_GPIO_PORTDD_PORTDD0              (0x1)
#define MCF_GPIO_PORTDD_PORTDD1              (0x2)
#define MCF_GPIO_PORTDD_PORTDD2              (0x4)
#define MCF_GPIO_PORTDD_PORTDD3              (0x8)
#define MCF_GPIO_PORTDD_PORTDD4              (0x10)
#define MCF_GPIO_PORTDD_PORTDD5              (0x20)
#define MCF_GPIO_PORTDD_PORTDD6              (0x40)
#define MCF_GPIO_PORTDD_PORTDD7              (0x80)

/* Bit definitions and macros for MCF_GPIO_DDRDD */
#define MCF_GPIO_DDRDD_DDRDD0                (0x1)
#define MCF_GPIO_DDRDD_DDRDD1                (0x2)
#define MCF_GPIO_DDRDD_DDRDD2                (0x4)
#define MCF_GPIO_DDRDD_DDRDD3                (0x8)
#define MCF_GPIO_DDRDD_DDRDD4                (0x10)
#define MCF_GPIO_DDRDD_DDRDD5                (0x20)
#define MCF_GPIO_DDRDD_DDRDD6                (0x40)
#define MCF_GPIO_DDRDD_DDRDD7                (0x80)

/* Bit definitions and macros for MCF_GPIO_SETDD */
#define MCF_GPIO_SETDD_SETDD0                (0x1)
#define MCF_GPIO_SETDD_SETDD1                (0x2)
#define MCF_GPIO_SETDD_SETDD2                (0x4)
#define MCF_GPIO_SETDD_SETDD3                (0x8)
#define MCF_GPIO_SETDD_SETDD4                (0x10)
#define MCF_GPIO_SETDD_SETDD5                (0x20)
#define MCF_GPIO_SETDD_SETDD6                (0x40)
#define MCF_GPIO_SETDD_SETDD7                (0x80)

/* Bit definitions and macros for MCF_GPIO_CLRDD */
#define MCF_GPIO_CLRDD_CLRDD0                (0x1)
#define MCF_GPIO_CLRDD_CLRDD1                (0x2)
#define MCF_GPIO_CLRDD_CLRDD2                (0x4)
#define MCF_GPIO_CLRDD_CLRDD3                (0x8)
#define MCF_GPIO_CLRDD_CLRDD4                (0x10)
#define MCF_GPIO_CLRDD_CLRDD5                (0x20)
#define MCF_GPIO_CLRDD_CLRDD6                (0x40)
#define MCF_GPIO_CLRDD_CLRDD7                (0x80)

/* Bit definitions and macros for MCF_GPIO_PDDPAR */
#define MCF_GPIO_PDDPAR_PDDPAR0              (0x1)
#define MCF_GPIO_PDDPAR_PST0_GPIO            (0)
#define MCF_GPIO_PDDPAR_PST0_PST0            (0x1)
#define MCF_GPIO_PDDPAR_PDDPAR1              (0x2)
#define MCF_GPIO_PDDPAR_PST1_GPIO            (0)
#define MCF_GPIO_PDDPAR_PST1_PST1            (0x2)
#define MCF_GPIO_PDDPAR_PDDPAR2              (0x4)
#define MCF_GPIO_PDDPAR_PST2_GPIO            (0)
#define MCF_GPIO_PDDPAR_PST2_PST2            (0x4)
#define MCF_GPIO_PDDPAR_PDDPAR3              (0x8)
#define MCF_GPIO_PDDPAR_PST3_GPIO            (0)
#define MCF_GPIO_PDDPAR_PST3_PST3            (0x8)
#define MCF_GPIO_PDDPAR_PDDPAR4              (0x10)
#define MCF_GPIO_PDDPAR_DDATA0_GPIO          (0)
#define MCF_GPIO_PDDPAR_DDATA0_DDATA0        (0x10)
#define MCF_GPIO_PDDPAR_PDDPAR5              (0x20)
#define MCF_GPIO_PDDPAR_DDATA1_GPIO          (0)
#define MCF_GPIO_PDDPAR_DDATA1_DDATA1        (0x20)
#define MCF_GPIO_PDDPAR_PDDPAR6              (0x40)
#define MCF_GPIO_PDDPAR_DDATA2_GPIO          (0)
#define MCF_GPIO_PDDPAR_DDATA2_DDATA2        (0x40)
#define MCF_GPIO_PDDPAR_PDDPAR7              (0x80)
#define MCF_GPIO_PDDPAR_DDATA3_GPIO          (0)
#define MCF_GPIO_PDDPAR_DDATA3_DDATA3        (0x80)


#endif /* __MCF52221_GPIO_H__ */
