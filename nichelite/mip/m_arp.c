/*
 * FILENAME: et_arp.c
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
 * Ethernet (and similar media) arp code.
 * 
 * MODULE: INET
 *
 * ROUTINES: et_send(), send_arp(), 
 * ROUTINES: find_oldest_arp(), make_arp_entry(), arpReply(), arprcv(), 
 * ROUTINES: send_via_arp(), arp_stats(), 
 *
 * PORTABLE: yes
 */

#include "license.h"
#include "ipport.h"
#include "q.h"
#include "netbuf.h"
#include "net.h"
#include "ether.h"
#include "arp.h"
#include "ip.h"


struct arptabent *   arpcache;   /* last ARP entry used */
static u_long cachetime;         /* time we created arpcache entry */
int   arp_ageout  =  600;        /* APR table refresh age, in seconds */

/* arp stats - In addition to MIB-2 */
unsigned    arpReqsIn = 0;    /* requests received */
unsigned    arpReqsOut = 0;   /* requests sent */
unsigned    arpRepsIn = 0;    /* replys received */
unsigned    arpRepsOut = 0;   /* replys sent */

struct arptabent  arp_table[MAXARPS];     /* the actual table */


/* FUNCTION: et_send()
 *
 * et_send() - fill in outgoing Ethernet-bound packet's Ethernet 
 * header and send it. Header info is in arp entry passed and MIB 
 * info. 
 * 
 * PARAM1: PACKET pkt
 * PARAM2: struct arptabent * tp
 *
 * RETURNS: Returns 0 if OK, or the usual ENP_ errors 
 */

int
et_send(PACKET pkt, struct arptabent * tp)
{
   char * ethhdr;
   IFMIB etif = m_netp->n_mib;    	/* mib info for this Ethernet interface */

   tp->lasttime = cticks;
   pkt->nb_prot -= ETHHDR_SIZE;  	/* prepare for prepending Ethernet header */
   pkt->nb_plen += ETHHDR_SIZE;
   ethhdr = pkt->nb_prot + ETHHDR_BIAS;   /* prepare to prepend Ethernet header */

   if (ethhdr < pkt->nb_buff)   	/* sanity check pointer */
      panic("et_send: prepend");

   MEMCPY(ethhdr + ET_DSTOFF, tp->t_phy_addr, 6);    	/* copy dest MAC address into packet */
   MEMCPY(ethhdr + ET_SRCOFF, etif->ifPhysAddress, 6);  /* MAC src */
   ET_TYPE_SET(ethhdr, (ARPIP));

#ifdef NPDEBUG
   if(m_netp->pkt_send == NULL)
   {
      panic("et_send");
   }
#endif
   return(m_netp->pkt_send(pkt));   /* send packet to media */
}

#define  arpsize  (ETHHDR_SIZE   +  sizeof(struct  arp_hdr))

/* FUNCTION: send_arp()
 *
 * send_arp() - send an arp for outgoing Ethernet packet pkt, which 
 * has no current arp table entry. This means making a partial entry 
 * and queuing the packet at the entry. Packet will be send when 
 * target IP host reply to the ARP we send herein. If no reply, 
 * timeout will eventually free packet. 
 *
 * 
 * PARAM1: PACKET pkt
 * PARAM2: ip_addr dest_ip
 *
 * RETURNS: Returns 0 if OK, or the usual ENP_ errors 
 */

