/*
 * FILENAME: tcputil.c
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
 * MODULE: MTCP
 *
 * ROUTINES: msoq_check(), m_setlport(), m_tcpdrop(), 
 * ROUTINES: m_tcpclose(), m_data_upcall(), m_newtcpcb(), so_flush(), 
 * ROUTINES: so_lookup(), m_delsocket(), m_template(), m_connected(), 
 * ROUTINES: m_disconnecting(), m_sbdrop(), get_soq(), put_soq(),
 * ROUTINES: socket_queue_name()
 *
 *
 * PORTABLE: yes
 */


#include "license.h"
#include "ipport.h"
#include "mtcp.h"

struct queue msoq;         /* global queue of M_SOCK structs */

static unshort nextlport;  /* next lport to assign in m_setlport() */

/* NEXTLPORT_LL - lower limit for nextlport */
#ifndef NEXTLPORT_LL
#define NEXTLPORT_LL       1024
#endif

/* NEXTLPORT_UL - upper limit for nextlport */
#ifndef NEXTLPORT_UL
#define NEXTLPORT_UL       65534
#endif

#ifdef NET_STATS
struct tcpstat tcpstat;    /* BSD tcp statistics */
#endif

int   TCPTV_MSL = (4 * PR_SLOWHZ);     /* max seg lifetime default */

#ifdef INCLUDE_SNMP
struct tcp_mib tcpmib;
#else    /* no SNMP, support tcp_mib locally for tcp_stats() */
struct TcpMib tcpmib;
#endif


#ifdef MSOQ_CHECK
/* sanity check of soq */
void
msoq_check()
{
   int ct;
   M_SOCK so;
   M_SOCK prevso;

   /* verify that q_tail is last element in queue, 
    * and that q_len matches number of elements in queue
    */
   ct = 0;
   prevso = NULL;
   for(so = (M_SOCK)(msoq.q_head); so; so = so->next)
   {
      ct++;
      prevso = so;
   }
   if(prevso != (M_SOCK)(msoq.q_tail))
   {
      dtrap();
   }
   if(ct != msoq.q_len)
   {
      dtrap();
   }
}
#endif   /* MSOQ_CHECK */

/* FUNCTION: m_setlport()
 *
 * Sets the socket's lport (local port number) to a reasonable 
 * non-zero value.
 *
 * PARAM1: M_SOCK so; the socket whose lport is to be set
 *
 * RETURNS: void;
 */
void
m_setlport(M_SOCK so)
{
   M_SOCK qso;

restart:
   /* guess the nextlport based on the last one +1, but constrain
    * it to be in the range [NEXTLPORT_LL...NEXTLPORT_UL] (inclusive)
    */
   if ((nextlport >= NEXTLPORT_LL) && (nextlport < NEXTLPORT_UL))
      nextlport++;
   else
      nextlport = NEXTLPORT_LL;
   nextlport = (nextlport);

   /* verify that nextlport is available by walking the socket
    * list and verifying that it's not in use 
    */
   ENTER_CRIT_SECTION(&msoq);
   for (qso = (M_SOCK)(msoq.q_head); qso->next; qso = qso->next)
   {
      /* if it's not available, start over with new nextlport */
      if (qso->lport == nextlport)
      {
         EXIT_CRIT_SECTION(&msoq);
         goto restart;
      }
   }
   EXIT_CRIT_SECTION(&msoq);

   /* use nextlport to set the socket's lport */
   so->lport = nextlport;
   nextlport = (nextlport);

}

/* FUNCTION: m_tcpdrop()
 *
 * Drop a TCP connection, reporting
 * the specified error.  If connection is synchronized,
 * then send a RST to peer.
 *
 * In BSD this is tcp_drop().
 * 
 * PARAM1: struct tcpcb *tp
 * PARAM2: int err
 *
 * RETURNS: 
 */

