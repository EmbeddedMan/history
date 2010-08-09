/* tk_util.s
 *
 * Copyright 2005 by InterNiche Technologies Inc. All rights reserved.
 *
 * Metrowerks/ColdFire assembly code for NicheTask port.
 */


/* code exported from this file */
.global _tk_switch
.global _tk_frame
.global _tk_getsp

.extern _tk_cur

/* offsets into the C code task structure */
#define	tk_next       0
#define	tk_fp         4
#define	tk_name       8
#define	tk_flags      12
#define	tk_count      16
#define	tk_guard      20
#define	tk_size       24
#define	tk_stack      28


.text

/* void tk_switch(struct task * next_task) - yield CPU, swap in next 
runnable task. Runnabletask must be set into tk_cur here, because we 
may return to new task code, not the code which called us.
*/


_tk_switch:
   move.w  #0x2700,SR			/* disable interrupts */

   move.l   4(A7),D0			/* save passed tk pointer in D0 */
#if 0   
   move.l   D2,-(A7)			/* push the non-volitile gp registers */
   move.l   D3,-(A7)
   move.l   D4,-(A7)
   move.l   D5,-(A7)
   move.l   D6,-(A7)
   move.l   D7,-(A7)

   move.l   A1,-(A7)
   move.l   A2,-(A7)
   move.l   A3,-(A7)
   move.l   A4,-(A7)
   move.l   A5,-(A7)
   move.l   A6,-(A7)
#else
   suba.l	#48,a7				//FSL make room on the stack for saving registers (12 register * 4 bytes = 48bytes to save)
   movem.l	d2-d7/a1-a6,(A7)	//FSL store the registers on the stack (push)
#endif
   
   move.l   (_tk_cur),A1		/* get pointer to tk_cur - current task */
   move.l   A7,tk_fp(A1)		/* Save SP in task struct */

   move.l   D0,_tk_cur			/* Install new (passed) task */
   move.l   D0,A1				/* address new task */
   move.l   tk_fp(A1),A7		/* Install new task's stack */
#if 0
   move.l  (A7)+,A6				/* restore gp regs */
   move.l  (A7)+,A5
   move.l  (A7)+,A4
   move.l  (A7)+,A3
   move.l  (A7)+,A2
   move.l  (A7)+,A1

   move.l  (A7)+,D7
   move.l  (A7)+,D6
   move.l  (A7)+,D5
   move.l  (A7)+,D4
   move.l  (A7)+,D3
   move.l  (A7)+,D2
#else
   movem.l	(A7),d2-d7/a1-a6	//FSL store the registers on the stack (pop)
   adda.l	#48,a7				//FSL free stack space (12 register * 4 bytes = 48bytes to save)
#endif

   move.w  #0x2000,SR			/* enable ints */
   rts                          /* return in new task context */


/* 
 stack_t * tk_frame(task*, int (*proc)(int), int parm);

        Prepare a new task to run (low level). tk_stack and 
tk_size should be set before this is called. 

        This fills in the task frame as though the task had 
called tk_block(). When the round robin scheduler gets to it, it
should look like any other task ready to resume.

Returns address for new task->fp
*/


_tk_frame:
   link.w  A6,#0xFFEC

   move.w  #0x2700,SR			/* disable ints */

   move.l  28(A7),A0          	/* get passed task */
   move.l  tk_stack(A0),D1    	/* get it's stack base */
   add.l   tk_size(A0),D1     	/* add stack size - top of new stack */
   move.l  #68,D0
   sub.l   D0,D1              	/* deduct room for frame */

   move.l  32(A7),D0          	/* get passed function pointer */
   move.l  D1,A0              	/* address stack (FP) for new task */
   move.l  A5,40(A0)            // _SDA_BASE
   move.l  D0,48(A0)          	/* Install it in task's stack */
   move.l  D1,D0              	/* return task->fp */

   move.w  #0x2000,SR         	/* re-enable ints */

   unlk   A6
   rts



_tk_getsp:
   move.l  A7,D0      /* //FSL return stack pointer */
   rts




.end

