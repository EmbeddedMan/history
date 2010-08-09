/*
 * FILENAME: tcp_in.c
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
 * ROUTINES: m_tcpreass(), tcp_input(), tcp_dooptions(), tcp_xmit_timer(), 
 *
 * PORTABLE: yes
 */

#include "license.h"
#include "ipport.h"
#include "mtcp.h"
#include "icmp.h" /* for icmp_destun() declaration */


#define DEBUG_DROP 1
#ifdef DEBUG_DROP  /* trace logic for dropped packets */
int      dropline;
#define  GOTO_DROP   {  dropline=__LINE__;   goto  drop; }
#define  GOTO_DROPWITHRESET   {  dropline=__LINE__;   goto  dropwithreset; }
#else /* the production flavor: */
#define  GOTO_DROP   {  goto  drop; }
#define  GOTO_DROPWITHRESET   {  goto  dropwithreset; }
#endif

char     tcpprintfs  =  0;
char     tcprexmtthresh =  3;
struct   tcpiphdr tcp_saveti;

#ifdef DO_DELAY_ACKS
char     _tcp_delay_ack =  1;
#else
char     _tcp_delay_ack =  0;
#endif   /* DO_DELAY_ACKS */

void  tcp_xmit_timer(struct tcpcb * tp);


/* FUNCTION: m_tcpoptions()
 *
 * Handle TCP options. Only one we do is MSS.
 *
 * PARAM1: struct tcpcb * tp
 * PARAM2: pointer to option string 
 * PARAM3: length of option string
 *
 * RETURNS: nothing
 */

void
m_tcpoptions(struct tcpcb * tp, char * opt, int optlen)
{
   /* see if it's mss option and length is valid */
   while(optlen >= 4)
   {
      /* we currently only support max seg size */
      if(*opt == TCPOPT_MAXSEG)
      {
         /* set tcpcb mss to the lesser of passed MSS and our own local MSS */
         tp->t_maxseg = *(u_short *)(opt + 2);
         tp->t_maxseg = (tp->t_maxseg);
         tp->t_maxseg = (u_short)MIN(tp->t_maxseg, TCP_MSS);
      }
      opt += 4;
      optlen -= 4;
   }
}



/* FUNCTION: m_tcpreass()
 *
 * Mini version of tcp_reass(). Puts passed packets in tcpcb's recevied
 * packet queue. Any packets not in sequence are kept in the tp->oosq,
 * out of sequence queue.
 *
 *
 * PARAM1: struct tcpcb * tp
 *
 * RETURNS: int; effective TCP header flags which tcp_rcv()
 *               should use for subsequent flags processing
 */

