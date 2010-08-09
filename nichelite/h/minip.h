/*
 * FILENAME: minip.h
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
 * Mini-IP compatability file. This stubs out INET directory routines
 * and variables expected by the rest of the libraries, checks for bogus
 * option settings, and generaly makes mini-IP a drop-in
 * replacment for the inet.lib
 *
 * This must be included AFTER the definitions in ipport.h - for example
 * at the end of ipport.h
 *
 * MODULE: MIP
 *
 * PORTABLE: yes
 */

#ifndef _MINIP_H_
#define  _MINIP_H_ 1

#ifndef _IPPORT_H_
#error This must be included AFTER the definitions in ipport.h
#endif

/* declare the global pointer to our one & only net iface */
struct net;    /* predecl */
extern struct net * m_netp;

/* map the IP interface calls to reflect that we only have one net */

#define if_netnumber(nptr)    0

#define ip_mymach(addr)  (m_netp->n_ipaddr)

#define if_getbynum(int)   (m_netp)

/* define away reg type - these drivers are required to recv IP and ARPs */
#define reg_type( type )


#ifdef USE_PPP
#define isbcast(ifc, address) (FALSE)  /* PPP has no broadcast */
#define MaxLnh 4
#else
#define isbcast(ifc, address) m_isbcast(address)
#define MaxLnh (ETHHDR_SIZE)
#endif


/* sanity test the ipport.h_h settings */

#ifdef MINI_IP
#ifdef   DYNAMIC_IFACES
#error DYNAMIC_IFACES option not allowed on mini build
#endif

#ifdef   IP_MULTICAST
#error IP_MULTICAST option not allowed on mini build
#endif

#ifdef   IP_FRAGMENTS
#error IP_FRAGMENTS option not allowed on mini build
#endif

#ifdef   IP_ROUTING
#error IP_ROUTING option not allowed on mini build
#endif

#ifdef   MULTI_HOMED
#error MULTI_HOMED option not allowed on mini build
#endif

#ifdef   PING_APP
#error PING_APP option not allowed on mini build
#endif

#ifdef   NATRT
#error NATRT option not allowed on mini build
#endif

#ifdef   IP_LOOPBACK
#error IP_LOOPBACK option not allowed on mini build
#endif

#endif /* MINI_IP sanity test */

#endif   /* _MINIP_H_ - end of file */   

