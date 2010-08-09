/*
 * FILENAME: mtcp.h
 *
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
 * Definitions for the mini TCP layer - internal to mtcp.
 *
 * MODULE: MTCP
 *
 * PORTABLE: yes
 */

#ifndef _MTCP_H
#define  _MTCP_H  1

#ifndef _IPPORT_H_
#error   Must include ipport.h before this file */
#endif

#include "q.h"
#include "netbuf.h"
#include "net.h"
#include "ip.h"

#include "msock.h"

#ifndef MIN
#define  MIN(a,b)    (a>b?b:a)
#define  MAX(a,b)    (a>b?a:b)
#endif


/*
 * The TCPT_REXMT timer is used to force retransmissions.
 * The TCP has the TCPT_REXMT timer set whenever segments
 * have been sent for which ACKs are expected but not yet
 * received.  If an ACK is received which advances tp->snd_una,
 * then the retransmit timer is cleared (if there are no more
 * outstanding segments) or reset to the base value (if there
 * are more ACKs expected).  Whenever the retransmit timer goes off,
 * we retransmit one unacknowledged segment, and do a backoff
 * on the retransmit timer.
 *
 * The TCPT_PERSIST timer is used to keep window size information
 * flowing even if the window goes shut.  If all previous transmissions
 * have been acknowledged (so that there are no retransmissions in progress),
 * and the window is too small to bother sending anything, then we start
 * the TCPT_PERSIST timer.  When it expires, if the window is nonzero,
 * we go to transmit state.  Otherwise, at intervals send a single byte
 * into the peer's window to force him to update our window information.
 * We do this at most as often as TCPT_PERSMIN time intervals,
 * but no more frequently than the current estimate of round-trip
 * packet time.  The TCPT_PERSIST timer is cleared whenever we receive
 * a window update from the peer.
 *
 * The TCPT_KEEP timer is used to keep connections alive.  If an
 * connection is idle (no segments received) for TCPTV_KEEP_INIT amount of time,
 * but not yet established, then we drop the connection.  Once the connection
 * is established, if the connection is idle for TCPTV_KEEP_IDLE time
 * (and keepalives have been enabled on the socket), we begin to probe
 * the connection.  We force the peer to send us a segment by sending:
 *   <SEQ=SND.UNA-1><ACK=RCV.NXT><CTL=ACK>
 * This segment is (deliberately) outside the window, and should elicit
 * an ack segment in response from the peer.  If, despite the TCPT_KEEP
 * initiated segments we cannot elicit a response from a peer in TCPT_MAXIDLE
 * amount of time probing, then we drop the connection.
 */

#define     TCPT_REXMT     0     /* retransmit */
#define     TCPT_PERSIST   1     /* retransmit persistance */
#define     TCPT_KEEP      2     /* keep alive */
#define     TCPT_2MSL      3     /* 2*msl quiet time timer */


/*
 * TCP sequence numbers are 32 bit integers operated
 * on with modular arithmetic.  These macros can be
 * used to compare such integers.
 */
#define     SEQ_LT(a,b)    ((long)((a)-(b))  <  0)
#define     SEQ_LEQ(a,b)   ((long)((a)-(b))  <= 0)
#define     SEQ_GT(a,b)    ((long)((a)-(b))  >  0)
#define     SEQ_GEQ(a,b)   ((long)((a)-(b))  >= 0)


extern   int   tcp_keepidle;     /* time before keepalive probes begin */
extern   int   tcp_keepintvl;    /* time between keepalive probes */
extern   int   tcp_maxidle;      /* time to drop after starting probes */

/*
 * Tcp/Ip-like header, after ip options removed. First part of IP
 * header if overwritten with double linked list links.
 */

struct ipovly
{
   struct ipovly *   ih_next;    /* list link */
   struct ipovly *   ih_prev;    /* list link */
   u_short  ih_len;              /* protocol length */
   struct   in_addr  ih_src;     /* source internet address */
   struct   in_addr  ih_dst;     /* destination internet address */
};

