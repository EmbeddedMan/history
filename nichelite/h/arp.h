/*
 * FILENAME: arp.h
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
 * ARP related constats and definitions.
 *
 * MODULE: INET
 *
 * PORTABLE: yes
 */

#ifndef _ARP_H
#define  _ARP_H   1

#define  ET_ARP      (0x0806)  		/* net endian 0x0806 */
#define  MACADDSIZ   6              /* biggest mac address we use */


/* The ARP table entry structure. It is empty (unused) if t_pro_addr 
 * == 0L. The first three entrys match those specified by the MIB in 
 * rfc 1156 for the Address Translation Tables. 
 */
 
struct arptabent  {
   unsigned long  t_pro_addr;       /* protocol address */
   unsigned char  t_phy_addr[6];    /* physical addr */
   struct net *   net;     			/* interface for which this entry is valid */
   PACKET   pending;       			/* packets waiting for resolution of this arp */
   u_long   createtime;    			/* time entry was created (cticks) */
   u_long   lasttime;      			/* time entry was last referenced */
   unshort  flags;         			/* mask of the ET flags */
};

#define  MAXARPS        8  /* maximum mumber of arp table entries */
extern   struct arptabent  arp_table[MAXARPS];  /* the actual table */

/* arp function prototypes */
int   etainit (void);         			/* init arp package */
int   arprcv (PACKET);        			/* arp received packet upcall */
void  at_entry (ip_addr, char*, int);   /* make an entry in the arp table */
void  arpReply (PACKET );
int   et_send (PACKET pkt, struct arptabent * tp);
int   send_arp (PACKET pkt, ip_addr dest_ip);
int   send_via_arp (PACKET pkt, ip_addr dest_ip);

struct arptabent * find_oldest_arp(ip_addr dest_ip);
struct arptabent * make_arp_entry(ip_addr dest_ip, NET net);

#ifdef DYNAMIC_IFACES
int   clear_arp_entries(ip_addr dest_ip, NET ifp);
#endif

/* arp stats - In addition to MIB */
extern   unsigned    arpReqsIn;     /* requests received */
extern   unsigned    arpReqsOut;    /* requests sent */
extern   unsigned    arpRepsIn;     /* replys received */
extern   unsigned    arpRepsOut;    /* replys sent */


/* Plummer's internals. All constants are already byte-swapped. */
#define     ARETH      (1)       /* Ethernet hardware type */
#define     ARREQ      (1)       /* byte swapped request opcode */
#define     ARREP      (2)       /* byte swapped reply opcode */
#define     ARPIP      (0x0800)  /* IP type in net endian */
#define     ARPHW      (1)       /* arp hardware type for Ethernet, in net endian */
#define     ARP8023HW  (6)       /* arp hardware type for IEEE 802.3 in net endian */

/* have two arp headers because of problems with some hardware insisting
 * 32 bit fields must be on 32 bit boundaries.
 */
 
/* the ARP header as it appears on the wire: */
struct arp_wire   {
   unshort     ar_hd;      /* hardware type */
   unshort     ar_pro;     /* protcol type */
   char        ar_hln;     /* hardware addr length */
   char        ar_pln;     /* protocol header length */
   unshort     ar_op;      /* opcode */
   char        data[20];   /* send IP, send eth, target IP, target eth */
};

/* offsets to fields in arp_wire->data[] */
#define  AR_SHA   0
#define  AR_SPA   6
#define  AR_THA   10
#define  AR_TPA   16

/* THE ARP header structure, with special macros around it to help
 * with declaring it "packed". 
 */

START_PACKED_STRUCT(arp_hdr)  /* macro to optionally pack struct */
   unshort     ar_hd;      /* hardware type */
   unshort     ar_pro;     /* protcol type */
   char        ar_hln;     /* hardware addr length */
   char        ar_pln;     /* protocol header length */
   unshort     ar_op;      /* opcode */
   char        ar_sha[6];  /* sender hardware address */
   ip_addr     ar_spa;     /* sender protocol address */
   char        ar_tha[6];  /* target hardware address */
   ip_addr     ar_tpa;     /* target protocol address */
END_PACKED_STRUCT(arp_hdr)

/* bits for tp->flags */
#define  ET_ETH2  1     /* this IP address uses Ethernet II */
#define  ET_SNAP  2     /* this IP address uses snap headers */


#ifdef IEEE_802_3
/* 8 bytes of useless filler, required for IEEE 802.3 support */
struct snap_hdr
{
   u_char   llc_etc[6];
   unshort  type;       /* the pkt protocol, 0x0800 for IP, etc. */
};
#endif   /* IEEE_802_3 */

#endif   /* _ARP_H 1 */

