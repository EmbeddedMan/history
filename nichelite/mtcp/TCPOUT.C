/*
 * FILENAME: tcp_out.c
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
 * ROUTINES: tcp_output(), tcp_send(),
 *
 * PORTABLE: yes
 */

#define TCPOUTFLAGS 1

#include "license.h"
#include "ipport.h"
#include "mtcp.h"

/*
 * Initial options.
 */
u_char   tcp_initopt[4] =  {  TCPOPT_MAXSEG, 4, 0x0,  0x0,  };

/* Size limits on socket buffer data. If a small number of bigbufs per
 * socket is used, these defaults should be adjusted downward by the port's 
 * initialization code.
 */
unsigned   mt_deftxwin = 1024 * 8;      /* default send window */
unsigned   mt_defrxwin = 1024 * 8;      /* default receive window */

/* Flags used when sending segments in tcp_output.
 * Basic flags (TH_RST,TH_ACK,TH_SYN,TH_FIN) are totally
 * determined by state, with the proviso that TH_FIN is sent only
 * if all data queued for output is included in the segment.
 */
u_char   tcp_outflags[TCP_NSTATES]  =
{
   TH_RST|TH_ACK, 0, TH_SYN, TH_SYN|TH_ACK,
   TH_ACK, TH_ACK,
   TH_FIN|TH_ACK, TH_FIN|TH_ACK, TH_FIN|TH_ACK, TH_ACK, TH_ACK,
};

/* FUNCTION: tcp_output()
 *
 * Tcp output routine: figure out what should be sent and send it.
 *
 * PARAM1: struct tcpcb *tp
 *
 * RETURNS: 0 if OK, else BSD sockets error code.
 */

