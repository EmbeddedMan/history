/*
 * FILENAME: util.c
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
 * ROUTINES: kbhit(), getch(), dputchar(), nv_defaults(), 
 * ROUTINES: prep_evb(), pre_task_setup(), post_task_setup()
 *
 *
 * PORTABLE: no
 */

#include "license.h"
#include "ipport.h"
#include "nvparms.h"
#include "nvfsio.h"
#include "menu.h"

#ifdef USE_PPP
#include "ppp_port.h"
#endif /* USE_PPP */

#ifdef USE_MODEM
#include "../modem/mdmport.h"
#endif /* USE_MODEM */


#ifdef NATRT
#include "../natrt/natport.h"
#endif  /* NATRT */

#ifdef USE_FEC
extern int fec_prep(int);
#endif
#ifdef USE_PPP
extern int prep_ppp(int ifaces_found);
#endif
#ifdef USE_SLIP
extern int prep_slip(int ifaces_found);
#endif

/*
 * Set up default IP addresses for potential interfaces.
 *
 * NOTE this assumes that we will have no more than one interface of
 * each type.  So does nv_defaults(), below, and it is what uses
 * these.  If you need more interfaces, you may need to customize
 * nv_defaults() as well and revise or do away with these macros.
 *
 * That said, what we do here is set up default IP address information
 * for our interfaces by type: one for the FEC, one for PPP,
 * and one for the MAC loopback software loopback driver.
 */
 
#define IP_ETHER_ADDR         (0x0a000068)   /* 10.0.0.104 */
#define IP_ETHER_NETMASK      (0xff000000)   /* 255.0.0.0 */
#define IP_ETHER_DEFGW        (0x0a000001)   /* 10.0.0.1 */

#define IP_PPP_ADDR           (0xcf877c2e)   /* 207.135.124.46 */
#define IP_PPP_NETMASK        (0xffffff00)   /* 255.255.255.0 */
#define IP_PPP_DEFGW          (0x00000000)   /* 0.0.0.0 */

#define IP_LB_ADDR            (0x7f000001)   /* 127.0.0.1 */
#define IP_LB_NETMASK         (0xff000000)   /* 255.0.0.0 */
#define IP_LB_DEFGW           (0x00000000)   /* 0.0.0.0 */

#ifdef IPSEC
#include "../ipsec/mgmtapi.h"
#endif

int      dhcp_trying =  FALSE;
ip_addr  dhcp_saveaddr[MAXNETS];
ulong    dhcp_started;

int      ppp_uart = FALSE;

extern  void  nv_defaults(void);    			/* set default NV parameters */
extern  int   prep_fec(int ifaces_found);
extern  int   ppplogcons();
extern	int	  uart_putc(int, unsigned char);	//FSL added function prototype
extern	void  processor_PIT_Timer_Init(void);	//FSL added function prototype


//FSL dputchar() makes call to uart_putc() using this global value "evbuart" (i.e. uart_putc(evbuart, uchar)
//FSL out_char() had used "0" as a paramter for UART choice!
//FSL now use "#define CONSOLE_UART" in processor.h to define console/uart used
int   evbuart = 0;


/* FUNCTION: kbhit()
 * 
 * PARAM1: 
 *
 * RETURNS: 
 */


int
kbhit(void)
{
   /* If PPP is compiled to use the UART, disallow console */
// FSL - Removed to allow functions to work when menu disabled
//#ifndef TK_STDIN_DEVICE
//   return (FALSE);
//#endif

   /* typically keyboard task will block in getc() */
   return (uart_present(CONSOLE_UART));
//FSL   return (uart_present(evbuart));
}


/* FUNCTION: getch()
 * 
 * PARAM1: 
 *
 * RETURNS: 
 */

int 
getch(void)
{
   /* If PPP is compiled to use the UART, disallow console */
// FSL - Removed to allow functions to work when menu disabled
//#ifndef TK_STDIN_DEVICE
//   return -1;
//#endif

   if(!kbhit())
      return -1;
   else
      return( uart_getc(CONSOLE_UART) & 0xFF );
//FSL      return( uart_getc(evbuart) & 0xFF );
}


void
dputchar(int _c)
{
#ifdef USE_PPP
   /* If PPP is using the UART, disallow console */
   if (ppp_uart)
      return;
#endif

   /* Convert LF in to CRLF */
   if (_c == '\n')
       uart_putc(CONSOLE_UART, '\r');
//FSL       uart_putc(evbuart, '\r');
   
   /* mask out any high bits */
   uart_putc(CONSOLE_UART, (char)(_c & 0x7f));
//FSL   uart_putc(evbuart, (char)(_c & 0x7f));
}


