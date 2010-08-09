/*
 * FILENAME: udp.c
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
 * UDP send and receive routines
 *
 * MODULE: INET
 *
 * ROUTINES: udpdemux(), udp_send(), udp_socket(), 
 * ROUTINES: udp_alloc(), udp_maxalloc(), udp_free(), udp_stats()
 *
 * PORTABLE: yes
 */


#include "license.h"
#include "ipport.h"
#include "q.h"
#include "netbuf.h"
#include "net.h"
#include "ip.h"
#include "icmp.h"
#include "udp.h"
#include "minip.h"

struct UdpMib  udp_mib;    /* udp stats block */

extern   UDPCONN  firstudp;


/* FUNCTION: udpdemux()
 *
 * This routine handles incoming UDP packets. They're handed to it by 
 * the internet layer. It demultiplexes the incoming packet based on 
 * the local port and upcalls the appropriate routine. 
 *
 * 
 * PARAM1: PACKET p
 *
 * RETURNS: 0 if OK or ENP error code
 */

int
udpdemux(PACKET p)
{
   struct ip * pip;  /* IP header below this layer */
   struct udp *   pup;  /* UDP header of this layer */
   struct ph   php;     /* pseudo header for checksumming */
   UDPCONN con;      /* UDP connection for table lookup */
   unsigned short osum, xsum; /* scratch checksum holders */
   unsigned plen; /* packet length */
   int   e;    /* general error holder */

   /* First let's verify that it's a valid UDP packet. */
   pip = ip_head(p);       /* we'll need IP header info */
   pup = (struct udp*)ip_data(pip);    /*  also need UDP header */
   plen = (pup->ud_len);

   if (plen > p->nb_plen)
   {
#ifdef   NPDEBUG
      if ((NDEBUG & UPCTRACE) && (NDEBUG & TPTRACE))
         dprintf("UDP: bad len pkt: rcvd: %u, hdr: %u.\n",
       p->nb_plen, (pup->ud_len) + UDPLEN);
#endif
      udp_mib.udpInErrors++;
      udp_free(p);
      return ENP_BAD_HEADER;
   }

#ifndef NO_UDP_CHECKSUM
   osum = pup->ud_cksum;
   /* did other guy use checksumming? */
   if (osum)
   {
      if (plen & 1) ((char *)pup)[plen] = 0;
         php.ph_src = p->fhost;
      php.ph_dest = pip->ip_dest;
      php.ph_zero = 0;
      php.ph_prot = UDP_PROT;
      php.ph_len  = pup->ud_len;

      pup->ud_cksum = cksum(&php, sizeof(struct ph)>>1);
      xsum = ~cksum(pup, (plen+1)>>1);
      if (!xsum)
         xsum = 0xffff;
      pup->ud_cksum = osum;
      if (xsum != osum)
      {
#ifdef   NPDEBUG
         if ((NDEBUG & UPCTRACE) && (NDEBUG & TPTRACE))
            dprintf("UDPDEMUX: bad xsum %04x right %04x from %u.%u.%u.%u\n",
               osum, xsum, PUSH_IPADDR(p->fhost));
#endif
         udp_mib.udpInErrors++;
         udp_free(p);
         return ENP_BAD_HEADER;
      }
   }
#endif   /* NO_UDP_CHECKSUM */

   /* adjust prot fields by size of IP and UDP headers. */
   e = (sizeof(struct udp) + sizeof(struct ip));
   p->nb_plen -= e;
   p->nb_prot += e;

#ifdef   NPDEBUG
   if ((NDEBUG & UPCTRACE) && (NDEBUG & TPTRACE))
   {
      dprintf("UDP: pkt[%u] from %u.%u.%u.%u:%d to %d\n",
       plen, PUSH_IPADDR(p->fhost), pup->ud_srcp, pup->ud_dstp);
   }
#endif

   /* check for special cases - we have a built-in snmp agent and 
    * echo server. We do SNMP before trying the demux table so 
    * external application programs can examine SNMP packets that 
    */
#ifdef INCLUDE_SNMPV3      /* If V3 is present, call SNMPV3 upcall */
   if (pup->ud_dstp == SNMP_PORT)
   {
      udp_mib.udpInDatagrams++;
      UNLOCK_NET_RESOURCE(NET_RESID);
      e = v3_udp_recv(p, pup->ud_srcp);      /* upcall imbedded snmp agent */
      LOCK_NET_RESOURCE(NET_RESID);
      if (e != ENP_NOT_MINE)
         return(e);
      /* else SNMP pkt was not for imbedded agent, fall to try demux table */
   }
#else                   /* Else call V1's upcall, if V1 is present */
#ifdef PREBIND_AGENT
   if (pup->ud_dstp == SNMP_PORT)
   {
      udp_mib.udpInDatagrams++;
      UNLOCK_NET_RESOURCE(NET_RESID);
      e = snmp_upc(p, pup->ud_srcp);      /* upcall imbedded snmp agent */
      LOCK_NET_RESOURCE(NET_RESID);
      if (e != ENP_NOT_MINE)
         return(e);
      /* else SNMP pkt was not for imbedded agent, fall to try demux table */
   }
#endif   /* PREBIND_AGENT */
#endif   /* INCLUDE_SNMPV3 */

   /* run through the demux table and try to upcall it */

   for (con = firstudp; con; con = con->u_next)
   {
      /* enforce all three aspects of tuple matching. Old code
       * assumed lport was unique, which is not always so.
       */
      if (con->u_lport && (con->u_lport != pup->ud_dstp))
         continue;
      if (con->u_fport && (con->u_fport != pup->ud_srcp))
         continue;
      if (con->u_fhost && (con->u_fhost != p->fhost))
         continue;

      /* if this endpoint has been bound to a local interface address,
       * make sure the packet was received on that interface address
       */
      if (con->u_lhost && (con->u_lhost != p->net->n_ipaddr)) 
         continue;

      /* fall to here if we found it */
      udp_mib.udpInDatagrams++;
      if (con->u_rcv)         /* if upcall address is set... */
      {
         UNLOCK_NET_RESOURCE(NET_RESID);
         e = ((*con->u_rcv)(p, con->u_data));   /* upcall it */
         LOCK_NET_RESOURCE(NET_RESID);
      }
      else
         e = ENP_LOGIC;

      /* if error occurred in upcall or there was no upcall hander
         its up to this routine to free the packet buffer */
      if (e)
      {
         udp_mib.udpInErrors++;
         udp_free(p);
      }

      return(e);
   }

#ifdef DNS_CLIENT
   /* see if this is a DNS reply packet */
   if (pup->ud_srcp == DNS_PORT)    /* sent from DNS port? */
   {
      UNLOCK_NET_RESOURCE(NET_RESID);
      e = dns_upcall(p, pup->ud_dstp);    /* upcall dns client code */
      LOCK_NET_RESOURCE(NET_RESID);
      udp_mib.udpInDatagrams++;
      if (e != ENP_NOT_MINE)  /* if mine, return here */
      {
         /* the DNS upcall does not appear to free the packet, so
            we have to do it */
         udp_free(p);
         return(e);
      }
   }
#endif   /* DNS_CLIENT */

#ifdef RIP_SUPPORT
   /* see if this is a RIP packet */
   if (pup->ud_srcp == RIP_PORT)    /* sent from DNS port? */
   {
      UNLOCK_NET_RESOURCE(NET_RESID);
      e = rip_udp_recv(p, pup->ud_dstp);     /* upcall RIP code */
      LOCK_NET_RESOURCE(NET_RESID);
      /* note that the RIP upcall does appear to free the packet,
         so we don't do it */
      udp_mib.udpInDatagrams++;
      if (e != ENP_NOT_MINE)  /* if mine, return here */
         return(e);
   }
#endif   /* RIP_SUPPORT */

#ifdef UDP_ECHO_SVR
   /* finally, see if this is an echo packet */
   if (pup->ud_dstp == 7)     /* echo port */
   {
      udp_mib.udpInDatagrams++;
      UNLOCK_NET_RESOURCE(NET_RESID);
      e = udp_send(pup->ud_srcp, 7, p);      /* echo it */
      LOCK_NET_RESOURCE(NET_RESID);
      return(e);
   }
#endif   /* UDP_ECHO_SVR */

   /* Fall to here if packet is not for us. Check if the packet was 
    * sent to an ip broadcast address. If it was, don't send a 
    * destination unreachable. 
    */
   if ((pip->ip_dest == 0xffffffffL) ||      /* Physical cable broadcast addr*/
       (pip->ip_dest == p->net->n_netbr))    /* subnet broadcast */
   {
#ifdef   NPDEBUG
      if ((NDEBUG & UPCTRACE) && (NDEBUG & TPTRACE))
         dprintf("UDP: ignoring ip broadcast\n");
#endif
      udp_mib.udpInErrors++;
      udp_free(p);
      return ENP_NOT_MINE;
   }

#ifdef   NPDEBUG
   if ((NDEBUG & UPCTRACE) && (NDEBUG & TPTRACE))
      dprintf("UDP: unexpected port %04x\n", pup->ud_dstp);
#endif

   /* send destination unreachable. */
   icmp_destun(p->fhost, ip_head(p), DSTPORT, p->net);

   udp_mib.udpNoPorts++;
   udp_free(p);
   return ENP_NOT_MINE;
}