int
m_tcpreass(struct tcpcb * tp, struct tcphdr * ptcp, PACKET pkt)
{
   long     offset;     /* offset of this pkt into rcv seq space */
   M_SOCK   so;         /* scratch */
   int      tiflags;    /* TCP header flags */
#ifdef MINI_TCP_OOSQ
   PACKET   tmp;        /* scratch */
   PACKET   prevtmp;    /* scratch */
   int      overlap;    /* data overlap with next packet */
#endif  

   struct tr_hdrinfo
   {
      tcp_seq th_seq;
      u_char th_flags;
      char padbyte;
   };

   offset = (long)(ptcp->th_seq - tp->rcv_nxt);
   tiflags = (int)ptcp->th_flags;
   so = (M_SOCK)(tp->t_inpcb);

   /* set flag to ACK received data */
   if (_tcp_delay_ack)
      tp->t_flags |= TF_DELACK;
   else
      tp->t_flags |= TF_ACKNOW;

   if((offset + (int)pkt->m_len) < 0)  /* packet is a duplicate */
   {
      TCP_STAT_INC(tcps_rcvduppack);
      TCP_STAT_ADD(tcps_rcvdupbyte, pkt->m_len);
      tcp_pktfree(pkt);
      return tiflags;
   }
   if(offset > 0)     /* packet is ahead of seq space */
   {
      tiflags &= ~TH_FIN;
#ifdef MINI_TCP_OOSQ
      ENTER_CRIT_SECTION(&so->oosq);
      /* put packet in order in the oos queue */
      ((struct tr_hdrinfo *)(pkt->nb_buff))->th_seq = ptcp->th_seq;
      ((struct tr_hdrinfo *)(pkt->nb_buff))->th_flags = ptcp->th_flags;
      tmp = so->oosq.p_head;        /* start at head of queue */
      if(tmp == NULL)      /* no queued OOS packets yet, start que */
      {
         pkt->m_next = NULL;
         so->oosq.p_head = so->oosq.p_tail = pkt;
         so->oosq.sb_cc = pkt->m_len;
      }
      else  /* add to existing OOS queue */
      {
         /* find a place to insert this packet in the queue */
         prevtmp = NULL;
         while (tmp && SEQ_GT(ptcp->th_seq, 
                              ((struct tr_hdrinfo *)(tmp->nb_buff))->th_seq))
         {
            prevtmp = tmp;
            tmp = tmp->m_next;
         }
         /* we are inserting between prevtmp and tmp:
          * we know that (if prevtmp) prevtmp's TCP seq < pkt's TCP seq 
          * and that (if tmp) pkt's TCP seq <= tmp's TCP seq
          * ...so if pkt's TCP seq == tmp's TCP seq, figure out
          * which is longer and keep that one, preferring tmp.
          */
         pkt->m_next = tmp;
         if ((tmp) && 
             (ptcp->th_seq == ((struct tr_hdrinfo *)(tmp->nb_buff))->th_seq))
         {
            if (tmp->m_len < pkt->m_len)
            {
               pkt->m_next = tmp->m_next;
               so->oosq.sb_cc -= tmp->m_len;
               TCP_STAT_INC(tcps_rcvduppack);
               TCP_STAT_ADD(tcps_rcvdupbyte, tmp->m_len);
               tcp_pktfree(tmp);
            }
            else
            {
               TCP_STAT_INC(tcps_rcvduppack);
               TCP_STAT_ADD(tcps_rcvdupbyte, pkt->m_len);
               tcp_pktfree(pkt);
               EXIT_CRIT_SECTION(&so->oosq);
               return tiflags;
            }
         }
         if (prevtmp)
            prevtmp->m_next = pkt;
         else
            so->oosq.p_head = pkt;
         if (!(pkt->m_next))
            so->oosq.p_tail = pkt;
         so->oosq.sb_cc += pkt->m_len;
      }
      TCP_STAT_INC(tcps_rcvoopack);
      TCP_STAT_ADD(tcps_rcvoobyte, pkt->m_len);
      EXIT_CRIT_SECTION(&so->oosq);
#else  /* MINI_TCP_OOSQ */
      tcp_pktfree(pkt);
#endif /* MINI_TCP_OOSQ */
      return tiflags;
   }

   /* fall to here if he packet contains the next data in the seq space.
    * The packet may also contain previously received data at the front,
    * which we must trim before putting the packet on the rcvdq.
    */
   if(offset < 0)
   {
      pkt->m_data += offset;
      pkt->m_len -= offset;
      TCP_STAT_INC(tcps_rcvpartduppack);
      TCP_STAT_ADD(tcps_rcvpartdupbyte, -((long)offset));
   }

   put_soq(&so->rcvdq, pkt);
   tp->rcv_nxt += pkt->m_len;

#ifdef MINI_TCP_OOSQ
   /* see if we can now process any Out of Order packets */
   ENTER_CRIT_SECTION(&so->oosq);
   while(so->oosq.p_head)
   {
      tmp = (PACKET)(so->oosq.p_head);
      overlap = (int)(tp->rcv_nxt - 
                      (((struct tr_hdrinfo *)(tmp->nb_buff))->th_seq));
      /* if oosq packet is still out of order, stop */
      if (overlap < 0) 
         break;
      /* if oosq packet contains a FIN, keep that */
      if (((struct tr_hdrinfo *)(tmp->nb_buff))->th_flags & TH_FIN)
         tiflags |= TH_FIN;
      /* see if next packet is completely covered by new one */
      if(overlap >= (int)tmp->m_len)
      {
         TCP_STAT_INC(tcps_rcvduppack);
         TCP_STAT_ADD(tcps_rcvdupbyte, tmp->m_len);
         tcp_pktfree(get_soq(&so->oosq));  /* dump next oosq pkt */
         continue;
      }
      /* see if next packet is partially covered by new one */
      if(overlap > 0)
      {
         /* adjust data and move tmp from oosq to rcvdq. 
          * The data adjustment invalidates tmp's tcp header seq,
          * but that's OK since we don't need it anymore once 
          * it's in rcvdq, and it's about to go there.
          */
         TCP_STAT_INC(tcps_rcvpartduppack);
         TCP_STAT_ADD(tcps_rcvpartdupbyte, overlap);
         tmp->m_data += overlap;
         tmp->m_len -= overlap;
         so->oosq.sb_cc -= overlap;
      }
      /* get here if data in tmp comes exactly at tp->rcv_nxt */
      put_soq(&so->rcvdq, get_soq(&so->oosq));  /* move tmp to rvcdq */
      tp->rcv_nxt += tmp->m_len;
   }
   EXIT_CRIT_SECTION(&so->oosq);
#endif  /* MINI_TCP_OOSQ */

   tcp_wakeup(&so->rcvdq);   /* wake any sockets waiting for recv */   

   return tiflags;
}


/* FUNCTION: tcp_rcv()
 *
 * TCP input routine, follows pages 65-76 of the
 * protocol specification dated September, 1981 very closely.
 *
 * 
 * PARAM1: struct mbuf *m0
 * PARAM2: unsigned ifindex
 *
 * RETURNS: int; always SUCCESS (0)
 */