#ifdef INCLUDE_NVPARMS 

/* FUNCTION: nv_defaults()
 *
 * This is called from the NV parameters system if there is
 * no NV file in flash. It builds a list of functional default
 * NV values for testing and/or system generation. It may not
 * be required on a shipping product.
 *
 * PARAM1: none
 *
 * RETURNS: void
 */

static char * logfilename = "login.nv";
static char * srvfilename = "server.nv";
static char * natfilename = "natdb.nv";

void
nv_defaults()
{
   int      iface = 0;

   /* store default IP info */

#ifdef USE_FEC
   if (iface < MAXNETS)
   {
      inet_nvparms.ifs[iface].ipaddr  = (IP_ETHER_ADDR);
      inet_nvparms.ifs[iface].subnet  = (IP_ETHER_NETMASK);
      inet_nvparms.ifs[iface].gateway = (IP_ETHER_DEFGW);
#ifndef DHCP_CLIENT
      inet_nvparms.ifs[iface].client_dhcp = 0;              /* don't do DHCP */
#else
      inet_nvparms.ifs[iface].client_dhcp = 1;              /* use DHCP */
#endif

      printf("nv_defaults: set net %d IP to %u.%u.%u.%u\n",
              iface, PUSH_IPADDR(inet_nvparms.ifs[iface].ipaddr) );
      iface++;
   }
#endif  /* USE_FEC */

#if defined(USE_PPP) || defined(USE_SLIP)
   if (iface < MAXNETS)
   {
      /* PPP/SLIP interface */
      inet_nvparms.ifs[iface].ipaddr = (IP_PPP_ADDR);
      inet_nvparms.ifs[iface].subnet = (IP_PPP_NETMASK);
      inet_nvparms.ifs[iface].gateway = (IP_PPP_DEFGW);
      inet_nvparms.ifs[iface].client_dhcp = 0;              /* don't do DHCP */

      dprintf("nv_defaults: set PPP/SLIP net %d IP to %u.%u.%u.%u\n",
              iface, PUSH_IPADDR(inet_nvparms.ifs[iface].ipaddr) );

      iface++;
   }
#ifdef USE_PPP
   strcpy(ppp_nvparms.username, "username");
   strcpy(ppp_nvparms.password, "password");
#endif   /* USE_PPP */
#endif

#ifdef USE_MODEM
   strcpy(modem_nvparms.dial_phone, "5551212\n");
   strcpy(modem_nvparms.modem_init, "AT&D0&C0\n");
   strcpy(ppp_nvparms.loginfile, logfilename);
   strcpy(ppp_nvparms.logservefile, srvfilename);
#endif   /* USE_MODEM */

#ifdef USE_COMPORT
   comport_nvparms.comport = '1';
   comport_nvparms.LineProtocol = 1;
#endif   /* USE_COMPORT */

#ifdef MAC_LOOPBACK
   if (iface < MAXNETS)
   {
      /* loopback */
      inet_nvparms.ifs[iface].ipaddr = (IP_LB_ADDR);
      inet_nvparms.ifs[iface].subnet = (IP_LB_NETMASK);
      inet_nvparms.ifs[iface].gateway = (IP_LB_DEFGW);
      inet_nvparms.ifs[iface].client_dhcp = 0;              /* don't do DHCP */
      iface++;
   }
#endif /* MAC_LOOPBACK */

#ifdef NATRT
   natrt_nvparms.nat_enabled = 1;
   natrt_nvparms.nat_inet = 0;
   natrt_nvparms.nat_localnet = 1;
   natrt_nvparms.tcp_timeout = 300;
   natrt_nvparms.udp_timeout = 500;
   /* force TCP MSS to 1430 through NAT router -- it doesn't need it,
    * but this helps prevent some hosts from fragmenting TCP 
    */
   natrt_nvparms.nat_tcpmss = 1430;
#endif   /* NATRT */

   /* create some required NV files */
   if (*logfilename)
      nv_fopen(logfilename, "w+");
   if (*srvfilename)
      nv_fopen(srvfilename, "w+");
   if (*natfilename)
      nv_fopen(natfilename, "w+");
}
#endif  /* INCLUDE_NVPARMS */


int 
prep_evb(int ifaces_found)
{

#ifdef USE_FEC
    printf("Preparing device for networking\n");    
   /* set up FECs */
   ifaces_found = prep_fec(ifaces_found);
#endif

#ifdef USE_PPP
   ifaces_found = prep_ppp(ifaces_found);
#endif

#ifdef USE_SLIP
   ifaces_found = prep_slip(ifaces_found);
#endif

   return ifaces_found;
}