int
send_arp(PACKET pkt, ip_addr dest_ip)
{
   struct arptabent * oldest;
   char * ethhdr;
   struct arp_hdr * arphdr;
   IFMIB etif = m_netp->n_mib;    /* mib info for this Ethernet interface */
   PACKET arppkt;

   if(dest_ip == 0xFFFFFFFF)     /* broadcast? */
   {
      /* get unused or oldest entry in table */
      oldest = make_arp_entry(dest_ip, m_netp);

      /* set MAC destination to Ethernet broadcast (all FFs) */
      MEMSET(oldest->t_phy_addr, 0xFF, 6);
      return(et_send(pkt, oldest));
   }

   /* not broadcasting, so get a packet for an ARP request */
   LOCK_NET_RESOURCE(FREEQ_RESID); 
   arppkt = pk_alloc(arpsize);
   if (!arppkt)
   {
      pk_free(pkt);
      UNLOCK_NET_RESOURCE(FREEQ_RESID);
      return ENP_RESOURCE;
   }
   UNLOCK_NET_RESOURCE(FREEQ_RESID);
   arppkt->nb_prot = arppkt->nb_buff;
   arppkt->nb_plen = arpsize;
   arppkt->net = m_netp;

   /* get unused or oldest entry in table */
   oldest = make_arp_entry(dest_ip, m_netp);

   oldest->pending = pkt;           /* packet is "pended", not pk_free()d */

   /* build arp request packet */
   ethhdr = arppkt->nb_buff + ETHHDR_BIAS;     /* Ethernet header at start of buffer */
   arphdr = (struct arp_hdr *)(arppkt->nb_buff + ETHHDR_SIZE); /* arp header follows */
   arphdr->ar_hd = ARPHW;     /* net endian arp hardware type (Ethernet) */
   arphdr->ar_pro = ARPIP;
   arphdr->ar_hln = 6;
   arphdr->ar_pln = 4;
   arphdr->ar_op = ARREQ;
   arphdr->ar_tpa = dest_ip;        /* target's IP address */
   arphdr->ar_spa = m_netp->n_ipaddr;   /* my IP address */
   MEMCPY(arphdr->ar_sha, etif->ifPhysAddress, 6);
   MEMSET(ethhdr + ET_DSTOFF, 0xFF, 6);     /* destination to broadcast (all FFs) */
   MEMCPY(ethhdr + ET_SRCOFF, etif->ifPhysAddress, 6);
   ET_TYPE_SET(ethhdr, (ET_ARP));

   {
      struct arp_wire * arwp  =  (struct  arp_wire *)arphdr;
      MEMMOVE(&arwp->data[AR_SHA], arphdr->ar_sha, 6);
      MEMMOVE(&arwp->data[AR_SPA], &arphdr->ar_spa, 4);
      MEMMOVE(&arwp->data[AR_THA], arphdr->ar_tha, 6);
      MEMMOVE(&arwp->data[AR_TPA], &arphdr->ar_tpa, 4);
   }

#ifdef NPDEBUG
   if(m_netp->pkt_send == NULL)
   {
      panic("send_arp");
   }
#endif
   /* send arp request */
   m_netp->pkt_send(arppkt);  /* driver should free arppkt later */

   arpReqsOut++;
   return ENP_SEND_PENDING;
}


/* FUNCTION: find_oldest_arp()
 * 
 * Return LRU or first free entry in arp table - preperatory to 
 * making a new arp entry out of it. IP address passed is that of new 
 * entry so we can recycle previous entry for that IP if it exists. 
 *
 * PARAM1: ip_addr dest_ip
 *
 * RETURNS: LRU or first free entry in arp table
 */

struct arptabent * 
find_oldest_arp(ip_addr dest_ip)
{
   struct arptabent *   tp;
   struct arptabent *   oldest;

   /* find lru (or free) entry,  */
   oldest = arp_table;
   for (tp = arp_table; tp <= &arp_table[MAXARPS-1]; tp++)
   {
      if (tp->t_pro_addr == dest_ip)   /* ip addr already has entry */
      {
         tp->lasttime = cticks;
         return(tp);
      }
      if (!tp->t_pro_addr) /* entry is unused */
      {
         oldest = tp;   /* use free entry as "oldest" */
         /* need to keep scanning in case dest_ip already has an entry */
      }
      else if(oldest->lasttime > tp->lasttime)
      {
         oldest = tp;   /* found an older (LRU) entry */
      }
   }
   return oldest;
}