int
tcp_rcv(PACKET pkt)
{
   struct tcphdr * ptcp;
   struct ip *    pip;
   struct tcpcb * tp =  0;
   M_SOCK   so =  NULL;
   ip_addr mask;
   long     iss =  0;
   char *   opts;
   int      optlen = 0;
   int      len,  tlen, off;
   int      tiflags;
   int      todrop,  acked,   ourfinisacked, needoutput  =  0;
   int      dropsocket  =  0;
#ifdef DO_TCPTRACE
   int   ostate;
#endif

   TCP_STAT_INC(tcps_rcvtotal);
   TCP_MIB_INC(tcpInSegs);    /* keep MIB stats */

   pip = (struct ip *)pkt->nb_prot;

   if (pkt->nb_plen < ((sizeof (struct ip) + sizeof (struct tcphdr))))
   {
      TCP_STAT_INC(tcps_rcvshort);
      GOTO_DROP;
   }
   ptcp = (struct tcphdr *)ip_data(pip);

   if (tcp_cksum(pip) != ptcp->th_sum)
   {
      TCP_MIB_INC(tcpInErrs);    /* keep MIB stats */
      TCP_STAT_INC(tcps_rcvbadsum);  /* keep BSD stats */
      GOTO_DROP;
   }

   tlen = (pip->ip_len);     /* this was fudged by IP layer */

   /* Check that TCP offset makes sense, */
   off = GET_TH_OFF((*ptcp)) << 2;
   if (off < sizeof (struct tcphdr) || off > tlen) 
   {
#ifdef DO_TCPTRACE
      tcp_trace("tcp off: src %x off %d\n", ti->ti_src, off);
#endif
      TCP_STAT_INC(tcps_rcvbadoff);
      TCP_MIB_INC(tcpInErrs);   /* keep MIB stats */
      GOTO_DROP;
   }
   tlen -= (int)off;
   pip->ip_len = (u_short)tlen;  /* ip_len is now length of tcp data */

   /* set options pointer and length */
   if (off > sizeof (struct tcphdr)) 
   {
      opts = (char *)(ptcp + 1);
      optlen = off - sizeof (struct tcphdr);
      if (pkt->nb_plen < sizeof(struct ip) + off) 
      {
         TCP_STAT_INC(tcps_rcvshort);
         GOTO_DROP;
      }
   }
   else     /* no options in packet */
   {
      opts = NULL;      /* use opts as flag for option presence */
      optlen = 0;
   }

   tiflags = ptcp->th_flags;

   /* Drop TCP and IP headers; and any TCP options. */
   pkt->nb_prot += (TCPIPHDRSZ + optlen);
   pkt->nb_plen -= (TCPIPHDRSZ + optlen);

   /* Set up packet's TCP members. Future reference to TCP data should
    * be done via these:
    */
   pkt->m_data = pkt->nb_prot;
   pkt->m_len = pkt->nb_plen;


#ifdef DO_TCPTRACE
   tcp_trace("TCPIN: S:%08lx A:%08lx", ptcp->th_seq, ptcp->th_ack);
   tcp_trace("     : W:%04x  U:%04x F:%02x", ptcp->th_win, ptcp->th_urp, tiflags);
#endif

   /* find the socket for the segment */
findpcb:
   so = so_lookup(pip, ptcp);
   if(so == NULL)
   {
      /* no socket, send ICMP "desination unreachable" packet */
      icmp_destun(pip->ip_src, pip, DSTPORT, pkt->net);
      GOTO_DROPWITHRESET;
   }

   /*
    * If the state is CLOSED (i.e., TCB does not exist) then
    * all data in the incoming segment is discarded.
    * If the TCB exists but is in CLOSED state, it is embryonic,
    * but should either do a listen or a connect soon.
    */
   tp = so->tp;
   if (tp == NULL)
      GOTO_DROPWITHRESET;
   if (tp->t_state == TCPS_CLOSED)
      GOTO_DROP;

#ifdef DO_TCPTRACE
   if (so->so_options & SO_DEBUG) 
   {
      ostate = tp->t_state;
      tcp_saveti = *ti;
   }
#endif

   /* see if the matched socket is listening for a new connection */
   if (tp->t_state == TCPS_LISTEN) 
   {
      M_SOCK so_new;
      M_SOCK so_tmp;
      struct tcpcb * tp_new;

      /* clone the socket and the tcpcb for new connection. We DONT
       * call m_socket() for this since the NET_RESID is already locked
       * If it's being used in this build.
       */
      so_new = (M_SOCK)SOC_ALLOC(sizeof(struct msocket));
      if (so_new == NULL)
         GOTO_DROP;
      putq(&msoq, so_new);       /* put new socket in queue */

      /* copy listen socket data into new socket. */
      so_tmp = so_new->next;     /* save the link first */
      MEMCPY(so_new, so, sizeof(struct msocket));
      so_new->next = so_tmp;     /* restore the link */
      so = so_new;               /* make the socket the one to use */

      tp_new = m_newtcpcb(so);
      if(tp_new == NULL)
      {
         m_delsocket(so);
         GOTO_DROP;
      }
      tp = tp_new;
      tp->t_state = TCPS_LISTEN;

      /*
       * Mark socket as temporary until we're
       * committed to keeping it.  The code at
       * ``drop'' and ``dropwithreset'' check the
       * flag dropsocket to see if the temporary
       * socket created here should be discarded.
       * We mark the socket as discardable until
       * we're committed to it below in TCPS_LISTEN.
       */
      dropsocket++;
   }

   /*
    * Segment received on connection.
    * Reset idle time and keep-alive timer.
    */
   tp->t_idle = 0;
   tp->t_timer[TCPT_KEEP] = tcp_keepidle;

   /*
    * Process options if not in LISTEN state,
    * else do it below (after getting remote address).
    */
   if (opts && (tp->t_state != TCPS_LISTEN) )
   {
      m_tcpoptions(tp, opts, optlen);
      opts = NULL;
   }

   /*
    * Calculate amount of space in receive window,
    * and then do TCP input processing.
    * Receive window is amount of space in rcv queue,
    * but not less than advertised window.
    */
   { int win;

      win = mt_defrxwin - so->rcvdq.sb_cc;
      if (win < 0)
         win = 0;
      tp->rcv_wnd = (u_short)MAX(win, (int)(tp->rcv_adv - tp->rcv_nxt));
   }


   switch (tp->t_state) 
   {

   /*
    * If the state is LISTEN then ignore segment if it contains an RST.
    * If the segment contains an ACK then it is bad and send a RST.
    * If it does not contain a SYN then it is not interesting; drop it.
    * Don't bother responding if the destination was a broadcast.
    * Otherwise initialize tp->rcv_nxt, and tp->irs, select an initial
    * tp->iss, and send a segment:
    *     <SEQ=ISS><ACK=RCV_NXT><CTL=SYN,ACK>
    * Also initialize tp->snd_nxt to tp->iss+1 and tp->snd_una to tp->iss.
    * Fill in remote peer address fields if not previously specified.
    * Enter SYN_RECEIVED state, and process any other fields of this
    * segment in this state.
    */
   case TCPS_LISTEN: 
      {
         if (tiflags & TH_RST)
            GOTO_DROP;
         if (tiflags & TH_ACK)
            GOTO_DROPWITHRESET;
         if ((tiflags & TH_SYN) == 0)
            GOTO_DROP;

         mask = ~pkt->net->snmask;   /* mask for broadcast detection */
         if((pip->ip_dest & mask) == mask)
            GOTO_DROP;


         /* fill in IP connection info */
         so->fhost = pip->ip_src;
         so->fport = ptcp->th_sport;
         so->lhost = pkt->net->n_ipaddr;  /* our local address */

         so->ifp = pkt->net;     /* set interface for conn.*/
         m_template(tp);         /* set up header template for tcp sends */
         if(opts) 
            m_tcpoptions(tp, opts, optlen);

         if (iss)
            tp->iss = iss;
         else
            tp->iss = tcp_iss;
         tcp_iss += (unsigned)(TCP_ISSINCR/2);
         tp->irs = ptcp->th_seq;
         tcp_sendseqinit(tp);
         tcp_rcvseqinit(tp);
         tp->t_flags |= TF_ACKNOW;
         tp->t_state = TCPS_SYN_RECEIVED;
         tp->t_timer[TCPT_KEEP] = TCPTV_KEEP_INIT;
         dropsocket = 0;      /* committed to socket */
         TCP_STAT_INC(tcps_accepts);
         goto trimthenstep6;
      }

   /*
    * If the state is SYN_SENT:
    *   if seg contains an ACK, but not for our SYN, drop the input.
    *   if seg contains a RST, then drop the connection.
    *   if seg does not contain SYN, then drop it.
    * Otherwise this is an acceptable SYN segment
    *   initialize tp->rcv_nxt and tp->irs
    *   if seg contais ack then advance tp->snd_una
    *   if SYN has been acked change to ESTABLISHED else SYN_RCVD state
    *   arrange for segment to be acked (eventually)
    *   continue processing rest of data/controls, beginning with URG
    */
   case TCPS_SYN_SENT:
      so->ifp = pkt->net;     /* set interface for conn.*/
      if ((tiflags & TH_ACK) &&
          (SEQ_LEQ(ptcp->th_ack, tp->iss) ||
          SEQ_GT(ptcp->th_ack, tp->snd_max)))
      {
         GOTO_DROPWITHRESET;
      }
      if (tiflags & TH_RST) 
      {
         if (tiflags & TH_ACK)
            m_tcpdrop(tp, ECONNREFUSED);
         GOTO_DROP;
      }
      if ((tiflags & TH_SYN) == 0)
         GOTO_DROP;
      if (tiflags & TH_ACK) 
      {
         tp->snd_una = ptcp->th_ack;
         if (SEQ_LT(tp->snd_nxt, tp->snd_una))
            tp->snd_nxt = tp->snd_una;
      }
      tp->t_timer[TCPT_REXMT] = 0;
      tp->irs = ptcp->th_seq;
      tcp_rcvseqinit(tp);
      if (so->lhost != pip->ip_dest) 
      {
         /* 
          * the IP interface may have changed address since we sent our SYN
          * (e.g. PPP brings link up as a result of said SYN and gets new
          * address via IPCP); if so we need to update the inpcb and the
          * TCP header template with the new address.
          */
         if (pkt->net->n_ipaddr == pip->ip_dest) 
         {
            so->lhost = pip->ip_dest;
            tp->t_template->ti_i.ip_src = pip->ip_dest;
         }
      }
      tp->t_flags |= TF_ACKNOW;
      if ((tiflags & TH_ACK) && SEQ_GT(tp->snd_una, tp->iss)) 
      {
         TCP_MIB_INC(tcpActiveOpens);     /* keep MIB stats */
         TCP_STAT_INC(tcps_connects);
         tp->t_state = TCPS_ESTABLISHED;
         m_connected (so);
         tp->t_maxseg = MIN(tp->t_maxseg, TCP_MSS);

         /*
          * if we didn't have to retransmit the SYN,
          * use its rtt as our initial srtt & rtt var.
          */
         if (tp->t_rtt) 
         {
            tp->t_srtt = tp->t_rtt << 3;
            tp->t_rttvar = tp->t_rtt << 1;
            TCPT_RANGESET(tp->t_rxtcur, 
             (short)(((tp->t_srtt >> 2) + tp->t_rttvar) >> 1),
             TCPTV_MIN, TCPTV_REXMTMAX);
            tp->t_rtt = 0;
         }
      } else
         tp->t_state = TCPS_SYN_RECEIVED;

trimthenstep6:
      /* Advance ptcp->th_seq to correspond to first data byte. */
      ptcp->th_seq++;

      tp->snd_wl1 = ptcp->th_seq - 1;
      tp->rcv_up = ptcp->th_seq;
      goto step6;
   }

   /*
    * States other than LISTEN or SYN_SENT.
    * First check that at least some bytes of segment are within 
    * receive window.  If segment begins before rcv_nxt,
    * drop leading data (and SYN); if nothing left, just ack.
    */
   todrop = (int)(tp->rcv_nxt - ptcp->th_seq);
   if (todrop > 0) 
   {
      if (tiflags & TH_SYN) 
      {
         tiflags &= ~TH_SYN;
         ptcp->th_seq++;
         if (ptcp->th_urp > 1) 
            ptcp->th_urp--;
         else
            tiflags &= ~TH_URG;
         todrop--;
      }

      if ((todrop > (int)pkt->m_len) ||
          ((todrop == (int)pkt->m_len) && ( tiflags&TH_FIN) == 0))
      {
         TCP_STAT_INC(tcps_rcvduppack);
         TCP_STAT_ADD(tcps_rcvdupbyte, pip->ip_len);
         /*
          * If segment is just one to the left of the window,
          * check two special cases:
          * 1. Don't toss RST in response to 4.2-style keepalive.
          * 2. If the only thing to drop is a FIN, we can drop
          *    it, but check the ACK or we will get into FIN
          *    wars if our FINs crossed (both CLOSING).
          * In either case, send ACK to resynchronize,
          * but keep on processing for RST or ACK.
          */
         if ((tiflags & TH_FIN && todrop == (int)pip->ip_len + 1) ||
            (tiflags & TH_RST && ptcp->th_seq == tp->rcv_nxt - 1))
         {
            todrop = pip->ip_len;
            tiflags &= ~TH_FIN;
            tp->t_flags |= TF_ACKNOW;
         }
         else
            goto dropafterack;
      }
      else 
      {
         TCP_STAT_INC(tcps_rcvpartduppack);
         TCP_STAT_ADD(tcps_rcvpartdupbyte, todrop);
      }
#ifdef NPDEBUG
      if(todrop > (int)pkt->m_len)  /* sanity test */
      {
         dtrap();
         todrop = pkt->m_len;
      }
#endif
      if(todrop)
      {
         pkt->m_len -= todrop;    /* trim received packet data */
         pkt->m_data += todrop;
         ptcp->th_seq += todrop;
         pip->ip_len -= (u_short)todrop;
      }
      if (ptcp->th_urp > (u_short)todrop)
         ptcp->th_urp -= (u_short)todrop;
      else 
      {
         tiflags &= ~TH_URG;
         ptcp->th_urp = 0;
      }
   }

   /*
    * If segment ends after window, drop trailing data
    * (and PUSH and FIN); if nothing left, just ACK.
    */
   todrop = (int)((ptcp->th_seq + (short)pip->ip_len) - (tp->rcv_nxt + tp->rcv_wnd));
   if (todrop > 0) 
   {
      TCP_STAT_INC(tcps_rcvpackafterwin);
      if (todrop >= (int)pip->ip_len) 
      {
         TCP_STAT_ADD(tcps_rcvbyteafterwin, pip->ip_len);
         /*
          * If a new connection request is received
          * while in TIME_WAIT, drop the old connection
          * and start over if the sequence numbers
          * are above the previous ones.
          */
         if (tiflags & TH_SYN &&
             tp->t_state == TCPS_TIME_WAIT &&
             SEQ_GT(ptcp->th_seq, tp->rcv_nxt)) 
         {
            iss = (tcp_seq)(tp->rcv_nxt + (TCP_ISSINCR));
            if (iss & 0xff000000)
            {
               dtrap();    /* tmp, remove later???? -JB */
               iss = 0L;
            }
            m_tcpclose(tp);
            goto findpcb;
         }
         /*
          * If window is closed can only take segments at
          * window edge, and have to drop data and PUSH from
          * incoming segments.  Continue processing, but
          * remember to ack.  Otherwise, drop segment
          * and ack.
          */
         if (tp->rcv_wnd == 0 && ptcp->th_seq == tp->rcv_nxt) 
         {
            tp->t_flags |= TF_ACKNOW;
            TCP_STAT_INC(tcps_rcvwinprobe);
         } else
            goto dropafterack;
      } else
         TCP_STAT_ADD(tcps_rcvbyteafterwin, todrop);

      pip->ip_len -= (u_short)todrop;  /* trim data from end of packet */
      tiflags &= ~(TH_PUSH|TH_FIN);
   }

   /*
    * If the RST bit is set examine the state:
    *    SYN_RECEIVED STATE:
    *   If passive open, return to LISTEN state.
    *   If active open, inform user that connection was refused.
    *    ESTABLISHED, FIN_WAIT_1, FIN_WAIT2, CLOSE_WAIT STATES:
    *   Inform user that connection was reset, and close tcb.
    *    CLOSING, LAST_ACK, TIME_WAIT STATES
    *   Close the tcb.
    */

#ifdef DOS_RST
   /* DOS_RST - Fix for "Denial of Service (DOS) using RST"
    * An intruder can send RST packet to break on existing TCP
    * connection. It means that if a RST is received in
    * ESTABLISHED state from an intruder, then the connection gets
    * closed between the original peers. To overcome this 
    * vulnerability, it is suggested that we accept RST only when
    * the sequence numbers match. Else we send an ACK.
    */
   if ((tiflags & TH_RST) && (tp->t_state == TCPS_ESTABLISHED) &&
      (ti->ti_seq != tp->rcv_nxt))
   {
      /* RST received in established state and sequence numbers
       * don't match.
       */
      tiflags &= ~TH_RST;  /* clear reset flag */
      goto dropafterack;   /* send an ack and drop current packet */
   }
#endif /* DOS_RST */

   if (tiflags & TH_RST)
   {
      switch (tp->t_state) 
      {
   
      case TCPS_SYN_RECEIVED:
         so->error = ECONNREFUSED;
         goto close;
   
      case TCPS_ESTABLISHED:
         TCP_MIB_INC(tcpEstabResets);     /* keep MIB stats */
      case TCPS_FIN_WAIT_1:
      case TCPS_FIN_WAIT_2:
      case TCPS_CLOSE_WAIT:
         so->error = ECONNRESET;
         close:
         tp->t_state = TCPS_CLOSED;
         TCP_STAT_INC(tcps_drops);
         m_tcpclose(tp);
         if (so->callback)
            so->callback(M_CLOSED, so, NULL);
         GOTO_DROP;
   
      case TCPS_CLOSING:
      case TCPS_LAST_ACK:
      case TCPS_TIME_WAIT:
         m_tcpclose(tp);
         GOTO_DROP;
      }
   }

   /*
    * If a SYN is in the window, then this is an
    * error and we send an RST and drop the connection.
    */

#ifdef DOS_SYN
   if ((tiflags & TH_SYN) && (tp->t_state == TCPS_ESTABLISHED))
   {
      /* DOS_SYN - Fix for "Denial of Service (DOS) attack using SYN"
       * An intruder can send SYN to reset the connection.
       * Hence normal behaviour can cause a vulnerability.
       * To protect against this attack, just ignore the SYN packet
       * when we are in established state.
       * One solution is to send an ACK.
       * But our guess is that just dropping the SYN should also
       * work fine.
       */
      GOTO_DROP;
   }
#else
   if (tiflags & TH_SYN) 
   {
      m_tcpdrop(tp, ECONNRESET);
      GOTO_DROPWITHRESET;
   }
#endif /* end of else of DOS_SYN */

   /*
    * If the ACK bit is off we drop the segment and return.
    */
   if ((tiflags & TH_ACK) == 0)
      GOTO_DROP;

   /*
    * Ack processing.
    */
   switch (tp->t_state) 
   {

   /*
    * In SYN_RECEIVED state if the ack ACKs our SYN then enter
    * ESTABLISHED state and continue processing, otherwise
    * send an RST.
    */
   case TCPS_SYN_RECEIVED:
      if (SEQ_GT(tp->snd_una, ptcp->th_ack) ||
          SEQ_GT(ptcp->th_ack, tp->snd_max))
      {
         TCP_MIB_INC(tcpEstabResets);     /* keep MIB stats */
         GOTO_DROPWITHRESET;
      }
      TCP_STAT_INC(tcps_connects);
      TCP_MIB_INC(tcpPassiveOpens);     /* keep MIB stats */
      tp->t_state = TCPS_ESTABLISHED;
      m_connected(so);
      tp->t_maxseg = MIN(tp->t_maxseg, TCP_MSS);
      tp->snd_wl1 = ptcp->th_seq - 1;
      /* fall into ... */

   /*
    * In ESTABLISHED state: drop duplicate ACKs; ACK out of range
    * ACKs.  If the ack is in the range
    *   tp->snd_una < ptcp->th_ack <= tp->snd_max
    * then advance tp->snd_una to ptcp->th_ack and drop
    * data from the retransmission queue.  If this ACK reflects
    * more up to date window information we update our window information.
    */
   case TCPS_ESTABLISHED:
   case TCPS_FIN_WAIT_1:
   case TCPS_FIN_WAIT_2:
   case TCPS_CLOSE_WAIT:
   case TCPS_CLOSING:
   case TCPS_LAST_ACK:
   case TCPS_TIME_WAIT:

      if (SEQ_LEQ(ptcp->th_ack, tp->snd_una)) 
      {
         if (pip->ip_len == 0 && ptcp->th_win == tp->snd_wnd) 
         {
            TCP_STAT_INC(tcps_rcvdupack);
            /*
             * If we have outstanding data (not a
             * window probe), this is a completely
             * duplicate ack (ie, window info didn't
             * change), the ack is the biggest we've
             * seen and we've seen exactly our rexmt
             * threshhold of them, assume a packet
             * has been dropped and retransmit it.
             * Kludge snd_nxt & the congestion
             * window so we send only this one
             * packet.  If this packet fills the
             * only hole in the receiver's seq.
             * space, the next real ack will fully
             * open our window.  This means we
             * have to do the usual slow-start to
             * not overwhelm an intermediate gateway
             * with a burst of packets.  Leave
             * here with the congestion window set
             * to allow 2 packets on the next real
             * ack and the exp-to-linear thresh
             * set for half the current window
             * size (since we know we're losing at
             * the current window size).
             */
            if (tp->t_timer[TCPT_REXMT] == 0 ||
                ptcp->th_ack != tp->snd_una)
            {
               tp->t_dupacks = 0;
            }
            else if (++tp->t_dupacks == tcprexmtthresh) 
            {
               tcp_seq onxt = tp->snd_nxt;
               u_short  win   =
               MIN(tp->snd_wnd, tp->snd_cwnd) / 2 /
               tp->t_maxseg;

               if (win < 2)
                  win = 2;
               tp->snd_ssthresh = (u_short)(win * tp->t_maxseg);

               tp->t_timer[TCPT_REXMT] = 0;
               tp->t_rtt = 0;
               tp->snd_nxt = ptcp->th_ack;
               tp->snd_cwnd = tp->t_maxseg;
               (void) tcp_output(tp);

               if (SEQ_GT(onxt, tp->snd_nxt))
                  tp->snd_nxt = onxt;
               GOTO_DROP;
            }
         } else
            tp->t_dupacks = 0;
         break;
      }
      tp->t_dupacks = 0;
      if (SEQ_GT(ptcp->th_ack, tp->snd_max)) 
      {
         TCP_STAT_INC(tcps_rcvacktoomuch);
         goto dropafterack;
      }
      acked = (int)(ptcp->th_ack - tp->snd_una);
      TCP_STAT_INC(tcps_rcvackpack);
      TCP_STAT_ADD(tcps_rcvackbyte, acked);

      /*
       * If transmit timer is running and timed sequence
       * number was acked, update smoothed round trip time.
       * Since we now have an rtt measurement, cancel the
       * timer backoff (cf., Phil Karn's retransmit alg.).
       * Recompute the initial retransmit timer.
       */
      if (tp->t_rtt && SEQ_GT(ptcp->th_ack, tp->t_rtseq))
         tcp_xmit_timer(tp);

      /*
       * If all outstanding data is acked, stop retransmit
       * timer and remember to restart (more output or persist).
       * If there is more data to be acked, restart retransmit
       * timer, using current (possibly backed-off) value.
       */
      if (ptcp->th_ack == tp->snd_max) 
      {
         tp->t_timer[TCPT_REXMT] = 0;
         needoutput = 1;
      } else if (tp->t_timer[TCPT_PERSIST] == 0)
         tp->t_timer[TCPT_REXMT] = tp->t_rxtcur;
      /*
       * When new data is acked, open the congestion window.
       * If the window gives us less than ssthresh packets
       * in flight, open exponentially (maxseg per packet).
       * Otherwise open linearly (maxseg per window,
       * or maxseg^2 / cwnd per packet).
       */
      {
         u_short  cw =  tp->snd_cwnd;
         u_short  incr  =  tp->t_maxseg;

         if (cw > tp->snd_ssthresh)
            incr = MAX( (incr * incr / cw), (4 << 2) );

         tp->snd_cwnd = MIN(cw + (u_short)incr, (IP_MAXPACKET));
      }

      /* If he acked more than we have sent assume he's acking a FIN.
       * Note that we can also get into this when receiving ack of syn/ack
       * packet, but that should be harmless.
       */
      if (acked > (int)so->sendq.sb_cc)
      {
         tp->snd_wnd -= (u_short)so->sendq.sb_cc;
         m_sbdrop(&so->sendq, so->sendq.sb_cc);    /* drop everything */
         ourfinisacked = 1;                  /* set local flag */
      }
      else  /* acked some data */
      {
         m_sbdrop(&so->sendq, acked);        /* drop amount he acked */
         tp->snd_wnd -= (u_short)acked;
         ourfinisacked = 0;
         /* callback app in case it's sleeping on send */
         if(so->callback)
            so->callback(M_TXDATA, so, NULL);
      }

      /* wake up anybody sleeping on a send */
      if((tp->t_state == TCPS_ESTABLISHED) &&
         (acked > 0))
      {
         tcp_wakeup(&so->sendq);
      }

      /* update variable for our next seq number from rcvd ack */
      tp->snd_una = ptcp->th_ack;
      if (SEQ_LT(tp->snd_nxt, tp->snd_una))
         tp->snd_nxt = tp->snd_una;

      switch (tp->t_state) 
      {

      /*
       * In FIN_WAIT_1 STATE in addition to the processing
       * for the ESTABLISHED state if our FIN is now acknowledged
       * then enter FIN_WAIT_2.
       */
      case TCPS_FIN_WAIT_1:
         if (ourfinisacked) 
         {
            /*
             * If we can't receive any more
             * data, then closing user can proceed.
             * Starting the timer is contrary to the
             * specification, but if we don't get a FIN
             * we'll hang forever.
             */
            if (so->state & SS_CANTRCVMORE) 
            {
               m_disconnected(so);
               tp->t_timer[TCPT_2MSL] = tcp_maxidle;
            }
            tp->t_state = TCPS_FIN_WAIT_2;
         }
         break;

       /*
       * In CLOSING STATE in addition to the processing for
       * the ESTABLISHED state if the ACK acknowledges our FIN
       * then enter the TIME-WAIT state, otherwise ignore
       * the segment.
       */
      case TCPS_CLOSING:
         if (ourfinisacked) 
         {
            tp->t_state = TCPS_TIME_WAIT;
            tcp_canceltimers(tp);
            tp->t_timer[TCPT_2MSL] = 2 * TCPTV_MSL;
            m_disconnected(so);
         }
         break;

      /*
       * In LAST_ACK, we may still be waiting for data to drain
       * and/or to be acked, as well as for the ack of our FIN.
       * If our FIN is now acknowledged, delete the TCB,
       * enter the closed state and return.
       */
      case TCPS_LAST_ACK:
         if (ourfinisacked) 
         {
            m_tcpclose(tp);
            GOTO_DROP;
         }
         break;

      /*
       * In TIME_WAIT state the only thing that should arrive
       * is a retransmission of the remote FIN.  Acknowledge
       * it and restart the finack timer.
       */
      case TCPS_TIME_WAIT:
         tp->t_timer[TCPT_2MSL] = 2 * TCPTV_MSL;
         goto dropafterack;
      }
   }

step6:
   /*
    * Update window information.
    * Don't look at window if no ACK: TAC's send garbage on first SYN.
    */
   if ((tiflags & TH_ACK) &&
       (SEQ_LT(tp->snd_wl1, ptcp->th_seq) || tp->snd_wl1 == ptcp->th_seq &&
       (SEQ_LT(tp->snd_wl2, ptcp->th_ack) ||
       tp->snd_wl2 == ptcp->th_ack && ptcp->th_win > tp->snd_wnd))) 
   {
      /* keep track of pure window updates */
      if (pip->ip_len == 0 &&
          tp->snd_wl2 == ptcp->th_ack && ptcp->th_win > tp->snd_wnd)
      {
         TCP_STAT_INC(tcps_rcvwinupd);
      }
      tp->snd_wnd = ptcp->th_win;
      tp->snd_wl1 = ptcp->th_seq;
      tp->snd_wl2 = ptcp->th_ack;
      if (tp->snd_wnd > tp->max_sndwnd)
         tp->max_sndwnd = tp->snd_wnd;
      needoutput = 1;
   }

   /*
    * pull receive urgent pointer along
    * with the receive window.
    */
   if (SEQ_GT(tp->rcv_nxt, tp->rcv_up))
      tp->rcv_up = tp->rcv_nxt;

   /*
    * Process the segment text, merging it into the TCP sequencing queue,
    * and arranging for acknowledgment of receipt if necessary. If a FIN 
    * has already been received on this connection then we just ignore 
    * the data.
    *
    * NOTE: this step also involves moving the received packet to a socket
    * queue or back into the free queue, so the pkt should not be 
    * referenced after this.
    */
   if ((pkt->m_len || (tiflags & TH_FIN)) && 
      (TCPS_HAVERCVDFIN(tp->t_state) == 0))
   {
      /* put data packets in the received pkt que */
      if(pkt->m_len)
         tiflags = m_tcpreass(tp, ptcp, pkt);
      else 
         tcp_pktfree(pkt);
 
      /*
       * Note the amount of data that peer has sent into
       * our window, in order to estimate the sender's
       * buffer size.
       */
      len = (int)(mt_defrxwin - (tp->rcv_adv - tp->rcv_nxt));
      if (len > (int)tp->max_rcvd)
         tp->max_rcvd = (u_short)len;
      if (so->callback)
      {
         m_data_upcall(so);
         /* if we have FIN and app has all data, do shutdown */
         if ((tiflags & TH_FIN) 
#ifdef MINI_TCP_OOSQ             
             && (!so->oosq.p_head)
#endif
             )
         {
            so->error = ESHUTDOWN;
            /* if sock has callback, do close notify */
            if(so->callback)
               so->callback(M_CLOSED, so, NULL);
            so->state |= SS_UPCFIN;    /* flag that upcall was FINed */
         }
      }

   } 
   else
   {
      tcp_pktfree(pkt);
      tiflags &= ~TH_FIN;
   }

   /*
    * If FIN is received ACK the FIN and let the user know
    * that the connection is closing.
    */
   if ((tiflags & TH_FIN) 
#ifdef MINI_TCP_OOSQ
       && (!so->oosq.p_head)
#endif
       )
   {
      if (TCPS_HAVERCVDFIN(tp->t_state) == 0) 
      {
         so->state |= SS_CANTRCVMORE;
         tp->t_flags |= TF_ACKNOW;
         tp->rcv_nxt++;
      }
      switch (tp->t_state) 
      {
      /*
       * In SYN_RECEIVED and ESTABLISHED STATES
       * enter the CLOSE_WAIT state.
       */
      case TCPS_SYN_RECEIVED:
      case TCPS_ESTABLISHED:
         tp->t_state = TCPS_CLOSE_WAIT;
         break;

       /*
       * If still in FIN_WAIT_1 STATE FIN has not been acked so
       * enter the CLOSING state.
       */
      case TCPS_FIN_WAIT_1:
         tp->t_state = TCPS_CLOSING;
         break;

       /*
       * In FIN_WAIT_2 state enter the TIME_WAIT state,
       * starting the time-wait timer, turning off the other 
       * standard timers.
       */
      case TCPS_FIN_WAIT_2:
         tp->t_state = TCPS_TIME_WAIT;
         tcp_canceltimers(tp);
         tp->t_timer[TCPT_2MSL] = 2 * TCPTV_MSL;
         so->state &= ~(SS_ISCONNECTING|SS_ISCONNECTED|SS_ISDISCONNECTING);
         so->state |= (SS_CANTRCVMORE|SS_CANTSENDMORE);
         tcp_wakeup ((char *)so);
         break;

      /*
       * In TIME_WAIT state restart the 2 MSL time_wait timer.
       */
      case TCPS_TIME_WAIT:
         tp->t_timer[TCPT_2MSL] = 2 * TCPTV_MSL;
         break;
      }
   }
#ifdef DO_TCPTRACE
   if (so->so_options & SO_DEBUG)
      tcp_trace("TCP_IN: state: %d, tcpcb: %x saveti: %x", 
    ostate, tp, &tcp_saveti);
#endif

   /* see if we need to do a connect upcall */
   if((tp->t_flags & TF_OPENUP) &&       /* flag is set */
      (so->callback != NULL))             /* callback exists */
   {
      tp->t_flags &= ~TF_OPENUP;          /* clear flag */
      so->callback(M_OPENOK, so, NULL);   /* notify application */
      /* wake any task which is blocked on connect */
      tcp_wakeup(so);
   }
    
   /* Return any desired output. */
   if (needoutput || (tp->t_flags & TF_ACKNOW))
      (void) tcp_output(tp);
   return SUCCESS;

dropafterack:
   /*
    * Generate an ACK dropping incoming segment if it occupies
    * sequence space, where the ACK reflects our state.
    */
   if (tiflags & TH_RST)
      GOTO_DROP;
   tcp_pktfree(pkt);
   tp->t_flags |= TF_ACKNOW;
   (void) tcp_output (tp);
   return SUCCESS;

dropwithreset:

   TCP_MIB_INC(tcpInErrs);    /* keep MIB stats */

   /*
    * Generate a RST, dropping incoming segment.
    * Make ACK acceptable to originator of segment.
    * Don't bother to respond if destination was broadcast.
    */
   if (tiflags & TH_RST)
      GOTO_DROP;

   mask = ~pkt->net->snmask;           /* set mask for broadcast detection */
   if((pip->ip_dest & mask) == mask)   /* don't reset broadcasts */
      GOTO_DROP;

   if(tp == NULL || so == NULL)
      GOTO_DROP;

   /* If socket is in LISTEN state template is not set */
   if(tp->t_template == NULL)
      GOTO_DROP;

   if (tiflags & TH_SYN)
      tp->snd_nxt++;

   tp->t_flags |= TF_SENDRST; /* force a reset */
   tcp_output(tp);
   TCP_MIB_INC(tcpOutRsts);

drop:
   tcp_pktfree(pkt);

   /* destroy temporarily created socket */
   if (dropsocket)
      m_delsocket(so);
   return SUCCESS;
}



