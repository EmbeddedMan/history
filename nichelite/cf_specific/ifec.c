/* FILENAME: ifec.c
 *
 * Copyright 2005 by InterNiche Technologies Inc. All rights reserved.
 *
 * Freescale 10/100 Fast Ethernet Controller Driver
 *
 * ROUTINES: fec_stats(), prep_fec(), fec_init(),  
 * ROUTINES: fec_pkt_send(), fec_rxalloc(), tx_internal(), fec_tx(),
 * ROUTINES: fec_isr(), FEC_init(), FEC_reset(), 
 * ROUTINES: FEC_readMIB(), FEC_clearMIB(), 
 * ROUTINES: FEC_hwinit(), alloc_uncached(), fec_close(), 
 * ROUTINES: fec_link_setup()
 *
 * PORTABLE: no
 */

#include "ipport.h"
#include "q.h"
#include "netbuf.h"
#include "net.h"
#include "ether.h"
#include "fecport.h"

/* Buffer descriptors must be aligned on 8-byte boundary */
BD * RxBDs;    /* RX buffer descriptors */
BD * TxBDs;    /* TX buffer descriptors */

queue  fectxq;
PACKET rxpend[NUM_RXBDS];
PACKET txpend[NUM_TXBDS];
u_char * TxLilPkts[NUM_TXBDS];

static int next_txbd = 0;
static int last_txbd = 0;
static int next_rxbd = 0;

static int phy_ready = FALSE;

/* For now, only support ONE fec device */
struct fec_mib  *fec_mib_reg;
int    fec_iface;        /* nets[] index for fec device */
IFMIB  fec_ifmib;
struct fec_statistics fecstats;
static unsigned int rx_copies = 0;
static unsigned int tx_copies = 0;
static unsigned int tx_fullcopy = 0;
static unsigned int tx_int_txb = 0;
static unsigned int tx_int_txf = 0;
static unsigned int rx_int_rxb = 0;
static unsigned int rx_int_rxf = 0;

/* Note the first octet should be a zero
 * Example:
 * unsigned char mac_addr_fec[8] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
 *

MAC Type Address range 

Globally Unique 
*0-**-**-**-**-**		<-- this is what is used for our example
*4-**-**-**-**-**
*8-**-**-**-**-**
*C-**-**-**-**-** 

Locally Administered 
*2-**-**-**-**-**
*6-**-**-**-**-**
*A-**-**-**-**-**
*E-**-**-**-**-** 

Multicast 
*1-**-**-**-**-**
*3-**-**-**-**-**
*5-**-**-**-**-**
*7-**-**-**-**-**
*9-**-**-**-**-**
*B-**-**-**-**-**
*D-**-**-**-**-**
*F-**-**-**-**-**
Note: except broadcast address  

Broadcast FF-FF-FF-FF-FF-FF 
 
*/

unsigned char mac_addr_fec[8] = { 0x00, 0xcf, 0x52, 0x08, 0xcf, 0x01, 0x01, 0x01 };

extern void FEC_ICR_init(void);		//FSL added function prototype
extern void FEC_IMR_init(void);		//FSL added function prototype

/* internal routines: */
int   FEC_hwinit(void);				//FSL fix spelling....changed from fec_ to FEC_
int   fec_link_setup(void);
int   FECInit(void);

/* FUNCTION: dump_pkt()
 *
 * Print information about a packet
 *
 * PARAM1: pkt;         PACKET to dump
 *
 * RETURN: none
 */

void dump_pkt(PACKET pkt)
{
   int i;
   if(pkt == NULL)
   {
      printf("dump_pkt(): NULL pkt pointer\n");
      return;
   }
   printf("nb_plen = %d\n", pkt->nb_plen);
   for(i = 0; i < pkt->nb_plen; i++)
   {
      if((i%16) == 0)
      {
         printf("\n");
      }
      printf("%x ", (unsigned char)(pkt->nb_prot)[i]);
   }
   printf("\n");
}


/* FUNCTION: prep_fec()
 *
 * Initialize a FEC interface
 *
 * PARAM1: iface        FEC interface number
 *
 * RETURN: iface + 1
 *
 */

int
prep_fec(int iface)
{
   fec_iface = iface;
   nets[iface]->n_lnh = ETHHDR_SIZE;   /* Ethernet header size */
   nets[iface]->n_hal = 6;             /* hardware address length */
   nets[iface]->n_mtu = 1514;          /* max frame size */

   fec_ifmib = nets[iface]->n_mib;

   fec_ifmib->ifDescr = (u_char*)"Fast Ethernet";

   fec_ifmib->ifSpeed = 100000000;
   fec_ifmib->ifAdminStatus = 2;       /* status = down */
   fec_ifmib->ifOperStatus = 2;        /* will be set up in init()  */
   fec_ifmib->ifLastChange = cticks * (100/TPS);
   fec_ifmib->ifPhysAddress = (u_char*)&mac_addr_fec[0];
   fec_ifmib->ifType = ETHERNET;

   /* install our hardware driver routines */
   nets[iface]->n_init = fec_init;
   nets[iface]->pkt_send = fec_pkt_send;
   nets[iface]->raw_send = NULL;
   nets[iface]->n_close = fec_close;
   nets[iface]->n_stats = fec_stats;
/*   nets[iface]->n_flags |= NF_NBPROT;*/

#ifdef DHCP_CLIENT
	nets[iface]->n_flags |= NF_DHCPC;
#endif

#ifdef IP_V6
   nets[iface]->n_flags |= (NF_NBPROT | NF_BCAST | NF_MCAST | NF_IEEE48 | NF_IPV6);
#else
   nets[iface]->n_flags |= (NF_NBPROT | NF_BCAST | NF_MCAST | NF_IEEE48 );
#endif

#ifdef MCF5275					//FSL MCF5275 has two FEC's so we will a 1 for either iface=0 or 1
   return (iface<2 ? 1:0);		/* return index of next iface */
#else   
   return (iface+1);			/* return index of next iface */
#endif
   
}


/* FUNCTION: fec_init()
 *
 * Initialize a FEC device
 *
 * PARAM1: iface        FEC device number
 *
 * RETURN: SUCCESS if successful, otherwise an error code
 */

int
fec_init(int iface)
{
   int e;

   if(iface != fec_iface)
      return ENP_PARAM;

   e = FEC_hwinit();
   if (e)
      return (e);

   nets[iface]->n_mib->ifAdminStatus = 1;  /* status = UP */
   nets[iface]->n_mib->ifOperStatus = 1;
   nets[iface]->n_mib->ifLastChange = cticks * (100/TPS);

   dprintf("Ethernet started, Iface: %d, IP: %u.%u.%u.%u\n",
      fec_iface, PUSH_IPADDR(nets[fec_iface]->n_ipaddr) );

   return (SUCCESS);   /* Ethernet is now running */
}



