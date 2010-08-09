/*
 * FILENAME: icmp.c
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
 * MODULE: INET
 *
 * ROUTINES: icmprcv(), icmp_destun(), icmp_stats(),
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


struct IcmpMib icmp_mib;   /* storage for MIB statistics */

long  icmp_unhand = 0;

/* FUNCTION: icmprcv()
 *
 * ICMP received packet upcall handler. Returns 0 if we processed the 
 * packet, or ENP_NOT_MINE, or a negative error code. 
 *
 * 
 * PARAM1: PACKET p
 *
 * RETURNS: 
 */

int
icmprcv(PACKET p)      /* the incoming packet */
{
   unsigned len;        /* packet length, minus IP & MAC headers */
   ip_addr host;        /* the foreign host */
   struct ping *  e;
   struct ip * pip;

   len = p->nb_plen;    /* adjusted for us by IP layer */
   host = p->fhost;  /* filled in by IP layer */
   pip = ip_head(p);    /* find IP header */
   e = (struct ping *)ip_data(pip); /* ...and icmp header */

#ifdef   NPDEBUG
   if ((NDEBUG & UPCTRACE) && (NDEBUG & IPTRACE))
      dprintf("ICMP: p[%u] from %u.%u.%u.%u\n", len, PUSH_IPADDR(host));
#endif

   icmp_mib.icmpInMsgs++;        /* received one more icmp */

   if(e->ptype == ECHOREQ)  /* got ping request, send reply */
   {
      len -= ip_hlen(pip);    /* strip IP header from reply length */

      /* reset data pointer to IP header since we use p for reply */
      p->nb_prot = (char*)e;
      icmp_mib.icmpInEchos++;
#ifdef   NPDEBUG
      if ((NDEBUG & UPCTRACE) && (NDEBUG & IPTRACE))
         dprintf("ICMP: echo reply to %u.%u.%u.%u\n", PUSH_IPADDR(host));
#endif
      e->ptype = ECHOREP;
      e->pchksum = 0;
      if (len&1)  /* padd odd length packets  for checksum routine */
         ((char *)e)[len] = 0;

      e->pchksum = ~cksum(e, (len+1)>>1);
      pip->ip_src = pip->ip_dest;
      pip->ip_dest = host;
      icmp_mib.icmpOutEchoReps++;
      icmp_mib.icmpOutMsgs++;
      p->fhost = host;
      p->nb_plen = len;
      if (ip_write(ICMP_PROT, p))
      {
#ifdef   NPDEBUG
         if (NDEBUG & (UPCTRACE))
            dprintf("icmp: reply failed\n");
#endif
      }
      /* reused p will be freed by net->xxx_send() */
      return 0;
   }
#ifdef MINI_PING
   else if(e->ptype == ECHOREP)  /* got ping reply */
   {
      icmp_mib.icmpInEchoReps++;      
   }
#endif   /* MINI_PING */
   else
   {
      icmp_unhand++;
   }
   LOCK_NET_RESOURCE(FREEQ_RESID);
   pk_free(p);
   UNLOCK_NET_RESOURCE(FREEQ_RESID);
   return ENP_NOT_MINE;
}


/* FUNCTION: icmp_destun()
 *
 * icmp_destun() - Send an ICMP destination unreachable packet.
 * 
 * PARAM1: ip_addr host
 * PARAM2: struct ip *ip
 * PARAM3: unsigned typecode     - type and code fields
 * PARAM4: net packet came from 
 *
 * If the type field is 0, then type is assumed to be DESTIN.
 *
 * RETURNS: void
 */

void
icmp_destun(ip_addr host,  /* host to complain to */
   struct ip * ip,   /* IP header of offending packet */
   unsigned typecode,    /* type & code of DU to send (PROT, PORT, HOST) */
   NET   net)        /* interface that this packet came in on */
{
   PACKET p;
   struct destun *   d;
   int   i;

#ifdef NPDEBUG
   if (NDEBUG & PROTERR)
      dprintf("icmp: sending dest unreachable to %u.%u.%u.%u\n",
         PUSH_IPADDR(host));
#endif   /* NPDEBUG */

   LOCK_NET_RESOURCE(FREEQ_RESID);
   p = pk_alloc(512 + IPHSIZ);   /* get packet to send icmp dest unreachable */
   UNLOCK_NET_RESOURCE(FREEQ_RESID);

   if (p == NULL)
   {
#ifdef NPDEBUG
      if (NDEBUG & IPTRACE)
         dprintf("icmp: can't alloc pkt\n");
#endif   /* NPDEBUG */
      icmp_mib.icmpOutErrors++;
      return;
   }

   /* allow space for icmp header */
   p->nb_prot += sizeof(struct ip);
   p->nb_plen -= sizeof(struct ip);
   p->net = net;     /* Put in the interface that this packet came in on */

   d = (struct destun *)p->nb_prot;

   if (typecode & 0xFF00)                /* if the type was sent */
      d->dtype = (char)(typecode >>8);   /* then use it */
   else                                  /* else use default */
      d->dtype = DESTIN;
   d->dcode = (char)(typecode & 0xFF);
   MEMCPY(&d->dip, ip, (sizeof(struct ip) + ICMPDUDATA));

   d->dchksum = 0;
   d->dchksum = ~cksum(d, sizeof(struct destun)>>1);

   p->nb_plen =  sizeof(struct destun);
   p->fhost = host;
   i = ip_write(ICMP_PROT, p);
   if ((i))
   {
      icmp_mib.icmpOutErrors++;
#ifdef   NPDEBUG
      if (NDEBUG & (IPTRACE|NETERR|PROTERR))
         dprintf("ICMP: Can't send dest unreachable\n");
#endif   /* NPDEBUG  */
      return;
   }
   icmp_mib.icmpOutMsgs++;
   icmp_mib.icmpOutDestUnreachs++;
   return;
}


#ifdef NET_STATS

/* FUNCTION: icmp_stats()
 * 
 * icmp_stats() - Printf info about icmp MIB statistics.
 *
 * PARAM1: void * pio
 *
 * RETURNS: 
 */

int
icmp_stats(void * pio)
{
   ns_printf(pio,"ICMP layer stats:\n");
   ns_printf(pio,"icmpInMsgs %lu    icmpInErrors %lu, echoReqs %lu, echoReps %lu, unhandledTypes: %lu\n",
    icmp_mib.icmpInMsgs, icmp_mib.icmpInErrors, icmp_mib.icmpInEchos, icmp_mib.icmpInEchoReps, icmp_unhand);

   ns_printf(pio,"icmpOutMsgs %lu    icmpOutErrors %lu, \n",
    icmp_mib.icmpOutMsgs, icmp_mib.icmpOutErrors,
    icmp_mib.icmpOutEchoReps, icmp_mib.icmpOutRedirects);

   return 0;
}

#endif   /* NET_STATS */

/* end of file icmp.c */


