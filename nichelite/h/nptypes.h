/*
 * FILENAME: nptypes.h
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
 *
 * PORTABLE: yes
 */


#ifndef NPTYPES_H
#define  NPTYPES_H   1


typedef  unsigned short unshort;
typedef  unsigned long ulong;


/* now do it the way the CMU snmp code likes it: */
typedef   unsigned char u_char;
typedef   unsigned short u_short;
typedef   unsigned long u_long;

typedef  unsigned long ip_addr;

/* general stuff C code */
#ifndef  TRUE
#define  TRUE  -1
#endif

#ifndef  FALSE
#define  FALSE    0
#endif

#ifndef  NULL
#define  NULL  ((void*)0)
#endif

/* usefull macros: */
#ifndef min
#define  min(x,y)    ((x)  <  (y)   ?  (x)   :  (y))
#endif
#ifndef max
#define  max(x,y)    ((x)  >  (y)   ?  (x)   :  (y))
#endif

#endif   /* NPTYPES_H */



