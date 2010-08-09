// *** nichelite.c ****************************************************
// This file contains the bulk of my "changes" to the nichelite tcpip
// stack.  I've tried to keep these as localized as possible, so we
// can accept new versions of the nichelite tcpip stack with minimal
// trouble.

#include "ipport.h"
#include "netbuf.h"
#include "net.h"
#include "mtcp.h"
#include "msock.h"
#include "tcpapp.h"
#include "osport.h"

extern unsigned char disable_autorun;
extern unsigned char far __heap_addr[], __heap_size[];

volatile unsigned long cticks = 0;

uint8 powerup_config_flags;

int uart_yield;

#define FEC_LEVEL  4

#define assert(x)  if (! (x)) { asm { halt } }

M_SOCK rich_so;


// *** MCF52235.C EXTENSIONS ******************************************

// we use a shared timer
void
processor_PIT_Timer_Init()
{
}

//processor specific FEC ICR initialization
void 
FEC_ICR_init(void)		
{
	uint32 i;
	
   /* Set up FEC interrupts (vectors 23 through 35) */
   for (i = 23; i < 36; i++)
   {
      /* Hook FEC interrupt to our ISR */
    /* //jpw - should be macro'ed */
	  MCF_INTC0_ICR(i) = MCF_INTC_ICR_IL(FEC_LEVEL);
   }
	
}

//configure the FEC IMRs in processor specific file
void 
FEC_IMR_init()			
{
	MCF_INTC0_IMRL &= 	~(MCF_INTC_IMRL_INT_MASK23); // TXF
	MCF_INTC0_IMRL &= 	~(MCF_INTC_IMRL_INT_MASK24); // TXB
	MCF_INTC0_IMRL &= 	~(MCF_INTC_IMRL_INT_MASK27); // RXF
	MCF_INTC0_IMRL &= 	~(MCF_INTC_IMRL_INT_MASK29); // MII
}


// *** MCF52235_SYSINIT.C EXTENSIONS **********************************

void mcf5223x_ePHY_init(void)
{
	uint32 		myctr; 					//generic counter variable
	uint16 		mymrdata, mymwdata;    	//temp variable for MII read/write data
  	uint16		reg0, reg1, reg4;				//


//FSL replace function call	fec_mii_init((SYSTEM_CLOCK));  with below MCF_FEC_MSCR macro
    /*
     * Configure MII interface speed. Must be <= 2.5MHz
     *
     * Desired MII clock is 2.5MHz
     * MII Speed Setting = System_Clock_Bus_Speed / (2.5MHz * 2)
     */
    MCF_FEC_MSCR = MCF_FEC_MSCR_MII_SPEED((uint32)(SYS_CLK_MHZ/5));

  	// set phy address to zero
  	MCF_EPHY_EPHYCTL1 = MCF_EPHY_EPHYCTL1_PHYADD(ETH_PORT);

  	//Enable EPHY module with PHY clocks disabled
  	//Do not turn on PHY clocks until both FEC and EPHY are completely setup (see Below)
  	MCF_EPHY_EPHYCTL0 = (uint8)(MCF_EPHY_EPHYCTL0  & ~(MCF_EPHY_EPHYCTL0_DIS100 | MCF_EPHY_EPHYCTL0_DIS10)); 

  	//Disable auto_neg at start-up
  	MCF_EPHY_EPHYCTL0 = (uint8)(MCF_EPHY_EPHYCTL0 | (MCF_EPHY_EPHYCTL0_ANDIS));

  	//Enable EPHY module
  	MCF_EPHY_EPHYCTL0 = (uint8)(MCF_EPHY_EPHYCTL0_EPHYEN | MCF_EPHY_EPHYCTL0);

		// Force ePHY to manual, 100mbps, Half Duplexe
		(void)fec_mii_read(0, 0, &reg0);
		reg0 |= 0x2000;								// 100Mbps
		reg0 &= ~0x0100;							// Half Duplexe
		reg0 &= ~0x1000;							// Manual Mode	
		(void)fec_mii_write( 0, 0, reg0 );
//		(void)fec_mii_write( 0, 0, (reg0|0x0200) ); // Force re-negotiate 

	// Startup delay
//	for (myctr=150000; myctr >0; myctr--){uart_isr(0);}

#if 0
	//Enable PHY interrupts in Reg 16 (PHY Interrupt Control Register)
	//Set PHY Interrupt Control Register
	mymwdata = PHY_R16_ACKIE | PHY_R16_PRIE | PHY_R16_LCIE | PHY_R16_ANIE;
	mymwdata = mymwdata | PHY_R16_PDFIE | PHY_R16_RFIE | PHY_R16_JABIE;
	while (!(fec_mii_write(FEC_PHY0, PHY_REG_IR, mymwdata)))
	{		
	};
	MCF_INTC0_ICR36 = MCF_INTC_ICR_IL(3);
	MCF_INTC0_IMRH &=  ~(MCF_INTC_IMRH_INT_MASK36);
	MCF_EPHY_EPHYCTL0 = MCF_EPHY_EPHYCTL0 | (MCF_EPHY_EPHYCTL0_EPHYIEN );
#endif

	//for (myctr=10000; myctr >0; myctr--){uart_check();}

	//*****************************************************************************
	//
	// Work-around for bug in hardware autonegotiation.
	// Attempt to connect at 100Mbps - Half Duplexe
	// Wait for seconds
	// Attempt to connect at 10Mbps - Half Duplexe
	// 
	// Returns 10, or 100 on success, 0 on failure
	//*****************************************************************************
	if( 1 )
	{
		// Force ePHY to manual, 100mbps, Half Duplexe
		while( !fec_mii_read(0, 0, &reg0) ){};
		reg0 |= 0x2000;								// 100Mbps
		reg0 &= ~0x0100;							// Half Duplexe
		reg0 &= ~0x1000;							// Manual Mode	
		while( !fec_mii_write( 0, 0, reg0 ) ){};
		while( !fec_mii_write( 0, 0, (reg0|0x0200) )){}; // Force re-negotiate 
	
		for( myctr=400000; myctr; myctr-- )
		{
			//uart_check();
			(void)fec_mii_read(0, 1, &reg1);
			if( reg1 & 0x0004 )
			{
//				printf( "\nLink UP - 100 HD" );				
				return;
			}
		}
	
		// Force ePHY to manual, 10mbps, Half Duplexe
		while( !fec_mii_read(0, 0, &reg0) ){};
		reg0 &= ~0x2000;							// 10Mbps
		reg0 &= ~0x0100;							// Half Duplexe
		reg0 &= ~0x1000;							// Manual Mode	
		while( !fec_mii_write( 0, 0, reg0 ) ){};
		while( !fec_mii_write( 0, 0, (reg0|0x0200) )){}; // Force re-negotiate 
	
#if 0
		for( myctr=20000; myctr; myctr-- )
		{
			//uart_check();
			(void)fec_mii_read(0, 1, &reg1);
			printf( "\nLink UP - 10 HD" );
			if( reg1 & 0x0004 )
			{
				printf( "\nLink UP - 10 HD" );				
				return;
			}
		}
#endif		
	}
	
//	printf("\nLink DOWN" );
	
	return;
}


