/*
 * FILENAME: ipnet.c
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
 * System code called from ip_startup(), et.al. to do low level
 * initialization of network structures, packet queues, etc.
 *
 * MODULE: INET
 *
 * ROUTINES: Netinit(), net_stats(), pktdemux(), 
 *
 * PORTABLE: yes
 */


#include "license.h"
#include "ipport.h"
#include "q.h"
#include "netbuf.h"
#include "net.h"
#include "ip.h"
#include "minip.h"

#ifdef INCLUDE_ARP
#include "arp.h"
#endif

#include "ether.h"

queue    rcvdq;         /* queue of all received (but undemuxed) packets */

unsigned NDEBUG = 0;    /* Debugging...*/

unsigned ifNumber = 0;  /* number of net interfaces */

/* Mini IP has only a single interface, so set up everything here
 * if your compiler complains about the next line, it means 
 * your ipport.h defined MAXNETS != 1
 */
#if (MAXNETS != 1)
#error On Mini IP builds "MAXNETS" must be set to 1
#endif   /* MAXNETS != 1 */

/* static memory for interface structs and interface mib data */
struct net  netstatic[STATIC_NETS];  /* actual net structs */


/* pointer array to all nets, to support older code */
struct net *   nets[1] = { &netstatic[0] };

/* master list of nets, staticly initialized */
queue netlist = { (qp)&netstatic[0], (qp)&netstatic[0], 1, 1, 1};

/* declare the global pointer to our one & only net iface */
struct net * m_netp;

/* pointer to firstpass net interface initialization */
int (*port_prep)(int);

#ifndef USE_PPP
int m_isbcast(char * address);
#endif

#ifdef  LOSSY_IO
/* support for an option that allows us to deliberatly loose packets */
int   in_lastloss = 0;  /* packets since last loss */
int   in_lossrate = 10; /* number of packets to punt (30 is "1 in 30") */
#endif   /* LOSSY_IO */

#ifdef USE_IPFILTER
int  ipf_init(void);
void ipf_cleanup(void);
#endif


/* FUNCTION: ip_startup()
 *
 * startup() - Start up a customized IP stack as defined by ipport.h 
 * 
 * PARAM1: none
 *
 * RETURNS: returns NULL if OK, or text of an error message. For the mini
 * version the error message is pretty terse.
 */

char *   
ip_startup()
{
   int   e; /* error holder */

   /* initialize freeq */
   LOCK_NET_RESOURCE(FREEQ_RESID);
   e = pk_init();
   UNLOCK_NET_RESOURCE(FREEQ_RESID);
   if (e)   /* report error (memory ran out?) */
      return "mem";

   /* 2 lines of code code deal with the full-sized IP concepts of multiple nets */
   m_netp = &netstatic[0];
   m_netp->n_mib = &m_netp->mib;

   /* start the network interface. If PPP is in use call ppp_prep,
    * otherwise call the port-specific prep callback.
    */
#ifdef USE_PPP
   e = prep_ppp(0);
   if(e != 1)
      return "prep_ppp";
#else
   /* call the first pass hardware init routine, if set: */
   if(port_prep)
   {
      e = port_prep(ETH_PORT);
      if(e != 1)
         return "prep";
   }
#endif   /* USE_PPP */

   /* ifNumber indicates the number of interfaces for the stack */
   ifNumber=e;

   /* call the hardware device init routine */
   if (m_netp->n_init != NULL)  /* If iface has init routine... */
   {
      if ((e = (*m_netp->n_init)(ETH_PORT)) != 0)  /* call init routine */
         return "init";
   }
   /* set ifAdminStatus in case init() routine forgot to. IfOperStatus
    * is not nessecarily up at this point, as in the case of a modem which
    * is now in autoanswer mode.
    */
   m_netp->n_mib->ifAdminStatus = NI_UP;

   /* build broadcast addresses */
   m_netp->n_netbr = m_netp->n_ipaddr | ~m_netp->snmask;

#ifdef INCLUDE_SNMP
   e = snmp_init();
   if (e)
   {
      return("SNMP");
   }
#endif   /* INCLUDE_SNMP */

#ifdef USE_IPFILTER
   e = ipf_init();
   if (e)
      return("unable to initialize IP Filter table");
   else
      exit_hook(ipf_cleanup);
#endif

   return(NULL);
}