/* FUNCTION: tcp_xmit_timer()
 * 
 * PARAM1: struct tcpcb * tp
 *
 * RETURNS: 
 */

void
tcp_xmit_timer(struct tcpcb * tp)
{
   short delta;

   TCP_STAT_INC(tcps_rttupdated);
   if (tp->t_srtt != 0)
   {

      /*
       * srtt is stored as fixed point with 3 bits
       * after the binary point (i.e., scaled by 8).
       * The following magic is equivalent
       * to the smoothing algorithm in rfc793
       * with an alpha of .875
       * (srtt = rtt/8 + srtt*7/8 in fixed point).
       * Adjust t_rtt to origin 0.
       */
      delta = (short)(tp->t_rtt - 1 - (tp->t_srtt >> 3));
      if ((tp->t_srtt += delta) <= 0)
         tp->t_srtt = 1;
      /*
       * We accumulate a smoothed rtt variance
       * (actually, a smoothed mean difference),
       * then set the retransmit timer to smoothed
       * rtt + 2 times the smoothed variance.
       * rttvar is stored as fixed point
       * with 2 bits after the binary point
       * (scaled by 4).  The following is equivalent
       * to rfc793 smoothing with an alpha of .75
       * (rttvar = rttvar*3/4 + |delta| / 4).
       * This replaces rfc793's wired-in beta.
       */
      if (delta < 0)
         delta = -delta;
      delta -= (short)(tp->t_rttvar >> 2);
      if ((tp->t_rttvar += delta) <= 0)
         tp->t_rttvar = 1;
   }
   else 
   {
      /* 
       * No rtt measurement yet - use the
       * unsmoothed rtt.  Set the variance
       * to half the rtt (so our first
       * retransmit happens at 2*rtt)
       */
      tp->t_srtt = tp->t_rtt << 3;
      tp->t_rttvar = tp->t_rtt << 1;
   }
   tp->t_rtt = 0;
   tp->t_rxtshift = 0;
   TCPT_RANGESET(tp->t_rxtcur, 
    (short)(((tp->t_srtt >> 2) + tp->t_rttvar) >> 1),
    TCPTV_MIN, TCPTV_REXMTMAX);
}

/* end of file tcp_in.c */


