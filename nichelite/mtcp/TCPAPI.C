/*
 * FILENAME: tcpapi.c
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
 * The mini-sockets API for the mini TCP layer.
 *
 * MODULE: MTCP
 *
 * ROUTINES: m_connect(), m_listen(), m_send(), m_recv(), m_close(). m_ioctl(),
 * ROUTINES: m_socket(), tcp_pktalloc(), tcp_pktfree(), tcp_send(), tcp_recv(),
 * ROUTINES: m_getpeername(), 
 *
 * PORTABLE: yes
 */

#include "license.h"
#include "ipport.h"
#include "mtcp.h"


/* FUNCTION: m_socket()
 *
 * Allocates a socket structure for an active connect. 
 *
 * PARAM1: none
 *
 * RETURNS: returns the M_SOCK if OK, else NULL.
 */

M_SOCK
m_socket()
{
   M_SOCK so;

   so = (M_SOCK)SOC_ALLOC(sizeof(struct msocket));
   if (so)
   {
      LOCK_NET_RESOURCE(NET_RESID);    /* do net resource protection */
      putq(&msoq, so);                 /* put new socket in queue */
      UNLOCK_NET_RESOURCE(NET_RESID);
   }
   return(so);
}

/* FUNCTION: m_connect()
 *
 * Starts an active connect. If socket has been set to non-blocking and
 * no problems are detected it returns immediatly with EINPROGRESS.
 *
 * PARAM1: socket to start connect on
 * PARAM2: structure with port and address to conenct to.
 * PARAM3: pointer to callback routine for NB connects
 *
 * RETURNS: 0 if socket is connected, else BSD error code
 */


int
m_connect(M_SOCK so, struct sockaddr_in * sin, M_CALLBACK(name))
{
   struct tcpcb * tp;
   int e = 0;

   LOCK_NET_RESOURCE(NET_RESID);

   if(so->tp)  /* socket already has a tcpcb? */
   {
      e = EISCONN;
      goto rtn;
   }

   so->fhost = sin->sin_addr.s_addr;
   so->fport = sin->sin_port;
   if (so->lport == 0)
      m_setlport(so);
   tp = m_newtcpcb(so);
   if(!tp)
   {
      e = ENOMEM;
      goto rtn;
   }

   so->callback = name;             /* set callback routine */

   so->state |= SS_ISCONNECTING;    /* set state bitmask */
   tp->t_state = TCPS_SYN_SENT;     /* TCP state (after syn send) */
   tp->t_timer[TCPT_KEEP] = TCPTV_KEEP_INIT;
   tp->iss = tcp_iss;               /* setup initial SEQ number */
   tcp_iss += (tcp_seq)(TCP_ISSINCR/2);
   tcp_sendseqinit(tp);
   
   TCP_STAT_INC(tcps_connattempt);  /* Keep detailed (BSDish) stats */
   m_template(tp);                  /* set up header template for tcp sends */

   tcp_output(tp);                  /* send opening syn packet */

   if(so->state & SS_NBIO)    /* if non-blocking, return now */
   {
      /* Socket may have blocked to connect in tcp_output() and then been
       * set to non-blocking in connect callback - e.g., FTP server does this.
       */
      if(so->state & SS_ISCONNECTED)
         goto rtn;

      /* fall to here if it's really in progress */
      e = so->error = EINPROGRESS;
      goto rtn;
   }

   while(so->state & SS_ISCONNECTING)
   {
      tcp_sleep(so);
   }
   if(so->state & SS_ISCONNECTED)
      e = 0;
   else
      e = so->error;

rtn:
   UNLOCK_NET_RESOURCE(NET_RESID);
   return e;
}

/* FUNCTION: m_listen()
 *
 * Start a listen on the passed port (and optional IP address). The listen 
 * is implemented by creating a partially filled in M_SOCK and tcpcb. The socket
 * is returned to the caller for passing to later calls, like m_close(), but 
 * will never actually become a working connection. Any passive connects 
 * which succeed on this socket cause the callback routine to be called 
 * with a code of M_OPENED and passed a new, connected, socket.
 *
 *
 * PARAM1: struct sockaddr_in * - local port/foreign IP addr
 * PARAM2: callback routine
 * PARAM3: int * error - OUT - error return
 *
 * RETURNS: listening socket if success, else returns INVALID_SOCKET and
 * sets the passed error holder to one of the BSD error codes.
 */