/* FUNCTION: fec_hwinit()
 * 
 * Initialize the FEC hardware
 *
 * PARAMS: none
 *
 * RETURNS: 0 if OK, else negative error code
 */

int
FEC_hwinit(void)
{
   int   err;
   int   i;

   FEC_reset();
   FEC_ICR_init();			//FSL configure the FEC ICRs in processor specific file
	
	/* Enable FEC Rx Frame interrupts to ColdFire core */
   FEC_IMR_init();			//FSL configure the FEC IMRs in processor specific file

   /* clear all of the MIB counters */
   FEC_clearMIB();

   if(bigbufsiz < MAX_ETH_PKT)
   {
      dprintf("FEC: bigbufs too small\n");
      return ENP_LOGIC;
   }
   
   /* set mib based on hardcoded net number */
   fec_ifmib = nets[0]->n_mib;
   
   err = fec_link_setup();    /* bring up the link */
   if (err)
      return (err);

   fec_ifmib->ifOperStatus = 1;

   return (SUCCESS);
}


/* FUNCTION: FEC_reset()
 * 
 * PARAMS: none
 *
 * RETURNS: none
 *
 * Performs a hardware reset of the FEC device.
 * The Tx and Rx circuitry is disabled.
 * All FEC interrupts are disabled.
 */

void
FEC_reset(void)
{
   /* reset the FEC */
#ifndef	MCF5275   
   MCF_FEC_ECR = MCF_FEC_ECR_RESET;
   while (MCF_FEC_ECR & MCF_FEC_ECR_RESET) 
   {
      /* null */ ;
   }
#else
   MCF_FEC_ECR(ETH_PORT) = MCF_FEC_ECR_RESET;
   while (MCF_FEC_ECR(ETH_PORT) & MCF_FEC_ECR_RESET) 
   {
      /* null */ ;
   }

#endif
  
}


/* FUNCTION: fec_link_setup()
 *
 * Bring up the FEC link
 *
 * PARAMS: none
 *
 * RETURN: SUCCESS if successful, otherwise an error code
 *
 * This is called to bring the link up, either in initial
 * setup or after a link status change interrupt.
 */

int
fec_link_setup(void)
{
   int   err;

   err = FECInit();     /* set up FEC and BD rings */
   if (err)
      return err;
#ifndef MCF5275
   MCF_FEC_ECR = MCF_FEC_ECR_ETHER_EN;

   MCF_FEC_RDAR = MCF_FEC_RDAR_R_DES_ACTIVE;
   MCF_FEC_TDAR = MCF_FEC_TDAR_X_DES_ACTIVE;
#else
   MCF_FEC_ECR(ETH_PORT) = MCF_FEC_ECR_ETHER_EN;

   MCF_FEC_RDAR(ETH_PORT) = MCF_FEC_RDAR_R_DES_ACTIVE;
   MCF_FEC_TDAR(ETH_PORT) = MCF_FEC_TDAR_X_DES_ACTIVE;
#endif

   return SUCCESS;   /* OK return */
}


/* FUNCTION NAME:  FECInit 
 *
 *  FEC Ethernet Initialization Routine.
 *
 * Set up various registers needed for Ethernet, including
 * Chip and FEC registers, interrupt related registers
 * and port registers.
 *
 * PARAM1: None
 *
 * RETURNS: 0 if OK, else nonzero error code
 */

#ifndef MCF5275		//FSL non-MCF5275 code
					//FSL Since MCF5275 has two Ethernet FECs had to modify
					//FSL	base code.int
FECInit(void)
{
   int   i;
   BD *  rbd;      /* scratch BD pointers */
   BD *  tbd;
   char * cp;

   FEC_reset();

   /* Configure interrupt registers */
   MCF_FEC_EIMR = MCF_FEC_EIMR_ALL_MASKS;  /* mask on all of 'em */

   /* clear any pending events */
   MCF_FEC_EIR = MCF_FEC_EIR_ALL_EVENTS;

#ifdef DEBUG_FEC
   /* If these values are not sticking return an error */
   if (MCF_FEC_EIMR != MCF_FEC_EIMR_ALL_MASKS)
   {
      dtrap();
      dprintf("Error: FEC registers not responding\n");
      return -1;
   }
#endif

   /* Reset values are satisfactory for t_r_fstart and t_x_fstart, so
    * this step is skipped.  In some applications, the user may want
    * to tune these values to provide more fifo space to rx or tx.
    */

   /* Program this station's Ethernet physical address */
	/* Set the source address for the controller */
	MCF_FEC_PALR = (UINT32)((0
		| (mac_addr_fec[0] <<24)
		| (mac_addr_fec[1] <<16)
		| (mac_addr_fec[2] <<8)
		| (mac_addr_fec[3] <<0)));
	MCF_FEC_PAUR = (UINT32)((0
		| (mac_addr_fec[4] <<24)
		| (mac_addr_fec[5] <<16)
        | MCF_FEC_PAUR_TYPE(0x00008808)));

	/* Initialize the hash table registers */

   MCF_FEC_IALR = 0;
   MCF_FEC_IAUR = 0;

   /* This driver doesn't support multicast frames yet, so we clear the hash
    * table registers (t_g_hash_table_high and t_g_hash_table_low).
    * To allow all multicasts in, we set all the bits in the group
    * hash table registers. 
    */
   MCF_FEC_GALR = 0xffffffff;
   MCF_FEC_GAUR = 0xffffffff;

   /* Program receive buffer size */
   MCF_FEC_EMRBR = MAX_ETH_PKT;

/* The below options are defaults that can be overridden in processor.h */
#ifndef FULL
#define FULL	0
#endif

#ifndef HALF
#define HALF	1
#endif

#ifndef DUPLEX
#define DUPLEX	HALF			//FSL enter HALF or FULL to select Ethernet duplex mode
#endif

#if (DUPLEX)
   /* Configure FEC receiver mode half duplex: */
   MCF_FEC_RCR = (MCF_FEC_RCR_MAX_FL(MAX_ETH_PKT) |
                      MCF_FEC_RCR_MII_MODE |
                      MCF_FEC_RCR_DRT); 		/* half duplex */

   /* Configure FEC transmitter mode:  */
   MCF_FEC_TCR = ( 0 );							/* half duplex */
#else
   /* Configure FEC receiver mode full duplex: */
   MCF_FEC_RCR = (MCF_FEC_RCR_MAX_FL(MAX_ETH_PKT) |
                      MCF_FEC_RCR_MII_MODE); 	/* full duplex */

   /* Configure FEC transmitter mode:  */
   MCF_FEC_TCR = ( 0 | MCF_FEC_TCR_FDEN); 		/* full duplex */
#endif

    /* //FSL inlined the MII clock configuration code and made
     * it dependent on the SYS_CLK_MHz located in evb specific file
     * Configure MII interface speed. Must be <= 2.5MHz
     *
     * Desired MII clock is 2.5MHz
     * MII Speed Setting = System_Clock_Bus_Speed / (2.5MHz * 2)
     */
    MCF_FEC_MSCR = MCF_FEC_MSCR_MII_SPEED((uint32)(SYS_CLK_MHZ/5));
    
   /* set up ring buffers - the FEC "BDs". Alloc extra space and round
    * it down to a 128-bit aligned address as required by the FEC
    */
   RxBDs = (BD *)memalign(16, NUM_RXBDS * sizeof(BD));
   if (RxBDs == NULL)
      return (ENP_NOMEM);

   TxBDs = (BD *)memalign(16, NUM_TXBDS * sizeof(BD));
   if (TxBDs == NULL)
      return (ENP_NOMEM);

   /* loop through the buffer pools setting up the BD entries */
   rbd = RxBDs;
   for (i = 0; i < NUM_RXBDS; i++)
   {
      PACKET pkt;

      pkt = pk_alloc(MAX_ETH_PKT);			//FSL get pkt space from HEAP
      if (!pkt)
      {
         dprintf("FECInit: fatal; rbd# %d out of Packet Buffers\n", i);
         return -1;
      }
      rxpend[i] = pkt;        /* place pkt HEAP address in the rxpend[] array */

      rbd->bd_addr = (u_char *)pkt->nb_buff; /* map buffer to the ring */
      rbd->bd_length = MAX_ETH_PKT;			//FSL indicate max buffer size
      rbd->bd_cstatus = MCF_FEC_RxBD_E;		//FSL mark buffer as empty
      rbd++;
   }
   /* mark last buffer with a "WRAP" bit  */
   --rbd;
   rbd->bd_cstatus |= MCF_FEC_RxBD_W;

   tbd = TxBDs;
   for (i = 0; i < NUM_TXBDS; i++)
   {
      /* alloc_uncached() cannot be used because of NPDEBUG,
       * hence duplicate the alignment scheme here.
       */
      TxLilPkts[i] = (u_char *)memalign(4, ETHHDR_SIZE);

      if (TxLilPkts[i] == NULL)
      {
         dprintf("FECInit: fatal; tbd# %d out of eth hdr frame Buffers\n", i);
         return (-1);
      }

      tbd->bd_cstatus = 0;
      tbd->bd_length = 0;
      tbd->bd_addr = 0;
      tbd++;
   }
   --tbd;
   tbd->bd_cstatus |= MCF_FEC_TxBD_W;

   /* Configure start of Rx BD ring */
   MCF_FEC_ERDSR =  (u_long) &RxBDs[0];
   /* Configure start of Tx BD ring */
   //FSL If processor header file had different naming for MCF_FEC_ETDSR one was added for compatibility
   MCF_FEC_ETDSR =  (u_long) &TxBDs[0];
   MCF_FEC_MIBC &= ~MCF_FEC_MIBC_MIB_DISABLE;

   return 0;
}


