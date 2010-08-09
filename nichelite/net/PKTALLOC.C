/*
 * FILENAME: pktalloc.c
 *
 * Copyright 1997- 2006 By InterNiche Technologies Inc. All rights reserved
 *
 * Portions Copyright 1986 by Carnegie Mellon
 * Portions Copyright 1984 by the Massachusetts Institute of Technology
 *
 * Copyright (c) 1982, 1986, 1988 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation and other 
 * materials related to such distribution and use acknowledge that 
 * the software was developed by the University of California, Berkeley.
 * The name of the University may not be used to endorse or promote 
 * products derived from this software without specific prior written 
 * permission. THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 * Rights, responsibilities and use of this software are controlled by
 * the agreement found in the "LICENSE.H" file distributed with this
 * source code.  "LICENSE.H" may not be removed from this distribution,
 * modified, enhanced nor references to it omitted.
 *
 * Code to manage the queues of free packet buffers.
 *
 * MODULE: INET
 *
 * ROUTINES: pk_init(), pk_alloc(), pk_validate(), pk_free(), 
 * ROUTINES: pk_prepend(), pk_gather(), 
 *
 * PORTABLE: yes
 */

#include "license.h"
#include "ipport.h"
#include "q.h"
#include "netbuf.h"
#include "net.h"
#include "ip.h"


/* We maintain 2 queues of free buffers, one for smallish packets
 * like ARPs, TCP acks,and PINGs; and big buffers for large datagrams
 * like FTP transfers.
 */
queue   bigfreeq;    /* big free buffers */
queue   lilfreeq;    /* small free buffers */

/* these next four values control the number and sizes of free 
 * buffers. The defaults below can be overwritten by the caller prior 
 * to calling 
 */

/* define defaults */
#ifndef  NUMBIGBUFS
#define  NUMBIGBUFS     30			//FSL can be overridden in ipport.h
#endif   /* NUMBIGBUFS */
#ifndef  NUMLILBUFS
#define  NUMLILBUFS     30			//FSL can be overridden in ipport.h
#endif   /* NUMLILBUFS */
#ifndef  BIGBUFSIZE
#define  BIGBUFSIZE     1536
#endif   /* BIGBUFSIZE */
#ifndef  LILBUFSIZE

#ifdef WEBPORT    /* web severs send a lot of packets <> 160 bytes */
#define  LILBUFSIZE     200
#elif  VALID_EMG_SESSION			//FSL added for FSL web server option
#define  LILBUFSIZE     200
#else  /* no Web server in this link */
#define  LILBUFSIZE     128
#endif   /* WEBPORT */
#endif   /* LILBUFSIZE */

unsigned lilbufs = NUMLILBUFS;      /* number of small bufs to init */
unsigned lilbufsiz = LILBUFSIZE;    /* big enough for most non-full size packets */
unsigned bigbufs = NUMBIGBUFS;      /* number of big bufs to init */
unsigned bigbufsiz = BIGBUFSIZE;    /* big enough for max. Ethernet packet */

#ifdef NPDEBUG
PACKET pktlog[MAXPACKETS]; /* record where the packets are */
#endif


/* FUNCTION: pk_alloc()
 *
 * PACKET pk_alloc(len) Returns a pointer to a netbuf structure 
 * (PACKET) with a buffer big enough for len bytes, or NULL if none 
 * is available. 
 *
 * 
 * PARAM1: unsigned len
 *
 * RETURNS:  a pointer to a netbuf structure 
 * (PACKET) with a buffer big enough for len bytes, or NULL if none 
 * is available. 
 */

