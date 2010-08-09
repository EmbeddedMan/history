/*
 * FILENAME: msock.h
 *
 * Copyright 2000 By InterNiche Technologies Inc. All rights reserved
 *
 * Definitions for the mini sockets API. This file should be included by
 * Applications which will use mini sockets.
 *
 * MODULE: MTCP
 *
 * PORTABLE: yes
 */

#ifndef _MSOCK_H
#define  _MSOCK_H  1

/* Mini-TCP's API emulates sockets wherev er it's possible to do so
 * with taking a huge efficency hit. To this end, we use some objects
 * with a name and design after BSD sockets:
 */

/* Berkeley style "Internet address" */

struct in_addr
{
   unsigned long  s_addr;
};

#define  INADDR_ANY     0L

#define  TCPIPHDRSZ     40

/* Berkeley style "Socket address" */

struct sockaddr_in
{
   short    sin_family;
   u_short  sin_port;
   struct   in_addr  sin_addr;
   char     sin_zero[8];
};

/* define the type of the msocket & it's callback routine */
struct msocket;   /* predeclaration */
#define M_SOCK   struct msocket *
#define M_CALLBACK(name)   int(* name)(int code, M_SOCK, void * data)

struct m_sockbuf
{
   PACKET   p_head;     /* oldest pkt in socket data queue */
   PACKET   p_tail;     /* newest pkt in socket data queue */
   unsigned sb_cc;      /* number of bytes tcp data in queue */
};

/* the msocket structure replaces both the socket and the inpcb
 * that the BSD code uses. We attach it to the BSD tcpcb structure
 * where the inpcb would go in a full bsd tcp. Thus this should
 * contain everything we need to handle a TCP connection which is
 * NOT in the tcpcb.
 */

struct tcpcb;      /* predecl */
 
struct msocket
{
   struct msocket * next;  /* queue link */
   unshort  lport;         /* IP/port tupple describing connection, local port */
   unshort  fport;         /* far side's port */
   ip_addr  lhost;         /* local IP address */
   ip_addr  fhost;         /* far side's IP address */
   struct tcpcb * tp;      /* back pointer to tcpcb */
   struct m_sockbuf sendq; /* packets queued for send, including unacked */
   struct m_sockbuf rcvdq; /* packets received but undelivered to app */
   struct m_sockbuf oosq;  /* packets received out of sequence */
   int      error;         /* last error, from BSD list */
   int      state;         /* bitmask of SS_ values from sockvar.h */
   int      so_options;    /* bitmask of SO_ options from socket.h */
   int      linger;        /* linger time if SO_LINGER is set */
   M_CALLBACK(callback);   /* socket callback routine */
   NET      ifp;           /* iface for packets when connected */
   char     t_template[40];/* tcp header template, pointed to by tp->template */   
   void *   app_data;      /* for use by socket application */
};

extern queue msoq; /* queue of existing msockets */

extern unsigned   mt_deftxwin;      /* default send window */
extern unsigned   mt_defrxwin;      /* default receive window */

#ifdef SYS_SOCKETNULL
#undef SYS_SOCKETNULL
#endif   /* SYS_SOCKETNULL */
#define SYS_SOCKETNULL (M_SOCK)((long)-1)

#ifdef INVALID_SOCKET
#undef INVALID_SOCKET
#endif   /* INVALID_SOCKET */
#define INVALID_SOCKET (M_SOCK)((long)-1)

/* define socket type - no "ifndef", be firm about this! */
typedef  struct msocket * SOCKTYPE;


/* macros to handle counting and adding to the BSD stype stats */
#ifdef NET_STATS
#define  TCP_STAT_INC(name)   (tcpstat.name++)
#define  TCP_STAT_ADD(name, amt)   (tcpstat.name += (long)(amt))
#else    /* no net statistics, define these away */
#define  TCP_STAT_INC(name)
#define  TCP_STAT_ADD(name, amt)
#endif   /* NET_STATS */

/* the "mini sockets" API calls - all a TCP layer really needs */
M_SOCK   m_socket(void);
int      m_connect(M_SOCK, struct sockaddr_in *, M_CALLBACK(name));
M_SOCK   m_listen(struct sockaddr_in *, M_CALLBACK(name), int * error);
int      m_ioctl(M_SOCK, int option, void * data);   /* uses SO_ options from socket.h */
int      m_close(M_SOCK);
int      tcp_send(M_SOCK, PACKET);
PACKET   tcp_recv(M_SOCK);

/* three more (optional) routines for gratuitous sockets compatability */
#ifdef BSDISH_SEND
int      m_send(M_SOCK, char * buf, unsigned datalen);
#endif
#ifdef BSDISH_RECV
int      m_recv(M_SOCK, char * buf, unsigned datalen);
#endif
#ifdef BSDISH_GETPEERNAME
int      m_getpeername(M_SOCK, struct sockaddr_in *);
#endif

