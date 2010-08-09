/*
 * FILENAME: tk_ntask.h
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
 * MODULE: MISCLIB
 *
 * PORTABLE: yes
 */

#ifndef TK_NTASK_H
#define TK_NTASK_H

#ifndef INICHE_TASKS
#error must define INICHE_TASKS in ipport.h to use this file
#endif

/* include the nichtask file in misclib */
#include "task.h"

/* the TK_ object macros, for defining task objects, etc. */
#define  TK_ENTRY(name)       int   name(int parm)
#define  TK_ENTRY_PTR(name)   int(*name)(int)
#define  TK_OBJECT(name)      task *   name
#define  TK_OBJECT_PTR(name)  task **  name

#ifndef  TK_RETURN_ERROR
#define  TK_RETURN_ERROR()    return   (-1)
#endif
#ifndef  TK_RETURN_OK
#define  TK_RETURN_OK()       parm++;  return   (0)
#endif

/* the TK_ function macros: */
#define  tk_yield()     {  tk_wake(tk_cur);  tk_block(); }
#define  TK_APP_WAKE(ev)   tk_ev_wake(ev)
#define  TK_WAKE(task)     tk_wake(*task)
#define  TK_BLOCK()        tk_block()
#define  TK_SLEEP(tks)     tk_sleep(tks)


#define  NET_PRIORITY         0     /* not used on tasks */
#define  MAIN_TASK_IS_NET     1     /* compiler main() is converted to net task */
#define  TK_THIS  tk_cur
#define  TK_NETMAINPARM       0     /* parameter to main/net task */
/*#undef   TK_STDIN_DEVICE*/
#define  TCPWAKE_ALREADY      1     /* tell tcp we already have this */

#if !defined(STK_TOPDOWN) && !defined(STK_BOTTOMUP)
#define  STK_TOPDOWN          1     /* Most CPU stacks are top down */
#endif

/*
 * synchronization primitives
 * 
 * Need one or the other of LOCK_NET_RESOURCE/UNLOCK_NET_RESOURCE and
 * ENTER_CRIT_SECTION/EXIT_CRIT_SECTION.  These should be implementable
 * as either functions or macros, so we allow e.g. ipport.h_h to 
 * override these definitions by defining the names as macros.
 * NicheTask targets generally use ENTER_CRIT_SECTION/EXIT_CRIT_SECTION,
 * so we default to function declaractions for them.
 */
#ifndef LOCK_NET_RESOURCE
#define  LOCK_NET_RESOURCE(x)
#endif
#ifndef UNLOCK_NET_RESOURCE
#define  UNLOCK_NET_RESOURCE(x)
#endif
#ifndef ENTER_CRIT_SECTION
void ENTER_CRIT_SECTION(void * p);
#endif
#ifndef EXIT_CRIT_SECTION
void EXIT_CRIT_SECTION(void * p);
#endif

/* declare tasks which need to be accessed by system code */
extern   task *   to_netmain;
extern   task *   to_emailer;
extern   task *   to_ftpclnt;
extern   task *   to_pingcheck;

/* define macros to  wake net task on packet receipt, etc */
#define  SignalPktDemux()  tk_wake(to_netmain)
#define  SignalEmailTask() tk_wake(to_emailer)
#define  SignalFtpClient() tk_wake(to_ftpclnt)


#endif  /* TK_NTASK_H */
