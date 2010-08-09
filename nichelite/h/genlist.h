/*
 * FILENAME: GENLIST.H
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
 * MODULE: MISCLIB
 *
 * GENERIC Implementation of a list. 
 *
 * ROUTINES:
 *
 * PORTABLE: yes
 */


#ifndef GENLIST_H    /* Make sure the header file is included only once */
#define  GENLIST_H            1

#include "ipport.h"

/* Generic list is used by SNMPv3 code and IPFILTER code. */
#if (defined(INCLUDE_SNMPV3) || defined(USE_IPFILTER) || defined(INICHE_SYSLOG))
#define USE_GENLIST     1
#endif

#define  GEN_MAX_ARRAY  10

#ifndef GEN_ALLOC
#define GEN_ALLOC(size) npalloc(size)
#endif
#ifndef GEN_FREE
#define GEN_FREE(ptr)   npfree(ptr)    
#endif

#ifndef MEMCMP
#define MEMCMP          memcmp
#endif
#ifndef MEMCPY
#define MEMCPY          memcpy
#endif

#ifndef SUCCESS
#define SUCCESS         0
#endif
#ifndef FAILURE
#define FAILURE         1
#endif


#ifndef MAX_NAME_LEN      /* usually defined in snmpport.h */
#define MAX_NAME_LEN    64  /* max number of subid's in a objid */
#endif

/* NicheElement had two members. One is a pointer to the DATA and the 
 * other is a pointer to the next element. If there is no next 
 * element, the pointer is NULL. GEN_STRUCT can be anything. 
 * Following are some thoughts on the way things are declared and 
 * intended. 
 * 1. If there is only one list, GEN_STRUCT can be 
 *    explicitly named to the STRUCT/CLASS 
 * 2. If there are multiple lists, then GEN_STRUCT can be "void*" 
 * and casting can be done whenever we are using it 
 * 3. If we need a CLASS implementation, all the functions can be 
 *   made as interfaces of the CLASS. So we just 
 *   need a class wrapper. Further, it is assumed that GEN_STRUCT has 
 *   two members defined as follows 
 *      "long id;" 
 *      "char name[MAX_NAME_LEN];" 
 * The list can be a singly indexed list (that 
 * is all elements can be uniquely identified by their IDs). Or it 
 * can be doubly indexed list (all elements can be uniquely 
 * identified by their ID and name combined). Lookup and deletion 
 * from the list has been provided for this use. 
 */
struct TemplateStruct
{
   long     id;
   char     name[MAX_NAME_LEN];
};

typedef struct TemplateStruct * GEN_STRUCT ;

struct NicheElement
{
   GEN_STRUCT  p_data;           /* Pointer to element/data */       
   struct     NicheElement *  next; /* Pointer to next data element */
};

typedef struct NicheElement * NICHE_ELE;


struct NicheList
{
   NICHE_ELE   head  ;     /* First element of the list */
   int         len_of_element;/* Len   of the   struct representing  element/data*/
};

typedef struct NicheList * NICHELIST;


int        niche_list_constructor  (NICHELIST list, int len_of_ele);
int        niche_list_destructor   (NICHELIST list);
int        niche_add               (NICHELIST list, GEN_STRUCT ptr_data);
int        niche_add_sorted        (NICHELIST list, GEN_STRUCT ptr_data);
int        niche_add_id_and_name   (NICHELIST list, long id, char * name);
int        niche_del               (NICHELIST list, GEN_STRUCT ptr_data);
int        niche_del_id            (NICHELIST list, long id);
int        niche_del_name          (NICHELIST list, char * name);
int        niche_del_id_and_name   (NICHELIST list, long id, char * name);
GEN_STRUCT niche_lookup_id         (NICHELIST list, long id);
GEN_STRUCT niche_lookup_name       (NICHELIST list, char *name);
GEN_STRUCT niche_lookup_id_and_name(NICHELIST list, long id, char *name);
int        niche_lookup_multi_match(NICHELIST list, long id, char * name, 
              GEN_STRUCT matches[]);
int        niche_list_show         (NICHELIST list);
int        niche_list_len          (NICHELIST list); 
GEN_STRUCT niche_list_getat        (NICHELIST list,int index); 
int        niche_element_show      (GEN_STRUCT ptr_data);



#define  NICHE_ERRBASE                    2000

#define  NICHE_NO_MEM                     (NICHE_ERRBASE+1)
#define  NICHE_NO_MEM_FOR_DATA            (NICHE_ERRBASE+2)
#define  NICHE_ADD_NOT_ENOUGH_MEMORY      (NICHE_ERRBASE+3)
#define  NICHE_DEL_LIST_EMPTY             (NICHE_ERRBASE+4)
#define  NICHE_DEL_NOT_FOUND              (NICHE_ERRBASE+5)
#define  NICHE_DEL_NOT_ENOUGH_MEMORY      (NICHE_ERRBASE+6)
#define  NICHE_LISTPTR_INVALID            (NICHE_ERRBASE+7)
#define  NICHE_DUP_ENTRY                  (NICHE_ERRBASE+8)


#endif   /*  GENLIST_H */

