/*  libport.h
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
 */

#ifndef _LIBPORT_H
#define _LIBPORT_H   1

#ifdef PRINTF_STDARG
#include <stdarg.h>
#endif 

/* options specific to the utilities library */

/* enabled options */
#define INICHE_STRISTR	1     /* bring in our stristr()  */



#ifdef INCLUDE_FLASHFS
#define FLASHFIRM 0xE0000  /* memory address to build FFS  */
#define FLASHBASE 0xE0000  /* base of flash to store FFS */
#define NUMNVFILES 8          /* numnber of flash files allowed */
#define NVFSIZE   4076        /* reserved size for each flash file */
#endif

/* misc prototypes for TCP/IP misclib port */
extern	void	panic(char * msg);
#ifdef PRINTF_STDARG
extern   int doprint(char * target, unsigned tlen, char * sp, va_list va);
#else  /* PRINTF_STDARG */
extern   int doprint(char * target, unsigned tlen, char * sp, int * vp);
#endif /* PRINTF_STDARG */

#ifndef isdigit
#define isdigit(_c)  ((_c >= '0' && _c <= '9') ? 1:0)
#endif

#endif /* _LIBPORT_H */