int
tcp_output(struct tcpcb * tp)
{
   struct msocket * so = (struct msocket *)tp->t_inpcb;
   int      len;     /* length of data to try to send */
   int      win;     /* scratch wind holder */
   int      off;     /* 0ffset (in bytes) into sendq queue of first unsent data */
   int      flags, error;  /* scratch */
   struct ip * pip;        /* pointer to IP header to send */
   struct tcphdr * ptcp;   /* tcp header to send */
   u_char * opt;
   unsigned optlen = 0;
   int      idle, sendalot;
   PACKET   sendp;

   /*
    * Determine length of data that should be transmitted,
    * and flags that will be used.
    * If there is some data or critical controls (SYN, RST)
    * to send, then transmit; otherwise, investigate further.
    */
   idle = (tp->snd_max == tp->snd_una);
again:
   sendalot = 0;
   off = (int)(tp->snd_nxt - tp->snd_una);

   /* figure out other guys window */
   win = (int)tp->snd_wnd;
   if (win > (long)tp->snd_cwnd) /* see if we need congestion control */
   {
      if (tp->snd_cwnd < tp->t_maxseg)
         dprintf("tcp_out: congested; snd_cwnd %u\n", tp->snd_cwnd);
      win = (int)(tp->snd_cwnd & ~(3)); /* keep data aligned */
   }

   len = (int)MIN(so->sendq.sb_cc, (unsigned)win) - off;

#ifdef NPDEBUG
   if(off < 0) { dtrap(); }
#endif

   flags = tcp_outflags[tp->t_state];

   /* see if we want to reset peer */
   if(tp->t_flags & TF_SENDRST)
   {
      flags |= TH_RST;
      len = 0;
   }

   /*
    * Before ESTABLISHED, force sending of initial options
    * unless TCP set to not do any options.
    */
   opt = NULL;
   if (flags & TH_SYN && ((tp->t_flags & TF_NOOPT) == 0))
   {
      opt = tcp_initopt;   /* always send MSS option! */
      optlen = sizeof (tcp_initopt);
      opt[2] = (u_char) ((TCP_MSS & 0xff00) >> 8);
      opt[3] = (u_char) (TCP_MSS & 0xff);
   }

   if (len < 0) 
   {
      /*
       * If FIN has been sent but not acked,
       * but we haven't been called to retransmit,
       * len will be -1.  Otherwise, window shrank
       * after we sent into it.  If window shrank to 0,
       * cancel pending retransmit and pull snd_nxt
       * back to (closed) window.  We will enter persist
       * state below.  If the window didn't close completely,
       * just wait for an ACK.
       */
      len = 0;
      if (win == 0) 
      {
         tp->t_timer[TCPT_REXMT] = 0;
         tp->snd_nxt = tp->snd_una;
      }
   }
   if (len > (int)tp->t_maxseg)
   {
      len = tp->t_maxseg;
      sendalot = 1;
   }
   if (SEQ_LT(tp->snd_nxt + len, tp->snd_una + so->sendq.sb_cc))
      flags &= ~TH_FIN;

   /* figure out what OUR advertised window should be */
   win = (int)(mt_defrxwin - so->rcvdq.sb_cc);

   /*
    * If our state indicates that FIN should be sent
    * and we have not yet done so, or we're retransmitting the FIN,
    * then we need to send.
    */
   if ((flags & TH_FIN) &&
       (so->sendq.sb_cc == 0) &&
       ((tp->t_flags & TF_SENTFIN) == 0 || tp->snd_nxt == tp->snd_una))
   {
      goto send;
   }

   /*
    * Send if we owe peer an ACK.
    */
   if (tp->t_flags & (TF_ACKNOW | TF_SENDKEEP))
      goto send;
   if (flags & (TH_SYN|TH_RST))
      goto send;
   if (SEQ_GT(tp->snd_up, tp->snd_una))
      goto send;

   /*
    * Sender silly window avoidance.  If connection is idle
    * and can send all data, a maximum segment,
    * at least a maximum default-size segment do it,
    * or are forced, do it; otherwise don't bother.
    * If peer's buffer is tiny, then send
    * when window is at least half open.
    * If retransmitting (possibly after persist timer forced us
    * to send into a small window), then must resend.
    */
   if (len)
   {
      if (len == (int)tp->t_maxseg)
         goto send;
      if ((idle || tp->t_flags & TF_NODELAY) &&
          len + off >= (int)so->sendq.sb_cc)
      {
         goto send;
      }
      if (tp->t_force)
         goto send;
      if (len >= (int)(tp->max_sndwnd / 2))
         goto send;
      if (SEQ_LT(tp->snd_nxt, tp->snd_max))
         goto send;
   }

   /*
    * Compare available window to amount of window
    * known to peer (as advertised window less
    * next expected input).  If the difference is at least two
    * max size segments or at least 35% of the maximum possible
    * window, then want to send a window update to peer.
    */
   if (win > 0)
   {
      int   adv = (int)win - (int)(tp->rcv_adv - tp->rcv_nxt);

      if (so->sendq.sb_cc == 0 && adv >= (int)(tp->t_maxseg * 2))
         goto send;
      if ((100 * (unsigned)adv / mt_defrxwin) >= 35)
         goto send;
   }

   /*
    * TCP window updates are not reliable, rather a polling protocol
    * using ``persist'' packets is used to insure receipt of window
    * updates.  The three ``states'' for the output side are:
    *   idle         not doing retransmits or persists
    *   persisting      to move a small or zero window
    *   (re)transmitting   and thereby not persisting
    *
    * tp->t_timer[TCPT_PERSIST]
    *   is set when we are in persist state.
    * tp->t_force
    *   is set when we are called to send a persist packet.
    * tp->t_timer[TCPT_REXMT]
    *   is set when we are retransmitting
    * The output side is idle when both timers are zero.
    *
    * If send window is too small, there is data to transmit, and no
    * retransmit or persist is pending, then go to persist state.
    * If nothing happens soon, send when timer expires:
    * if window is nonzero, transmit what we can,
    * otherwise force out a byte.
    */
   if (so->sendq.sb_cc && tp->t_timer[TCPT_REXMT] == 0 &&
       tp->t_timer[TCPT_PERSIST] == 0) 
   {
      tp->t_rxtshift = 0;
      tcp_setpersist(tp);
   }

   /*
    * No reason to send a segment, just return.
    */
   return (0);

send:
   ENTER_CRIT_SECTION(tp);

   /* Limit send length to the current buffer */
   if (len)
   {
      int   bufoff = off;

      sendp = (PACKET)so->sendq.p_head;

      /* find packet containing data to send (at "off") */
      while (sendp)  /* loop through socket send list */
      {
         if(bufoff <= 0)
            break;   /* off is in this buffer, break */

         bufoff -= sendp->m_len;
         sendp = sendp->m_next;
      }
      if (!sendp)
      { 
         if(bufoff == 0)  /* all data in queue is sent? */
         {
            len = 0;
            goto sentall;
         }
         else
         {
            dtrap();  /* shouldn't happen */
            EXIT_CRIT_SECTION(tp);
            return EINVAL;
         }
      }
#ifdef NPDEBUG
      if (bufoff != 0) { dtrap();  /* shouldn't happen */ }
#endif   /* NPDEBUG */

      /* if socket has multiple unsent pkts, set flag for send to loop */
      if ((so->sendq.p_head) && (len > (int)sendp->m_len))
      {
         flags &= ~TH_FIN; /* don't FIN on segment prior to last */
         sendalot = 1;     /* set to send more segments */
      }
      if((flags & TH_FIN) && (so->sendq.sb_cc > (unsigned)len))
      {
         /* This can happen on slow links (PPP) which retry the last 
          * segment - the one with the FIN bit attached to data.
          */
         flags &= ~TH_FIN; /* don't FIN on segment prior to last */
      }

      /* limit send to this packet's data */
      if(len > (int)sendp->m_len)
         len = (int)sendp->m_len;
   }
   else  /* no data to send, get an empty packet */
   {
sentall:
      /* big enough for headers and options */
      sendp = tcp_pktalloc(MaxLnh + TCPIPHDR_SIZE + optlen);
      if(sendp == NULL)
      {
         EXIT_CRIT_SECTION(tp);
         return ENOBUFS;
      }
   }
   EXIT_CRIT_SECTION(tp);

   /* set pointers to TCP and IP headers */
   pip = (struct ip *)(sendp->nb_buff + MaxLnh);
   ptcp = (struct tcphdr *)(pip + 1);

   /* Don't send partial packets. tcp_send() should have made sure
    * that the packets all fit into the other guy's TCP_MSS. If
    * his window is smaller than both our send size and his MSS, treat it
    * as a silly window and send no data.
    */
   if(len < (int)sendp->m_len)
   {
      if(!tp->t_force)
         len = (int)sendp->m_len; /* send whole packet as window probe */
      else
         len = 0;    /* else don't send any data */
   }

#ifdef NPDEBUG
   if (len) 
   {
      if (tp->t_force)
      {
         TCP_STAT_INC(tcps_sndprobe);
      }
      else if (SEQ_LT(tp->snd_nxt, tp->snd_max)) 
      {
         TCP_STAT_INC(tcps_sndrexmitpack);
         TCP_STAT_ADD(tcps_sndrexmitbyte, len);
      } 
      else 
      {
         TCP_STAT_INC(tcps_sndpack);
         TCP_STAT_ADD(tcps_sndbyte, len);
      }
   }
   else if (tp->t_flags & TF_ACKNOW)
      TCP_STAT_INC(tcps_sndacks);
   else if (flags & (TH_SYN|TH_FIN|TH_RST))
      TCP_STAT_INC(tcps_sndctrl);
   else if (SEQ_GT(tp->snd_up, tp->snd_una))
      TCP_STAT_INC(tcps_sndurg);
   else
      TCP_STAT_INC(tcps_sndwinup);
#endif /* NET_STATS */

   /* prepend TCP/IP header in the space provided. */
   MEMCPY((char*)pip, (char*)tp->t_template, TCPIPHDR_SIZE);

   /*
    * Fill in fields, remembering maximum advertised
    * window for use in delaying messages about window sizes.
    * If resending a FIN, be sure not to use a new sequence number.
    */
   if (flags & TH_FIN && tp->t_flags & TF_SENTFIN && 
       tp->snd_nxt == tp->snd_max)
   {
      tp->snd_nxt--;
   }

   /* send tcp packets seq & ack fields */
   if((tp->t_flags & TF_SENDKEEP) && (len == 0))
   {
      /* set seq for a BSD-ish keepalive */
      ptcp->th_seq = (tp->snd_nxt);
   }
   else  /* not a keepalive */
   {
      ptcp->th_seq = (tp->snd_nxt);
   }
   ptcp->th_ack = (tp->rcv_nxt);

   /*
    * If we're sending a SYN, check the IP address of the interface
    * that we will (likely) use to send the IP datagram -- if it's
    * changed from what is in the template (as it might if this is
    * a retransmission, and the original SYN caused PPP to start
    * bringing the interface up, and PPP has got a new IP address
    * via IPCP), update the template and the inpcb with the new 
    * address.
    */
   if (flags & TH_SYN)
   {
      ip_addr src = ip_mymach(pip->ip_src);

      if (src != pip->ip_src)
      {
         pip->ip_src = src;
         tp->t_template->ti_i.ip_src = src;
         so->lhost = src;
      }
   }

   /* fill in any required options. Since we only use the MSS option 
    * we never have to send a packet with both options and data. We
    * take advantage of this and put the options in the tcp data area
    * pointed to by datap. 
    */
   if (opt) 
   {
      if(len)
      {
         dtrap();
      }
      MEMCPY(sendp->m_data, opt, optlen);   /* copy in options */
      sendp->m_data += optlen;

      /* use portable macro to set tcp data offset bits */
      SET_TH_OFF((*ptcp), ((sizeof (struct tcphdr) + optlen) >> 2));
   }

   ptcp->th_flags = (u_char)flags;
   /*
    * Calculate receive window.  Don't shrink window,
    * but avoid silly window syndrome.
    */
   if (win < (long)(tp->rcv_adv - tp->rcv_nxt))
      win = (int)(tp->rcv_adv - tp->rcv_nxt);
   if (win > IP_MAXPACKET)
      win = IP_MAXPACKET;

   /* do check for Iniche buffer limits -JB- */
   if (bigfreeq.q_len == 0)   /* If queue length is 0, set window to 0 */
   {
      win = 0;
   }
   else if(win > (((long)bigfreeq.q_len - 1) * (long)bigbufsiz))
   {
      win = ((int)bigfreeq.q_len - 1) * bigbufsiz;
   }

   ptcp->th_win = ((u_short)win);
   if (SEQ_GT(tp->snd_up, tp->snd_nxt)) 
   {
      ptcp->th_urp = ((u_short)(tp->snd_up - tp->snd_nxt));
      ptcp->th_flags |= TH_URG;
   }
   else
   {
      /*
       * If no urgent pointer to send, then we pull
       * the urgent pointer to the left edge of the send window
       * so that it doesn't drift into the send window on sequence
       * number wraparound.
       */
      tp->snd_up = tp->snd_una;        /* drag it along */
   }

   /*
    * If anything to send and we can send it all, set PUSH.
    * (This will keep happy those implementations which only
    * give data to the user when a buffer fills or a PUSH comes in.)
    */
   if (len && ((off + len) == (int)so->sendq.sb_cc))
      ptcp->th_flags |= TH_PUSH;

   /*
    * In transmit state, time the transmission and arrange for
    * the retransmit.  In persist state, just set snd_max.
    */
   if (tp->t_force == 0 || tp->t_timer[TCPT_PERSIST] == 0) 
   {
      tcp_seq startseq = tp->snd_nxt;

      /*
       * Advance snd_nxt over sequence space of this segment.
       */
      if (flags & TH_SYN)
         tp->snd_nxt++;

      if (flags & TH_FIN)
      {
         tp->snd_nxt++;
         tp->t_flags |= TF_SENTFIN;
      }
      tp->snd_nxt += len;
      if (SEQ_GT(tp->snd_nxt, tp->snd_max)) 
      {
         tp->snd_max = tp->snd_nxt;
         /*
          * Time this transmission if not a retransmission and
          * not currently timing anything.
          */
         if (tp->t_rtt == 0) 
         {
            tp->t_rtt = 1;
            tp->t_rtseq = startseq;
            TCP_STAT_INC(tcps_segstimed);
         }
      }

      /*
       * Set retransmit timer if not currently set,
       * and not doing an ack or a keep-alive probe.
       * Initial value for retransmit timer is smoothed
       * round-trip time + 2 * round-trip time variance.
       * Initialize shift counter which is used for backoff
       * of retransmit time.
       */
      if (tp->t_timer[TCPT_REXMT] == 0 &&
          tp->snd_nxt != tp->snd_una) 
      {
         tp->t_timer[TCPT_REXMT] = tp->t_rxtcur;
         if (tp->t_timer[TCPT_PERSIST]) 
         {
            tp->t_timer[TCPT_PERSIST] = 0;
            tp->t_rxtshift = 0;
         }
      }
   }
   else if (SEQ_GT(tp->snd_nxt + len, tp->snd_max))
         tp->snd_max = tp->snd_nxt + len;

#ifdef DO_TCPTRACE
   tcp_trace("tcp_output: sending, state %d, tcpcb: %x",
    tp->t_state, tp );
#endif

   /* If packet has data then increment the use counter so that
    * the ip_write does not put the packet in the free queue. 
    */
   if(len > 0)
      sendp->inuse++;

   /* Set pkt pointers & lengths and send to IP level. */
   sendp->nb_prot = (char*)ptcp;
   sendp->nb_plen = (len + optlen + sizeof(struct tcphdr));

   /* fill in TCP checksum - first fixup IP header fields which 
    * figure into checksum. IP addrs are already done.
    */
   pip->ip_ver_ihl = 0x45;
   len = sendp->nb_plen + sizeof(struct ip); /* IP length */
   pip->ip_len = ((unshort)len);
   pip->ip_prot = TCPTP;
   ptcp->th_sum = tcp_cksum(pip);      /* fill in checksum */

   /* send the packet */
   sendp->fhost = pip->ip_dest;        /* pass fhost to IP layer */
   error = ip_write(TCPTP, sendp);     /* send to IP */

   if (error)
      return (error);

   TCP_MIB_INC(tcpOutSegs);   /* keep MIB stats */
   TCP_STAT_INC(tcps_sndtotal);

   /*
    * Data sent (as far as we can tell).
    * If this advertises a larger window than any other segment,
    * then remember the size of the advertised window.
    * Any pending ACK has now been sent.
    */
   if (win > 0 && SEQ_GT(tp->rcv_nxt+win, tp->rcv_adv))
      tp->rcv_adv = tp->rcv_nxt + (unsigned)win;
   tp->t_flags &= ~(TF_ACKNOW|TF_DELACK);
   if (sendalot)
      goto again;
   return (0);
}


/* FUNCTION: tcp_setpersist()
 * 
 * PARAM1: struct tcpcb *tp
 *
 * RETURNS: 
 */

void
tcp_setpersist(struct tcpcb * tp)
{
   int   t;

   t = ((tp->t_srtt >> 2) + tp->t_rttvar) >> 1;

   if (tp->t_timer[TCPT_REXMT])
      panic("tcp_output REXMT");

   /* Start/restart persistance timer. */
   TCPT_RANGESET(tp->t_timer[TCPT_PERSIST], (short)(t * tcp_backoff[tp->t_rxtshift]),
      TCPTV_PERSMIN, TCPTV_PERSMAX);

   if (tp->t_rxtshift < TCP_MAXRXTSHIFT)
      tp->t_rxtshift++;

}

/* end of file tcp_out.c */