PACKET
pk_alloc(unsigned len)
{
   PACKET p;

   if (len > bigbufsiz) /* caller wants oversize buffer? */
      return(NULL);

   if ((len > lilbufsiz) || (lilfreeq.q_len == 0)) /* must use a big buffer */
      p = (PACKET)getq(&bigfreeq);
   else
      p = (PACKET)getq(&lilfreeq);

   if (!p)
      return NULL;

   p->nb_prot = p->nb_buff + MaxLnh;   /* point past biggest mac header */
   p->nb_plen = 0;   /* no protocol data there yet */
   p->net = NULL;

#ifdef LINKED_PKTS
   p->pk_next = p->pk_prev = NULL;
   p->flags = 0;
#ifdef IP_V6
   p->ip6_hdr = NULL;      /* No "Current" IPv6 header */
   p->nexthop = NULL;      /* no next hop  */
   p->nb_pmtu = 1240;      /* Set minimum IPv6 Path MTU */
#endif   /* IP_V6 */
#endif /* LINKED_PKTS */

#ifdef NPDEBUG
   if ((p->nb_blen != bigbufsiz) && (p->nb_blen != lilbufsiz))
      panic("pk_alloc: illegal nb_blen size");
#endif

   p->inuse = 1;  /* initially buffer in use by 1 user */
   return(p);
}

/* FUNCTION: pk_validate()
 *
 * pk_validate() - Check if we can free this packet.
 * Does a number of checks to check the validity of the packet
 * Used when NPDEBUG is enabled to catch packet leaks.
 * 1. Check if the packet size is standard or not.
 *    If it is not one of bigbufsiz or lilbufsiz, then this
 *    packet has been corrupted.
 * 2. Checks if the packet's "next" link is zero or not.
 *    If its non-zero (and it inuse field is 1), then this
 *    pkt is being freed while being a part of a linked list.
 *    Meaning that freeing this packet would break the linked list
 *    and cause subsequent problems.
 * 3. Check if this packet has already been freed or not. That is,
 *    check whether this packet is already present in one of bigfreeq
 *    or lilfreeq.
 * 4. Check if the markers bordering the packet are intact or not.
 *    If not, then this packet has been corrupted.
 *
 * PARAM1: PACKET pkt
 *
 * RETURNS: 
 *    0    - if the the pkt can be freed by pk_free()
 *   -1    - if pkt was already freed
 *         - panic() is called if there is a fatal problem
 *
 */


#ifdef NPDEBUG
int 
pk_validate(PACKET pkt)   /* check if pk_free() can free the pkt */
{
   PACKET   p;
   int      j;

   if ((pkt->nb_blen != bigbufsiz) && (pkt->nb_blen != lilbufsiz))
      panic("pk_free: illegal nb_blen size");

   /* If packet link is non-zero, then this packet is
    * part of a chain and deleted this packet would break
    * the chain and cause memory leak for subsequent pkts.
    */
   if ((pkt->next) && (pkt->inuse == 1))
      panic("pk_free: packet is part of a chain");

   /* check if the packet is already in a freeq */
   if (pkt->nb_blen == bigbufsiz)  /* check in bigfreeq */
   {
      ENTER_CRIT_SECTION(&bigfreeq);
      for (p=(PACKET)bigfreeq.q_head; p; p = p->next)
         if (p == pkt)
      {
         dprintf("pk_free: buffer %p already in bigfreeq\n", pkt);
         dtrap();
         EXIT_CRIT_SECTION(&bigfreeq);
         return -1;        /* should this be a fatal error? -later. */
      }
      EXIT_CRIT_SECTION(&bigfreeq);
   }
   else   /* check in lilfreeq */
   {
      ENTER_CRIT_SECTION(&lilfreeq);
      for (p=(PACKET)lilfreeq.q_head; p; p = p->next)
         if (p == pkt)
      {
         dprintf("pk_free: buffer %p already in lilfreeq\n", pkt);
         dtrap();
         EXIT_CRIT_SECTION(&lilfreeq);
         return -1;        /* should this be a fatal error? -later. */
      }
      EXIT_CRIT_SECTION(&lilfreeq);
   }

   /* check for corruption of memory markers */
   for(j = 4; j > 0; j--)
   {
      if (*(pkt->nb_buff - j) != 'M')
      {
         panic("pk_free: corrupt packet buffer");
      }
   }
   if (*(pkt->nb_buff + pkt->nb_blen) != 'M')
   {
      panic("pk_free: corrupt packet buffer");
   }

   return 0;
}
#else
#define pk_validate(p)   0
#endif   /* NPDEBUG */