#define  ti_len   ti_i.ih_len
#define  ti_src   ti_i.ih_src
#define  ti_dst   ti_i.ih_dst
#define  ti_seq   ti_t.th_seq
#define  ti_ack   ti_t.th_ack


#ifndef TCPALLOCS_ALREADY

/* Memory heap alloc/free mappings */
#define  TCB_ALLOC(size)   (struct  tcpcb *)npalloc(size)
#define  TCB_FREE(ptr)     npfree((void*)ptr)
#define  SOC_ALLOC(size)   (M_SOCK)npalloc(size)
#define  SOC_FREE(ptr)     npfree((void*)ptr)

#endif /* TCPALLOCS_ALREADY */

extern   void  tcp_setpersist (struct tcpcb *);
extern   void  tcp_canceltimers (struct tcpcb *);

#ifndef TCPWAKE_ALREADY
extern void tcp_sleep(void *);
extern void tcp_wakeup(void *);
#endif

typedef  u_long      tcp_seq;
extern   tcp_seq     tcp_iss;    /* tcp initial send seq # */

#define     TCP_ISSINCR    (long)(0x0001F4FF)   /* increment for tcp_iss */

/* Macros to initialize tcp sequence numbers for send and
 * receive from initial send and receive sequence numbers.
 * These are left as BSDish MACROs since they are not called
 * often.
 */
#define   tcp_rcvseqinit(tp) \
   (tp)->rcv_adv = (tp)->rcv_nxt = (tp)->irs + 1

#define   tcp_sendseqinit(tp) \
   (tp)->snd_una = (tp)->snd_nxt = (tp)->snd_max = (tp)->snd_up = \
       (tp)->iss

/*
 * TCP header.
 * Per RFC 793, September, 1981.
 */

struct tcphdr
{
   u_short  th_sport;      /* source port */
   u_short  th_dport;      /* destination port */
   tcp_seq  th_seq;        /* sequence number */
   tcp_seq  th_ack;        /* acknowledgement number */

   u_char   th_doff;       /* data offset: high 4 bits only */

   /* these macros get/set the raw value, usually 5 */
#define  GET_TH_OFF(th) (th.th_doff >> 4)
#define  SET_TH_OFF(th, off)  (th.th_doff =  (u_char)(off   << 4))

   u_char   th_flags;
#define     TH_FIN   0x01
#define     TH_SYN   0x02
#define     TH_RST   0x04
#define     TH_PUSH  0x08
#define     TH_ACK   0x10
#define     TH_URG   0x20
   u_short     th_win;     /* window */
   u_short     th_sum;     /* checksum */
   u_short     th_urp;     /* urgent pointer */
};

#define  TCPOPT_EOL     0
#define  TCPOPT_NOP     1
#define  TCPOPT_MAXSEG  2

/* The BSD-ish structure for tcp & IP headers together (not IP options) */
struct tcpiphdr
{
     struct ip       ti_i; /* overlaid ip structure */
     struct tcphdr   ti_t; /* tcp header */
};


/* The TCP control block (tcpcb) structure - similar to BSD */

#define     TCPT_NTIMERS   4