/****************************************************************************************/
#else			//FSL MCF5275 code
/****************************************************************************************/


int
FECInit(void)
{
   int   i;
   BD *  rbd;      /* scratch BD pointers */
   BD *  tbd;
   char * cp;

   FEC_reset();

   /* Configure interrupt registers */
   MCF_FEC_EIMR(ETH_PORT) = MCF_FEC_EIMR_ALL_MASKS;  /* mask on all of 'em */

   /* clear any pending events */
   MCF_FEC_EIR(ETH_PORT) = MCF_FEC_EIR_ALL_EVENTS;

#ifdef DEBUG_FEC
   /* If these values are not sticking return an error */
   if (MCF_FEC_EIMR(ETH_PORT) != MCF_FEC_EIMR_ALL_MASKS)
   {
      dtrap();
      dprintf("Error: FEC registers not responding\n");
      return -1;
   }
#endif

   /* Reset values are satisfactory for t_r_fstart and t_x_fstart, so
    * this step is skipped.  In some applications, the user may want
    * to tune these values to provide more fifo space to rx or tx.
    */

   /* Program this station's Ethernet physical address */
	/* Set the source address for the controller */
	MCF_FEC_PALR(ETH_PORT) = (UINT32)((0
		| (mac_addr_fec[0] <<24)
		| (mac_addr_fec[1] <<16)
		| (mac_addr_fec[2] <<8)
		| (mac_addr_fec[3] <<0)));
	MCF_FEC_PAUR(ETH_PORT) = (UINT32)((0
		| (mac_addr_fec[4] <<24)
		| (mac_addr_fec[5] <<16)
        | MCF_FEC_PAUR_TYPE(0x00008808)));

	/* Initialize the hash table registers */

   MCF_FEC_IALR(ETH_PORT) = 0;
   MCF_FEC_IAUR(ETH_PORT) = 0;

   /* This driver doesn't support multicast frames yet, so we clear the hash
    * table registers (t_g_hash_table_high and t_g_hash_table_low).
    * To allow all multicasts in, we set all the bits in the group
    * hash table registers. 
    */
   MCF_FEC_GALR(ETH_PORT) = 0xffffffff;
   MCF_FEC_GAUR(ETH_PORT) = 0xffffffff;

   /* Program receive buffer size */
   MCF_FEC_EMRBR(ETH_PORT) = MAX_ETH_PKT;

/* The below options are defaults that can be overridden in processor.h */
#ifndef FULL
#define FULL	0
#endif

#ifndef HALF
#define HALF	1
#endif

#ifndef DUPLEX
#define DUPLEX	HALF			//FSL enter HALF or FULL to select Ethernet duplex mode
#endif

#if (DUPLEX)
   /* Configure FEC receiver mode half duplex: */
   MCF_FEC_RCR(ETH_PORT) = (MCF_FEC_RCR_MAX_FL(MAX_ETH_PKT) |
                      MCF_FEC_RCR_MII_MODE |
                      MCF_FEC_RCR_DRT); 		/* half duplex */

   /* Configure FEC transmitter mode:  */
   MCF_FEC_TCR(ETH_PORT) = ( 0 );							/* half duplex */
#else
   /* Configure FEC receiver mode full duplex: */
   MCF_FEC_RCR(ETH_PORT) = (MCF_FEC_RCR_MAX_FL(MAX_ETH_PKT) |
                      MCF_FEC_RCR_MII_MODE); 	/* full duplex */

   /* Configure FEC transmitter mode:  */
   MCF_FEC_TCR(ETH_PORT) = ( 0 | MCF_FEC_TCR_FDEN); 		/* full duplex */
#endif

    /* //FSL inlined the MII clock configuration code and made
     * it dependent on the SYS_CLK_MHz located in evb specific file
     * Configure MII interface speed. Must be <= 2.5MHz
     *
     * Desired MII clock is 2.5MHz
     * MII Speed Setting = System_Clock_Bus_Speed / (2.5MHz * 2)
     */
    MCF_FEC_MSCR(ETH_PORT) = MCF_FEC_MSCR_MII_SPEED((uint32)(SYS_CLK_MHZ/5));
    
   /* set up ring buffers - the FEC "BDs". Alloc extra space and round
    * it down to a 128-bit aligned address as required by the FEC
    */
   RxBDs = (BD *)memalign(16, NUM_RXBDS * sizeof(BD));
   if (RxBDs == NULL)
      return (ENP_NOMEM);

   TxBDs = (BD *)memalign(16, NUM_TXBDS * sizeof(BD));
   if (TxBDs == NULL)
      return (ENP_NOMEM);

   /* loop through the buffer pools setting up the BD entries */
   rbd = RxBDs;
   for (i = 0; i < NUM_RXBDS; i++)
   {
      PACKET pkt;

      pkt = pk_alloc(MAX_ETH_PKT);			//FSL get pkt space from HEAP
      if (!pkt)
      {
         dprintf("FECInit: fatal; rbd# %d out of Packet Buffers\n", i);
         return -1;
      }
      rxpend[i] = pkt;        /* place pkt HEAP address in the rxpend[] array */

      rbd->bd_addr = (u_char *)pkt->nb_buff; /* map buffer to the ring */
      rbd->bd_length = MAX_ETH_PKT;			//FSL indicate max buffer size
      rbd->bd_cstatus = MCF_FEC_RxBD_E;		//FSL mark buffer as empty
      rbd++;
   }
   /* mark last buffer with a "WRAP" bit  */
   --rbd;
   rbd->bd_cstatus |= MCF_FEC_RxBD_W;

   tbd = TxBDs;
   for (i = 0; i < NUM_TXBDS; i++)
   {
      /* alloc_uncached() cannot be used because of NPDEBUG,
       * hence duplicate the alignment scheme here.
       */
      TxLilPkts[i] = (u_char *)memalign(4, ETHHDR_SIZE);

      if (TxLilPkts[i] == NULL)
      {
         dprintf("FECInit: fatal; tbd# %d out of eth hdr frame Buffers\n", i);
         return (-1);
      }

      tbd->bd_cstatus = 0;
      tbd->bd_length = 0;
      tbd->bd_addr = 0;
      tbd++;
   }
   --tbd;
   tbd->bd_cstatus |= MCF_FEC_TxBD_W;

   /* Configure start of Rx BD ring */
   MCF_FEC_ERDSR(ETH_PORT) =  (u_long) &RxBDs[0];
   /* Configure start of Tx BD ring */
   //FSL If processor header file had different naming for MCF_FEC_ETDSR one was added for compatibility
   MCF_FEC_ETDSR(ETH_PORT) =  (u_long) &TxBDs[0];
   MCF_FEC_MIBC(ETH_PORT) &= ~MCF_FEC_MIBC_MIB_DISABLE;

   return 0;
}

