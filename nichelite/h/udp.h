/*
 * FILENAME: udp.h
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
 * UDP protocol definitions
 *
 * MODULE: INET
 *
 * PORTABLE: yes
 */


#ifndef _UDP_H_
#define  _UDP_H_  1


/* UDP Header structure */

struct udp
{
   unshort  ud_srcp;    /* source port */
   unshort  ud_dstp;    /* dest port */
   unshort  ud_len;     /* length of UDP packet */
   unshort  ud_cksum;   /* UDP checksum */
};

/* The UDP Connection structure */
struct udp_conn
{
   struct udp_conn * u_next;
   unshort  u_flags;    /* flags for this connection */
   unshort  u_lport;    /* local port (host byte order) */
   unshort  u_fport;    /* foreign port (host byte order) */
#if  (defined(IP_V4) || defined(MINI_TCP))
   ip_addr  u_lhost;    /* local host IP address (network byte order) */
   ip_addr  u_fhost;    /* foreign host IP address (network byte order) */
#endif
#ifdef IP_V6
   ip6_addr u_l6host;   /* local host IPv6 address */
   ip6_addr u_f6host;   /* foreign host IPv6 address */
#endif
   int      (*u_rcv)(PACKET, void*);   /* incoming packet handler */
   void *   u_data;     /* user 'per-connection' token */
   int      (*u_durcv)();  /* incoming dest. unreach. handler */
};

/* Flags for use in u_flags: */
#define UDPCF_V4     0x0001   /* flag: tied to IPv4 */
#define UDPCF_V6     0x0002   /* flag: tied to IPv6 */

/* UDP stats (MIB information), see RFC 1156 */
struct UdpMib
{
   u_long   udpInDatagrams;   /* total delivered Datagrams */
   u_long   udpNoPorts;       /* undelivered datagrams: unused port */
   u_long   udpInErrors;      /* undelivered datagrams: other reasons */
   u_long   udpOutDatagrams;  /* successfully sent datagrams */
};

extern   struct UdpMib  udp_mib;

typedef  struct UdpMib * UDPMIB;

typedef  struct udp_conn * UDPCONN;

struct ph         /* pseudo header for checksumming */
{
   ip_addr  ph_src;  /* source address */
   ip_addr  ph_dest; /* dest address */
   char     ph_zero; /* zero (reserved) */
   char     ph_prot; /* protocol */
   unshort  ph_len;  /* udp length */
};


#define  UDP_PROT    17 /* UDP Internet protocol number */
#define  UDPLEN   sizeof(struct  udp)

#define  udp_head(pip)     ((struct udp *)ip_data((pip)))
#define  udp_data(pup)     ((char *)(pup) +  sizeof(struct  udp))

extern   UDPCONN  firstudp;

/* Some reserved UDP ports */
#define  RIP_PORT    520
#define  DNS_PORT    53
#define  SNMP_PORT   161

/* estimate worst case header sizes for packet allocation */

/* Figure out a minimum value 
 * IP Hdr is 20 bytes and Ethernet header is 14 bytes 
 */
#define UDP_LLHDR (34 + ETHHDR_BIAS  + IPSECOPT_SIZE) 

#ifdef USE_PPPOE
#define  UDPHDRSMINLEN  (10 + UDP_LLHDR)
#elif  defined(IEEE_802_3)
#define  UDPHDRSMINLEN  (8 + UDP_LLHDR)
#else
#define  UDPHDRSMINLEN  UDP_LLHDR
#endif

/* Figure out the actual value: minimum value padded out to 
 * a multiple of 4
 */
#if ((UDPHDRSMINLEN & (4 - 1)) == 0)
#define  UDPHDRSLEN  UDPHDRSMINLEN
#else
#define  UDPHDRSLEN  (UDPHDRSMINLEN + 4 - (UDPHDRSMINLEN & (3)))
#endif


/* function prototypes -JB- */
int      udpdemux(PACKET);
int      udp_send(unshort /*fport*/, unshort /*lport*/, PACKET);

void     udpswap(struct udp *);
unshort  udp_socket(void);
PACKET   udp_alloc(int, int);
int      udp_maxalloc(void);
void     udp_free(PACKET);
UDPCONN  udp_open(ip_addr, unshort /*fport*/, unshort /*lport*/,
   int(*)(PACKET, void * /*data*/) , void *  /*data*/);
void     udp_close(UDPCONN);
int      udp_stats(void * pio);


#ifdef IP_V6
PACKET   udp6_alloc(int, int);
int      udp6_maxalloc(void);
UDPCONN  udp6_open(ip6_addr * /*faddr*/, unshort /*fport*/, 
                   unshort /*lport*/,
                   int(*)(PACKET, void * /*data*/) , void *  /*data*/);
int      udp6_send(ip6_addr * /*faddr*/, unshort /*fport*/, 
                   unshort /*lport*/, PACKET);
#endif

/* some hardwired protocol callbacks: */
int      snmp_upc(PACKET, unshort /*lport*/);      /* SNMPv1 receive upcall */
int      v3_udp_recv(PACKET, unshort /*lport*/);   /* SNMPv3 receive upcall */
int      dns_upcall(PACKET pkt, unshort);          /* DNS */
int      rip_udp_recv(PACKET p, unshort);          /* RIP */

#endif   /* _UDP_H_ */