/* FUNCTION: pk_free()
 *
 * pk_free() - Return a packet to the free queue. 
 *
 * PARAM1: PACKET pkt
 *
 * RETURNS: void
 */

void
pk_free(PACKET pkt)   /* PACKET to place in free queue */
{
   int e;

#ifdef LINKED_PKTS
   /* If we support scatter/gather, then we have to loop through the
    * whole chain of packets that were passed.
    */
   while(pkt)
   {
      PACKET pknext;
      pknext = pkt->pk_next;
#endif /* LINKED_PKTS */

      /* validate the pkt before freeing */
      e = pk_validate(pkt);
      if (e)
      {
#ifdef LINKED_PKTS
         if (e == -1)
         {
            pkt = pknext;
            continue; /* skip this pkt, examine the next pkt */
         }
#endif
         return;
      }
   
      if (pkt->inuse-- > 1)   /* more than 1 owner? */
         return;  /* packet was cloned, don't delete yet */
   
      if (pkt->nb_blen == bigbufsiz)
         q_add(&bigfreeq, (qp)pkt);
      else
         q_add(&lilfreeq, (qp)pkt);
#ifdef LINKED_PKTS
      pkt = pknext;
   }
#endif 

}


/* FUNCTION: pk_init()
 *
 * init the free queue (or queues) for use by pk_alloc() and 
 * pk_free() above.  
 *
 * 
 * PARAM1: none
 *
 * RETURNS: Returns 0 if OK, else negative error code. 
 */

int
pk_init()
{
   PACKET packet;
   unsigned i;
   unsigned numpkts = bigbufs + lilbufs;

   for (i = 0; i < numpkts; i++)
   {
      packet = (PACKET)NB_ALLOC(sizeof(struct netbuf));
      if (packet == NULL)
         goto no_pkt_buf;

#ifdef NPDEBUG
      if (i >= MAXPACKETS)
      {
         dprintf("pk_init: bad define\n");
         return -1;
      }
      pktlog[i] = packet;     /* save for debugging */
#endif

      packet->nb_tstamp = 0L;

      if (i < bigbufs)
      {
#ifdef NPDEBUG
         {
            int j;

            /* for DEBUG compiles, bracket the data area with special chars */
            packet->nb_buff = (char *)BB_ALLOC(bigbufsiz+3);
            if (!(packet->nb_buff))
               goto no_pkt_buf;

            /* Add memory markers for sanity check */
            for(j = 0; j < 4; j++)
               *(packet->nb_buff + j) = 'M'; /* MMs at start of buf */

            *(packet->nb_buff + bigbufsiz + 4) = 'M';
            packet->nb_buff += 4;   /* bump buf past MMs */
         }
#else
         packet->nb_buff = (char *)BB_ALLOC(bigbufsiz);
#endif
         if (!(packet->nb_buff))
            goto no_pkt_buf;
         packet->nb_blen = bigbufsiz;
         q_add(&bigfreeq, packet);        /* save it in big pkt free queue */
      }
      else     /* get a small buffer */
      {
#ifdef NPDEBUG
         {
            int j;

            /* for DEBUG compiles, bracket the data area with special chars */
            packet->nb_buff = (char *)LB_ALLOC(lilbufsiz+4+1);
            if (!(packet->nb_buff))
               goto no_pkt_buf;

            /* Add memory markers for sanity check */
            for(j = 0; j < 4; j++)
               *(packet->nb_buff + j) = 'M'; /* MMs at start of buf */

            *(packet->nb_buff + lilbufsiz + 4) = 'M';
            packet->nb_buff += 4;
         }
#else
         packet->nb_buff = (char *)LB_ALLOC(lilbufsiz);
#endif
         if (!(packet->nb_buff))
            goto no_pkt_buf;
         packet->nb_blen = lilbufsiz;
         q_add(&lilfreeq, packet);        /* save it in little free queue */
      }
   }
   bigfreeq.q_min = bigbufs;
   lilfreeq.q_min = lilbufs;
   return 0;

no_pkt_buf:
#ifdef NPDEBUG
   dprintf("Netinit: calloc failed getting buffer %d\n", i);
#endif
   return(-1);
}