/* FUNCTION: make_arp_entry()
 *
 * make_arp_entry(ip_addr, NET) - find the first unused (or the 
 * oldest) arp table entry, and make a new entry for to prepare it 
 * for an arp reply. If the IP address already has an entry, the 
 * entry is returned with only the timestap modified. 
 *
 * PARAM1: ip_addr dest_ip
 * PARAM2: NET net
 *
 * RETURNS: Returns pointer to arp table entry selected. 
 */

struct arptabent *   
make_arp_entry(ip_addr dest_ip, NET net)
{
   struct arptabent *   oldest;

   /* find usable (or existing) ARP table entry */
   oldest = find_oldest_arp(dest_ip);

   /* partially fill in arp entry */
   oldest->createtime = cticks;

   /* If recycling entry, don't leak packets which may be stuck here */
   if ((oldest->pending) &&  
       (oldest->t_pro_addr != dest_ip)) /* don't clear pending if recycling */
   {
      LOCK_NET_RESOURCE(FREEQ_RESID);
      pk_free(oldest->pending);
      UNLOCK_NET_RESOURCE(FREEQ_RESID);
      oldest->pending = NULL;
   }

   oldest->t_pro_addr = dest_ip;
   oldest->net = net;
   MEMSET(oldest->t_phy_addr, '\0', 6);   /* clear mac address */
   return oldest;
}



/* FUNCTION: arpReply()
 *
 * arpReply() - do arp reply to the passed arp request packet. packet 
 * must be freed (or reused) herein. 
 *
 * 
 * PARAM1: PACKET pkt
 *
 * RETURNS: void
 */

void
arpReply(PACKET pkt)
{
   PACKET outpkt;
   struct arp_hdr *  in;
   struct arp_hdr *  out;
   char * ethout;
   char * ethin;

   LOCK_NET_RESOURCE(FREEQ_RESID);
   outpkt = pk_alloc(sizeof(struct arp_hdr));
   UNLOCK_NET_RESOURCE(FREEQ_RESID);
   if (!outpkt)
   {
      dtrap();
      return;
   }
   outpkt->net = pkt->net;    /* send back out the iface it came from */

   in = (struct arp_hdr *)(pkt->nb_buff + ETHHDR_SIZE);
   out = (struct arp_hdr *)(outpkt->nb_buff + ETHHDR_SIZE);

   /* prepare outgoing arp packet */
   out->ar_hd = ARPHW;
   out->ar_pro = ARPIP;
   out->ar_hln = 6;
   out->ar_pln = 4;
   out->ar_op = ARREP;
   out->ar_tpa = in->ar_spa;     /* swap IP addresses */
   out->ar_spa = in->ar_tpa;
   MEMCPY(out->ar_tha, in->ar_sha, 6);    /* move his MAC address */
   MEMCPY(out->ar_sha, outpkt->net->n_mib->ifPhysAddress, 6);  /* fill in our mac address */

   /* prepend Ethernet unicast header to arp reply */
   ethin = pkt->nb_buff + ETHHDR_BIAS;
   ethout = outpkt->nb_buff + ETHHDR_BIAS;
   MEMCPY(ethout + ET_DSTOFF, ethin + ET_SRCOFF, 6);
   MEMCPY(ethout + ET_SRCOFF, outpkt->net->n_mib->ifPhysAddress, 6);
   ET_TYPE_SET(ethout, (ET_ARP));

   {
      struct arp_wire * arwp  =  (struct  arp_wire *)out;
      MEMMOVE(&arwp->data[AR_SHA], out->ar_sha, 6);
      MEMMOVE(&arwp->data[AR_SPA], &out->ar_spa, 4);
      MEMMOVE(&arwp->data[AR_THA], out->ar_tha, 6);
      MEMMOVE(&arwp->data[AR_TPA], &out->ar_tpa, 4);
   }

   outpkt->nb_plen = ETHHDR_SIZE + sizeof(struct arp_hdr);
   outpkt->nb_prot = outpkt->nb_buff;
   outpkt->net->pkt_send(outpkt);

   /* input 'pkt' will be freed by caller */
   arpRepsOut++;
}