/* FUNCTION: pktdemux()
 *
 * pktdemux() - The received packet demuxing task. We try to run this 
 * whenever there are unprocessed packets in rcvdq. It dequeues the 
 * packet and sends it to a protocol stack based on type. 
 *
 * 
 * PARAM1: none
 *
 * RETURNS: void
 */

void
pktdemux()
{
   PACKET   pkt;
   NET      ifc;                /* interface packet came from */
   IFMIB    mib;
   char *   eth;

   while (rcvdq.q_len)
   {
      /* If we get receive interupt from the net during this
       * lock, the MAC driver needs to wait or reschedule
       */
      LOCK_NET_RESOURCE(RXQ_RESID);
      pkt = (PACKET)q_deq(&rcvdq);
      UNLOCK_NET_RESOURCE(RXQ_RESID);
      if (!pkt) panic("pktdemux: got null pkt");
         ifc = pkt->net;

      mib = ifc->n_mib;
      /* maintain mib stats for unicast and broadcast */
      if (isbcast(ifc, (char*)pkt->nb_buff))
         mib->ifInNUcastPkts++;
      else
         mib->ifInUcastPkts++;

#ifdef  LOSSY_IO
      if(NDEBUG & LOSSY_RX)
      {
         if(in_lastloss++ == in_lossrate)
         {
            LOCK_NET_RESOURCE(FREEQ_RESID);
            pk_free(pkt);        /* punt packet */
            UNLOCK_NET_RESOURCE(FREEQ_RESID);
            in_lastloss = (cticks & 0x07) - 4;  /* pseudo random reset */
            continue;            /* act as if we sent OK */
         }
      }
#endif   /* LOSSY_IO */

#ifdef USE_PPP
         LOCK_NET_RESOURCE(NET_RESID);
         ip_rcv(pkt);   /* type was already checked by PPP code */
         UNLOCK_NET_RESOURCE(NET_RESID);
         continue;
#else /* not USE_PPP, must be Ethernet */
      /* get pointer to Ethernet header */
      eth = pkt->nb_buff + ETHHDR_BIAS;
      pkt->type = (ET_TYPE_GET(eth));
      if (pkt->type == IPTP)	/* IP type */
      {
         LOCK_NET_RESOURCE(NET_RESID);
         ip_rcv(pkt);
         UNLOCK_NET_RESOURCE(NET_RESID);
         continue;
      }
#ifdef INCLUDE_ARP
      if (pkt->type == ARPTP)	/* ARP type */
      {
         LOCK_NET_RESOURCE(NET_RESID);
         arprcv(pkt);
         UNLOCK_NET_RESOURCE(NET_RESID);
         continue;
      }
#endif   /* INCLUDE_ARP */
#endif   /* USE_PPP */

#ifdef NPDEBUG
      if (NDEBUG & UPCTRACE)
         dprintf("rcvdemux: got unregistered type %x\n", pkt->type);
#endif
      ifc->n_mib->ifInUnknownProtos++;
      LOCK_NET_RESOURCE(FREEQ_RESID);
      pk_free(pkt);           /* return to free buffer */
      UNLOCK_NET_RESOURCE(FREEQ_RESID);
   }
}

#ifndef USE_PPP
/* The Ethernet version of MAC broadcast identifier. The PPP version
 * is just ifdeffed to FALSE
 */
int
m_isbcast(char * address)
{
   /* we only test the first 4 bytes - reliable and fast, as long as the
    * IEEE never assigns FFFFFF as a vendor code.
    */
   if(*(u_long*)address == 0xFFFFFFFF)
      return TRUE;
   else
      return FALSE;
}
#endif   /* USE_PPP */


