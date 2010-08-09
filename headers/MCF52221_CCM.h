/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2007/03/19 Revision: 0.91
 */

#ifndef __MCF52221_CCM_H__
#define __MCF52221_CCM_H__


/*********************************************************************
*
* Chip Configuration Module (CCM)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_CCM_CCR                          (*(vuint16*)(&__IPSBAR[0x110004]))
#define MCF_CCM_RCON                         (*(vuint16*)(&__IPSBAR[0x110008]))
#define MCF_CCM_CIR                          (*(vuint16*)(&__IPSBAR[0x11000A]))


/* Bit definitions and macros for MCF_CCM_CCR */
#define MCF_CCM_CCR_Mode(x)                  (((x)&0x7)<<0x8)
#define MCF_CCM_CCR_MODE_SINGLECHIP          (0x600)
#define MCF_CCM_CCR_MODE_EZPORT              (0x500)

/* Bit definitions and macros for MCF_CCM_RCON */
#define MCF_CCM_RCON_MODE                    (0x1)
#define MCF_CCM_RCON_RLOAD                   (0x20)

/* Bit definitions and macros for MCF_CCM_CIR */
#define MCF_CCM_CIR_PRN(x)                   (((x)&0x3F)<<0)
#define MCF_CCM_CIR_PIN(x)                   (((x)&0x3FF)<<0x6)


#endif /* __MCF52221_CCM_H__ */