/* FUNCTION: arprcv()
 *
 * arprcv(PACKET) - process an incoming arp packet. 
 *
 * 
 * PARAM1: PACKET pkt
 *
 * RETURNS: Returns 0 if it was for us, 
 * ENP_NOT_MINE (1) if the arp packet is not for my IP 
 * address, else a negative error code. 
 */

int
arprcv(PACKET pkt)
{
   struct arp_hdr *  arphdr;
   struct arptabent *   tp;

   arphdr = (struct arp_hdr *)(pkt->nb_buff + ETHHDR_SIZE);

   {
      struct arp_wire * arwp  =  (struct  arp_wire *)arphdr;
      MEMMOVE(&arphdr->ar_tpa, &arwp->data[AR_TPA], 4);
      MEMMOVE(arphdr->ar_tha, &arwp->data[AR_THA], 6);
      MEMMOVE(&arphdr->ar_spa, &arwp->data[AR_SPA], 4);
      MEMMOVE(arphdr->ar_sha, &arwp->data[AR_SHA], 6);
   }

   /* check ARP's target IP against our net's: */
   if(arphdr->ar_tpa != pkt->net->n_ipaddr)
   {
      LOCK_NET_RESOURCE(FREEQ_RESID);
      pk_free(pkt);     /* not for us, dump & ret (proxy here later?) */
      UNLOCK_NET_RESOURCE(FREEQ_RESID);
      return(ENP_NOT_MINE);
   }

   if (arphdr->ar_op == ARREQ)   /* is it an arp request? */
   {
      arpReqsIn++;   /* count these */
      arpReply(pkt); /* send arp reply */
      /* make partial ARP table entry */
      make_arp_entry(arphdr->ar_spa, pkt->net);
      /* fall thru to arp reply logic to finish our table entry */
   }
   else     /* ARP reply, count and fall thru to logic to update table */
   {
      arpRepsIn++;
   }

   /* scan table for matching entry */
   /* check this for default gateway situations later, JB */
   for (tp = arp_table;   tp <= &arp_table[MAXARPS-1]; tp++)
   {
      if (tp->t_pro_addr == arphdr->ar_spa)     /* we found IP address, update entry */
      {
         MEMCPY(tp->t_phy_addr, arphdr->ar_sha, 6);   /* update MAC adddress */
         tp->lasttime = cticks;
         if (tp->pending)     /* packet waiting for this IP entry? */
         {
            PACKET outpkt = tp->pending;
            tp->pending = NULL;
            et_send(outpkt, tp);    /* try send again */
         }
         LOCK_NET_RESOURCE(FREEQ_RESID);
         pk_free(pkt);
         UNLOCK_NET_RESOURCE(FREEQ_RESID);
         return(0);
      }
   }
   /* fall to here if packet is not in table */
   LOCK_NET_RESOURCE(FREEQ_RESID);
   pk_free(pkt);
   UNLOCK_NET_RESOURCE(FREEQ_RESID);
   return ENP_NOT_MINE;
}



/* FUNCTION: send_via_arp()
 * 
 * send_via_arp() - Called when we want to send an IP packet on a 
 * media (i.e. Ethernet) which supports ARP. packet is passed, along 
 * with target IP address, which may be the packet's dest_ip or a 
 * gateway/router. We check the ARP cache (and scan arp table if 
 * required) for MAC address matching the passed dest_ip address. If 
 * the MAC address is not already known, we broadcast an arp request 
 * for the missing IP address and attach the packet to the "pending" 
 * pointer. The packet will be sent when the ARP reply comes in, or 
 * freed if we time out. We flush the cache every second to force the 
 * regular freeing of any "pending" packets. We flush every entry on 
 * the ageout interval so bogus ARP addresses won't get permanently 
 * wedged in the table. This happens when someone replaces a PC's MAC 
 * adapter but does not change the PC's IP address. 
 *
 * PARAM1: PACKET pkt
 * PARAM2: ip_addr dest_ip
 *
 * RETURNS: Returns 0 if packet went to mac sender; ENP_SEND_PENDING
 * if awaiting arp reply, or SEND_FAILED if error 
 */