// *** MAIN.C EXTENSIONS **********************************************

int
dhcp_callback(int iface, int state)
{
    if (! rich_so) {
        return dhc_main_ipset(iface, state);
    }
    return 0;
}

int main (void)
{
    int i;
    extern struct net netstatic[1];  /* actual net structs */
    
	// clear mask all bit to allow interrupts
    MCF_INTC0_IMRL &= ~(MCF_INTC_IMRL_MASKALL);

	// initialize the ethernet LEDs
	MCF_GPIO_PLDPAR = (0
					| MCF_GPIO_PORTLD_PORTLD0 // ACTLED
					| MCF_GPIO_PORTLD_PORTLD1 // LNKLED
					| MCF_GPIO_PORTLD_PORTLD2 // SPDLED
					| MCF_GPIO_PORTLD_PORTLD3 // DUPLED
					| MCF_GPIO_PORTLD_PORTLD4 // COLLED
					| MCF_GPIO_PORTLD_PORTLD5 // RXLED
					| MCF_GPIO_PORTLD_PORTLD6 // TXLED
					);
	
	// initialize the phy
    mcf5223x_ePHY_init();
    
   	splx(0);

   /* The C lib already has a small heap in the program image (which
    * is why the printf already works) but we need more. So we
    * take what's left of the 16 megs on the card.
    */
   mheap_init((char *)__heap_addr, (long)__heap_size);  /* start, length */

   printf("\nHeap size = %d bytes\n", (long)__heap_size );

   port_prep = prep_evb;

   netstatic[0].mib.ifDescr = (u_char *)"Fast Ethernet Controller";

    // if we don't have an IP address...
    i = main_ip_address();
    if (disable_autorun || ! i || i == -1) {
        // use dhcp
        powerup_config_flags = 1;
    } else {
        // use the ip address
        netstatic[0].n_ipaddr = i;
    }

   /* We set the station's Ethernet physical (MAC) address
    * from the address already in use by dBUG. This prevents
    * ARP problems on the development server. Production systems
    * usually read this from flash or eprom.
    */
    
#ifdef NPDEBUG
   dprintf("\netheraddr = %02X:%02X:%02X:%02X:%02X:%02X\n\n",
            mac_addr_fec[0], mac_addr_fec[1], mac_addr_fec[2],
            mac_addr_fec[3], mac_addr_fec[4], mac_addr_fec[5]);
#endif

   /* Heap memory saving trick - reduce the time a TCP socket
    * will linger in CLOSE_WAIT state. For systems with limited
    * heap space and a busy web server, this makes a big difference.
    */
   TCPTV_MSL = 1;    /* set low max seg lifetime default */
   
    // set default window sizes
    mt_defrxwin = 64;
    mt_deftxwin = 2048;
    
    dhc_set_callback(ETH_PORT, dhcp_callback);

   netmain();     /* Start and run net tasks, no return. */
   return 0;
}

