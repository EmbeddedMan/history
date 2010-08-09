/*
 * FILENAME: tcpapp.h
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
 * Single include file for INterNicher TCP applications. This defines everything
 * the app needs to call sockets. Works for Full-Sized stack or mini stack.
 *
 * MODULE: TCP
 *
 * PORTABLE: yes
 */


#ifndef _TCPAPP_H
#define  _TCPAPP_H  1

#include "ipport.h"

#include "q.h"
#include "netbuf.h"
#include "net.h"


#ifdef MINI_TCP
#include "msock.h"
#else /* not MINI_TCP */
#include "tcpport.h"   /* embedded system includes */
#endif   /* MINI_TCP */

#endif	/* _TCPAPP_H */


