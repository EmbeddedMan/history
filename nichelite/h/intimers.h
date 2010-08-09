/*
 * FILENAME: intimers.c
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
 * Handles InterNiche task & application interval timers.
 *
 * MODULE: MISCLIB
 *
 * PORTABLE: yes
 */

#ifndef _INTIMERS_H
#define _INTIMERS_H  1

/* interval timer structure */
struct intimer
{
   void     (*callback)(long parm);
   long     parm;
   u_long   interval;   /* fire interval, in TPS units */
   u_long   tmo;        /* ctick to fire */
};

/* MAX number of interval timers in the system */
#ifndef NUM_INTIMERS       /* Allow override from ipport.h */
#define NUM_INTIMERS 5
#endif

extern struct intimer intimers[NUM_INTIMERS];   /* the timers */

/* entry points to interval timer package */
long  in_timerset(void (*callback)(long), long msecs, long parm);
int   in_timerkill(long timer);

#endif /* _INTIMERS_H */



