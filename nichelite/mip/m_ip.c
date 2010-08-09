/*
 * FILENAME: ip.c
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
 * This contains the small version of the base IP code. Restrictions:
 * - 1 iface
 * - no fragments
 * - no real routing
 *
 * MODULE: MINET
 *
 * ROUTINES: ip_write(), ip_rcv(), ip_stats(),
 *
 * PORTABLE: yes
 */

#include "license.h"
#include "ipport.h"
#include "q.h"
#include "netbuf.h"
#include "net.h"
#include "arp.h"
#include "ip.h"
#include "icmp.h"
#include "udp.h"

static unsigned uid = 1;        /* base for unique IP packet ID */

struct IpMib   ip_mib = {     /* IP stats for snmp */
   1,          /* ipForwarding */
   IP_TTL,     /* ipDefaultTTL */
   /* let the rest of MIB init to zero */
};

#ifndef TCP_PROT
#define TCP_PROT  6        /* TCP protocol type byte in IP header */
#endif

#ifdef INCLUDE_TCP
extern int tcp_rcv(PACKET);
#endif

#ifdef USE_IPFILTER
int ipf_filter(PACKET p, int dir);
#endif


/* FUNCTION: ip_write()
 *
 * ip_write() - Fill in the internet header in the packet p and send 
 * the packet through the appropriate net interface. This will 
 * involve using routing. Call p->fhost set to target IP address,
 * and with PACKET p->nb_plen & p->nb_prot fields set to start and
 * length of upper layer, ie UDP. This is in contrast to the IP->UDP
 * upcall, which leaves nb_prot at the IP header.
 *
 * 
 * PARAM1: u_char prot
 * PARAM2: PACKET p
 *
 * RETURNS: Returns 0 if sent OK, ENP_SEND_PENDING (1) if 
 * waiting for ARP, else negative error code if error detected. 
 */

int
ip_write(
   u_char   prot,
   PACKET   p)
{
   ip_addr firsthop;
   struct ip * pip;

   ip_mib.ipOutRequests++;

   /* decide if firsthop is local host or default gateway */
   if((m_netp->n_ipaddr & m_netp->snmask) == (p->fhost & m_netp->snmask))
   {
      firsthop = p->fhost;				//FSL firsthop is local
   }
   else if((0xFFFFFFFF & m_netp->snmask) == (p->fhost & m_netp->snmask))
   {   /* it's a broadcast for this net */
      firsthop = p->fhost;				//FSL firsthop is broadcast
   }
   else
   {
      if(m_netp->n_defgw)
         firsthop = m_netp->n_defgw;	//FSL firsthop goes to gateway
      else
      {									//FSL error...couldn't find firsthop so free heap space
         ip_mib.ipOutNoRoutes++;
         LOCK_NET_RESOURCE(FREEQ_RESID);
         pk_free(p);						//FSL free PACKET p which was allocated from heap
         UNLOCK_NET_RESOURCE(FREEQ_RESID);
         return(ENP_NO_ROUTE);
      }
   }
   p->net = m_netp;     /* set send net for compatability w/drivers */


   /* prepend IP header to packet data */
   p->nb_prot -= sizeof(struct ip);       /* this assumes no send options! */
   p->nb_plen += sizeof(struct ip);

   pip = (struct ip*)p->nb_prot;

   pip->ip_ver_ihl = 0x45;       /* 2 nibbles; VER:4, IHL:5. */
   pip->ip_flgs_foff = 0;        /* fragment flags and offset */
   pip->ip_time = IP_TTL;        /* default number of hops, really */
   pip->ip_id = ((unshort)uid);  /* IP datagram ID */
   pip->ip_src = m_netp->n_ipaddr;
   pip->ip_dest = p->fhost;
   pip->ip_len = ((unshort)p->nb_plen);
   pip->ip_tos = 0;
   pip->ip_prot = prot;          /* install protocol ID (TCP, UDP, etc) */
   pip->ip_chksum = IPXSUM;      /* clear checksum field for summing */
   pip->ip_chksum = ~cksum(pip, 10);

   uid++;

#ifdef  LOSSY_IO
   if(NDEBUG & LOSSY_TX)
   {
      if(in_lastloss++ == in_lossrate)
      {
         LOCK_NET_RESOURCE(FREEQ_RESID);
         pk_free(p);                 /* punt packet */
         UNLOCK_NET_RESOURCE(FREEQ_RESID);
         in_lastloss = (cticks & 0x07) - 4;  /* pseudo random reset */
         return 0;                     /* act as if we sent OK */
      }
   }
#endif   /* LOSSY_IO */

   /* send packet to MAC layer. This will try to resolve MAC layer addressing 
    * and send packet. This can return SUCCESS, PENDING, or error codes
    */
#ifdef USE_PPP
    return(m_netp->pkt_send(p));
#else /* Ethernet-ish */
    return(send_via_arp(p, firsthop));
#endif
}


/* FUNCTION: ip_rcv()
 *
 * This is the IP receive upcall routine. It handles packets received 
 * by network ISRs, etc. verifies their Ip headers and does the 
 * upcall to the upper layer that should receive the packet. Returns 
 * 0 if packet was processed succesfully, ENP_NOT_MINE if not for me, 
 * or a negative error code if packet was badly formed. 
 *
 * 
 * PARAM1: PACKET p
 *
 * RETURNS: 
 */

