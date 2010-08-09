/*
 * FILENAME: ether.h
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
 * information common to all Ethernet drivers
 *
 * MODULE: INET
 *
 *
 * PORTABLE: yes
 */

#ifndef _ETHER_H_
#define  _ETHER_H_   1

/* initialization modes: 0 = rcv normal + broadcast packets */
#define     LOOPBACK    0x01  /* send packets in loopback mode */
#define     ALLPACK     0x02  /* receive all packets: promiscuous */
#define     MULTI       0x04  /* receive multicast packets */

/* Ethernet packet header */

START_PACKED_STRUCT(ethhdr)
u_char   e_dst[6];
u_char   e_src[6];
unshort  e_type;
END_PACKED_STRUCT(ethhdr)

/* ETHHDR_SIZE - size of packet header structure for allocation 
 * purposes Note this is a default -- it should be overridden in 
 * ipport.h if the need arises. 
 */

#ifndef ETHHDR_SIZE
#define ETHHDR_SIZE (sizeof(struct ethhdr))
#endif   /* ETHHDR_SIZE */

/* ETHHDR_BIAS - where to locate the struct ethhdr within the 
 * allocated space, as an offset from the start in bytes Note this is 
 * a default -- it should be overridden in ipport.h if the need 
 * arises. 
 */

#ifndef  ETHHDR_BIAS
#define  ETHHDR_BIAS 0
#endif   /* ETHHDR_BIAS */

/* ET_DSTOFF - offset of destination address within Ethernet header
 */
#define ET_DSTOFF       (0)

/* ET_SRCOFF - offset of source address within Ethernet header
 */
#define ET_SRCOFF       (6)

/* ET_TYPEOFF - offset of Ethernet type within Ethernet header
 */
#define ET_TYPEOFF      (12)

/* ET_TYPE_GET(e) - get Ethernet type from Ethernet header pointed to
 *                  by char * e
 * Note returned Ethernet type is in host order!
 */
#define ET_TYPE_GET(e)  \
        (((unsigned)(*((e) + ET_TYPEOFF)) << 8) + \
         (*((e) + ET_TYPEOFF + 1) & 0xff))

/* ET_TYPE_SET(e, type) - set Ethernet type in Ethernet header pointed to
 *                        by char * e to value (type)
 * Note Ethernet type is value is expected to be in host order!
 */
#define ET_TYPE_SET(e, type) \
        *((e) + ET_TYPEOFF) = (unsigned char)(((type) >> 8) & 0xff); \
        *((e) + ET_TYPEOFF + 1) = (unsigned char)((type) & 0xff);

/* minimum & maximun length legal Ethernet packet sizes */
#define     ET_MINLEN   60
#define     ET_MAXLEN   1514
#define     MINTU       60
#define     MTU         1514

extern   unsigned char  ETBROADCAST[];

#endif   /* _ETHER_H_ */