#endif		//end MCF5275 FECInit()

/* FUNCTION: FEC_readMIB
 *
 * Read the MIB hardware registers
 *
 * PARAM1: fec_mib_reg;       pointer the MIB register structure
 * 
 * RETURNS: 0 if successful, otherwise negative error code
 */
 
int
FEC_readMIB(struct fec_mib *mib)
{
#ifndef	MCF5275		//FSL non-MCF5275 code
					//FSL Since MCF5275 has two Ethernet FECs had to modify
					//FSL	base code.
   uint32 save = MCF_FEC_MIBC;

   if (!(save & MCF_FEC_MIBC_MIB_DISABLE))
   {
      /* disable MIB counters */
      MCF_FEC_MIBC = MCF_FEC_MIBC_MIB_DISABLE;
      /* wait for MIB to go idle */
      while (!(MCF_FEC_MIBC & MCF_FEC_MIBC_MIB_IDLE))
      {
         /* null */ ;
      }
   }

   /* copy hardware registers into struct */
   MEMCPY(mib, (const void *)&MCF_FEC_RMON_T_DROP, sizeof(struct fec_mib));
   /* re-enable MIB counters */
   MCF_FEC_MIBC = save;

#else			//FSL MCF5275 code

   uint32 save = MCF_FEC_MIBC(ETH_PORT);

   if (!(save & MCF_FEC_MIBC_MIB_DISABLE))
   {
      /* disable MIB counters */
      MCF_FEC_MIBC(ETH_PORT) = MCF_FEC_MIBC_MIB_DISABLE;
      /* wait for MIB to go idle */
      while (!(MCF_FEC_MIBC(ETH_PORT) & MCF_FEC_MIBC_MIB_IDLE))
      {
         /* null */ ;
      }
   }

   /* copy hardware registers into struct */
   MEMCPY(mib, (const void *)&MCF_FEC_RMON_T_DROP(ETH_PORT), sizeof(struct fec_mib));
   /* re-enable MIB counters */
   MCF_FEC_MIBC(ETH_PORT) = save;
#endif

   return (SUCCESS);
}


/* FUNCTION: FEC_clearMIB
 *
 * Reset the MIB hardware registers to zero
 *
 * PARAM1: none
 * 
 * RETURNS: 0 if successful, otherwise negative error code
 */
int
FEC_clearMIB(void)
{
#ifndef	MCF5275		//FSL non-MCF5275 code
					//FSL Since MCF5275 has two Ethernet FECs had to modify
					//FSL	base code.

   uint32 save = MCF_FEC_MIBC;

   if (!(save & MCF_FEC_MIBC_MIB_DISABLE))
   {
      /* disable MIB counters */
      MCF_FEC_MIBC = MCF_FEC_MIBC_MIB_DISABLE;
      /* wait for MIB to go idle */
      while (!(MCF_FEC_MIBC & MCF_FEC_MIBC_MIB_IDLE))
      {
         /* null */ ;
      }
   }

   /* zero the counters */
   MEMSET((void *)&MCF_FEC_RMON_T_DROP, 0, sizeof(struct fec_mib));
   /* re-enable MIB counters */
   MCF_FEC_MIBC = save;

#else			//FSL MCF5275 code

   uint32 save = MCF_FEC_MIBC(ETH_PORT);

   if (!(save & MCF_FEC_MIBC_MIB_DISABLE))
   {
      /* disable MIB counters */
      MCF_FEC_MIBC(ETH_PORT) = MCF_FEC_MIBC_MIB_DISABLE;
      /* wait for MIB to go idle */
      while (!(MCF_FEC_MIBC(ETH_PORT) & MCF_FEC_MIBC_MIB_IDLE))
      {
         /* null */ ;
      }
   }

   /* zero the counters */
   MEMSET((void *)&MCF_FEC_RMON_T_DROP(ETH_PORT), 0, sizeof(struct fec_mib));
   /* re-enable MIB counters */
   MCF_FEC_MIBC(ETH_PORT) = save;

#endif

   return (SUCCESS);
}


/* FUNCTION: fec_pkt_send
 *
 * Send an Ethernet packet
 *
 * PARAM1: pkt                pointer to network buffer structure
 *
 * RETURN: 0 if successful, otherwise error code
 */

