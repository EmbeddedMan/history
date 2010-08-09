/*
 * FILENAME: netbuf.h
 *
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
 *
 * Descriptions of the packet buffer structure, etc. 
 *
 * MODULE: INET
 *
 *
 * PORTABLE: yes
 */

 
/* Each buffer is in a 
 * queue, either the free queue or the used queue (or the buffer is 
 * currently being used by a user program or the interrupt level 
 * routines, in which case it does not appear in a queue). When a 
 * buffer is in a queue, it has a pointer to the next buffer in the 
 * queue. If there is no next buffer, its pointer points at null. 
 * Also, each buffer knows its own length. 
 */

#ifndef _NETBUF_H
#define  _NETBUF_H   1

/* 
 * save socket options to be passed via the PACKET.
 */
struct ip_socopts
{
	u_char ip_tos;     /* TOS */
	u_char ip_tll;     /* TLL */
};


/* This include file gives the structure of PACKET buffers. 
 * INCOMING: Incoming packets are always front-aligned in the 
 * nb_buff field. The nb_prot pointer is set to nb_buff by the 
 * receiver and advanced by each layer of the stack before 
 * upcalling the next; ie the Ethernet driver bumps the prot field 
 * by 14 and decrements plen by 14. PACKETs are pk_alloc()ed by 
 * the receiving net layer and pk_free()ed by the transport layer 
 * or application when it's finished with them. OUTGOING: 
 * Protocols install data into nb_buff with a front pad big enough 
 * to accomadate the biggest likely protocol headers, ususally 
 * about 62 bytes (14 ether + 24 IP + 24 TCP, where IP & TCP each 
 * have option fields.) prot plen are set for this data, and the 
 * protocol headers are prepended as the packet goes down the 
 * stack. nb_buff is not used in this case except for overflow 
 * checks. PACKETs are pk_alloc()ed by the sending protocol and 
 * freed by the lower layer level that dispatches them, usually 
 * net link layer driver. They can be held by ARP for several 
 * seconds while awaiting arp replys on initial sends to a new IP 
 * host, and the ARP code will free them when a reply comes in or 
 * times out. 
 */

struct netbuf
{
   struct netbuf * next;   /* queue link */
   char   * nb_buff;    /* beginning of raw buffer */
   unsigned nb_blen;    /* length of raw buffer */
   char   * nb_prot;    /* beginning of protocol/data */
   unsigned nb_plen;    /* length of protocol/data */
   long     nb_tstamp;  /* packet timestamp */
   struct net *   net;  /* the interface (net) it came in on, 0-n */
   ip_addr  fhost;      /* IP address asociated with packet */
   unsigned short type; /* IP==0800 filled in by recever(rx) or net layer.(tx) */
   unsigned inuse;      /* use count, for cloning buffer */
   unsigned flags;      /* bitmask of the PKF_ defines */

#ifdef PPP_MULTILINK
   void * link;         /* which of Multiple links to use */
#endif /* PPP_MULTILINK */
   
#ifdef MINI_TCP            /* Mini TCP has no mbuf wrappers, so: */
   char *   m_data;        /* pointer to TCP data in nb_buff */
   unsigned m_len;         /* length of m_data */
   struct netbuf * m_next; /* sockbuf que link */
#endif   /* MINI_TCP */

#ifdef IP_MULTICAST
   struct ip_moptions * imo;  /* IP multicast options */
#endif

#ifdef LINKED_PKTS
   /* support scatter/gather - required for IPv6, tunneling, et.al. */
   struct netbuf * pk_prev;   /* previous pkt in chain */
   struct netbuf * pk_next;   /* next ptk in chain */
   int      nb_tlen;          /* total length of nb_prots in pk_next list */
#ifdef IP_V6
   struct ipv6 * ip6_hdr;     /* "Current" IPv6 header */
   struct in6_addr * nexthop; /* for pass to ipv6_send() */
   int      nb_pmtu;          /* Path MTU for sends */
#endif   /* IP_V6 */
#endif   /* LINKED_PKTS */
   struct ip_socopts *soxopts; /* socket options */
};

/* bits for pkt->flags field */
#define  PKF_BCAST    0x01
#define  PKF_MCAST    0x02
#define  PKF_IPOPTS   0x04     /* ip_socopts present */

typedef struct netbuf * PACKET;     /* struct netbuf in netbuf.h */

/* count & sizes for the fre buffer queues */
extern   unsigned lilbufs;    /* number of small bufs to init */
extern   unsigned lilbufsiz;  /* big enough for average packet */
extern   unsigned bigbufs;    /* number of big bufs to init */
extern   unsigned bigbufsiz;  /* big enough for max. Ethernet packet */

/* Pack buffer routines, Defined in the new system for each port: */
PACKET   pk_alloc(unsigned size);      /* allocate a packet for sending */
void     pk_free(PACKET);
int      pk_init(void);    /* call at init time to set up for pk_allocs */
PACKET   pk_prepend(PACKET pkt, int bigger);    /* prepend new buffer */
PACKET   pk_gather(PACKET pkt, int headerlen);  /* "gather" a buffer list */

#endif   /*  _NETBUF_H */


