/*
 * FILENAME: app_ping.h
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
 * Header file for  app_ping.c
 * NOTE - this file may by included even if PING_APP is not defined in the
 * system build, since UDP_ECHO and others use some of these values.
 *
 * MODULE: TCP
 *
 * PORTABLE: yes
 */

#ifndef APP_PING_H
#define  APP_PING_H     1

#include "in_utils.h"

struct PingInfo 
{
   u_long   delay;         /* delay between 2 pings */
   int      length;        /* ping packet length */
   ip_addr  ipadd;   
   u_long   nextping;      /* cticks value to do next ping */
   long     times;         /* number of times to ping */
   int      out;
   int      in;
   GEN_IO   pio;           /* To communicate with invocator of ping */
   struct PingInfo * next; /* Next ping packet */
};

typedef struct PingInfo * PING_INFO ;

extern   ip_addr  activehost; /* default ping host */

#define  TIMEFOR1TICK      (1000/TPS)  /* Time between each tick in millisecs */


int         ping_init(void);
PING_INFO   ping_new (void);
int         ping_delete(PING_INFO p);
int         ping_addq(PING_INFO p);
int         ping_delq(PING_INFO p);
PING_INFO   ping_search(GEN_IO pio);
int         ping_start(void * pio);
int         ping_send(PING_INFO p);
int         ping_end(void * pio);
int         ping_setdelay(void * pio);
int         ping_setlength(void * pio);   /* menu routine to set default ping size*/
int         ping_sethost(void * pio);     /* set default host for pings, et.al. */
int         pingUpcall(PACKET p);
void        ping_check(void);
PING_INFO   ping_demux(ip_addr fhost);
int         ping_stats(void * pio);

#define     PING_ALLOC(size)  npalloc(size) 
#define     PING_FREE(ptr)    npfree(ptr)

#define     PINGERRBASE       200

/* List of error codes returned by ping_delq() */
#define     PING_DELQ_BAD_OBJECT       (PINGERRBASE+11)
#define     PING_DELQ_Q_EMPTY          (PINGERRBASE+12)
#define     PING_DELQ_OBJ_NOT_FOUND    (PINGERRBASE+13)

/* List of error codes returned by ping_delete() */
#define     PING_DEL_NULL_ARGUMENT     (PINGERRBASE+21)

/* List of error codes returned by ping_start() */
#define     PING_ST_NOIP               (PINGERRBASE+31)
#define     PING_ST_BAD_ARG2           (PINGERRBASE+32)
#define     PING_ST_ALLOC_ERR          (PINGERRBASE+33)

/* List of error codes returned by ping_end() */
#define     PING_END_NO_SESSION        (PINGERRBASE+41)

/* List of error codes returned by ping_setdelay() */
#define     PING_DELAY_BAD_ARG         (PINGERRBASE+51)

/* List of error codes returned by ping_setlength() */
#define     PING_LEN_BAD_ARG           (PINGERRBASE+61)

#endif   /* APP_PING_H */

