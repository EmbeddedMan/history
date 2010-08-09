/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2007/03/19 Revision: 0.91
 */

#ifndef __MCF52221_PMM_H__
#define __MCF52221_PMM_H__


/*********************************************************************
*
* Power Management (PMM)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_PMM_LPICR                        (*(vuint8 *)(&__IPSBAR[0x12]))
#define MCF_PMM_LPCR                         (*(vuint8 *)(&__IPSBAR[0x110007]))


/* Bit definitions and macros for MCF_PMM_LPICR */
#define MCF_PMM_LPICR_XLPM_IPL(x)            (((x)&0x7)<<0x4)
#define MCF_PMM_LPICR_ENBSTOP                (0x80)

/* Bit definitions and macros for MCF_PMM_LPCR */
#define MCF_PMM_LPCR_STPMD                   (0x8)
#define MCF_PMM_LPCR_LPMD(x)                 (((x)&0x3)<<0x6)
#define MCF_PMM_LPCR_LPMD_RUN                (0)
#define MCF_PMM_LPCR_LPMD_DOZE               (0x40)
#define MCF_PMM_LPCR_LPMD_WAIT               (0x80)
#define MCF_PMM_LPCR_LPMD_STOP               (0xC0)


#endif /* __MCF52221_PMM_H__ */