void
m_tcpdrop(struct tcpcb * tp, int err)
{
   M_SOCK so =  (M_SOCK)tp->t_inpcb;

   /* set socket error for upcall to app */
   so->error = err;

   /* if socket was past LISTEN state, notify app that it's going away */
   if((so->callback) && (tp->t_state > TCPS_LISTEN))
      so->callback(M_CLOSED, so, NULL);

   /* If connected, send reset to peer */
   if (TCPS_HAVERCVDSYN(tp->t_state)) 
   {
      tp->t_state = TCPS_CLOSED;
      (void) tcp_output(tp);
      TCP_STAT_INC(tcps_drops);
   }
   else
      TCP_STAT_INC(tcps_conndrops);

   m_tcpclose(tp);
}


/* FUNCTION: m_tcpclose()
 *
 * Close a TCP control block:
 *   discard all space held by the tcp
 *   discard internet protocol block
 *   wake up any sleepers
 *
 * In BSD this is tcp_close().
 * 
 * PARAM1: struct tcpcb *tp
 *
 * RETURNS: 
 */

void
m_tcpclose(struct tcpcb * tp)
{
   M_SOCK so =  (M_SOCK)tp->t_inpcb;

   so_flush(so);
   TCB_FREE (tp);
   so->tp = NULL;       /* prevent further refs to tp */
   m_disconnected(so);
   TCP_STAT_INC(tcps_closed);
}


/* FUNCTION: m_data_upcall()
 *
 * called by tcp_input() when data is received 
 * for a socket with an upcall handler set. The upcall handler is a 
 * m_socket structure member.
 *
 * The upcall routine description is as follows:
 *
 * int rx_upcall(struct socket so, PACKET pkt, int error); 
 *
 * ....where: so is socket which got data. pkt - pkt containing recieved 
 * data, or NULL if error. error is 0 if good pkt, else BSD socket 
 * error
 *
 *    The upcall() returns 0 if it accepted data, non-zero if not. 
 * End of file is signaled to the upcall by ESHUTDOWN eror code. If 
 * LOCK_NET_RESOURCE() is used, the resource is already locked when 
 * the upcall is called. The upcall will NOT be called from inside a 
 * CRIT_SECTION macro pairing. 
 *
 * 
 * PARAM1: m_socket
 *
 * RETURNS: nothing
 */

void
m_data_upcall(M_SOCK so)
{
   int      err;
   PACKET   pkt;

   /* don't upcall application if there's no data */
   if (so->rcvdq.sb_cc == 0)
      return;

   /* don't re-enter the upcall routine */
   if (so->state & SS_INUPCALL)
      return;

   /* Set flags. SS_UPCALLED is used by select() logic to wake sockets blocked
    * on receive, SS_INUPCALL is the re-entry guard.
    */
   so->state |= (SS_UPCALLED|SS_INUPCALL);

   while(so->rcvdq.p_head)
   {
      pkt = so->rcvdq.p_head;
      err = so->callback(M_RXDATA, so, pkt);
      if(err)     /* if app returned error, quit */
         break;

      /* dequeue the packet data the application just accepted */      
      get_soq(&so->rcvdq);
      tcp_pktfree(pkt);
   }
   so->state &= ~SS_INUPCALL;    /* clear re-entry flag */
   return;
}


/* FUNCTION: tcp_newtcpcb()
 *
 * Create a new TCP control block, making an
 * empty reassembly queue and hooking it to the argument
 * protocol control block.
 *
 * PARAM1: struct inpcb *inp
 *
 * RETURNS: 
 */