int
fec_pkt_send(struct netbuf * pkt)
{
   int   err;
   int   s;
   int   i;

#ifdef DEBUG_FEC
   printf("fec_pkt_send() 1: fec_iface = %d %x\n", fec_iface, pkt->nb_plen);
   printf("fec_pkt_send() src: %x %x %x %x %x %x\n", 
          (unsigned char)(pkt->nb_prot)[0], (unsigned char)(pkt->nb_prot)[1], 
          (unsigned char)(pkt->nb_prot)[2], (unsigned char)(pkt->nb_prot)[3], 
          (unsigned char)(pkt->nb_prot)[4], (unsigned char)(pkt->nb_prot)[5]);
   printf("fec_pkt_send() dst: %x %x %x %x %x %x\n", 
          (unsigned char)(pkt->nb_prot)[6], (unsigned char)(pkt->nb_prot)[7],
          (unsigned char)(pkt->nb_prot)[8], (unsigned char)(pkt->nb_prot)[9], 
          (unsigned char)(pkt->nb_prot)[10],(unsigned char)(pkt->nb_prot)[11]);
   printf("fec_pkt_send() type: %x %x\n", 
          (unsigned char)(pkt->nb_prot)[12],(unsigned char)(pkt->nb_prot)[13]);
   printf("\n");
   for (i = 0; i < pkt->nb_plen; i++)
   {
      if ((i%16) == 0)
      {
         printf("\n");
      }
      printf("%x ", (unsigned char)(pkt->nb_prot)[i]);
   }
   printf("\n\n");
#endif

   err = fec_tx(pkt);
   if (err < 0)
   {
      ENTER_CRIT_SECTION(&s);
      pk_free(pkt);     /* free the failed send */
      EXIT_CRIT_SECTION(&s);
   }
   else  /* sent started without detected error */
   {
      /* maintain MIB interface stats */
      nets[fec_iface]->n_mib->ifOutOctets += pkt->nb_plen;
      if (*pkt->nb_prot & 1)
         nets[fec_iface]->n_mib->ifOutNUcastPkts++;
      else
         nets[fec_iface]->n_mib->ifOutUcastPkts++;
/*      err = 0;*/
   }

#ifdef DEBUG_FEC
   printf("fec_pkt_send() 2: %d %lu\n", err, nets[fec_iface]->n_mib->ifOutOctets);
#endif
   return err;
}


/* FUNCTION: fec_tx()
 *
 * Called from fec_pkt_send() to handle low level send for FEC device.
 *
 * PARAM1: PACKET pkt
 *
 * RETURNS: 0 if OK, else negative error code
 */

int 
fec_tx(PACKET pkt)
{
   int err = 0;
   int   s;       /* Interrupt level holder */
#ifdef DEBUG_FEC
   printf("fec_tx() 1\n");
#endif

   ENTER_CRIT_SECTION(&s);

   /* see if next free BD is still in use */
   if (TxBDs[next_txbd].bd_cstatus  & MCF_FEC_TxBD_R)
   {
      dtrap();                /* really? */
      EXIT_CRIT_SECTION(&s);
      return (ENP_RESOURCE);
   }
   putq(&fectxq, (qp)pkt);    /* put pkt to send in FEC TX queue */

   EXIT_CRIT_SECTION(&s);

#ifdef DEBUG_FEC
   printf("fec_tx() 2: % err = %d %lu\n", err, fecstats.fec_ints);
#endif

   ENTER_CRIT_SECTION(&s);
   err = fectx_internal();    /* move TX queue pkts to send BDs */
   if (err)                   /* free packet if there was error */
      pk_free(pkt);
   EXIT_CRIT_SECTION(&s);

#ifdef DEBUG_FEC
   printf("fec_tx() 3: % err = %d %lu\n", err, fecstats.fec_ints);
#endif
   return (err);
}


/* FUNCTION: fectx_internal
 *
 * Low-level transmit routine
 *
 * PARAMS: none
 *
 * RETURN: 0 if success, otherwise an error code
 *
 * This function takes any packets in the tx pending queue fectxq,
 * sets up the TxBDs with their pointer and length; and sets the
 * bits to start the actual transmit.
 *
 * Interrupts should be DISABLED before calling this function.
 */

