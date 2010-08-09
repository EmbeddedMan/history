/*
 * FILENAME: q.h
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
 * Software Definitions for the basic queing package. 
 *
 * MODULE: INET
 *
 * PORTABLE: yes
 */


#ifndef _Q_H_
#define  _Q_H_ 1

struct   q_elt    {     /* queue element: cast to right type */
   struct   q_elt   *   qe_next; /* it's just a pointer to next elt */
};

typedef struct q_elt * qp;    /* q pointer */

#define  q_elt qp

typedef   struct   queue   
{        /* queue header */
   q_elt q_head;        /* first element in queue */
   q_elt q_tail;        /* last element in queue */
   int   q_len;         /* number of elements in queue */
   int   q_max;         /* maximum length */
   int   q_min;         /* minimum length */
} queue;

/* The following macros implement most of the common queue operations */

/* Add an element to the head of the queue */

#define   q_addh(q, elt)    { \
   if ((q)->q_head == 0) (q)->q_tail = (elt); \
   (elt)->qe_next = (q)->q_head; \
   (q)->q_head = (elt); \
   (q)->q_len++; \
   if(++((q)->q_len) > (q)->q_max) (q)->q_max = (q)->q_len; \
}

/* Add an element to the tail of a queue */

#define   q_addt(q, elt)   { \
   (elt)->qe_next = 0; \
   if ((q)->q_head == 0) { \
      (q)->q_head = (elt); \
   } else { \
      (q)->q_tail->qe_next = (elt); \
   } \
   (q)->q_tail = (elt); \
   if(++((q)->q_len) > (q)->q_max) (q)->q_max = (q)->q_len; \
}

/* Add an element after a specified element in the queue.  If prev == */
/* &q->q_head, can be used to add an element to the head of the queue */

#define   q_adda(q, prev, new)   { \
   if ((q)->q_tail == (prev) || (q)->q_tail == 0) { \
      (q)->q_tail = (new); \
   } \
   (new)->qe_next = (prev)->qe_next; \
   (prev)->qe_next = (new); \
   if(++((q)->q_len) > (q)->q_max) (q)->q_max = (q)->q_len; \
}

/* Delete an element from a queue, given a pointer to the preceeding element */
/* Will delete the first element if prev == &q->q_head */

#define   q_dela(q, elt, prev)   { \
   if ((q)->q_tail == (elt)) { \
      if ((q)->q_head == (elt)) \
         (q)->q_tail = 0; \
      else \
         (q)->q_tail = (prev); \
   } \
   (prev)->qe_next = (elt)->qe_next; \
   (elt)->qe_next = 0; \
   if(--((q)->q_len) < (q)->q_min) (q)->q_min = (q)->q_len; \
}

/* The above MACROs are can be hard to port because of lack of 
 * prototyping, compile time snags, and no built in protection from 
 * premeption. Though they are fast, Code is more portable if
 * the queues are accessed via calls to the following routines:
 */

/* full prototypes: */
void     putq(queue*, void*);    /* add item to tail of queue */
void *   getq(queue*);           /* de-queue item from head of queue */
queue *  createq(void);
qp       qdel(queue*, void*);    /* delete an item from a queue */

#define  q_add(q,p)  putq(q,p)   /* allow old style names */
#define  q_deq(q) getq(q)
#define  aq_deq(q)   getq(q)     /* putq() and getq() are always atomic */
#define  q_create()  createq() 

#endif   /* _Q_H_ */