#define tcp_errno(s) (s->error)     /* return BSD socket error */

/* callback op codes: */
#define M_OPENOK  1     /* socket open complete */
#define M_OPENERR 2     /* active open failed */
#define M_CLOSED  3     /* socket has closed */
#define M_RXDATA  4     /* passing received data */
#define M_TXDATA  5     /* blocked transmit now ready */

PACKET   tcp_pktalloc(int);
void     tcp_pktfree(PACKET p);


#ifdef MSOQ_CHECK
void  msoq_check(void);    /* sanity check of soq */
#else
#define msoq_check()
#endif   /*  MSOQ_CHECK */

#define TCPIPHDR_SIZE   (sizeof(struct tcphdr) + sizeof(struct ip))

/* Option flags per-socket, also used as IOCTL parameters. Values are
 * carried over from the Full-sockets stack in case compatability is
 * ever a concern.
 */
#define     SO_DEBUG       0x0001      /* turn on debugging info recording */
#define     SO_KEEPALIVE   0x0008      /* keep connections alive */
#define     SO_LINGER      0x0080      /* linger on close if data present */
#define     SO_BIO         0x1014      /* set socket into blocking mode */
#define     SO_NONBLOCK    0x1015      /* set/get blocking mode via optval param */
#define     SO_NBIO        0x1013      /* set socket into NON-blocking mode */

/* mini_Socket state bits. These may be directly accessed by the app. */
#define  SS_NOFDREF           0x0001   /* no file table ref any more */
#define  SS_ISCONNECTED       0x0002   /* socket connected to a peer */
#define  SS_ISCONNECTING      0x0004   /* in process of connecting to peer */
#define  SS_ISDISCONNECTING   0x0008   /*  in process  of disconnecting */
#define  SS_CANTSENDMORE      0x0010   /* can't send more data to peer */
#define  SS_CANTRCVMORE       0x0020   /* can't receive more data from peer */
#define  SS_RCVATMARK         0x0040   /* at mark on input */
#define  SS_PRIV              0x0080   /* privileged for broadcast, raw... */
#define  SS_NBIO              0x0100   /* non-blocking ops */
#define  SS_ASYNC             0x0200   /* async i/o notify */
#define  SS_UPCALLED          0x0400   /* zerocopy data has been upcalled (for select) */
#define  SS_INUPCALL          0x0800   /* inside zerocopy upcall (reentry guard) */
#define  SS_UPCFIN            0x1000   /* inside zerocopy upcall (reentry guard) */


/* BSD sockets errors */

#ifndef SOCKERRORS_ALREADY    /* allow 3rd party override */

#define     ENOBUFS        1
#define     ETIMEDOUT      2
#define     EISCONN        3
#define     EOPNOTSUPP     4
#define     ECONNABORTED   5
#define     EWOULDBLOCK    6
#define     ECONNREFUSED   7
#define     ECONNRESET     8
#define     ENOTCONN       9
#define     EALREADY       10
#define     EINVAL         11
#define     EMSGSIZE       12
#define     EPIPE          13
#define     EDESTADDRREQ   14
#define     ESHUTDOWN      15
#define     ENOPROTOOPT    16
#define     EHAVEOOB       17
#define     ENOMEM         18
#define     EADDRNOTAVAIL  19
#define     EADDRINUSE     20
#define     EAFNOSUPPORT   21
#define     EINPROGRESS    22
#define     ELOWER         23 /* lower layer (IP) error */

#endif   /* SOCKERRORS_ALREADY - end 3rd party override */

/* Mini TCP statistics. This is exported so menu routines can access it. */

struct   tcpstat
{
   u_long   tcps_connattempt;    /* connections initiated */
   u_long   tcps_accepts;        /* connections accepted */
   u_long   tcps_connects;       /* connections established */
   u_long   tcps_drops;          /* connections dropped */
   u_long   tcps_conndrops;      /* embryonic connections dropped */
   u_long   tcps_closed;         /* conn. closed (includes drops) */
   u_long   tcps_segstimed;      /* segs where we tried to get rtt */
   u_long   tcps_rttupdated;     /* times we succeeded */
   u_long   tcps_delack;         /* delayed acks sent */
   u_long   tcps_timeoutdrop;    /* conn. dropped in rxmt timeout */
   u_long   tcps_rexmttimeo;     /* retransmit timeouts */
   u_long   tcps_persisttimeo;   /* persist timeouts */
   u_long   tcps_keeptimeo;      /* keepalive timeouts */
   u_long   tcps_keepprobe;      /* keepalive probes sent */
   u_long   tcps_keepdrops;      /* connections dropped in keepalive */