struct tcpcb * 
m_newtcpcb(M_SOCK so)
{
   struct tcpcb * tp;
   short t_time;

   tp = TCB_ALLOC(sizeof (*tp));
   if (tp == NULL)
      return (struct tcpcb *)NULL;
   tp->t_maxseg = TCP_MSS;
   tp->t_flags = 0;        /* sends options! */

   /* install back pointer to socket */
   tp->t_inpcb = (struct inpcb *)so;
   so->tp = tp;

   /*
    * Init srtt to TCPTV_SRTTBASE (0), so we can tell that we have no
    * rtt estimate.  Set rttvar so that srtt + 2 * rttvar gives
    * reasonable initial retransmit time.
    */
   tp->t_srtt = TCPTV_SRTTBASE;
   tp->t_rttvar = TCPTV_SRTTDFLT << 2;

   t_time = ((TCPTV_SRTTBASE >> 2) + (TCPTV_SRTTDFLT << 2)) >> 1;
   TCPT_RANGESET(tp->t_rxtcur, t_time, TCPTV_MIN, TCPTV_REXMTMAX);
   tp->snd_cwnd = (u_short)(mt_deftxwin);
   tp->snd_ssthresh = 65535;        /* XXX */
   return (tp);
}


/* FUNCTION: so_flush()
 *
 * Free all the buffers in a socket's receive and send
 * queues in preparation for termination.
 *
 * PARAM1: socket
 *
 * RETURNS: none
 */


void
so_flush(M_SOCK so)
{
   while(so->sendq.p_head)
      tcp_pktfree(get_soq(&so->sendq));
   while(so->rcvdq.p_head)
      tcp_pktfree(get_soq(&so->rcvdq));

   so->state |= (SS_CANTRCVMORE|SS_CANTSENDMORE);
}


/* FUNCTION: so_lookup()
 *
 *    Look up the first socket matching the passed received
 * IP and TCP headers.
 *    This allows the socket to contain wildcard(0) foreign tupple members,
 * but tries for an exact match first.
 *
 * PARAM1: IP header with IP addresses to match
 * PARAM2: TCP header with ports to match
 *
 * RETURNS: matching socket if found, else null.
 */

M_SOCK
so_lookup(struct ip * pip, struct tcphdr * ptcp)
{
   M_SOCK so;        /* scratch */
   M_SOCK wild;      /* socket which had a wildcard match */

msoq_check();
   wild = NULL;
   for(so = (M_SOCK)msoq.q_head; so; so = so->next)
   {
      /* local port must always match - do quick test */
      if(so->lport != ptcp->th_dport)
         continue;   /* not even a partial match */

      /* see if it's an exact match */
      if((so->fhost == pip->ip_src) &&
         (so->lhost == pip->ip_dest) &&
         (so->fport == ptcp->th_sport))
      {
         return so;     /* got exact match */
      }

      /* see if this one is a wildcard match */
      if(((so->fhost == 0) || (so->fhost == pip->ip_src)) &&
         ((so->lhost == 0) || (so->lhost == pip->ip_dest)) &&
         ((so->fport == 0) || (so->fport == ptcp->th_sport)))
      {
         wild = so;  /* remember wildcard match, but keep looking */
      }
   }
#ifdef NPDEBUG
   if(wild && (wild->tp->t_state != TCPS_LISTEN))
   {
      dtrap();
   }
#endif   /* NPDEBUG */
   return wild;   /* return partial match or null */
}


/* FUNCTION: m_delsocket()
 *
 * Delete the passed socket. Removes the structure from msoq and
 * releases the memory.
 *
 * If a tcpcb is attached (tp), it will also be freed.
 *
 * PARAM1: socket to delete
 *
 * RETURNS: nothing
 */

void
m_delsocket(M_SOCK so)
{
   M_SOCK   tmp;     /* scratch for queue lookup; */
   M_SOCK   last;    /* for queue deletion; */

   so_flush(so);     /* free any data buffers */

   /* search global list for socket to dequeue */
   last = NULL;
   ENTER_CRIT_SECTION(&msoq);
   for(tmp = (M_SOCK)msoq.q_head; tmp; tmp = tmp->next)
   {
      if(tmp == so)  /* found socket to drop */
      {
         if(msoq.q_head == (qp)so)  /* deleting head? */
            msoq.q_head = (qp)so->next;
         else  /* not deleting head; just unlink */
            last->next = tmp->next;
         /* if deleting tail, update queue */
         if(msoq.q_tail == (qp)so)
            msoq.q_tail = (qp)last;
         msoq.q_len--;
         break;
      }
      last = tmp;
   }
   EXIT_CRIT_SECTION(&msoq);
msoq_check();
   if(!tmp)
   {
      dtrap();    /* socket not in list */
      return;
   }
   if(so->tp)
   {
      TCB_FREE(so->tp);
      so->tp = NULL;    /* prevent further tp references via socket */
   }
   SOC_FREE(so);
}