/* hardware setup called from main before anything else (e.g.
 * before tasks, printf, memory alloc, etc. 
 *
 * Return NULL if OK, else brief error message
 */

char *
pre_task_setup()
{
   processor_PIT_Timer_Init();		// processor specific PIT config for 1 msec interrupt
   return NULL;
}


/* more setup called after tasks are set up
 *
 * Return NULL if OK, else brief error message
 */

char *
post_task_setup()
{
#ifndef SUPERLOOP
#ifdef IPSEC
#ifndef IKE
   tk_yield();
   if(prep_ipsec())
   {
      printf("prep_ipsec() failed\n!\n");
   }
#endif
#endif
#endif
   return NULL;
}


char *
alloc_uncached(int size)
{
   char  *cp;

   /* 
    * FEC Ethernet buffers must be aligned on 8 or 16 byte boundaries.
    * provide this by allocating 16 extra bytes and then masking off the
    * low bits before return. This is mapped to BB_ALLOC() and LB_ALLOC()
    * which, when in debug mode, will add 4 bytes to the
    * head for a memory corruption marker. We have to adjust for this
    * also, with another 4 bytes of padding on the front so that the
    * pkt->nb_buffs all end up on 16 byte boundaries.
    */

#ifdef NPDEBUG
   cp = (char *)npalloc(size + 20);  /* 16 for rounding, 4 for marker */
   cp = cp + 4;
#else
   cp = (char *)npalloc(size + 16);
#endif

   if (cp == NULL)
      panic("alloc_uncached");

   cp = (char *)(((long)(cp + 15)) & 0xFFFFFFF0);

#ifdef NPDEBUG
   return (cp - 4);     /* allow for memory marker */
#else
   return (cp);
#endif
}

/* The Freescale BSP provides a printf() and sprintf(), but no ANSI 
 * vprintf() or vsprintf().
 *
 * We can't just use our ttyio.c printf, since their's is in a linked file 
 * and not a library - the linker fails with multiply defined symbol errors.
 * Current workaround is to provide our own vprintf(), etc; wrapped around 
 * their "printfk()" layer.
 */

/* This structure and "dest" values MUST match those buried in printf.c */
typedef struct
{
	int	dest;
	void (*func)(char);
	char *loc;
} PRINTK_INFO;

#define DEST_CONSOLE          (1)
#define DEST_STRING           (2)

#if 0  // we use our own vprintf() and vsprintf()
#include "io.h"

int
vprintf(const char * fmt,
        va_list ap)
{
   int rvalue;
   PRINTK_INFO info;

   info.dest = DEST_CONSOLE;
   info.func = &out_char;

   rvalue = printk(&info, fmt, ap);
   return rvalue;    
}

int
vsprintf(char * outbuf,
        const char * fmt,
        va_list ap)
{
   int rvalue;
   PRINTK_INFO info;

   info.dest = DEST_STRING;
   info.loc = outbuf;

   rvalue = printk(&info, fmt, ap);
   *(outbuf + rvalue) = 0;

   return rvalue;    
}

#endif

/* Wrappers for heap calls, with memory clearing and counters */

unsigned int npfree_counts = 0;
unsigned int npalloc_counts = 0;
unsigned int npalloc_successes = 0;
unsigned int npalloc_fails = 0;

char *
npalloc(unsigned size)
{
   char * buf;

   npalloc_counts++;
   buf =  calloc1(size);
   if (buf)
   {
      npalloc_successes++;
      memset(buf, 0, size);
   }
   else
   {
      npalloc_fails++;
      printf("calloc1 failed: size: %u, failures: %u\n", size, npalloc_fails);
   }
   return buf;
}


void
npfree(void * ptr)
{
   mem_free((char*)ptr);
   npfree_counts++;
}


/* FUNCTION: memalign()
 *
 * Allocate memory with a given memory alignment
 *
 * PARAM1: align        alignment factor
 * PARAM2: size         number of bytes to allocate
 *
 * RETURN: char *       pointer to allocated memory,
 *                      or NULL if allocation failed
 *
 * 
 */

char *
memalign(unsigned align, unsigned size)
{
   char *ptr;

   /* align must be a power of 2 */
   if (align & (align - 1))
      return ((void *)NULL);

   ptr = (char *)npalloc(size + align - 1);
   if (ptr != NULL)
   {
      ptr = (char *)((unsigned)ptr & ~(align - 1));
   }

   return (ptr);
}