int
ip_rcv(PACKET p)
{
   struct ip * pip;     /* the internet header */
   unsigned short csum; /* packet checksum */
   unsigned short tempsum;
   NET nt;
   unsigned len;

#ifdef NPDEBUG
   if ((NDEBUG & UPCTRACE) && (NDEBUG & IPTRACE))
      dprintf("ip_rcv: got packet, len:%d, if:%d\n",
         p->nb_plen, net_num(p->net));
#endif

   nt = p->net;      /* which interface it came in on */
   ip_mib.ipInReceives++;
   pip = ip_head(p);

   /* test received MAC len against IP header len */
   if (p->nb_plen < (unsigned)(pip->ip_len))
   {
#ifdef NPDEBUG
      if ((NDEBUG & UPCTRACE) && (NDEBUG & IPTRACE))
         dprintf("ip_rcv: bad pkt len\n");
#endif

      ip_mib.ipInHdrErrors++;
      LOCK_NET_RESOURCE(FREEQ_RESID);
      pk_free(p);
      UNLOCK_NET_RESOURCE(FREEQ_RESID);
      return(ENP_BAD_HEADER);
   }

   /* use length from IP header; MAC value may be padded */
   len = (pip->ip_len);
   p->nb_plen = len;       /* fix pkt len */

   if ( ((pip->ip_ver_ihl & 0xf0) >> 4) != IP_VER)
   {
#ifdef NPDEBUG
      if ((NDEBUG & UPCTRACE) && (NDEBUG & IPTRACE))
         dprintf("ip_rcv: bad version number\n");
#endif
      ip_mib.ipInHdrErrors++;
      LOCK_NET_RESOURCE(FREEQ_RESID);
      pk_free(p);
      UNLOCK_NET_RESOURCE(FREEQ_RESID);
      return(ENP_BAD_HEADER);
   }

#ifdef USE_IPFILTER
   /* Do IP filtering. If packet is accepted, ipf_filter() returns
    * SUCCESS. Discard the packet for all other return values 
    */
   if (ipf_filter(p,1) != SUCCESS)  /* 1 - inbound pkt */
   {
      LOCK_NET_RESOURCE(FREEQ_RESID);
      pk_free(p);
      UNLOCK_NET_RESOURCE(FREEQ_RESID);
      IN_PROFILER(PF_IP, PF_EXIT);
      return ENP_NO_ROUTE;
   }
#endif

   csum = pip->ip_chksum;
   pip->ip_chksum = 0;
   tempsum = ~cksum(pip, ip_hlen(pip) >> 1);
   if (csum != tempsum)
   {
      pip->ip_chksum = csum;
#ifdef NPDEBUG
      if ((NDEBUG & UPCTRACE) && (NDEBUG & IPTRACE))
         dprintf("ip_rcv: bad xsum\n");
#endif
      ip_mib.ipInHdrErrors++;
      LOCK_NET_RESOURCE(FREEQ_RESID);
      pk_free(p);
      UNLOCK_NET_RESOURCE(FREEQ_RESID);
      return(ENP_BAD_HEADER);
   }

   pip->ip_chksum = csum;

   if ((pip->ip_dest != nt->n_ipaddr) &&  /* Quick check on our own addr */
       (pip->ip_dest != 0xffffffffL) &&   /* Physical cable broadcast addr*/
       (pip->ip_dest != nt->n_netbr) )    /* All subnet broadcast */
   {
      ip_mib.ipInAddrErrors++;
      LOCK_NET_RESOURCE(FREEQ_RESID);
      pk_free(p);
      UNLOCK_NET_RESOURCE(FREEQ_RESID);
      return(ENP_NOT_MINE);
   }

   pip = ip_head(p);
   p->fhost = pip->ip_src;

#ifdef NPDEBUG
   if ((NDEBUG & UPCTRACE) && (NDEBUG & IPTRACE))
      dprintf("ip_rcv: pkt prot %u from %u.%u.%u.%u\n",
         pip->ip_prot, PUSH_IPADDR(pip->ip_src));
#endif

   switch (pip->ip_prot)
   {
// #ifdef INCLUDE_UDP
   case UDP_PROT:
      ip_mib.ipInDelivers++;
      return(udpdemux(p));
// #endif   /* INCLUDE_UDP */

#ifdef FULL_ICMP
   case ICMP_PROT:
      ip_mib.ipInDelivers++;
      return(icmprcv(p));
#endif   /* INCLUDE_ICMP */

#ifdef MINI_TCP
   case TCP_PROT:
      ip_mib.ipInDelivers++;
      return(tcp_rcv(p));
#endif   /* INCLUDE_TCP */

   default: /* unknown upper protocol */
      break;
   }

   ip_mib.ipUnknownProtos++;
   LOCK_NET_RESOURCE(FREEQ_RESID);
   pk_free(p);
   UNLOCK_NET_RESOURCE(FREEQ_RESID);
   return ENP_NOT_MINE;
}

#ifdef NET_STATS

/* FUNCTION: ip_stats()
 * 
 * PARAM1: void * pio
 *
 * RETURNS: 
 */

int
ip_stats(void * pio)
{
   ns_printf(pio,"IP MIB statistics:\n");
   ns_printf(pio,"Gateway: %s     default TTL: %lu\n",
     (ip_mib.ipForwarding == 1)?"YES":"NO", ip_mib.ipDefaultTTL);
   ns_printf(pio,"rcv:  total: %lu    header err: %lu    address err: %lu\n",
     ip_mib.ipInReceives, ip_mib.ipInHdrErrors, ip_mib.ipInAddrErrors);
   ns_printf(pio,"rcv:  unknown Protocls: %lu    delivered: %lu\n",
     ip_mib.ipUnknownProtos, ip_mib.ipInDelivers);
   ns_printf(pio,"send: total: %lu    discarded: %lu     No routes: %lu\n",
     ip_mib.ipOutRequests, ip_mib.ipOutDiscards, ip_mib.ipOutNoRoutes);

   return 0;
}

#endif   /* NET_STATS */