int
fectx_internal(void)
{
   BD * txbd;    /* scratch */
   BD * txbd2;

   PACKET pkt;
   int frame1_index;

   if (fectxq.q_head == NULL)           /* no more sends ? */
      return 0;

#ifdef DEBUG_FEC_2
   printf("fectx_internal() 1\n");
#endif
   /* thread as many send pkts as we can into the send BDs */
   while (fectxq.q_head)
   {
      txbd = &TxBDs[next_txbd];        /* get next transmit ring entry */

      /* if entry is full pending a send, give up */
      if (txbd->bd_cstatus  & MCF_FEC_TxBD_R)
      {
#ifdef DEBUG_FEC_2
         printf("fectx_internal(): entry full\n");
#endif
         break;
      }
      pkt = (PACKET)getq(&fectxq);     /* get pkt to send */
      txpend[next_txbd] = pkt;
#ifdef DEBUG_FEC_2
      printf("fectx_internal(): %lx %lx\n", (u_long)pkt->nb_prot, (u_long)pkt->nb_buff);
#endif

      frame1_index = next_txbd;
      if (++next_txbd >= NUM_TXBDS)
         next_txbd -= NUM_TXBDS;

      /* set reserved bit to indicate no EOT yet */
      txbd->bd_cstatus |= MCF_FEC_TxBD_TO1;

      /* Check to see if Ethernet Header is aligned on 4 byte boundry.
       * The TCP, IP, PPOPE, IPSEC, etc headers are constructed back to
       * front, and alignment can only be forced for the IP Header.
       * However the same cannot be done for the Ethernet Header, using
       * the ETHHDR_SIZE, ETHHDR_BIAS or ALIGN_TYPE macros.
       *
       * Hence we use local buffer of size ETHHDR_SIZE that is aligned
       * on  a 4 byte boundry to do a 14 byte MEMMOVE() of just the
       * Ethernet header. This local buffer's Tx Descriptor is then chained
       * with another Tx Desc, whose data buffer points to IP Header of 
       * the orignial packet. Note that there are as many local buffers
       * for the Ethernet header as there are Tx Descriptors, hence we
       * are almost always assured that in case we have to do this 
       * work around then we have an available buffer.
       *
       */
      if ((((u_long)pkt->nb_prot) % 4) != 0)
      {
         tx_copies++;
         
#ifdef DEBUG_FEC_2
         printf("fectx_internal() 2: not aligned\n");
#endif
         txbd2 = &TxBDs[next_txbd];    /* get next transmit ring entry */

         /* If the next entry is full pending a send, then we have to resort
          * to a rather inefficient full MEMMOVE() in the same packet.
          */
         if (txbd2->bd_cstatus & MCF_FEC_TxBD_R)
         {
            goto fullcopy;
         }
#ifdef DEBUG_FEC_2
         printf("fectx_internal() 3: not full copy\n");
#endif

         txpend[next_txbd] = pkt;

         if (++next_txbd >= NUM_TXBDS)
            next_txbd -= NUM_TXBDS;       /* wrap TxBD index */

         /* set reserved bit to indicate no EOT yet */
         txbd2->bd_cstatus |= MCF_FEC_TxBD_TO1;

         /* set up hardware to send it */
         txbd2->bd_addr = (u_char*)pkt->nb_prot + ETHHDR_SIZE;

         if (pkt->nb_plen < 60)         /* pad to minimum size */
            txbd2->bd_length = 60 - ETHHDR_SIZE;
         else
            txbd2->bd_length = pkt->nb_plen - ETHHDR_SIZE;

         /* re-write earlier txpend[] as the first part of the frame now uses
          * a local buffer and hence should not be pk_free()'ed.
          */
         txpend[frame1_index] = NULL;

         /* Align Ethernet header on 4 byte boundary */
         MEMMOVE(TxLilPkts[next_txbd], pkt->nb_prot, ETHHDR_SIZE);

         /* set up hardware for 1st frame to send it */
         txbd->bd_addr = TxLilPkts[next_txbd];

         /* We end up here only if we have to MEMCPY() the  Ethernet Header */
         txbd->bd_length = ETHHDR_SIZE;

         /* Remember that we have to mark portions of the frame ready
          * back to front, to avoid a FIFO underrun.
          */
         txbd2->bd_cstatus |= (MCF_FEC_TxBD_R | MCF_FEC_TxBD_L | MCF_FEC_TxBD_TC);
         txbd->bd_cstatus |= (MCF_FEC_TxBD_R);

         goto start_transmit;

fullcopy:
#ifdef DEBUG_FEC_2
         printf("fectx_internal() 4: full copy\n");
#endif
         tx_fullcopy++;
         MEMMOVE(pkt->nb_buff, pkt->nb_prot, pkt->nb_plen);
         pkt->nb_prot = pkt->nb_buff;
      }

#ifdef DEBUG_FEC_2
      printf("fectx_internal() 5\n");
#endif

      /* set up hardware to send it */
      txbd->bd_addr = (u_char*)pkt->nb_prot;

      if (pkt->nb_plen < 60)            /* pad to minimum size */
         txbd->bd_length = 60;
      else
         txbd->bd_length = pkt->nb_plen;

      txbd->bd_cstatus |= (MCF_FEC_TxBD_R | MCF_FEC_TxBD_L | MCF_FEC_TxBD_TC);

start_transmit:
#ifndef	MCF5275		//FSL non-MCF5275 code
      MCF_FEC_TDAR =  MCF_FEC_TDAR_X_DES_ACTIVE;	//FSL Trigger FEC to start transmission
#else				//FSL MCF5275 code
      MCF_FEC_TDAR(ETH_PORT) =  MCF_FEC_TDAR_X_DES_ACTIVE;	//FSL Trigger FEC to start transmission
#endif

#ifdef DEBUG_FEC_2
      printf("fectx_internal() 6\n");
#endif
   }
#ifdef DEBUG_FEC_2
   printf("fectx_internal() end\n");
#endif

   return 0;
}


/* FUNCTION: fec_isr()
 *
 * This is the ISR handler for the FEC interrupt. 
 *
 * PARAMS: none
 *
 * RETURNS: none
 */

static   u_long   events;     /* make static so we can use for debugging */

