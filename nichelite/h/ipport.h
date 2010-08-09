/* ipport.h
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
 * All the platform specific defines for each port go here. 
 * All these should be re-examined with each new port of the code.
 * 
 */

#ifndef _IPPORT_H_
#define _IPPORT_H_ 1

#include "common.h"

#include <stdio.h>		/* C compiler files */
#include <stdlib.h>		/* C compiler files */

/* Data types not covered in CodeWarrior IDE */
typedef  unsigned char  u_char;
typedef  unsigned short u_short;
typedef  unsigned int   u_int;
typedef  unsigned long  u_long;
typedef  unsigned short unshort;
typedef  unsigned long  ulong;
typedef  unsigned long  ip_addr;
typedef           char  *caddr_t;

#ifndef TRUE
#define  TRUE	-1
#endif
#ifndef FALSE
#define  FALSE   0
#endif

#include "string.h"

/* Keep the Iniche nptypes.h file out of this build */
#define  NPTYPES_H   1

// FSL - Added powerup configuration support
//       This variable is set in mcfxxxx_sysinit.c
#ifdef M52233DEMO								//FSL currently only M52233DEMO has SW1 and SW2 to work with
extern uint8 powerup_config_flags;
#define POWERUP_CONFIG_SW1				0x01
#define POWERUP_CONFIG_SW2				0x08

#define POWERUP_CONFIG_DHCP_ENABLED		(powerup_config_flags&POWERUP_CONFIG_SW1)
#define POWERUP_CONFIG_ALT_IP			(powerup_config_flags&POWERUP_CONFIG_SW2)
#else
#define POWERUP_CONFIG_DHCP_ENABLED		DHCP	//FSL 0=disable, 1=enable DHCP ...this is important #define because most evb's do not have button to use to select DHCP option
#define POWERUP_CONFIG_ALT_IP			0		//FSL use default IP address
#endif
/*
 * Option macros to trade off features for size. Do not enable options
 * for modules you don't have or your link will get unresolved
 * externals.  
 */

extern int cpu_frequency;
#define SYS_CLK_MHZ  (cpu_frequency/1000000)
#define __interrupt__  __declspec(interrupt)


#define INCLUDE_ARP     1  /* use Ethernet ARP */
#define FULL_ICMP       1  /* use all ICMP || ping only */
#define OMIT_IPV4       1  /* not IPV4, use with MINI_IP */
#define MINI_IP         1  /* Use Nichelite mini-IP layer */
#define MINI_TCP        1  /* Use Nichelite mini-TCP layer */
#define MINI_PING       1  /* Build Light Weight Ping App for Niche Lite */
#define BSDISH_RECV     1  /* Include a BSD recv()-like routine with mini_tcp */
#define BSDISH_SEND     1  /* Include a BSD send()-like routine with mini_tcp */
#define NB_CONNECT      1  /* support Non-Blocking connects (TCP, PPP, et al) */
#define MUTE_WARNS      1  /* gen extra code to suppress compiler warnings */
//#define IN_MENUS        1  /* support for InterNiche menu system */
//#define NET_STATS       1  /* include statistics printfs */
#define QUEUE_CHECKING  1  /* include code to check critical queues */
#define INICHE_TASKS    1  /* InterNiche multitasking system */
#define MEM_BLOCKS      1  /* list memory heap stats */

#ifdef TFTP_PROJECT
#define TFTP_CLIENT     1  /* include TFTP client code */
#define VFS_FILES       1  /* include Virtual File System */
#endif

//#define TFTP_SERVER     1  /* include TFTP server code */
//#define DNS_CLIENT      1  /* include DNS client code */
//#define DNS_CLIENT_UPDT 1	//FSL added this
//#define INICHE_TIMERS   1  	/* Provide Interval timers */
// To enable DHCP, uncomment the line below
#define DHCP_CLIENT     1  	/* include DHCP client code */
// #define INCLUDE_NVPARMS 1  /* non-volatile (NV) parameters logic */
// #define NPDEBUG         1  /* turn on debugging dprintf()s */
// #define USE_MEMDEV      1   /* Psuedo VFS files mem and null */
#define NATIVE_PRINTF   1   /* use target build environment's printf function */
#define NATIVE_SPRINTF  1   /* use target build environment's printf function */
#define PRINTF_STDARG   1   /* build ...printf() using stdarg.h */
//#define TK_STDIN_DEVICE 1   /* Include stdin (uart) console code */
#define BLOCKING_APPS   1   /* applications block rather than poll */
#define INCLUDE_TCP     1  	/* this link will include NetPort TCP w/MIB */

/**** end of option list ***/

/***** ColdFire specific options ***/