M_SOCK
m_listen (struct sockaddr_in * sin, M_CALLBACK(name), int * error)
{
   M_SOCK   so;

   /* create a socket and tp (TCP control block (tcpcb) back pointer) to support the listen */
   so = m_socket();
   if(!so)
   {
      *error = ENOMEM;
      return INVALID_SOCKET;
   }

   LOCK_NET_RESOURCE(NET_RESID);
   so->fhost = sin->sin_addr.s_addr;
   so->lport = sin->sin_port;
   if (so->lport == 0)
      m_setlport(so);
   so->callback = name;
   
   so->tp = m_newtcpcb(so);
   if(!so->tp)
   {
      m_delsocket(so);
      *error = ENOMEM;
      so = INVALID_SOCKET;
      goto rtn;
   }
   so->tp->t_state = TCPS_LISTEN;

rtn:
   UNLOCK_NET_RESOURCE(NET_RESID);
   return so;
}


/* FUNCTION: tcp_send()
 *
 * Send a packet allocated via tcp_pktalloc(). User should have filled
 * data to be sent at pkt->m_data and set length in pkt->m_len.
 *
 * An OK return means the data is queued for sending and is now the 
 * responsability of the stack. An error return means the pkt has
 * NOT been queued or freed and is still owned by the caller.
 *
 * PARAM1: so - socket to send on. Must be open
 * PAMAR2: pkt - filled in data packet to send.
 *
 * RETURNS: 0 if OK or BSD error code.
 */

int
tcp_send(M_SOCK so, PACKET pkt)
{
   int err;

   LOCK_NET_RESOURCE(NET_RESID);

   /* make sure we are not overfilling the socket send buffer */
   while((pkt->m_len + so->sendq.sb_cc) > mt_deftxwin)
   {
      if((so->state & SS_ISCONNECTED) == 0)  /* not connected? */
         err = ENOTCONN;
      else if(so->state & SS_NBIO)    /* If non-blocking return now */
         err = EWOULDBLOCK;
      else
      {
         tcp_sleep(&so->sendq);  /* else wait for data ack */
         continue;
      }
      goto rtn;
   }

   /* setup packet protocol data pointers to TCP data */
   pkt->nb_prot = pkt->m_data;
   pkt->nb_plen = pkt->m_len;

   if(so->tp == NULL)      /* guard against TCP close by fhost */
   {
      err = EPIPE;         /* host killed connection */
   }
   else     /* connection seems OK, send it */
   {
      put_soq(&so->sendq, pkt);        /* place pkt in send que */
      err = tcp_output(so->tp);        /* call TCP send routine */
   }

rtn:
   UNLOCK_NET_RESOURCE(NET_RESID);
   return err;
}


/* FUNCTION: tcp_recv()
 *
 * Return next received packet on passed socket. Caller is responsible
 * for returning pkt to freeq via pk_free(). pkt->m_data points to data,
 * pkt->m_len is length of data.
 *
 * PARAM1: socket to receive on
 *
 * RETURNS: pkt if one is ready, NULL if no packet is ready and socket
 * is non-blocking.
 */

PACKET
tcp_recv(M_SOCK so)
{
   PACKET pkt;

   LOCK_NET_RESOURCE(NET_RESID);    /* do net resource protection */

   pkt = NULL;

   if(so->rcvdq.p_head)
      goto returnit;

   /* non-blocking sockets return a null now */
   if(so->state & SS_NBIO)
      goto returnpkt;

   /* wait till blocking socket gets data or disconnects */
   while(so->rcvdq.p_head == NULL)
   {
      if((so->state & SS_ISCONNECTED) == 0)
         goto returnpkt;
      UNLOCK_NET_RESOURCE(NET_RESID);
      tk_yield();
      LOCK_NET_RESOURCE(NET_RESID);
   }

returnit:
   pkt = get_soq(&so->rcvdq);
returnpkt:
   UNLOCK_NET_RESOURCE(NET_RESID);
   return pkt;
}



/* FUNCTION: m_ioctl()
 *
 *    Implement selected SO_ options from socket.h. This one routine
 * maintains both the so->so_options (socket options) and tp->t_state
 * (TCP ioclt) masks.
 *
 * PARAM1: 
 *
 * RETURNS: 
 */