/* FUNCTION: m_template()
 *
 * Create template to be used to send tcp packets on a connection.
 * Call after host entry created, allocates an mbuf and fills
 * in a skeletal tcp/ip header, minimizing the amount of work
 * necessary when the connection is used.
 *
 * 
 * PARAM1: struct tcpcb * tp
 *
 * RETURNS: 
 */

void
m_template(struct tcpcb * tp)
{
   M_SOCK so;
   struct tcpiphdr * n;

   so = (M_SOCK)tp->t_inpcb;

#ifdef NPDEBUG
   if(!so)
   {
      dtrap();
   }
#endif

   /* set local pointer and tp back pointer to socket buffer */
   n = tp->t_template = (struct tcpiphdr *)(&so->t_template[0]);
   MEMSET(n, 0, 40);

   /* fill in template TCP/IP header fields we know at this point */
   n->ti_i.ip_len = (sizeof (struct tcpiphdr) - sizeof (struct ip));
   n->ti_i.ip_src = so->lhost;
   n->ti_i.ip_dest = so->fhost;
   n->ti_t.th_sport = so->lport;
   n->ti_t.th_dport = so->fport;
   n->ti_t.th_doff = (5 << 4);
msoq_check();
   return;
}


/* FUNCTION: m_connected()
 *
 * Called to handle state change when a socket connects.
 *
 * PARAM1: socket that connected
 *
 * RETURNS: nothing
 */

void
m_connected(M_SOCK so)
{
   so->state &= ~(SS_ISCONNECTING|SS_ISDISCONNECTING);
   so->state |= SS_ISCONNECTED;
   so->error = 0;

   /* set flag to do callback to indicate socket connected. We defer the 
    * call back until after the packet which created the connected state is 
    * fully processed so that that callback routine can safly send a data
    * packet (or even start a shutdown).
    */
   so->tp->t_flags |= TF_OPENUP;
}


/* FUNCTION: m_disconnecting()
 * 
 * Called by the tcp layer when a socket is disconnecting. Sets
 * the required flags and does the wakeup() calls.
 *
 * PARAM1: M_SOCK so
 *
 * RETURNS: 
 */

void
m_disconnecting(M_SOCK so)
{
   so->state &= ~SS_ISCONNECTING;
   so->state |= (SS_ISDISCONNECTING|SS_CANTRCVMORE|SS_CANTSENDMORE);
   tcp_wakeup(&so->sendq);
   tcp_wakeup(&so->rcvdq);
}


void
m_disconnected(M_SOCK so)
{
   so->state &= ~(SS_ISCONNECTING|SS_ISCONNECTED|SS_ISDISCONNECTING);
   so->state |= (SS_CANTRCVMORE|SS_CANTSENDMORE);
   tcp_wakeup (&so->sendq);
   tcp_wakeup (&so->rcvdq);
}


/* FUNCTION: m_sbdrop()
 *
 * Drop packets from the head of an msock data queue. This is called when 
 * an ack is received for buffered send data.
 *
 * PARAM1: queue * que - que to drop from
 * PARAM2: unsigned todrop - number of bytes to drop
 *
 * RETURNS: 0 if OK or BSD error.
 */

int
m_sbdrop(struct m_sockbuf * que, unsigned todrop)
{
   PACKET   pkt;

   while(todrop)
   {
      pkt = (PACKET)(que->p_head);   /* head packet in queue */
      if(!pkt)
      {
         dtrap();    /* tried to drop more than we had? */
         return EINVAL;
      }

      /* see if we can drop the whole packet */
      if(todrop >= pkt->m_len)
      {
         todrop -= pkt->m_len;
         tcp_pktfree(get_soq(que));
      }
      else if(todrop > 0)    /* other guy acked a partial packet */
      {
         dtrap();  /* watch first time -JB- */
         pkt->m_len -= todrop;   /* strip data from front of pkt */
         pkt->m_data += todrop;
         return 0;               /* done */
      }
      else     /* todrop was negative */
      {
         dtrap();    /* programming error */
         return 0;
      }
   }
   return 0;
}