   u_long   tcps_sndtotal;       /* total packets sent */
   u_long   tcps_sndpack;        /* data packets sent */
   u_long   tcps_sndbyte;        /* data bytes sent */
   u_long   tcps_sndrexmitpack;  /* data packets retransmitted */
   u_long   tcps_sndrexmitbyte;  /* data bytes retransmitted */
   u_long   tcps_sndacks;        /* ack-only packets sent */
   u_long   tcps_sndprobe;       /* window probes sent */
   u_long   tcps_sndurg;         /* packets sent with URG only */
   u_long   tcps_sndwinup;       /* window update-only packets sent */
   u_long   tcps_sndctrl;        /* control (SYN|FIN|RST) packets sent */

   u_long   tcps_rcvtotal;       /* total packets received */
   u_long   tcps_rcvpack;        /* packets received in sequence */
   u_long   tcps_rcvbyte;        /* bytes received in sequence */
   u_long   tcps_rcvbadsum;      /* packets received with ccksum errs */
   u_long   tcps_rcvbadoff;      /* packets received with bad offset */
   u_long   tcps_rcvshort;       /* packets received too short */
   u_long   tcps_rcvduppack;     /* duplicate-only packets received */
   u_long   tcps_rcvdupbyte;     /* duplicate-only bytes received */
   u_long   tcps_rcvpartduppack; /* packets with some duplicate data */
   u_long   tcps_rcvpartdupbyte; /* dup. bytes in part-dup. packets */
   u_long   tcps_rcvoopack;      /* out-of-order packets received */
   u_long   tcps_rcvoobyte;      /* out-of-order bytes received */
   u_long   tcps_rcvpackafterwin;   /* packets with data after window */
   u_long   tcps_rcvbyteafterwin;   /* bytes rcvd after window */
   u_long   tcps_rcvafterclose;  /* packets rcvd after "close" */
   u_long   tcps_rcvwinprobe;    /* rcvd window probe packets */
   u_long   tcps_rcvdupack;      /* rcvd duplicate acks */
   u_long   tcps_rcvacktoomuch;  /* rcvd acks for unsent data */
   u_long   tcps_rcvackpack;     /* rcvd ack packets */
   u_long   tcps_rcvackbyte;     /* bytes acked by rcvd acks */
   u_long   tcps_rcvwinupd;      /* rcvd window update packets */

   u_long   tcps_mcopies;        /* m_copy() actual copies */
   u_long   tcps_mclones;        /* m_copy() clones */
   u_long   tcps_mcopiedbytes;   /* m_copy() # bytes copied */
   u_long   tcps_mclonedbytes;   /* m_copy() # bytes cloned */

   u_long   tcps_oprepends;      /* ip_output() prepends of header to data */
   u_long   tcps_oappends;       /* ip_output() appends of data to header */
   u_long   tcps_ocopies;        /* ip_output() copies */
   u_long   tcps_predack;        /* VJ predicted-header acks */
   u_long   tcps_preddat;        /* VJ predicted-header data packets */
   u_long   tcps_zioacks;        /* acks recvd during zio sends */
};
extern   struct tcpstat    tcpstat; /* tcp statistics */

/* Compatability definitions for "sys_" and "t_" sockets calls */

#define  t_socket(fam, type, flags)    m_socket()
#define  t_closesocket(sock)           m_close(sock)
#define  t_socketclose(sock)           m_close(sock)
#define  t_setsockopt(sock, opt, parm) m_ioctl(sock, opt, parm)
#define  t_errno(sock)                 (sock->error)
#ifdef BSDISH_SEND
#define  t_send(sock, buf, len, flags) m_send(sock, buf, len)
#endif
#ifdef BSDISH_RECV
#define  t_recv(sock, buf, len, flags) m_recv(sock, buf, len)
#endif
#ifdef BSDISH_GETPEERNAME
#define  t_getpeername(sock, sin)      m_getpeername(sock, (struct sockaddr_in *)(sin))
#endif

#define  sys_socket(fam, type, flags)     m_socket()
#define  sys_closesocket(sock)            m_close(sock)
#define  sys_socketclose(sock)            m_close(sock)
#ifdef BSDISH_SEND
#define  sys_send(sock, buf, len, flags)  m_send(sock, buf, len)
#endif
#ifdef BSDISH_RECV
#define  sys_recv(sock, buf, len, flags)  m_recv(sock, buf, len)
#endif
#define  sys_errno(sock) (sock->error)

#define SYS_EWOULDBLOCK    EWOULDBLOCK /* portable error code */

#define WP_SOCKTYPE        SOCKTYPE    /* old webport socktype */

#define SOCKTYPE           SOCKTYPE    /* map MACRO to typedef */

#define  AF_INET     2     /* Another gratuitous BSD-ism */
#define  PF_INET     2     /* ...again... */

#endif   /* _MSOCK_H */

