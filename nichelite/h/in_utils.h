/*
 * FILENAME: in_utils.h
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
 * MODULE: TCP
 *
 * PORTABLE: yes
 */


#ifndef IN_UTILS_H
#define  IN_UTILS_H     1

#include "ipport.h"
#include "libport.h"

#ifndef MAXIOSIZE
#define MAXIOSIZE       156
#endif

struct GenericIO
{
   char *   inbuf;      /* Pointer to command line  */

   /* Function to send the output string  */
   int (* out)(long id, char * outbuf, int len);  

   /* Identifier for the IO device. For TCP it would represent a SOCKET */
   long id;

   /* Get a character input from the I/O device This is needed to
    * show scrollable items
    */
   int (*getch)(long id);
};

typedef struct GenericIO * GEN_IO ;
extern   unsigned long  pingdelay;


/* misc prototypes for TCP/IP demo system.*/
extern   void  hexdump(void * pio, void * buffer, unsigned len);
extern   char* nextcarg(char * args);
extern   int   std_out(long s, char * buf, int len);
extern   int   std_in(long s);
extern   int   con_page(void * pio, int line);

/* MINI stack does not support gracefull net shutdown. If the port
 * has not defined this call in ipport.h then map it to panic
 */
#ifndef netexit
#define netexit(err) exit(err);
#endif /* netexit */
 

/* Sometimes ns_printf can be #defined to dprintf, etc. In that case
 * skip the standard declaration.
 */

#ifndef ns_printf
extern   int   ns_printf(void * pio, char * format, ...);
#endif

/* InterNiche alloc/free entry points */
int   mh_stats(void * IO);
void  mheap_init(char * base, long size);
char* calloc1(unsigned size);
void  mem_free(char * buf);


#ifdef INICHE_LIBS
/* prototypes for routines in strlib.c file: */
int   strlen(char * str);
char* strcpy(char * str1, char * str2);
char* strncpy(char * str1, char * str2, int max);
int   strcmp(char * str1, char * str2);
char* strcat(char * str1, char * str2);
char* strchr(char * str, char chr);
int   strncmp(char * str1, char * str2, int length);
char * strstr(char * str1, char * str2);

int   atoi(char *);
#else /* not INICHE_LIBS, use compilers include files */
#include "string.h"
#endif   /* INICHE_LIBS */

/* prototypes for routines in strilib.c file: */
#ifdef INICHE_STRICMP
int   stricmp(const char * s1, const char * s2);
#endif   /* INICHE_STRICMP */
#ifdef INICHE_STRNICMP
int   strnicmp(const char * s1, const char * s2, int len);
#endif   /* INICHE_STRNICMP */

#ifdef INICHE_STRISTR
char* stristr(char * str1, char * str2);
#endif

#endif   /* IN_UTILS_H */