struct tcpcb
{
   struct   tcpiphdr *  seg_next;   /* sequencing queue */
   struct   tcpiphdr *  seg_prev;
   int      t_state;             /* state of this connection */
   int      t_timer[TCPT_NTIMERS];  /* tcp timers */
   int      t_rxtshift;          /* log(2) of rexmt exp. backoff */
   int      t_rxtcur;            /* current retransmit value */
   int      t_dupacks;           /* consecutive dup acks recd */
   u_short  t_maxseg;            /* maximum segment size */
   char     t_force;             /* 1 if forcing out a byte */
   u_char   t_flags;
#define     TF_ACKNOW   0x01     /* ack peer immediately */
#define     TF_DELACK   0x02     /* ack, but try to delay it */
#define     TF_NODELAY  0x04     /* don't delay packets to coalesce */
#define     TF_NOOPT    0x08     /* don't use tcp options */
#define     TF_SENTFIN  0x10     /* have sent FIN */
#define     TF_SENDRST  0x20     /* send a reset */
#define     TF_SENDKEEP 0x40     /* send a keepalive */
#define     TF_OPENUP   0x80     /* we owe app an "open" upcall */
   struct tcpiphdr * t_template; /* skeletal packet for transmit */
   struct inpcb * t_inpcb;       /* back pointer to internet pcb */
   /*
    * The following fields are used as in the protocol specification.
    * See RFC783, Dec. 1981, page 21.
    */
   /* send sequence variables */
   tcp_seq     snd_una;    /* send unacknowledged */
   tcp_seq     snd_nxt;    /* send next */
   tcp_seq     snd_up;     /* send urgent pointer */
   tcp_seq     snd_wl1;    /* window update seg seq number */
   tcp_seq     snd_wl2;    /* window update seg ack number */
   tcp_seq     iss;        /* initial send sequence number */
   u_short     snd_wnd;    /* send window */
   /* receive sequence variables */
   u_short     rcv_wnd;    /* receive window */
   tcp_seq     rcv_nxt;    /* receive next */
   tcp_seq     rcv_up;     /* receive urgent pointer */
   tcp_seq     irs;        /* initial receive sequence number */

   /*
    * Additional variables for this implementation.
    */
   /* receive variables */
   tcp_seq     rcv_adv;    /* advertised window */

   /* retransmit variables */
   /* highest sequence number sent used to recognize retransmits */
   tcp_seq   snd_max;

   /* congestion control (for slow start, source quench, retransmit after loss) */
   u_short     snd_cwnd;      /* congestion-controlled window */
   /* snd_cwnd size threshhold for for slow start exponential to
    * linear switch
    */
   u_short snd_ssthresh;

   /*
    * transmit timing stuff.
    * srtt and rttvar are stored as fixed point; for convenience in smoothing,
    * srtt has 3 bits to the right of the binary point, rttvar has 2.
    * "Variance" is actually smoothed difference.
    */
   int      t_idle;        /* inactivity time */
   int      t_rtt;         /* round trip time */
   tcp_seq     t_rtseq;    /* sequence number being timed */
   int      t_srtt;        /* smoothed round-trip time */
   int      t_rttvar;      /* variance in round-trip time */
   u_short  max_rcvd;      /* most peer has sent into window */
   u_short     max_sndwnd; /* largest window peer has offered */
   /* out-of-band data */
   char     t_oobflags;    /* have some */
   char     t_iobc;        /* input character */
#define     TCPOOB_HAVEDATA   0x01
#define     TCPOOB_HADDATA    0x02
};


#define     TCP_NSTATES    11

#define     TCPS_CLOSED          0  /* closed */
#define     TCPS_LISTEN          1  /* listening for connection */
#define     TCPS_SYN_SENT        2  /* active, have sent syn */
#define     TCPS_SYN_RECEIVED    3  /* have send and received syn */
/* states < TCPS_ESTABLISHED are those where connections not established */
#define     TCPS_ESTABLISHED     4  /* established */
#define     TCPS_CLOSE_WAIT      5  /* rcvd fin, waiting for close */
/* states > TCPS_CLOSE_WAIT are those where user has closed */
#define     TCPS_FIN_WAIT_1      6  /* have closed, sent fin */
#define     TCPS_CLOSING      7  /* closed xchd FIN; await FIN ACK */
#define     TCPS_LAST_ACK        8  /* had fin and close; await FIN ACK */
/* states > TCPS_CLOSE_WAIT && < TCPS_FIN_WAIT_2 await ACK of FIN */
#define     TCPS_FIN_WAIT_2      9  /* have closed, fin is acked */
#define     TCPS_TIME_WAIT       10 /* in 2*msl quiet wait after close */

#define     TCPS_HAVERCVDSYN(s)     ((s)  >= TCPS_SYN_RECEIVED)
#define     TCPS_HAVERCVDFIN(s)     ((s)  >= TCPS_TIME_WAIT)

extern u_char   tcp_outflags[TCP_NSTATES];


#ifndef  TCP_MSS           /* allow override in ipport.h */
#define  TCP_MSS  0x05b0   /* hardcode for Ethernet/PPP for now.... */
#endif