#define FREESCALE_MCF	    1	//FSL changed from Motorola to Freescale
#define USE_INTS            1
#define USE_FEC             1   /* Build with Freescale FEC drivers */


/* default setups of some sub-options */

#define exit_hook(x)
#define net_system_exit (FALSE)

extern int iniche_net_ready;

#define IP_TTL		64 		/* define IP hop count for this port */
#define FULL_ICMP	1 		/* force full ICMP for TCP support */
#define MAXNETS		1  		/* NicheLite only supports one network interface */

/* number of entries in IP routing table */
#define RT_TABS     16

/* #undef stuff from the CodeWarrior stdio.h which interferes with vfsfiles.h */

#ifdef getc
#undef getc
#endif
#ifdef ferror
#undef ferror
#endif

#ifdef VFS_FILES
#define HT_EXTDEV     1  /* allow custom file devices (memdev, null, etc.) */
#define DF_DEV        1  /* data flash device */
#define HT_RWVFS      1  /* support read/write VFS */                           
#define VFS_STRIPPATH 1  /* used to strip path for filenames in vfsfiles.c */
#define vfs_lock()       /* define pesky macros away */
#define vfs_unlock()
#endif /* VFS_FILES */

extern volatile unsigned long cticks;  			/* clock ticks since startup */

extern void PIT_Timer_Init(uint8, uint16);   	/* start clock tick counting; called at init time */ //FSL added the "extern" reference
void clock_c(void);      /* undo effects of clock_init (i.e. restore ISR vector */

#define TPS                200					//FSL 5ms=1/200 per tick

#define ETHHDR_SIZE  (14)

/* Prototype our heap routines */
extern   char *   npalloc(unsigned size);
extern   void     npfree(void * ptr);


/* ColdFire routine to get an aligned memory bugger */
char * memalign(unsigned, unsigned);

/* define the various IP stack block and buffer allocation routines */
#define RT_ALLOC(size)  npalloc(size)   /* route block alloc */
#define RT_FREE(ptr)    npfree(ptr)
#define NB_ALLOC(size)  npalloc(size)   /* netbuf structure alloc */
#define NB_FREE(ptr)    npfree(ptr)
#define UC_ALLOC(size)  npalloc(size)   /* UDP connection block alloc */
#define UC_FREE(ptr)    npfree(ptr)
#define TK_ALLOC(size)  npalloc(size)   /* task control block */
#define TK_FREE(ptr)    npfree(ptr)
#define STK_ALLOC(size) npalloc(size)   /* task stack */
#define STK_FREE(ptr)   npfree(ptr)

/* Special allocation routine for ColdFire FEC Ethernet, which
 * requires all receive buffers to be on 16 byte boundaries.
 */
extern	char *  alloc_uncached(int size);

#define BB_ALLOC(size)  alloc_uncached(size)   /* Big packet buffer alloc */
#define BB_FREE(ptr)    npfree(ptr)
#define LB_ALLOC(size)  alloc_uncached(size)   /* Little packet buffer alloc */
#define LB_FREE(ptr)    npfree(ptr)


/* map memory routines to standard lib */
#define MEMCPY(dst,src,size)  memcpy(dst,src,size)
#define MEMSET(ptr,val,size)  memset(ptr,val,size)
#define MEMCMP(p1,p2,size)    memcmp(p1,p2,size)
#define MEMMOVE(p1,p2,size)   memmove(p1,p2,size)

/* Macro to do non-portable address compares. Checks to see
 * if address p2 is inside the buffer p1 of length len1.
 */

#define IN_RANGE(p1, len1, p2) ( (p1 <= p2) && ((p1 + len1) > p2) ) 

/* Stack generic error codes: generally full success is 0,
 * definite errors are negative numbers, and indeterminate conditions
 * are positive numbers. These may be changed if they conflict with
 * defines in the target system. They apply to the IP stack,
 * and many of the applications. If you have to change
 * these values, be sure to recompile ALL sources.
 */

#define SUCCESS         0  /* whatever the call was, it worked */

/* programming errors */
#define ENP_PARAM     -10  /* bad parameter */
#define ENP_LOGIC     -11  /* sequence of events that shouldn't happen */
#define ENP_NOCIPHER  -12  /* No corresponding cipher found for the cipher id */

/* system errors */
#define ENP_NOMEM     -20  /* malloc or calloc failed */
#define ENP_NOBUFFER  -21  /* ran out of free packets */
#define ENP_RESOURCE  -22  /* ran out of other queue-able resource */
#define SEND_DROPPED ENP_RESOURCE /* full queue or similar lack of resource */
#define ENP_BAD_STATE -23  /* TCP layer error */
#define ENP_TIMEOUT   -24  /* TCP layer error */