/* FUNCTION: udp_send()
 *
 * udp_send() - Prepend a udp header on a packet, checksum it and 
 * pass it to IP for sending. PACKET p should have fhost set to 
 * target IP address, nb_prot pointing to udpdata, & nb_plen set 
 * before call. If you expect to get any response to this packet you 
 * should have opened a UDPCONN with udp_open() prior to calling 
 * this. 
 *
 * 
 * PARAM1: unshort fport
 * PARAM2: unshort lport
 * PARAM3: PACKET p
 *
 * RETURNS: Returns 0 if sent OK, else non-zero error code if error 
 * detected. 
 */

int
udp_send(unshort fport, unshort lport, PACKET p)
{
   struct udp* pup;
   struct ph   php;
   int         udplen;
   int         e;

#ifdef   NPDEBUG
   if (NDEBUG & (INFOMSG|TPTRACE))
      dprintf("UDP: pkt [%u] %04x -> %u.%u.%u.%u:%04x\n", p->nb_plen, lport,
    PUSH_IPADDR(p->fhost), fport);
#endif

   LOCK_NET_RESOURCE(NET_RESID);
   /* prepend UDP header to upper layer's data */
   p->nb_prot -= sizeof(struct udp);			//FSL subtract size of udp header to get address where udp header should start
   pup = (struct udp*)p->nb_prot;				//FSL pup pointer points to udp header starting address within PACKET p
   udplen = p->nb_plen + sizeof(struct udp);	//FSL data packet PACKET p length gets added with udp header length
   p->nb_plen = udplen;							//FSL PACKET p length field gets updated
   if (udplen & 1) ((char *)pup)[udplen] = 0;	//FSL? if udplen is odd, force the high odd array/byte to zero ?

   pup->ud_len = (unshort)udplen;   		/* fill in the UDP header within the PACKET p */
   pup->ud_srcp = lport;
   pup->ud_dstp = fport;

#ifdef NO_UDP_CKSUM     /* ddr added option */
   pup->ud_cksum = 0;
#else
   php.ph_src = m_netp->n_ipaddr;				//FSL fill in the php (pseudo header structure) fields
   php.ph_dest = p->fhost;
   php.ph_zero = 0;
   php.ph_prot = UDP_PROT;
   php.ph_len = pup->ud_len;
   pup->ud_cksum = cksum(&php, sizeof(struct ph)>>1);	//FSL calculate php checksum and place in PACKET p udp checksum field
   pup->ud_cksum = ~cksum(pup, (udplen+1)>>1);			//FSL calculate new PACKET p checksum and place in PACKET p
   if (pup->ud_cksum == 0)
      pup->ud_cksum = 0xffff;
#endif

   udp_mib.udpOutDatagrams++;

   p->nb_plen = udplen;       		//FSL update the length of raw data in PACKET p
   e = ip_write(UDP_PROT, p);		//FSL send to IP layer 
   UNLOCK_NET_RESOURCE(NET_RESID);
   return e;
}


