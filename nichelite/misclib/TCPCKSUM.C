/*
 * FILENAME: tcpcksum.c
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
 *  General purpose TCP checksum routine. Uses internet cksum()
 *  routine to do actual 1's complement buffer calc, this is
 *  usually optimized in assembly language.
 *
 * MODULE: MISCLIB
 *
 * ROUTINES: tcp_cksum(), 
 *
 * PORTABLE: yes
 */

#include "license.h"
#include "ipport.h"

#if defined(INCLUDE_TCP) || defined(NATRT) || defined(USE_PPP) || defined(MINI_TCP)

#include "tcpapp.h"
#include "ip.h"
#include "tcp.h"


/* FUNCTION: tcp_cksum()
 *
 * tcp_cksum(struct ip - The IP header passed is assumed to be 
 * prepended to the rest of a TCP packet, i.e. followed by TCP header 
 * and data to be checksummed. Returns the correct checksum for the 
 * packet. The packet data is unchanged 
 *
 * 
 * PARAM1: struct ip * pip
 *
 * RETURNS: 
 */

unshort
tcp_cksum(struct ip * pip)
{
   unshort  iphlen;  /* length of IP header, usually 20 */
   unshort  tcplen;  /* length of TCP header and data */
   char *   tcpdata;    /* pointer to end of tcp data, for 0 padding */
   char     oddchar;    /* tmp save of odd char we 0 for pad */
   unshort  oldsum;
   unshort  newsum;
   struct tcphdr * tp;

#ifdef MUTE_WARNS    /* stifle compiler warnings */
   tcpdata = NULL;
#endif   /* MUTE_WARNS */

   oddchar = 0;

   iphlen = (unshort)ip_hlen(pip);
   tcplen = (pip->ip_len) - iphlen;

   tp = (struct tcphdr*)ip_data(pip);      /* get TCP header */
   oldsum = tp->th_sum;   /* Save passed checksum */

   /* The TCP checksum spec. says to Init TCP header cksum field to 0 before
    * summing. We also need to inclue in the sum a protocol type (always 6
    * for TCP). the Ip length, and the IP addresses. Nominally this is done 
    * by constructing a "pseudo header" and prepending it to the tcp header
    * and data to be summed. Instead, we add the type & length (they 
    * cannot overflow a 16 bit field) and put them in the cksum field. 
    * We include the IP addresses by passing them to the lower level 
    * fast sum routine. This results in their values being factored into 
    * the sum and the cksum field contributes zero.
    */
   tp->th_sum = (tcplen + 6);

   /* zero pad odd sized packets for checksumming */
   if (tcplen & 1)   
   {
      tcpdata = ((char*)pip) + iphlen + tcplen;    /* end of packet */
      oddchar = *tcpdata;
      *tcpdata = 0;  /* zero out pad byte */
      tcplen++;      /* bump length to pass to cksum() */
   }

   /* Pass a pointer to the beginning of the IP address area into the IP header
    * the the low level sum routine. Add the size of these two IP addresses to
    * the length, and convert the length to 16 bit words.
    */
   newsum = ~cksum( ((char*)tp) - 8, (tcplen + 8)/2 );

   /* restore what we clobbered */
   tp->th_sum = oldsum;   /* put back passed checksum */
   if (oddchar)
      *tcpdata = oddchar;  /* restore odd byte if we zeroed it */

   return newsum;
}

#endif   /* INCLUDE_TCP */


