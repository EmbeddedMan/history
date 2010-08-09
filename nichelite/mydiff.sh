find . -name \*.[chs] |
while read i; do
  if [ -f /ColdFire_Lite_CW6.4.ro/src/projects/NicheLite/Source/$i ]; then
    d=`diff /ColdFire_Lite_CW6.4.ro/src/projects/NicheLite/Source/$i $i`
    if [ $? != 0 ]; then
      echo "######## $i ########"
      echo "$d"
      echo
    fi
  fi
done

for i in *.[ch]; do
  if [ -f /ColdFire_Lite_CW6.4.ro/src/drivers/ethernet/$i ]; then
    d=`diff /ColdFire_Lite_CW6.4.ro/src/drivers/ethernet/$i $i`
    if [ $? != 0 ]; then
      echo "######## $i ########"
      echo "$d"
      echo
    fi
  fi
done

exit 0


######## ./cf_specific/iutil.c ########
403a404
> #if 0  // we use our own vprintf() and vsprintf()
436a438
> #endif

######## ./cf_specific/osporttk.c ########
33,34c33
< extern int __SP_END;       /* stack information from linker */
< extern int __STACK_SIZE;
---
> extern unsigned char far __SP_END[], __STACK_SIZE[];
50c49
<       new_task = tk_init((stack_t *)(&__SP_END), (int)(&__STACK_SIZE));
---
>       new_task = tk_init((stack_t *)(__SP_END), (int)(__STACK_SIZE));

######## ./cf_specific/tk_util.s ########
113a114
>    move.l  A5,40(A0)            // _SDA_BASE

######## ./h/ip.h ########
428c428
< unsigned short cksum(void*, unsigned);
---
> __declspec(compact_abi) unsigned short cksum(void*, unsigned);

######## ./h/ipport.h ########
81a82,86
> extern int fsys_frequency;
> #define SYS_CLK_MHZ  (fsys_frequency/1000000)
> #define __interrupt__  __declspec(interrupt)
>
>
92,93c97,98
< #define IN_MENUS        1  /* support for InterNiche menu system */
< #define NET_STATS       1  /* include statistics printfs */
---
> //#define IN_MENUS        1  /* support for InterNiche menu system */
> //#define NET_STATS       1  /* include statistics printfs */
106c111
< #define INICHE_TIMERS   1     /* Provide Interval timers */
---
> //#define INICHE_TIMERS   1   /* Provide Interval timers */
115c120
< #define TK_STDIN_DEVICE 1   /* Include stdin (uart) console code */
---
> //#define TK_STDIN_DEVICE 1   /* Include stdin (uart) console code */
274,275c279,280
< #define NUMBIGBUFS   8            //FSL stack not work for <4
< #define NUMLILBUFS   6
---
> #define NUMBIGBUFS   4            //FSL stack not work for <4
> #define NUMLILBUFS   16
287c292
< extern  int  dprintf(char * format, ...);
---
> //extern  int  dprintf(char * format, ...);

######## ./h/task.h ########
90,92c90,92
< stack_t * tk_frame(task *, int(*)(int), unsigned);
< void      tk_switch(task *);  /* run the next task */
< stack_t * tk_getsp(void);             /* get current stack pointer */
---
> __declspec(compact_abi) stack_t * tk_frame(task *, int(*)(int), unsigned);
> __declspec(compact_abi) void      tk_switch(task *);  /* run the next task */
> __declspec(compact_abi) stack_t * tk_getsp(void);             /* get current stack pointer */