/* FUNCTION: udp_socket()
 *
 * udp_socket() - Select a port number at random, but avoid reserved 
 * ports from 0 thru 1024. Also leave range from 1025 thru 1199 
 * available for explicit application use. 
 *
 * 
 * PARAM1: void
 *
 * RETURNS: Returns a useable port 
 * number in local endian 
 */


#define  MINSOCKET   1200
static unshort usocket = 0;   /* next socket to grab */

unshort
udp_socket(void)
{
   UDPCONN tmp;

   if (usocket < MINSOCKET)
   {
      /* logic for for init and after wraps */
      usocket = (unshort)(cticks & 0x7fff);
      if (usocket < MINSOCKET)
         usocket += MINSOCKET;
   }
   /* scan existing connections, making sure socket isn't in use */
   for (tmp = firstudp; tmp; tmp = tmp->u_next)
   {
      if (tmp->u_lport == usocket)
      {
         usocket++;     /* bump socket number */
         tmp = firstudp;   /* restart scan */
         continue;
      }
   }
   return usocket++;
}



/* FUNCTION: udp_alloc()
 *
 * udp_alloc(sizes) - returns a pointer to a packet buffer big enough 
 * for the specified sizes.
 *
 * 
 * PARAM1: int datalen
 * PARAM2: int optlen
 *
 * RETURNS:  Returns buffer, or NULL in no buffer was available. 
 */