int
send_via_arp(PACKET pkt, ip_addr dest_ip)
{
   struct arptabent *   tp;

   /* Force refresh of cache once a second */
   if ((cachetime + TPS) < cticks)
      arpcache = NULL;

   /* look at the last ARP entry used. Good chance it's ours: */
   if (arpcache && arpcache->t_pro_addr == dest_ip)
      return(et_send(pkt, arpcache));

   /* scan arp table for an existing entry */
   for (tp = arp_table;   tp <= &arp_table[MAXARPS-1]; tp++)
   {
      /* age out pending entrys here: */
      if (tp->pending)
      {
         /* if over a second old.. */
         if (( cticks > TPS ) &&
             (tp->createtime < (cticks - TPS)))
         {
            LOCK_NET_RESOURCE(FREEQ_RESID);
            pk_free(tp->pending);   /* free the blocked IP packet */
            UNLOCK_NET_RESOURCE(FREEQ_RESID);
            tp->pending = NULL;     /* clear pointer */
            tp->t_pro_addr = 0;     /* marks entry as "unused" */
         }
      }

      /* See if it's time to age out this entry anyway. We don't kill 
       * entries we've referenced in the last second for 
       */
      if (((tp->createtime + (TPS * (u_long)arp_ageout)) < cticks) && 
          (tp->lasttime + TPS) < cticks)
      {
         tp->t_pro_addr = 0;     /* marks entry as "unused" */
      }

      if (tp->t_pro_addr == dest_ip)   /* we found our entry */
      {
         if (tp->pending)  /* arp already pending for this IP? */
         {
            LOCK_NET_RESOURCE(FREEQ_RESID);
            pk_free(pkt);  /* sorry, we have to dump this one.. */
            UNLOCK_NET_RESOURCE(FREEQ_RESID);
            return SEND_DROPPED;    /* packet already waiting for this IP entry */
         }
         else  /* just send it */
         {
            arpcache = tp;       /* cache this entry */
            cachetime = cticks;  /* mark time we cached */
            return(et_send(pkt, tp));
         }
      }
   }
   return(send_arp(pkt, dest_ip));
}

#ifdef NET_STATS

/* FUNCTION: arp_stats()
 * 
 * PARAM1: void * pio
 *
 * RETURNS: 
 */

int
arp_stats(void * pio)   /* passed output device */
{
   struct arptabent *   atp;
   int   i;
   int   arp_entrys  =  0;

   ns_printf(pio, "arp Requests In: %u,   out: %u\n", arpReqsIn, arpReqsOut);
   ns_printf(pio, "arp Replys   In: %u,   out: %u\n", arpRepsIn, arpRepsOut);

   /* count number of arp entrys in use: */
   for (i = 0; i < MAXARPS; i++)
   {
      if (arp_table[i].t_pro_addr)
         arp_entrys++;
   }

   if (arp_entrys)
   {
      ns_printf(pio, "X)  MAC Address   iface pend    IP      ctime  ltime\n");
      for (i = 0; i < MAXARPS; i++)
      {
         atp = &arp_table[i];
         if (atp->t_pro_addr)
         {
            ns_printf(pio, "%d)  ", i);

            ns_printf(pio, "%02x%02x%02x-%02x%02x%02x ",
            atp->t_phy_addr[0],
            atp->t_phy_addr[1],
            atp->t_phy_addr[2],
            atp->t_phy_addr[3],
            atp->t_phy_addr[4],
            atp->t_phy_addr[5]);

            ns_printf(pio, "  %d   %s   %u.%u.%u.%u   %lu  %lu\n",
            if_netnumber(atp->net)+1,
            atp->pending?"Y":"N",
            PUSH_IPADDR(atp->t_pro_addr),
            (long)atp->createtime,
            (long)atp->lasttime);
         }
      }
   }
   else
   {
      ns_printf(pio, "Currently no arp table entrys.\n");
   }

   return 0;
}
#endif   /* NET_STATS */

 /* end of file et_arp.c */