int
m_ioctl(M_SOCK so, int option, void * data)
{
   int e = 0;

   LOCK_NET_RESOURCE(NET_RESID);

   /* map iniche type NBIO to BSD type option */
   if(option == SO_NBIO)
   {
      if((data == NULL) ||    /* treat null as a pointer to zero */
         (*(int*)data != 0))  /* set the masks for non-blocking */
      {
         so->so_options |= SO_NBIO;
         option = SO_NONBLOCK;
      }
      else     /* set socket and tp masks for blocking */
      {
         so->so_options &= ~SO_NBIO;
         option = SO_NBIO;
      }
   }

   switch (option)
   {
      case SO_NONBLOCK:
         so->state |= SS_NBIO;
         break;
      case SO_BIO:
         so->state &= ~SS_NBIO;
         break;
      case SO_DEBUG:    /* toggle debug option based on data as ptr to boolean */
         if(*(int*)data == 0)    /* (*data) is FALSE, clear debug bit */
            so->so_options &= ~SO_DEBUG;
         else
            so->so_options |= SO_DEBUG;
         break;      
      case SO_LINGER:
         /* This mini ioctl only sets the linger option bit and has no
          * mechanism to clear it.
          */
         so->so_options |= SO_LINGER;
         so->linger = *(int*)data;  /* number of seconds */
         break;
      default:
         e = EOPNOTSUPP;
         /* FALLTHROUGH */
   }

   UNLOCK_NET_RESOURCE(NET_RESID);
   return e;
}


/* FUNCTION: m_close()
 *
 * close the socket
 *
 * PARAM1: 
 *
 * RETURNS: 
 */

int
m_close(M_SOCK so)
{
   struct tcpcb * tp;
   M_SOCK tmp;
   int e = 0;

   LOCK_NET_RESOURCE(NET_RESID);    /* do net resource protection */

   /* search msoq to make sure sock exists */
   for(tmp = (M_SOCK)msoq.q_head; tmp; tmp = tmp->next)
      if(tmp == so)
         break;
   if(tmp == NULL)      /* bogus or stale socket */
   {
      e = EINVAL;
      goto rtn;
   }
   if (so->tp == NULL)     /* tp already cleaned up */
   {
      m_delsocket(so);
      goto rtn;
   }
   tp = so->tp;      /* make a local copy of the tcpcb */

   /* mark socket as closed so it can be deleted by tcp_slowtimo()
    * after the tp is cleaned up
    */
   so->state |= SS_NOFDREF; 

   /* set state so that tcp_output() does the desired close */
   if((so->so_options & SO_LINGER) && (so->linger == 0))
   {
      /* application has explicitly set the socket to have a linger time 
       * of zero, which is how BSD let applications support TCP resets.
       * Support this here:
       */
      so_flush(so);                    /* flush socket data queues */
      tp->t_state = TCPS_CLOSED;   /* set up to send reset */
   }
   else     /* adjust the state for normal shutdown */
   {
      switch(tp->t_state)
      {
      case TCPS_ESTABLISHED:
      case TCPS_SYN_RECEIVED:
         tp->t_state = TCPS_FIN_WAIT_1;
         break;
      case TCPS_LISTEN:
      case TCPS_SYN_SENT:
         tp->t_state = TCPS_CLOSED;    /* this will force tp deletion below */
         break;
      case TCPS_CLOSE_WAIT:
         tp->t_state = TCPS_LAST_ACK;
         break;
      default:
         dtrap();    /* other states shouldn't happen */
         break;
      }
   }

   if (tp->t_template)
      tcp_output(tp);        /* send final data then FIN, or RST */

   if (tp && (tp->t_state == TCPS_CLOSED))
   {
      m_tcpclose(tp);
      tp=NULL;
   }
   /* if close was forced (LINGER & longer == 0) or tp was closed in tcp_output()
    * (e.g. we sent a reset packet) then delete socket now 
    */
   if (tp == NULL)
      m_delsocket(so);

rtn:
   UNLOCK_NET_RESOURCE(NET_RESID);

   return e;
}


/* FUNCTION: tcp_pktalloc()
 *
 * Allcate a packet for sending tcp data. when returned, pkt->nb_prot
 * points to a buffer big enough for the data size passed.
 *
 * PARAM1: size of TCP data for packet, limited to MTU - header size
 *
 * RETURNS: pointer to a packet, or NULL if a big enough packet was not
 * available.
 */

