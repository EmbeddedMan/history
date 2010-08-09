/*
 * FILENAME: userpass.h
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
 * User & password defines for demo TCP/IP application. 
 * This is used by FTP, WebPort, PPP(pap), and any other network 
 * protocols which need a user name and password. 
 *
 * MODULE: TCP
 *
 *
 * PORTABLE: yes
 */


#ifndef _USERPASS_H_
#define  _USERPASS_H_   1


/* these #defines can be overridden from ipport.h */
#ifndef NUM_NETUSERS
#define  NUM_NETUSERS   1
#endif

#ifndef MAX_USERLENGTH
#define  MAX_USERLENGTH 8
#endif


/* the "app codes" for chgeck_permit() */
#define  HTTP_USER      1
#define  FTP_USER       2
#define  PPP_USER       3
#define  TELNET_USER    4

struct userpass
{
   char  username[MAX_USERLENGTH];
   char  password[MAX_USERLENGTH];
   void *   permissions;   /* for use by porting apps. */
};

extern   struct userpass   users[NUM_NETUSERS];
typedef  struct userpass   USER;

int    add_user(char * username, char * password, void *);
int    check_permit(char * username, char * password, int appcode, void *);
char * get_password(char * user);

#endif   /* _USERPASS_H_ */


