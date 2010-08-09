/*
 * FILENAME: dhcputil.c
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
 * DHCP utilities which apply to both client and server
 *
 * MODULE: INET
 *
 * ROUTINES: find_opt(),  *
 * PORTABLE: yes
 */

#include "license.h"
#include "ipport.h"


#ifdef DHCP_CLIENT   /* find_opt() is used only by DHCP client */

#include "dhcpclnt.h"


/* FUNCTION: find_opt()
 *
 * find_opt() - Search an options string for a particular option 
 * code. 
 *
 * 
 * PARAM1: u_char opcode
 * PARAM2: u_char * opts
 *
 * RETURNS:  Return pointer to that code if found, NULL if not found.
 */

u_char * 
find_opt(u_char opcode, u_char * opts)
{
   u_char * end   =  opts  +  DHCP_OPTSIZE;  /* limit scope of search */

   while (opts < end)
   {
      if (*opts == opcode) /* found it */
         return opts;
      if (*opts == DHOP_END)  /* end of options; opcode not found */
         return NULL;
      if (*opts == DHOP_PAD)  /* PAD has only 1 byte */
         opts++;
      else     /* all other options should have a length field */
         opts += (*(opts+1))+2;
   }
   /* no DHOP_END option?? */
   return NULL;
}

#endif   /* DHCP_CLIENT */