__interrupt__
void
fec_isr(void)
{
   BD *     bdp;
   int      i;

   fecstats.fec_ints++;
#ifndef	MCF5275		//FSL non-MCF5275 code
   events = MCF_FEC_EIR;    /* save hardware's events mask */
#else				//FSL MCF5275 code
   events = MCF_FEC_EIR(ETH_PORT);    /* save hardware's events mask */
#endif   

   /* handle end of transmit interrupts */
   if (events & MCF_FEC_EIR_TXF)
   {
      fecstats.fec_txints++;
      if ((events & MCF_FEC_EIR_TXF) ==	MCF_FEC_EIR_TXF)	//FSL removed | MCF_FEC_EIR_TXB
                   
         tx_int_txf++;										//FSL transmitted Frame
      if ((events & MCF_FEC_EIR_TXB) == MCF_FEC_EIR_TXB)
         tx_int_txb++;										//FSL transmitted Buffer

      /* free any finished transmit BDs */
      bdp = &TxBDs[last_txbd];								//FSL last_txbd declared static at top of file and initial value is 0
      while (!(bdp->bd_cstatus & MCF_FEC_TxBD_R))			//FSL TxBD_R=0 data has been transmitted (or error)
      {	
         if ((bdp->bd_cstatus & MCF_FEC_TxBD_TO1) == 0)		/* Already cleared? */
            break;

         if (txpend[last_txbd])							//FSL if buffer has been used (i.e. not NULL)
         {
            /* It's safe to call pk_free() from the ISR since both
             * ports (Superloop & NicheTask) protect the free queue
             * by disabling interrupts.
             */
            if (bdp->bd_cstatus & MCF_FEC_TxBD_L)   	/* Free it only if its the last */
            {
               pk_free(txpend[last_txbd]);				//FSL return PACKET to heap
            }
            txpend[last_txbd] = NULL;					//FSL clear buffer to indicate it is free/available
         }

         /* clear the EOT, L, and TC bits */
         bdp->bd_cstatus &= ~( MCF_FEC_TxBD_TO1 |
                               MCF_FEC_TxBD_L   |
                               MCF_FEC_TxBD_TC );

         /* next Tx BD */
         if (++last_txbd >= NUM_TXBDS)			//FSL if increment past last entry in ring buffer
            last_txbd -= NUM_TXBDS;				//FSL reset to beginning of ring buffer
         bdp = &TxBDs[last_txbd];				//FSL update bdp to new/"next" TxBD entry
      }

      /* see if we can start another transmit */
      fectx_internal();
   }

   /* handle receive interrupts. wait for complete frames */
   if (events & MCF_FEC_EIR_RXF)
   {
      PACKET   rxpkt;

      fecstats.fec_rxints++;
      if ((events & MCF_FEC_EIR_RXF) == MCF_FEC_EIR_RXF)	//FSL removed | MCF_FEC_EIR_RXB
                   
         rx_int_rxf++;										//FSL received Frame
      if ((events & MCF_FEC_EIR_RXB) == MCF_FEC_EIR_RXB)
         rx_int_rxb++;										//FSL received Buffer

      bdp = &RxBDs[next_rxbd];		//FSL bdp pointer points to buffer descriptor ring entry to process

      /* while next receive buffer has packet do receive processing */
      while ((bdp->bd_cstatus & MCF_FEC_RxBD_E) == 0)		//FSL RxBD_E=0 data buffer has new data (or error)
      {
         /* if receive had an error, just count the error and leave the
          * buffer in the BD ring
          */
         if (bdp->bd_cstatus & (MCF_FEC_RxBD_ERRS | 0x0400))
         {
            if (bdp->bd_cstatus & MCF_FEC_RxBD_LG)
               fecstats.fec_lgerrs++;
            if (bdp->bd_cstatus & MCF_FEC_RxBD_NO)
               fecstats.fec_noerrs++;
            if (bdp->bd_cstatus & MCF_FEC_RxBD_CR)
               fecstats.fec_crerrs++;
            if (bdp->bd_cstatus & MCF_FEC_RxBD_OV)
               fecstats.fec_overrs++;
            if (bdp->bd_cstatus & MCF_FEC_RxBD_TR)
               fecstats.fec_trerrs++;
         }
         else              /* we received a good BD */
         {
            PACKET pkt;    /* new packet for receive ring */
            int bd2, len;
            BD  *bdp2;

            if ((bdp->bd_cstatus & MCF_FEC_RxBD_L) == 0)	//FSL "if" code ONLY runs if data buffer not last in frame
            {
               /* All frames should fit in a single BD. If they
                * don't, i.e. RxBD_L is not set, the rest of the
                * frame is in the next BD(s), so just combine them. */

               bd2 = next_rxbd;								//FSL this code was outside the "if"
               do 
               {
                  if ((bd2 = bd2 + 1) >= NUM_RXBDS)				//FSL adjust bd2 to point to next ring buffer entry (i.e. the one after current bdp entry)
                     bd2 -= NUM_RXBDS;
                  bdp2 = &RxBDs[bd2];
 
                  if ((bdp2->bd_cstatus & MCF_FEC_RxBD_E) == 0)	//FSL check second buffer to see if data is available (i.e. RxBD_E=0)
                  {
                     len = bdp2->bd_length;						//FSL get length from second BD
                     /* last BD length is total frame length */
                     if (bdp2->bd_cstatus & MCF_FEC_RxBD_L)		//FSL check if second buffer is last in frame
                        len -= bdp->bd_length;					//FSL length of second buffer is second buffer length -  first buffer length

                     if (bdp->bd_length + len <= MAX_ETH_PKT)	//FSL total data/buffer length = first + second buffer length
                     {
                        rx_copies++;
                        
                        /* append the data to the first BD */	//FSL? how do you know that the second buffer will not exceed the first buffer length (i.e. not overright someone elses data)?
                        MEMCPY((void *)(bdp->bd_addr + bdp->bd_length),
                               (void *)bdp2->bd_addr, len);
                        bdp->bd_length += len;					//FSL update the new total buffer length
                        bdp->bd_cstatus = (bdp2->bd_cstatus & ~MCF_FEC_RxBD_W) |	//FSL clear wrap bit of second buffer
                                          (bdp->bd_cstatus & MCF_FEC_RxBD_W);		//FSL set wrap bit of first buffer
                        bdp2->bd_cstatus |= 0x0400;  /* unused bit */
                     }																
                     else     /* too much data to copy */ 							
                     {																
                        /* TODO: increment some error counter */					
                        goto next_recv;
                     }
                  }
                  else
                  {
                     /* TODO: increment some error counter */
                     goto next_recv;
                  }
               }
               while ((bdp2->bd_cstatus & MCF_FEC_RxBD_L) == 0);
            }		//FSL end if((bdp->bd_cstatus & MCF_FEC_RxBD_L) == 0)

            /* First, get a fresh packet for rx ring. If we can't get one then
             * we have to dump the current packet by leaving it in the ring,
             * since the FEC won't let us have an empty ring entry.
             */

               /* If we can fit this packet into a little buffer, then get one
                * and copy the packet. This slows performance a bit, but helps
                * preserve big buffers in memory-tight systems.
                */
//FSL               if ((len < lilbufsiz) &&
               if ((bdp->bd_length < lilbufsiz) &&
                  ((pkt = pk_alloc(bdp->bd_length)) != NULL))
               {
                  /* copy received data into newly alloced small packet. Leave the
                   * big packet in the FEC ring BD, and plug the small packet's address
                   * into the rxpkt variable.
                   */
                  MEMCPY(pkt->nb_buff, (void*)bdp->bd_addr, bdp->bd_length);
                  rxpkt = pkt;             				/* put little pkt in local variable */
                  rxpkt->net = nets[fec_iface]; 		/* install net in rxpkt */
                  input_ippkt(rxpkt, bdp->bd_length); 	/* pass to IP stack */
               }
            else  /* buffer won't fit in lilbuf, use bigbuf */
            {
               pkt = pk_alloc(MAX_ETH_PKT);
               if (pkt)
               {
                  rxpkt = rxpend[next_rxbd];    /* get received packet */
                  rxpend[next_rxbd] = pkt;      /* replace it's buffer in the ring */

                  bdp->bd_addr = (u_char*)pkt->nb_buff;
   
                  /* pass packet to the received queue */
                  rxpkt->net = nets[fec_iface];         /* install net in rxpkt */
                  input_ippkt(rxpkt, bdp->bd_length);   /* pass to IP stack */
               }
               else  /* couldn't get a free packet */
               {
                  nets[fec_iface]->n_mib->ifInDiscards++;   /* count drops */
               }
            }
         }		//FSL end else /* we received a good BD */

next_recv:

         /* clear BD and set it up for another receive */
         bdp->bd_cstatus &= MCF_FEC_RxBD_W;    /* save WRAP bit */
         bdp->bd_cstatus |= MCF_FEC_RxBD_E;		//FSL indicate empty buffer
         bdp->bd_length = MAX_ETH_PKT;			//FSL set length to max value buffer can handle

         if (++next_rxbd >= NUM_RXBDS)			//FSL point to next RxBD
            next_rxbd -= NUM_RXBDS;				//FSL reset if past end of ring buffer
         bdp = &RxBDs[next_rxbd];      			/* look at next RxBD entry */
      }
#ifndef	MCF5275		//FSL non-MCF5275 code
      MCF_FEC_RDAR = MCF_FEC_RDAR_R_DES_ACTIVE;	//FSL enable FEC to check Rx buffers for new data
#else				//FSL MCF5275 code
      MCF_FEC_RDAR(ETH_PORT) = MCF_FEC_RDAR_R_DES_ACTIVE;	//FSL enable FEC to check Rx buffers for new data
#endif      
   }     /* end of receive interupt logic */

   if (events & MCF_FEC_EIR_MII)
   {
      phy_ready = TRUE;
      fecstats.fec_phyints++;
   }

   if (events & (MCF_FEC_EIR_HBERR | MCF_FEC_EIR_BABT |
                 MCF_FEC_EIR_EBERR | MCF_FEC_EIR_BABR ))
   {
      fecstats.fec_errints++;

      if (events & MCF_FEC_EIR_HBERR)
         fecstats.fec_ehberrs++;
      if (events & MCF_FEC_EIR_BABT)
         fecstats.fec_bablterrs++;
      if (events & MCF_FEC_EIR_EBERR)
         fecstats.fec_busyerrs++;
      if (events & MCF_FEC_EIR_BABR)
         fecstats.fec_bablterrs++;
      
#ifndef MCF5275		//FSL non-MCF5275 code
      /* If error shut the FEC off, turn in back on */
      if ((MCF_FEC_ECR & MCF_FEC_ECR_ETHER_EN) == 0)
      {
         dtrap();
         MCF_FEC_ECR = MCF_FEC_ECR_ETHER_EN;
         MCF_FEC_RDAR = MCF_FEC_RDAR_R_DES_ACTIVE;
         MCF_FEC_TDAR = MCF_FEC_TDAR_X_DES_ACTIVE;
      }
#else				//FSL MCF5275 code
      /* If error shut the FEC off, turn in back on */
      if ((MCF_FEC_ECR(ETH_PORT) & MCF_FEC_ECR_ETHER_EN) == 0)
      {
         dtrap();
         MCF_FEC_ECR(ETH_PORT) = MCF_FEC_ECR_ETHER_EN;
         MCF_FEC_RDAR(ETH_PORT) = MCF_FEC_RDAR_R_DES_ACTIVE;
         MCF_FEC_TDAR(ETH_PORT) = MCF_FEC_TDAR_X_DES_ACTIVE;
      }
#endif      
      
   }

   /* When locked under the debugger for long periods the rx_active
    * shuts itself off. Tmp fix:
    */
#ifndef	MCF5275		//FSL non-MCF5275 code    
					//FSL Since MCF5275 has two Ethernet FECs had to modify
					//FSL	base code.
   if (MCF_FEC_RDAR == 0)
   {
      MCF_FEC_RDAR = MCF_FEC_RDAR_R_DES_ACTIVE;	//FSL enable FEC to check Rx buffers for new data
   }

   MCF_FEC_EIR = events;    /* clear the events mask */
#else				//FSL MCF5275 code
   if (MCF_FEC_RDAR(ETH_PORT) == 0)
   {
      MCF_FEC_RDAR(ETH_PORT) = MCF_FEC_RDAR_R_DES_ACTIVE;	//FSL enable FEC to check Rx buffers for new data
   }

   MCF_FEC_EIR(ETH_PORT) = events;    /* clear the events mask */
#endif
   
}


