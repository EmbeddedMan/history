/*
 * Filename   : osport.h 
 *
 * Copyright 2005 By InterNiche Technologies Inc. All rights reserved
 *
 * Definitions to map NicheTask "TK_" macros. 
 *
 * This version for ColdFire CPU, CodeWarrior IDE
 *
 * PORTABLE: NO
 */

#ifdef INICHE_TASKS

#include "q.h"
#include "netbuf.h"
#include "net.h"
#include "tk_ntask.h"

#include "app_ping.h"

#define  TK_RETURN_UNREACHABLE()  /* define to nothing */

/* task stack sizes */
#define  NET_STACK_SIZE 	2048
#define  APP_STACK_SIZE 	2048
#define  CLOCK_STACK_SIZE 	2048

#define  IO_STACK_SIZE     2048
#define  WEB_STACK_SIZE    APP_STACK_SIZE
#define  FTP_STACK_SIZE    APP_STACK_SIZE
#define  PING_STACK_SIZE   2048
#define  TN_STACK_SIZE     APP_STACK_SIZE
#define  IKE_STACK_SIZE    APP_STACK_SIZE

/* table with an entry for each internet task/thread. This is filled
in the netmain.c, so it should have the same values in the same order 
in all ports */

struct inet_taskinfo {
   TK_OBJECT_PTR(tk_ptr);  /* pointer to static task object */
   char * name;            /* name of task */
   TK_ENTRY_PTR(entry);    /* pointer to code to start task at */
   int   priority;         /* priority of task */
   int   stacksize;        /* size (bytes) of task's stack */
};

int   TK_NEWTASK(struct inet_taskinfo * nettask);
void  TK_NETRX_BLOCK(void);

//FSL should remove if not used
//   TK_WAKE(&to_pingcheck);    /* wake ping task for later sends */

#endif   /*  INICHE_TASKS */