PACKET
tcp_pktalloc(int datasize)
{
   PACKET   pkt;
   int      headersize = 40 + MaxLnh;

   /* Don't exhaust the queues grabing send packets, or we won't be able to
    * receive any acks and the sends will be stuck in the socket queues
    * forever.
    */
   if((lilfreeq.q_len <= 2) &&
      (bigfreeq.q_len <= 1))
   {
      return NULL;
   }

   LOCK_NET_RESOURCE(FREEQ_RESID);
   pkt = pk_alloc(datasize + headersize);
   UNLOCK_NET_RESOURCE(FREEQ_RESID);
   if(!pkt)
      return NULL;

   /* default nb_prot starts after tcp header */
   pkt->nb_prot = pkt->nb_buff + headersize;
   pkt->nb_plen = pkt->m_len = 0;   /* no data in new packet */
   pkt->m_data = pkt->nb_prot;      /* assume tcp data will start at nb_prot */
   pkt->m_next = NULL;

   return pkt;
}


/* FUNCTION: tcp_pktfree()
 *
 * tcp_pktfree(PACKET p) - free a packet allocated by (presumably) 
 * tcp_pktalloc(). This is a simple wrapper around pk_free() to lock 
 * and unlock the free-queue resource. 
 *
 * 
 * PARAM1: PACKET p
 *
 * RETURNS: 
 */

void
tcp_pktfree(PACKET p)
{
   LOCK_NET_RESOURCE(FREEQ_RESID);
   pk_free(p);
   UNLOCK_NET_RESOURCE(FREEQ_RESID);
}


#ifdef BSDISH_SEND

/* FUNCTION: m_send()
 *
 * A workalike for the BSD sockets send() call
 *
 * PARAM1: M_SOCK socket,
 * PARAM2: char * buffer
 * PARAM3: unsigned length
 *
 * RETURNS: number of bytes actually sent, or -1 if error.
 */

int
m_send(M_SOCK so, char * data, unsigned datalen)
{
   PACKET   pkt;
   struct tcpcb * tp;
   unsigned sent, tosend;
   int      err;
   
   LOCK_NET_RESOURCE(NET_RESID);

   /* make sure connection is established. */
   tp = so->tp;
   if((tp == NULL) || (tp->t_state < TCPS_ESTABLISHED))
   {
      so->error = ESHUTDOWN;
      UNLOCK_NET_RESOURCE(NET_RESID);
      return -1;
   }

   sent = 0;
   while(datalen > 0)
   {
      tosend = min(datalen, TCP_MSS);
      pkt = tcp_pktalloc(tosend);
      if(!pkt)
      {
         /* handle out-of-packets condition. */
         if((so->state & SS_NBIO) == 0)
         {
            /* blocking socket; wait for packets to become free */
            tcp_sleep(&so->sendq);  /* let system spin a bit */

            if((so->state & SS_ISCONNECTED) == 0)
            {
               /* socket closed while waiting, report this to caller. */
               so->error = ESHUTDOWN;
               goto rtnerr;
            }
            continue;      /* else loop back to wait some more */
         }

         /* fall to here if this is a non-blocking socket */
         if(sent == 0)     /* no bytes went out */
         {
            so->error = EWOULDBLOCK;
            goto rtnerr;
         }
         /* return number of bytes we sent before running out of buffers. */
         return sent;
      }
      MEMCPY(pkt->m_data, data, tosend);
      pkt->m_len = tosend;    /* set length in packet mbuf vars */

sendagain:
      UNLOCK_NET_RESOURCE(NET_RESID);
      err = tcp_send(so, pkt);   /* pass packet to tcp layer */
      LOCK_NET_RESOURCE(NET_RESID);

      /* See if socket is full to legal limit, */
      if(err == EWOULDBLOCK)
      {
         /* If socket is non-blocking, return now */
         if(so->state & SS_NBIO)
         {
            tcp_pktfree(pkt);    /* free last packet we allocated */
            if(sent == 0)        /* if no data went out, set error to explain why */
            {
               so->error = err;
               goto rtnerr;
            }
            goto rtn;           /* else return amount of data sent OK */
         }

         /* else sleep until some data is acked by remote host. */
         tcp_sleep(&so->sendq);
         goto sendagain;
      }
      else if(err)   /* error other than EWOULDBLOCK */
      {
         /* Return any other error code. Return -1 instead of "sent" so 
          * caller knows there was a hard error.
          */
         so->error = err;
         goto rtnerr;
      }
      /* packet sent OK, adjust data variables for next loop */
      data += tosend;         /* fix data pointer for amount copied */
      datalen -= tosend;      /* ...and data lengnth */
      sent += tosend;         /* ...and local byte counter */
   }

rtn:
   UNLOCK_NET_RESOURCE(NET_RESID);
   return sent;
rtnerr:
   UNLOCK_NET_RESOURCE(NET_RESID);
   return -1;
}

