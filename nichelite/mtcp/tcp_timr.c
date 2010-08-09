/*
 * FILENAME: tcp_timr.c
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
 * ROUTINES: tcp_fasttimo(), tcp_slowtimo(), tcp_canceltimers(), 
 * ROUTINES: tcp_timers(),
 *
 * PORTABLE: yes
 */

#include "license.h"
#include "ipport.h"
#include "mtcp.h"


int      tcp_keepidle   =  TCPTV_KEEP_IDLE;
int      tcp_keepintvl  =  TCPTV_KEEPINTVL;
int      tcp_maxidle;

tcp_seq tcp_iss;

struct tcpcb * tcp_timers(struct tcpcb * tp, int timer); /* predecl */


#ifdef DO_DELAY_ACKS

/* FUNCTION: tcp_fasttimo()
 *
 * Fast timeout routine for processing delayed acks
 *
 * 
 * PARAM1: 
 *
 * RETURNS: 
 */

void
tcp_fasttimo()
{
   struct tcpcb * tp;
   M_SOCK   so;
   int      s;

   for(so = (M_SOCK)msoq.qhead; so; so = so->next)
   {
      tp = so->tp;
      if (tp->t_flags & TF_DELACK)
      {
         tp->t_flags &= ~TF_DELACK;
         tp->t_flags |= TF_ACKNOW;
         TCP_STAT_INC(tcps_delack);
         (void) tcp_output(tp);
      }
   }
}
#endif   /* DO_DELAY_ACKS */



/* FUNCTION: tcp_slowtimo()
 *
 * Tcp protocol timeout routine called every 500 ms.
 * Updates the timers in all active tcb's and
 * causes finite state machine actions if timers expire.
 *
 * 
 * PARAM1: 
 *
 * RETURNS: 
 */

void
tcp_slowtimo()
{
   struct tcpcb * tp;
   int   i;
   M_SOCK   so;
   M_SOCK   nextso;

   tcp_maxidle = TCPTV_KEEPCNT * tcp_keepintvl;
msoq_check();

   for(so = (M_SOCK)msoq.q_head; so; so = nextso)
   {
      tp = so->tp;      /* get tcpcb for this socket */
      nextso = so->next;

      /* if tcpcb is deleted... */
      if (tp == NULL)
      {
         /* if the socket has been closed, delete it */
         if (so->state & SS_NOFDREF)
            m_delsocket(so);
         continue;
      }

      /* if socket is in advanced state of shutdown but has no 
       * CLOSE timer, start one now.
       */
      if((so->tp->t_state > TCPS_FIN_WAIT_2) &&
         (tp->t_timer[TCPT_2MSL] == 0))
      {
         tp->t_timer[TCPT_2MSL] = 2 * TCPTV_MSL;
         continue;
      }

      for (i = 0; (i < TCPT_NTIMERS) && (tp == so->tp); i++) 
      {
         if (tp->t_timer[i] && --tp->t_timer[i] == 0) 
            tcp_timers(tp, i);
      }

      /* if the tcpcb has been closed, clean up the socket
       * and move to the next one
       */
      if (tp != so->tp)
      {
         m_delsocket(so);  /* get rid of zombie socket */
         continue;
      }

      /* also nudge sockets which may have missed an upcall. */
      if (so->callback)         /* If call back set... */
      {
         unsigned ready = so->rcvdq.sb_cc;
         /* If socket has data try to deliver it to app */
         if(ready > 0)
         {
            m_data_upcall(so);         /* show data to app */
            if(ready != so->rcvdq.sb_cc)   /* did app accept any data? */
               tcp_output(tp);         /* push out a Window update */
         }
         else  /* no current data - if connection is shutting down, signal app */
         {
            if((tp->t_state > TCPS_ESTABLISHED) &&    /* socket closing */
               ((so->state & SS_UPCFIN) == 0))        /* and didn't tell app yet */
            {
               so->callback(M_CLOSED, so, NULL);
               so->state |= SS_UPCFIN;          /* flag that upcall was FINed */
            }
         }
      }

      tp->t_idle++;
      if (tp->t_rtt)
         tp->t_rtt++;
   }
   tcp_iss += (unsigned)(TCP_ISSINCR/PR_SLOWHZ);      /* increment iss */

   if (tcp_iss & 0xff000000)
      tcp_iss = 0L;
}


