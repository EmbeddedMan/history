/* FILENAME: fecport.h
 *
 * Copyright 2003-2005 by InterNiche Technologies Inc. All rights reserved.
 *
 * 12/27/2005 - created from mcf5235/fecport.h
 *
 * Freescale 10/100 Fast Ethernet Controller Driver
 *
 * MODULE: FEC
 * PORTABLE: no
 */

#ifndef _FECPORT_H_
#define _FECPORT_H_ 1

#include "fec.h"

/* InterNiche MAC routines to support Freescale ColdFire FEC */
int   prep_fec(int);
int   fec_init(int);
int   fec_pkt_send(struct netbuf *);
void  fec_stats(void *, int);
int   fec_close(int);
int   fec_tx(PACKET);
int   fectx_internal(void);
void  fec_isr(void);
int   input_ippkt(PACKET, int);
void  FEC_reset(void);
// int   FECInit(int);						//FSL declared in ifec.c
int   FEC_readMIB(struct fec_mib *);
int   FEC_clearMIB(void);
//void  fec_intinit(void);				//FSL redundant and spelling
int   fec_link_setup(void);

/* default number of Receive and Transmit Buffer Descriptors */
#ifndef NUM_RXBDS
#define NUM_RXBDS    2
#endif
#ifndef NUM_TXBDS
#define NUM_TXBDS    2
#endif


/*--------------------------*/
/* Buffer Descriptor Format */
/*--------------------------*/

typedef struct BufferDescriptor 
{
   volatile unshort  bd_cstatus;     /* control and status */
   volatile unshort  bd_length;      /* transfer length */
   volatile u_char * bd_addr;        /* buffer address */
} BD;

void  dump_bd(void * pio, BD * bdp, int count);

#define ETH_ADDR_LEN    (6)
#define ETH_TYPE_LEN    (2)
#define ETH_CRC_LEN     (4)
#define ETH_HDR_LEN     (ETH_ADDR_LEN * 2 + ETH_TYPE_LEN)

/* Maximum packet size we can handle on a FEC. Since we don't
 * scatter/gather this is both max buffer and max frame size, and
 * applies to both transmit and receive.
 */

#define MAX_ETH_PKT 0x000005ee		//FSL 0x5ee=1518 which is default for FEC

/* #define MAX_ETH_PKT (bigbufsiz & 0xFFF0) */

/* Allocation routine for FEC buffers. Override this to make sure The
 * buffer memory is not cached on MMU systems.
 */
char *alloc_uncached(int size);

/* Allocate data from non-cached memory (Ethernet buffers & control blocks) */
extern unsigned bigbufsiz;
extern unsigned lilbufsiz;
extern void     dump_bd(void * pio, BD * bdp, int count);
extern int      input_ippkt(PACKET pkt, int length);

struct fec_statistics
{
   /* FEC stats: */
   /* interrupt counters */
   u_long   fec_ints;         /* total ISR calls */
   u_long   fec_txints;       /* transmit ints */
   u_long   fec_rxints;       /* receive ints */
   u_long   fec_errints;      /* error reporting ints */
   u_long   fec_phyints;      /* ints from phy chip */

   /* Transmit BD errors */
   u_long   fec_clserrs;      /* late collision */
   u_long   fec_unerrs;       /* underrun */
   u_long   fec_retries;      /* total retries (non fatal ) */
   u_long   fec_rlerrs;       /* retry limit exceeded (fatal) */
   u_long   fec_hberrs;       /* heartbeat */

   /* Ethernet net errors */
   u_long   fec_ehberrs;      /* heartbeat (again?) */
   u_long   fec_bablterrs;    /* Babble - transmitter */
   u_long   fec_busyerrs;     /* Ethernet busy */
   u_long   fec_bablrerrs;    /* Babble - receiver */

   /* Receive BD errors */
   u_long   fec_lgerrs;       /* frame too large */
   u_long   fec_noerrs;       /* non-octet aligned frame */
   u_long   fec_sherrs;       /* short frame */
   u_long   fec_crerrs;       /* CRC check failed */
   u_long   fec_overrs;       /* overrun */
   u_long   fec_trerrs;       /* truncated packet */
};

extern struct fec_statistics fecstats;

#endif  /* _FECPORT_H_ */