#define ENP_NOFILE    -25  /* expected file was missing */
#define ENP_FILEIO    -26  /* file IO error */

/* net errors */
#define ENP_SENDERR   -30  /* send to net failed at low layer */
#define ENP_NOARPREP  -31  /* no ARP for a given host */
#define ENP_BAD_HEADER -32 /* bad header at upper layer (for upcalls) */
#define ENP_NO_ROUTE  -33  /* can't find a reasonable next IP hop */
#define ENP_NO_IFACE  -34  /* can't find a reasonable interface */
#define ENP_HARDWARE  -35  /* detected hardware failure */

/* conditions that are not really fatal OR success: */
#define ENP_SEND_PENDING 1 /* packet queued pending an ARP reply */
#define ENP_NOT_MINE     2 /* packet was not of interest (upcall reply) */

/* ARP holding packet while awaiting a response from fhost */
#define ARP_WAITING   ENP_SEND_PENDING

void dtrap(void); 			/* routine with permanent breakpoint */

#define _NPPP            4 	/* override ppp_port.h setting */

/* ...thus the structs we'd normally define packed are normal */
#define START_PACKED_STRUCT(sname) struct sname {
#define END_PACKED_STRUCT(sname)  };

/* define base & size of device memory */
#define  MEMDEV_BASE  0x000000
#ifdef USE_MEMDEV
#define  MEMDEV_SIZE  0x200000
#else
#undef  MEMDEV_SIZE
#endif /* USE_MEMDEV */

//FSL #define  TCP_MSS  2000
#define  TCP_MSS  1456

/* define number and sizes of free buffers */
//FSL overridding the settings in pk_alloc.c
#define NUMBIGBUFS   4	    //FSL stack not work for <4
#define NUMLILBUFS   16

/* some maximum packet buffer numbers */
#define MAXBIGPKTS   NUMBIGBUFS
#define MAXLILPKTS   NUMLILBUFS
#define MAXPACKETS   (MAXLILPKTS+MAXBIGPKTS)

/* FEC buffer descriptors */
#define NUM_RXBDS    2
#define NUM_TXBDS    2

#ifdef NATIVE_PRINTF
extern  int  dprintf(char * format, ...);
/* extern  int  printf(char * format, ...); */
#else
extern  void dprintf(char * format, ...);
/* debug printf */
#define printf dprintf
#endif

/* Send startup errors to the right place */
#define initmsg dprintf


/* net stats printf with redirectable device: */
extern int  ns_printf (void * pio, char * format, ...);

#define MAXSENDLOOPS 50  /* MAX number of FTP server send loops */

#define COMMSIZE     64  /* max bytes in community string */

/* get rid of GCC stdio.h version of putchar*/
#undef putchar

/* hook putchar to iniche primitive version */
#define putchar(_c)  dputchar(_c)

#define	atol(str)    atoi(str)

int  prep_evb(int first_iface);   /* set up interfaces */

/* hook in main.c to install our "prep" routine */
extern int (* port_prep)(int already_found);

#define min(a,b) (a>b?b:a)
#define max(a,b) (a>b?a:b)

/* Macro to get rid of "unused argument" warnings. With compilers that
 * can suppress these warnings, define this to nothing.
 */

#define USE_ARG(x) 
#define USE_VOID(x) USE_ARG(x)

#define MAX_NVSTRING   128       /* MAX length of a nparms string */

#define OSPORT_H        "osport.h"

/* pull in some NicheLite definitions */
#include "minip.h"

/* some TK_ macro support */
extern void TK_APP_BLOCK(void * event);

void  ENTER_CRIT_SECTION(void*p);
void  EXIT_CRIT_SECTION(void * p);

#define LOCK_NET_RESOURCE(r)
#define UNLOCK_NET_RESOURCE(r)

void  tk_yield(void);

#include "tk_ntask.h"		/* This has definition */
#define tcp_wakeup(ev) tk_ev_wake(ev)
#define tcp_sleep(ev)  tk_ev_block(ev)

#define NET_RESID   0    	/* stack code resource lock */
#define RXQ_RESID   1    	/* received packet queue resource lock */
#define FREEQ_RESID 2    	/* free packet queue resource lock */
#define CE_RES_ID   3
#define ACQUIRE_CE_RESOURCE(x)
#define RELEASE_CE_RESOURCE(x)

extern char * pre_task_setup(void);
extern char * post_task_setup(void);

#define  STK_TOPDOWN    1  	/* type of stack on CPU */

#define strrchr char_posr

extern char *char_pos(char *line, char val);
extern char *char_posr(char *line, char val);

#endif /* _IPPORT_H_ */

/* end of file ipport.h */