#ifdef C_CHECKSUM
unsigned short 
cksum(void * ptr, unsigned len)
{
   void *local_p = ptr;
    return(ccksum(local_p, len));    
}
#endif


#if 0
int rand(void)
{
   /* fix this later: use SEC engine to get a real random rumber! */
   unsigned long retval = slt_value(SLT_0);
   printf("TBD: rand: %lu\n", retval);
   return(int)(retval);
}
#endif

#ifdef IPSEC
#ifndef IKE
int prep_ipsec(void)
{
   unsigned char sha1hmac_key[SHA1_DIGEST_LENGTH] =
      {0x9F,0x16,0x26,0x50,0x6D,0xBD,0x75,0x40,0xC8,0xCE,
       0x52,0x60,0x70,0xFB,0x69,0xF9,0x70,0xFB,0x69,0xF9};
   unsigned char md5hmac_key[MD5_DIGEST_LENGTH] =
      {0x9F,0x16,0x26,0x50,0x6D,0xBD,0x75,0x40,
       0xC8,0xCE,0x52,0x60,0x70,0xFB,0x69,0xF9};
   unsigned char tdes192_key[TDES192_KEY_SIZE] =
      {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,
       0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,
       0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef};
   unsigned char aes128_key[AES128_KEY_SIZE] =
       {0x9F,0x16,0x26,0x50,0x6D,0xBD,0x75,0x40,
        0xC8,0xCE,0x52,0x60,0x70,0xFB,0x69,0xF9};
   int err = 0;
   char *ipsec_dest_ip = "10.0.0.161";
   char *ipsec_src_ip = "10.0.0.162";
/*   char *ipsec_src_ip = "192.168.1.206";
   char *ipsec_dest_ip = "192.168.1.207";*/
   /* Configure IPSec with a policy to bypass all traffic --
    * without some configuration, it will drop all traffic 
    * with a "no policy" error, and this is closest to the
    * IPSec-less stack configuration. For IKE, this bypass
    * policy is added in ikeport.c after IKE initialization
    */
#ifdef IP_V6
   err = IPSecAdminAddManualSA(IPSEC_MODE_TUNNEL,/* mode */
            IP_PROTO_TCP,
            "3ffe:501:ffff:1000:210:dcff:fef0:3e90",
            "3ffe:501:ffff:1000:200:e8ff:fe90:9602",
            "3ffe:501:ffff:1000:200:e8ff:fe90:9602",
            ALG_ESP_DES,/* encryption algorithm */
            TDES192_KEY_SIZE,/* encr key length */
            (const unsigned char *)tdes192_key,/* inbound encr key */
            (const unsigned char *)tdes192_key,/* outbound encr key */
            0,/* authentication algorithm */
            0,/* auth key length */
            0,/* inbound auth key */
            0,          /* outbound auth key */
            0x555,/* ESP inbound SPI */
            0x333, /* ESP outbound SPI */
            0x0,/* AH inbound SPI */
            0x0/* AH outbound SPI */
            );
#endif
#ifdef IP_V4
   /* example of using IPV4 addresses */
   err = IPSecAdminAddManualSA(IPSEC_MODE_TUNNEL,/* mode */
            0,    /* all */
            ipsec_src_ip, 
            ipsec_dest_ip,
            ipsec_dest_ip,
            ALG_ESP_3DES,/* encryption algorithm */
            TDES192_KEY_SIZE,/* encr key length */
            (const unsigned char *)tdes192_key,/* inbound encr key */
            (const unsigned char *)tdes192_key,/* outbound encr key */
            ALG_AH_SHA,/* authentication algorithm */
            SHA1_DIGEST_LENGTH,/* auth key length */
            sha1hmac_key,/* inbound auth key */
            sha1hmac_key,          /* outbound auth key */
            0xddccbbaa,/* ESP inbound SPI */
            0xaabbccdd, /* ESP outbound SPI */
            0xddccbbaa,/* AH inbound SPI */
            0xaabbccdd /* AH outbound SPI */
            );
   if(err)
   {
      printf("prep_ipsec(): IPSecAdminAddManualSA() failed: %d\n!", err);
   }
   else
   {
      printf("prep_ipsec(): created IPSEC tunnel to: %s\n", ipsec_dest_ip);
   }
#endif
#ifndef SUPERLOOP
   tk_yield();
#endif
   IPSecAdminAddBypassPolicy("any", "any", 0, SP_PRIORITY_MEDIUM);
   return(err);
}
#endif   /* IKE */
#endif   /* IPSEC */