/* input_ippkt() - handle ptk to IP stack.
 * 
 * This is called from the ISR to enque a received packet for the IP (or
 * ARP) code. This Superloop/NicheTask port uses disabling interrupts to
 * protect critical structures instead of semaphores, so this routine
 * does not have to deal with the issue of the ISR occuring while the
 * receive queue's protecting semaphore is locked by the network task.
 * 
 */

int
input_ippkt(PACKET pkt, int length)
{
   struct ethhdr * et;
   et = (struct ethhdr *)(pkt->nb_buff);
   pkt->nb_plen = length - (ETH_HDR_LEN + ETH_CRC_LEN);
   pkt->net->n_mib->ifInOctets += pkt->nb_plen;
   pkt->net = nets[0];
   pkt->nb_prot = pkt->nb_buff + ETH_HDR_LEN;
   pkt->nb_tstamp = cticks;
   pkt->type = et->e_type;

#if 0
   dprintf("ippkt: len = %d\n", length);

   /* If a sender of a packet has the same MAC address we do,
    * then dtrap(). This can happen easily with these
    * addressless boards in a lab situation.
    * Check the last 4 bytes of our hardcoded address:
    *
    * skh - this is not an error if dest == BROADCAST
    */
   if (*(long*)(pkt->nb_buff + 8) == *(long*)(&mac_addr_fec[2]))
   {
      dtrap();
   }
#endif   /* NPDEBUG */

   /* rcvdq is protected by Interupt disabling when it's being accessed
    * by the network task, so it's safe for us to call putq().
    */
   /* -AK- later this should have been done as a part of the ISR. Chk. */
   putq(&rcvdq, pkt);   /* give received pkt to stack */
   SignalPktDemux();    /* wake Interniche net task */

   return (0);
}


void
fec_stats(void * pio, int iface)
{
   ns_printf(pio, "Interrupts: %lu, tx: %lu/%lu/%lu, rx: %lu/%lu/%lu, err: %lu, phy: %lu\n",
      fecstats.fec_ints,
      fecstats.fec_txints, tx_int_txf, tx_int_txb,
      fecstats.fec_rxints, rx_int_rxf, rx_int_rxb,
      fecstats.fec_errints, fecstats.fec_phyints);

   /* Transmit BD errors */
   ns_printf(pio, "TXBD errors: colls: %lu, underrun: %lu, "
         "ret: %lu, rl: %lu, heartbeat: %lu,\n",
         fecstats.fec_clserrs,fecstats.fec_unerrs,fecstats.fec_retries,
         fecstats.fec_rlerrs,fecstats.fec_hberrs);

   /* Ethernet net errors */
   ns_printf(pio, "NET errors: eh: %lu, bablt: %lu, busy: %lu, bablr\n",
      fecstats.fec_ehberrs,fecstats.fec_bablterrs,
      fecstats.fec_busyerrs,fecstats.fec_bablrerrs);

   /* Receive BD errors */
   ns_printf(pio, "RXBD errors: lg: %lu, no: %lu, sh: %lu, crc: %lu,"
      " over: %lu, tr: %lu\n",
      fecstats.fec_lgerrs, fecstats.fec_noerrs, fecstats.fec_sherrs,
      fecstats.fec_crerrs, fecstats.fec_overrs, fecstats.fec_trerrs);

   /* Debug counters */
   ns_printf(pio, "TxBD copies: %lu/%lu, RxBD copies: %lu\n",
      tx_copies, tx_fullcopy, rx_copies);

   ns_printf(pio, "RxBDs[]: (next: %d)\n", next_rxbd);
   dump_bd(pio, RxBDs, NUM_RXBDS);
   ns_printf(pio, "TxBDs[]: (next: %d)\n", next_txbd);
   dump_bd(pio, TxBDs, NUM_TXBDS);
}


/* dump_bd() - Utility to pretty-print a BD ring to console */

void
dump_bd(void * pio, BD * bdp, int count)
{
   int i;

   /* dump the Ethernet BD lists */
   for (i = 0; i < count; i++)
   {
      ns_printf(pio, "%06lx %04lx %03lx", bdp->bd_addr, bdp->bd_cstatus,
	      bdp->bd_length);
      if ((i % 4) == 3)
         ns_printf(pio, "\n");
      else
         ns_printf(pio, " - ");
      bdp++;
   }
}


int
fec_close(int iface)
{
   nets[fec_iface]->n_mib->ifAdminStatus = 2;  /* status = DOWN */
   nets[fec_iface]->n_mib->ifOperStatus = 2;
   nets[fec_iface]->n_mib->ifLastChange = cticks * (100/TPS);

   return(0);
}