#endif   /* BSDISH_SEND */

#ifdef BSDISH_RECV

/* FUNCTION: m_recv()
 *
 * A workalike for the BSD sockets recv() call
 *
 * PARAM1: M_SOCK socket,
 * PARAM2: char * buffer
 * PARAM3: unsigned length
 *
 * RETURNS: number of bytes actually read, or -1 if error. 
 */

int
m_recv(M_SOCK so, char * buf, unsigned buflen)
{
   PACKET   pkt;
   int      tocopy;
   int      len;
   
   LOCK_NET_RESOURCE(NET_RESID);

   len = 0;    /* amount we have copied */

   /* handle the no data case first */
   while(so->rcvdq.sb_cc == 0)
   {
      /* If socket is disconnected (or disconnecting), indicate
       * this to the caller by returning a zero.
       */
      if((so->state & SS_CANTRCVMORE) 
#ifdef MINI_TCP_OOSQ
         && (so->oosq.p_head == NULL)
#endif
         )
      {
         so->error = ESHUTDOWN;
         goto rtn;
      }
      /* If THE socket is non-blocking and no data is ready, it 
       * returns -1 and error is set to EWOULDBLOCK.
       */
      if(so->state & SS_NBIO)
      {
         so->error = EWOULDBLOCK;
         len = -1;
         goto rtn;
      }
      UNLOCK_NET_RESOURCE(NET_RESID);
      tk_yield();
      LOCK_NET_RESOURCE(NET_RESID);
   }

   /* fall to here if socket has received data in queue */
   while((buflen > 0) && so->rcvdq.sb_cc)
   {
      /* move data from 1st pkt to caller's buffer */
      pkt = (PACKET)so->rcvdq.p_head;
      tocopy = min(buflen, pkt->m_len);
      MEMCPY(buf, pkt->m_data, tocopy);
      buflen -= tocopy;
      buf += tocopy;
      len += tocopy;

       /* dequeue data we moved */
      if(tocopy >= (int)pkt->m_len)
      {
         /* we moved whole packet, free it */
         tcp_pktfree(get_soq(&so->rcvdq));

         /* see if we need to update TCP rcv window */
         tcp_output(so->tp);
      }
      else  /* acked a partial packet */
      {
         /* drop data from head of packet */
         pkt->m_data += tocopy;
         pkt->m_len -= tocopy;
         so->rcvdq.sb_cc -= tocopy;
      }
   }
#ifdef NPDEBUG
   if(len == 0)
   {
      dtrap();    /* should never happen */
   }
#endif
rtn:
   UNLOCK_NET_RESOURCE(NET_RESID);
   return len;
}

#endif   /* BSDISH_RECV */

/* FUNCTION: tcpt_rangeset
 *
 * Force a time value to be in a certain range.
 *
 * PARAM1: short value,
 * PARAM2: short min.
 * PARAM3: short max.
 *
 * RETURNS: the time value, set within the range.
 */

short
tcpt_rangeset(short value, short tvmin, short tvmax)
{
   short tv = value; 

   if (value < tvmin)
      tv = (short)tvmin;
   else if (value > tvmax)
      tv = (short)tvmax;

   return tv;
}

#ifdef BSDISH_GETPEERNAME

/* m_getpeername() - NicheLite workalike to the BSD getpeername() */

int
m_getpeername(SOCKTYPE sock, struct sockaddr_in * addr)
{
   LOCK_NET_RESOURCE(NET_RESID);
   addr->sin_addr.s_addr = sock->fhost;
   addr->sin_port = sock->lport;
   UNLOCK_NET_RESOURCE(NET_RESID);

   return 0;   /* no real error checking */
}

#endif  /* BSDISH_GETPEERNAME */