/* FUNCTION: tcp_canceltimers()
 *
 * Cancel all timers for TCP tp.
 * 
 * PARAM1: struct tcpcb *tp
 *
 * RETURNS: 
 */

void
tcp_canceltimers(struct tcpcb * tp)
{
   int   i;

   for (i = 0; i < TCPT_NTIMERS; i++)
      tp->t_timer[i] = 0;
}

unsigned char tcp_backoff [TCP_MAXRXTSHIFT + 1] =
{ 1, 2, 4, 8, 16, 32, 64, 64, 64, 64, 64, 64, 64 };



/* FUNCTION: tcp_timers()
 *
 * TCP timer processing.
 * 
 * PARAM1: struct tcpcb *tp
 * PARAM2: int timer
 *
 * RETURNS: 
 */

struct tcpcb * 
tcp_timers(struct tcpcb * tp, int timer)
{
   int   rexmt;

   switch (timer) 
   {

   /*
    * 2 MSL timeout in shutdown went off.  If we're closed but
    * still waiting for peer to close and connection has been idle
    * too long, or if 2MSL time is up from TIME_WAIT, delete connection
    * control block.  Otherwise, check again in a bit.
    */
   case TCPT_2MSL:
      if (tp->t_state != TCPS_TIME_WAIT &&
          tp->t_idle <= tcp_maxidle)
      {
         tp->t_timer[TCPT_2MSL] = (short)tcp_keepintvl;
      }
      else
         m_tcpclose(tp);
      break;

   /*
    * Retransmission timer went off.  Message has not
    * been acked within retransmit interval.  Back off
    * to a longer retransmit interval and retransmit one segment.
    */
   case TCPT_REXMT:
      TCP_MIB_INC(tcpRetransSegs);     /* keep MIB stats */
      if (++tp->t_rxtshift > TCP_MAXRXTSHIFT) 
      {
         tp->t_rxtshift = TCP_MAXRXTSHIFT;
         TCP_STAT_INC(tcps_timeoutdrop);
         m_tcpdrop(tp, ETIMEDOUT);
         break;
      }
      TCP_STAT_INC(tcps_rexmttimeo);
      rexmt = ((tp->t_srtt >> 2) + tp->t_rttvar) >> 1;
      rexmt *= tcp_backoff[tp->t_rxtshift];
      TCPT_RANGESET(tp->t_rxtcur, (short)rexmt, TCPTV_MIN, TCPTV_REXMTMAX);
      tp->t_timer[TCPT_REXMT] = tp->t_rxtcur;
      /*
       * If losing, let the lower level know and try for
       * a better route.  Also, if we backed off this far,
       * our srtt estimate is probably bogus.  Clobber it
       * so we'll take the next rtt measurement as our srtt;
       * move the current srtt into rttvar to keep the current
       * retransmit times until then.
       */
      if (tp->t_rxtshift > TCP_MAXRXTSHIFT / 4) 
      {
         tp->t_rttvar += (tp->t_srtt >> 2);
         tp->t_srtt = 0;
      }
      tp->snd_nxt = tp->snd_una;
      /*
       * If timing a segment in this window, stop the timer.
       */
      tp->t_rtt = 0;
      /*
       * Close the congestion window down to one segment
       * (we'll open it by one segment for each ack we get).
       * Since we probably have a window's worth of unacked
       * data accumulated, this "slow start" keeps us from
       * dumping all that data as back-to-back packets (which
       * might overwhelm an intermediate gateway).
       *
       * There are two phases to the opening: Initially we
       * open by one mss on each ack.  This makes the window
       * size increase exponentially with time.  If the
       * window is larger than the path can handle, this
       * exponential growth results in dropped packet(s)
       * almost immediately.  To get more time between 
       * drops but still "push" the network to take advantage
       * of improving conditions, we switch from exponential
       * to linear window opening at some threshhold size.
       * For a threshhold, we use half the current window
       * size, truncated to a multiple of the mss.
       *
       * (the minimum cwnd that will give us exponential
       * growth is 2 mss.  We don't allow the threshhold
       * to go below this.)
       */
      {
         unsigned win = MIN(tp->snd_wnd, tp->snd_cwnd);
         win = win / 2 / tp->t_maxseg;
         if (win < 2)
            win = 2;
         tp->snd_cwnd = tp->t_maxseg;
         tp->snd_ssthresh = (u_short)win * tp->t_maxseg;
      }
      (void) tcp_output(tp);
      break;

   /*
    * Persistance timer into zero window.
    * Force a byte to be output, if possible.
    */
   case TCPT_PERSIST:
      TCP_STAT_INC(tcps_persisttimeo);
      tcp_setpersist(tp);
      tp->t_force = 1;
      (void) tcp_output(tp);
      tp->t_force = 0;
      break;

   /*
    * Keep-alive timer went off; send something
    * or drop connection if idle for too long.
    */
   case TCPT_KEEP:
      TCP_STAT_INC(tcps_keeptimeo);
      if (tp->t_state < TCPS_ESTABLISHED)
         goto dropit;
      if ((((M_SOCK)(tp->t_inpcb))->so_options & SO_KEEPALIVE) &&
          tp->t_state <= TCPS_CLOSE_WAIT) 
      {
         if (tp->t_idle >= tcp_keepidle + tcp_maxidle)
            goto dropit;
         /*
          * Send a packet designed to force a response
          * if the peer is up and reachable:
          * either an ACK if the connection is still alive,
          * or an RST if the peer has closed the connection
          * due to timeout or reboot.
          * Using sequence number tp->snd_una-1
          * causes the transmitted zero-length segment
          * to lie outside the receive window;
          * by the protocol spec, this requires the
          * correspondent TCP to respond.
          */
         TCP_STAT_INC(tcps_keepprobe);

         /*
          * The keepalive packet must have nonzero length
          * to get a 4.2 host to respond.
          */

         dtrap(); /* watch this first time through */
         tp->t_flags |= TF_SENDKEEP;
         tcp_output(tp);
         tp->t_flags &= ~TF_SENDKEEP;

         tp->t_timer[TCPT_KEEP] = (short)tcp_keepintvl;
      }
      else
         tp->t_timer[TCPT_KEEP] = (short)tcp_keepidle;
      break;

dropit:
      TCP_STAT_INC(tcps_keepdrops);
      m_tcpdrop (tp, ETIMEDOUT);
      break;
   }
   return tp;
}


/* FUNCTION: tcp_tick()
 *
 * tcp_tick() - Run the tcp timers. This should be called at least 
 * TPS times per second. 
 *
 * 
 * PARAM1: 
 *
 * RETURNS: 
 */

unsigned long nextslow = 0L;     /* next slow tcp timer time */
static int in_tcptick = 0;       /* reentry gaurd */

void
tcp_tick()
{
   /* guard against re-entry */
   if (in_tcptick)
      return;
   in_tcptick++;

   LOCK_NET_RESOURCE(NET_RESID);

   if (cticks >= nextslow) /* time to do it again */
   {
      tcp_slowtimo();      /* call routine in BSD tcp_timr.c */
      nextslow = cticks + (TPS/2);  /* another 500 ms */
   }

#ifdef DO_DELAY_ACKS
   tcp_fasttimo();
#endif   /* DO_DELAY_ACKS */

   UNLOCK_NET_RESOURCE(NET_RESID);

   in_tcptick--;
}


/* end of file tcp_timr.c */


