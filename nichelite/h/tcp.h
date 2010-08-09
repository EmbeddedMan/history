/*
 * FILENAME: tcp.h
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
 *
 * MODULE: INET
 *
 * PORTABLE: yes
 */


#ifndef _TCP_H
#define  _TCP_H   1

typedef   u_long   tcp_seq;

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
#define     TH_PUSH     0x08
#define     TH_ACK   0x10
#define     TH_URG   0x20
   u_short     th_win;     /* window */
   u_short     th_sum;     /* checksum */
   u_short     th_urp;     /* urgent pointer */
};

#define  TCPOPT_EOL        0
#define  TCPOPT_NOP        1
#define  TCPOPT_MAXSEG     2
#define  TCPOPT_WINSCALE   3
#define  TCPOPT_SACKOK     4
#define  TCPOPT_SACKDATA   5
#define  TCPOPT_RTT        8

#endif   /* _TCP_H */

/* end of file tcp.h */