/* FUNCTION: pk_prepend()
 *
 * Allocate a new buffer and link it ahead of the packet passed. The new
 * buffer is returned, with the passed buffer linked to it's scatter 
 * gather list. 
 *
 * CAUTIONS: 
 * - The new packet's nb_prot == nb_buff (no MAC header allowance)
 * - If pk_alloc fails, the passed packet is freed.
 *
 * PARAM1: packet to extend.
 * PARAM1: size of buffer to prepend.
 *
 * RETURNS: New packet pointer if OK or NULL if pk_alloc() fails.
 */

#ifdef LINKED_PKTS

PACKET
pk_prepend(PACKET pkt, int bigger)
{
   PACKET newpkt;

   LOCK_NET_RESOURCE(FREEQ_RESID);
   newpkt = pk_alloc(bigger);
   UNLOCK_NET_RESOURCE(FREEQ_RESID);
   if(!newpkt)
   {
      LOCK_NET_RESOURCE(FREEQ_RESID);
      pk_free(pkt);
      UNLOCK_NET_RESOURCE(FREEQ_RESID);
      return NULL;
   }
   newpkt->nb_prot = newpkt->nb_buff;  /* no MAC prepend */

   /* set lengths of this buffer - no data yet */
   newpkt->nb_plen = 0;       /* nothing in this buffer */
   newpkt->nb_tlen = pkt->nb_tlen;  /* total chain length unchanged */

   newpkt->pk_next = pkt;
   newpkt->pk_prev = NULL;
   newpkt->flags = pkt->flags;
   newpkt->net = pkt->net;
   pkt->pk_prev = newpkt;     /* link new pkt */
   return(newpkt);
}


/* FUNCTION: pk_gather()
 *
 * Allocate a new buffer and "gather" the data in the linked list passed into the
 * single buffer. The data starts at nb_buff, with no allowance for a MAC.
 *
 *
 * PARAM1: packet to gather
 * PARAM2: headerlen - space to leave at head for headers
 *
 * RETURNS: New packet pointer if OK or NULL if pk_alloc() fails.
 */

PACKET
pk_gather(PACKET pkt, int headerlen)
{
   PACKET   newpkt;
   PACKET   tmppkt;
   int      oldlen, newlen;
   char *   cp;

   oldlen = pkt->nb_tlen;

   LOCK_NET_RESOURCE(FREEQ_RESID);
   newpkt = pk_alloc(oldlen + headerlen);
   UNLOCK_NET_RESOURCE(FREEQ_RESID);
   if(!newpkt)
      return NULL;

   newpkt->nb_prot = newpkt->nb_buff + headerlen;
   newpkt->flags = pkt->flags;
   newpkt->net = pkt->net;
   newpkt->pk_prev = newpkt->pk_next = NULL;

   tmppkt = pkt;        /* save packet for pk_free call below */
   newlen = 0;
   cp = newpkt->nb_prot;
   while(pkt)
   {
      MEMCPY(cp, pkt->nb_prot, pkt->nb_plen);
      newlen += pkt->nb_plen;
      cp += pkt->nb_plen;  /* bump pointer to data */
      pkt = pkt->pk_next;  /* next packet in chain */
   }
   LOCK_NET_RESOURCE(FREEQ_RESID);
   pk_free(tmppkt);     /* free last packet in chain */
   UNLOCK_NET_RESOURCE(FREEQ_RESID);
   if(newlen != oldlen)    /* make sure length was right */
      panic("pk_gather");

   newpkt->nb_plen = newlen;
   return newpkt;
}

#endif /* LINKED_PKTS */


/* end of file pktalloc.c */

