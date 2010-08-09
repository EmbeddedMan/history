/* ColdFire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 */

#ifndef __MCF52235_CIM_H__
#define __MCF52235_CIM_H__

/*********************************************************************
*
* ColdFire Integration Module (CIM)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_CIM_RCR             (*(vuint8 *)(&__IPSBAR[0x110000]))
#define MCF_CIM_RSR             (*(vuint8 *)(&__IPSBAR[0x110001]))
#define MCF_CIM_CCR             (*(vuint16*)(&__IPSBAR[0x110004]))
#define MCF_CIM_LPCR            (*(vuint8 *)(&__IPSBAR[0x110007]))
#define MCF_CIM_RCON            (*(vuint16*)(&__IPSBAR[0x110008]))
#define MCF_CIM_CIR             (*(vuint16*)(&__IPSBAR[0x11000A]))

/* Bit definitions and macros for MCF_CIM_RCR */
#define MCF_CIM_RCR_LVDE        (0x01)
#define MCF_CIM_RCR_LVDRE       (0x04)
#define MCF_CIM_RCR_LVDIE       (0x08)
#define MCF_CIM_RCR_LVDF        (0x10)
#define MCF_CIM_RCR_FRCRSTOUT   (0x40)
#define MCF_CIM_RCR_SOFTRST     (0x80)

/* Bit definitions and macros for MCF_CIM_RSR */
#define MCF_CIM_RSR_LOL         (0x01)
#define MCF_CIM_RSR_LOC         (0x02)
#define MCF_CIM_RSR_EXT         (0x04)
#define MCF_CIM_RSR_POR         (0x08)
#define MCF_CIM_RSR_WDR         (0x10)
#define MCF_CIM_RSR_SOFT        (0x20)
#define MCF_CIM_RSR_LVD         (0x40)

/* Bit definitions and macros for MCF_CIM_CCR */
#define MCF_CIM_CCR_LOAD        (0x8000)

/* Bit definitions and macros for MCF_CIM_LPCR */
#define MCF_CIM_LPCR_LVDSE      (0x02)
#define MCF_CIM_LPCR_STPMD(x)   (((x)&0x03)<<3)
#define MCF_CIM_LPCR_LPMD(x)    (((x)&0x03)<<6)
#define MCF_CIM_LPCR_LPMD_STOP  (0xC0)
#define MCF_CIM_LPCR_LPMD_WAIT  (0x80)
#define MCF_CIM_LPCR_LPMD_DOZE  (0x40)
#define MCF_CIM_LPCR_LPMD_RUN   (0x00)

/* Bit definitions and macros for MCF_CIM_RCON */
#define MCF_CIM_RCON_RLOAD      (0x0020)

/********************************************************************/

#endif /* __MCF52235_CIM_H__ */
