/*
 * FILENAME: menu.h
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
 * Structure for simple application menu system.
 *
 * MODULE: TCP
 *
 *
 * PORTABLE: yes
 */


#ifndef MENU_H
#define  MENU_H   1

#include "in_utils.h"

/* a menu can be defined as an array of these structures. Selecting a 
 * key can invoke the corrosponding function. 
 */

struct menu_op                /* structure to describe each menu item */
{
   char *   opt;              /* the option name */
   int (*func)(void * pio);   /* routine name to execute the option */
   char *   desc;             /* description of the option */
};

#define  CBUFLEN     128            /* size of cbuf */
extern   char        cbuf[CBUFLEN]; /* user command line buffer */

#define  MAX_MENUS   25
extern   struct menu_op *  menus[MAX_MENUS];    /* master menu list */

/* menu routine prototypes: */
extern   int      install_menu(struct menu_op * newmenu);   /* add new menu */
extern   char *   getcmd(void);           /* get cmdline into cbuf while spinning tasks */
extern   void     showmenu(void * pio, struct menu_op *);
extern   int      help(void * pio);       /* display menu prompt text */
extern   int      stooges(void * pio);    /* called when system breaks */
extern   int      do_command(void * pio); /* parse & perform command */
extern   char *   nextarg(char*);         /* get nexxt arg from a string */
extern   int      stlen(char*);           /* like strlen, but not exactly */
extern   int      stcmp(char*, char*);    /* like strcmp, but not exactly */
extern   int      setdebug (void * pio);
extern   int      arp_stats(void * pio);
extern   u_long   sysuptime(void);

#endif   /* MENU_H */