PACKET
udp_alloc(int datalen, int optlen)
{
   int   len;
   PACKET p;

   len = (datalen + sizeof(struct udp) + 1) & ~1;
   LOCK_NET_RESOURCE(FREEQ_RESID);
   p = pk_alloc(len + IPHSIZ + MaxLnh + optlen);
   UNLOCK_NET_RESOURCE(FREEQ_RESID);
   if (!p)
      return NULL;

   /* set prot pointers past end of UDP header  */
   len = sizeof(struct ip) + (optlen >> 2) + sizeof(struct udp);
   p->nb_prot += len;
   p->nb_plen -= len;
   return(p);
}

/* FUNCTION: udp_free()
 *
 * udp_free() - frees an allocated packet buffer
 *
 * 
 * PARAM1: PACKET p
 *
 * RETURNS: void
 */

void
udp_free(PACKET p)
{
   LOCK_NET_RESOURCE(FREEQ_RESID);
   pk_free(p);
   UNLOCK_NET_RESOURCE(FREEQ_RESID);
}

#ifdef NET_STATS


/* FUNCTION: udp_stats()
 *
 * udp_stats() - print UDP stats
 *
 * 
 * PARAM1: void * pio
 *
 * RETURNS: 0
 */

int
udp_stats(void * pio)
{
   ns_printf(pio,"UDP MIB dump:\n");
   ns_printf(pio,"In: Good: %lu    No Port: %lu     Bad: %lu\n",
      udp_mib.udpInDatagrams, udp_mib.udpNoPorts, udp_mib.udpInErrors);
   ns_printf(pio,"Out: %lu\n", udp_mib.udpOutDatagrams);
   return 0;
}
#endif   /* NET_STATS */

/* FUNCTION: udp_maxalloc()
 *
 * udp_maxalloc() - returns the largest allocation possible
 *                  using udp_alloc()
 *
 * RETURNS: an int indicating the largest allocation possible
 *          using udp_alloc(); i.e. if the sum of udp_alloc()
 *          arguments datalen + optlen is greater than the
 *          returned value, the allocation will fail
 */

#ifndef MINI_TCP  /* only required for big TCP */
int
udp_maxalloc(void)
{
   return (bigbufsiz - (IPHSIZ + MaxLnh));
}
#endif

/* end of file udp.c */


