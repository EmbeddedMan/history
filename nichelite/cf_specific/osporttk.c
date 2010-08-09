/*
 * FILENAME: osporttk.c
 *
 * Copyright 2005 By InterNiche Technologies Inc. All rights reserved
 *
 * Code to map NicheTask "TK_" macros. On the EVB port, this
 * is an alternative to the superloop build.
 *
 *
 * PORTABLE: NO
 */

#include "ipport.h"        /* from Interniche directory */

#ifdef INICHE_TASKS

#include "osport.h"

void * net_task_sem_ptr = NULL;

extern TK_ENTRY(tk_netmain);

int TotalTasks = 0;


/* FUNCTION: TK_NEWTASK()
 * 
 * PARAM1: struct inet_taskinfo * nettask
 *
 * RETURNS: 
 */

extern unsigned char far __SP_END[], __STACK_SIZE[];

extern struct inet_taskinfo nettasks[];

int
TK_NEWTASK(struct inet_taskinfo * nettask)
{
   task * new_task;

   /* for some ports on task package, nettask is a special case in
    * that it gets the compiler's runtime stack as it's task stack
    */
   if (nettask == &nettasks[0])  /* should be at head of list */
   {
      if (*(nettask->name) != 'I')  /* double check */
         panic("nettask");
      new_task = tk_init((stack_t *)(__SP_END), (int)(__STACK_SIZE));
   }
   else
   {
      new_task = tk_new(tk_cur, nettask->entry, 
                        nettask->stacksize, nettask->name, 0);
   }

   if (new_task)
   {
      *nettask->tk_ptr = new_task;
      return 0;
   }
   else
   {
      return 0;
   }
}


/* FUNCTION: TK_NETRX_BLOCK()
 * 
 * PARAM1: 
 *
 * RETURNS: 
 */

void
TK_NETRX_BLOCK(void)
{
   tk_block();   
}


/* FUNCTION: TK_APP_BLOCK()
 * 
 * PARAM1: void * event
 *
 * RETURNS: 
 */

void
TK_APP_BLOCK(void * event)
{
   if (event == NULL)
   {
      tk_next();
   }
   else
      tk_ev_block(event);
}

#endif   /* INICHE_TASKS */