void
exit(int code)
{
   printf("Exit, code %d. Push RESET button now.\n", code);
   splx(7);
   while (1)
      asm { halt };
}

void
dtrap()
{
   printf("dtrap\n");
   exit(0);
}

static int crits = 0;
static int sysint;

void
ENTER_CRIT_SECTION(void * p)
{
   if(crits++ == 0)
   {
      sysint = splx(7);
   }
}

void
EXIT_CRIT_SECTION(void * p)
{
   if(--crits == 0)
   {
      splx(sysint);
   }
   if(crits < 0)
   {
      printf("negative crit section ct(%d)\n", crits);
      dtrap();
   }
}


// *** TASK.C EXTENSIONS **********************************************

void
os_yield(void)
{
    tk_yield();
}


// *** NETMAIN.C EXTENSION ********************************************

int rich_callback(int code, M_SOCK so, void * data)
{
	int e = 0;

	switch(code)
	{
		// socket open complete
		case M_OPENOK:
   		    if (rich_so) {
   		        m_close(rich_so);
   		    }
		    rich_so = so;
            led_unknown_progress();
			break;
      
		// socket has closed      
   		case M_CLOSED:  
	        m_close(rich_so);
   		    rich_so = NULL;
     		break;
      
		// passing received data
		// blocked transmit now ready
		case M_RXDATA:          				// received data packet, let recv() handle it 
		case M_TXDATA:          				// ready to send more, loop will do it
			e = -1;        						// return nonzero code to indicate we don't want it 
			break;
      
   		default:
      		dtrap();             				// not a legal case
      		return 0;
   }

   USE_VOID(data);
   return e;
}

TK_ENTRY(tk_rich)
{
   main2();

   USE_ARG(parm);  /* TK_ENTRY macro defines tk_nettick with 1 arg parm */
   TK_RETURN_UNREACHABLE();
   return 0;
}

int rich2_wakes;

TK_ENTRY(tk_rich2)
{
    int n;
      M_SOCK so;
      int error;
      static char buffer[1];
      static struct sockaddr_in addr;
      void terminal_receive(unsigned char *buffer, int length);

      
   /* wait till the stack is initialized */
   while (!iniche_net_ready)
   {
      TK_SLEEP(1);
   }

printf("listen\n");
   addr.sin_addr.s_addr 	= (INADDR_ANY);
   addr.sin_port        	= (1234);
   so = m_listen(&addr, rich_callback, &error);
   assert(so);
   TK_SLEEP(200);
   
   for (;;)
   {
        if (rich_so) {
            n = m_recv(rich_so, buffer, sizeof(buffer));
            assert(n >= 0);
            if (n) {
                terminal_receive((unsigned char *)buffer, n);
            }
            tk_yield();
        } else {
            tk_sleep(1);
        }
        rich2_wakes++;
   }
   USE_ARG(parm);  /* TK_ENTRY macro defines tk_nettick with 1 arg parm */
   TK_RETURN_UNREACHABLE();
}

TK_OBJECT(to_rich);
TK_ENTRY(tk_rich);
TK_OBJECT(to_rich2);
TK_ENTRY(tk_rich2);

struct inet_taskinfo
apptasks[] = {
    {
        &to_rich,
        "rich",
        tk_rich,
        NET_PRIORITY,
        2048
    },
    {
        &to_rich2,
        "rich2",
        tk_rich2,
        NET_PRIORITY,
        2048
    },
};

int num_app_tasks  = sizeof(apptasks)/sizeof(struct inet_taskinfo);

int
create_apptasks(void)
{
    int i;
    int e;
    
   /* Create the threads for apps */
   for (i = 0; i < num_app_tasks; i++)
   {
      e = TK_NEWTASK(&apptasks[i]);
      if (e != 0) 
      {
         dprintf("task create error\n");
         panic("netmain");
         return -1;  /* compiler warnings */
      }
   }
   return 0;
}