#ifndef  IP_MAXPACKET
#define  IP_MAXPACKET   (24<<10)    /* maximum packet size */
#endif

#ifdef INCLUDE_SNMP
struct variable;
#include "snmpport.h"
extern   struct tcp_mib tcpmib;
#else    /* no SNMP, support tcp_mib locally for tcp_stats() */
struct TcpMib
{
   long     tcpRtoAlgorithm;
   long     tcpRtoMin;
   long     tcpRtoMax;
   long     tcpMaxConn;
   u_long   tcpActiveOpens;
   u_long   tcpPassiveOpens;
   u_long   tcpAttemptFails;
   u_long   tcpEstabResets;
   u_long   tcpCurrEstab;
   u_long   tcpInSegs;
   u_long   tcpOutSegs;
   u_long   tcpRetransSegs;
   void *   tcpConnTable;  /*32 bit ptr */
   u_long   tcpInErrs;
   u_long   tcpOutRsts;
};
extern   struct TcpMib  tcpmib; 
#endif   /* INCLUDE_SNMP */

#define  TCP_MIB_INC(varname) {tcpmib.varname++;}

#define     PR_SLOWHZ   2  /* TCP ticks per second */
#define     PR_FASTHZ   5  /* 5 fast timeouts per second */

extern   int   TCPTV_MSL;  /* max seg lifetime - NOT a constant! */

#define  TCP_MAXRXTSHIFT   12                   /* maximum retransmits */

extern   unsigned char  tcp_backoff[TCP_MAXRXTSHIFT   +  1];

short    tcpt_rangeset(short value, short tvmin, short tvmax);

/* Map BSD-ish TCPT_RANGESET() macro to function */
#define TCPT_RANGESET(result, value, tvmin, tvmax) \
         result = tcpt_rangeset(value, tvmin, tvmax)


#define     TCPTV_SRTTBASE    0     /* base roundtrip time; if 0, no idea yet */

#define  TCPTV_SRTTDFLT    (3*PR_SLOWHZ)        /* assumed RTT if no info */
#define  TCPTV_PERSMIN     (5*PR_SLOWHZ)        /* retransmit persistance */
#define  TCPTV_PERSMAX     (60*PR_SLOWHZ)       /* maximum persist interval */
#define  TCPTV_KEEP_INIT   (75*PR_SLOWHZ)       /* initial connect keep alive */
#define  TCPTV_KEEP_IDLE   (120*60*PR_SLOWHZ)   /* dflt time before probing */
#define  TCPTV_KEEPINTVL   (75*PR_SLOWHZ)       /* default probe interval */
#define  TCPTV_KEEPCNT     8                    /* max probes before drop */
#define  TCPTV_MIN         (1*PR_SLOWHZ)        /* minimum allowable value */
#define  TCPTV_REXMTMAX    (64*PR_SLOWHZ)       /* max allowable REXMT value */
#define  TCPTV_KEEP_INIT   (75*PR_SLOWHZ)       /* initial connect keep alive */

unshort  tcp_cksum(struct ip * pip);
int      tcp_output (struct tcpcb *);

/* internal mtcp routines */
struct tcpcb * m_newtcpcb(M_SOCK so);
void     so_flush(M_SOCK so);
M_SOCK   so_lookup(struct ip *, struct tcphdr *);
void     m_setlport(M_SOCK so);
void     m_delsocket(M_SOCK so);
void     m_tcpoptions(struct tcpcb * tp, char * opt, int optlen);
void     m_template(struct tcpcb *);
void     m_connected(M_SOCK so);
void     m_disconnecting(M_SOCK so);
void     m_disconnected(M_SOCK so);
void     m_tcpdrop(struct tcpcb * tp, int err);
void     m_tcpclose(struct tcpcb * tp);
void     m_data_upcall(M_SOCK so);

int      m_sbdrop(struct m_sockbuf * que, unsigned);
PACKET   get_soq(struct m_sockbuf * que);
void     put_soq(struct m_sockbuf * que, PACKET pkt);


#endif   /* _MTCP_H */

