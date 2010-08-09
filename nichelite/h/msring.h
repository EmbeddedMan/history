/* FILENAME: msring.h
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
 * Definitions for ring buffer support for NicheLite M_SOCKs.
 * See misclib/msring.c for implementation.
 *
 * PORTABLE: YES
 * 
 * MODULE: H
 */

#ifndef MSRING_H_
#define MSRING_H_

#ifdef MINI_TCP

/* struct msring - a ring buffer structure for NicheLite sockets
 */
struct msring
{
   M_SOCK * buf;           /* ptr to storage for buffered M_SOCKs */
   unsigned buflen;        /* length (in M_SOCKs) of buf */
   unsigned in;            /* index into buf: where to add next M_SOCK */
   unsigned out;           /* index into buf: where to delete next M_SOCK */
};

/* function prototypes */
void msring_init(struct msring * ring,
                 M_SOCK * buf,
                 unsigned bufsize);
void msring_add(struct msring * ring,
                M_SOCK so);
int msring_del(struct msring * ring,
               M_SOCK * so);

#endif /* MINI_TCP */

#endif  /* MSRING_H_ */