/* FUNCTION: get_soq()
 *
 * Unlink and return packet at head of socket data que passed
 * 
 * PARAM1: struct m_sockbuf * que
 *
 * RETURNS: PACKET that was at head of queue
 */

PACKET
get_soq(struct m_sockbuf * que)
{
   PACKET pkt;
   
   if(que->p_head == NULL)
      return NULL;

   ENTER_CRIT_SECTION(que);
   pkt = que->p_head;
   que->p_head = pkt->m_next;
   if(pkt == que->p_tail)     /* if packet is also tail... */
      que->p_tail = NULL;     /* ...then que is now empty */
   que->sb_cc -= pkt->m_len;  /* deduct pkt data length from total */
   EXIT_CRIT_SECTION(que);

   return pkt;
}


/* FUNCTION: put_soq()
 *
 * Add a packet to the tail of socket data que.
 * 
 * PARAM1: struct m_sockbuf * que
 * PARAM2: PACKET pkt to add
 *
 * RETURNS: void
 */

void
put_soq(struct m_sockbuf * que, PACKET pkt)
{
   
   ENTER_CRIT_SECTION(que);
   pkt->m_next = NULL;        /* no next, will be last pkt in que */
   if(que->p_tail)
      que->p_tail->m_next = pkt;
   que->p_tail = pkt;         /* make it the new tail */
   if(que->p_head == NULL)    /* if queue was empty... */
      que->p_head = pkt;      /* packet is now head & tail */
   que->sb_cc += pkt->m_len;  /* add pkt data length to total */
   EXIT_CRIT_SECTION(que);
}


/* FUNCTION: socket_queue_name()
 *
 * this function checks to see if the passed in PACKET structure is 
 * in one of the socket queues. returns a pointer to name describing 
 * what queue the PACKET structure 
 *
 * 
 * PARAM1: PACKET pkt
 *
 * RETURNS: 
 */

#ifdef IN_MENUS

char *
socket_queue_name(PACKET pkt)
{
   M_SOCK so;
   PACKET tmp;

   for(so = (M_SOCK)msoq.q_head; so; so = so->next)
   {
      /* check to see if the pkt is in the rcv mbuf queue */
      for(tmp = so->rcvdq.p_head; tmp; tmp = tmp->m_next)
         if(tmp == pkt)
            return "sor";
      /* check to see if the pkt is in the send mbuf queue */
      for(tmp = so->sendq.p_head; tmp; tmp = tmp->m_next)
         if(tmp == pkt)
            return "snd";
   }
   /* not found in any socket queue */
   return 0;
}
#endif   /* IN_MENUS */

#ifndef TCPWAKE_ALREADY

/* the tcp process sleep and wakeup system.
 * these are the superloop versions, for other versions you need 
 * to define them in you port files and set TCPWAKE_ALREADY.
 *
 * A true multitasking version of these is in ..\misclib\netmain.c
 */


void *   last_arg;   /* for debugging */

/* FUNCTION: tcp_sleep()
 * 
 * PARAM1: void * timeout
 *
 * RETURNS: void
 */

void
tcp_sleep(void * timeout)
{
   UNLOCK_NET_RESOURCE(NET_RESID);
   tk_yield(); /* let the system run a bit... */
   LOCK_NET_RESOURCE(NET_RESID);

   last_arg = timeout;
}



/* FUNCTION: tcp_wakeup()
 * 
 * PARAM1: void * wake
 *
 * RETURNS: void
 */

void
tcp_wakeup(void * wake)
{
   last_arg = wake;
}
#endif   /* TCPWAKE_ALREADY */


