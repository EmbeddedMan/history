/*
 * File:    mii.c
 * Purpose:     
 *
 * Notes:       
 */

#include "common.h"
#include "mii.h"
//FSL fec_mii_write only used in SoftEthernetNegotiation() function
//FSL fec_mii_read used in the Int_handlers.c, for status information
//FSL 	and SoftEthernetNegotiation() function

extern int fsys_frequency;
#define SYS_CLK_MHZ  (fsys_frequency/1000000)

#ifndef MCF5275		//FSL Code for non-MCF5275 port
					//FSL Since MCF5275 has two Ethernet FECs had to modify
					//FSL	base code.
/********************************************************************/
/*
 * Write a value to a PHY's MII register.
 *
 * Parameters:
 *  phy_addr    Address of the PHY.
 *  reg_addr    Address of the register in the PHY.
 *  data        Data to be written to the PHY register.
 *
 * Return Values:
 *  0 on failure
 *  1 on success.
 *
 * Please refer to your PHY manual for registers and their meanings.
 * mii_write() polls for the FEC's MII interrupt event and clears it. 
 * If after a suitable amount of time the event isn't triggered, a 
 * value of 0 is returned.
 */
int
fec_mii_write(int phy_addr, int reg_addr, int data)
{
    int timeout;
    uint32 eimr;

    /* Clear the MII interrupt bit */
    MCF_FEC_EIR = MCF_FEC_EIR_MII;

    /* Mask the MII interrupt */
    eimr = MCF_FEC_EIMR;
    MCF_FEC_EIMR &= ~MCF_FEC_EIMR_MII;

    /* Write to the MII Management Frame Register to kick-off the MII write */
    MCF_FEC_MMFR = (vuint32)(0
        | MCF_FEC_MMFR_ST_01
        | MCF_FEC_MMFR_OP_WRITE
        | MCF_FEC_MMFR_PA(phy_addr)
        | MCF_FEC_MMFR_RA(reg_addr)
        | MCF_FEC_MMFR_TA_10
        | MCF_FEC_MMFR_DATA(data));

    /* Poll for the MII interrupt (interrupt should be masked) */
    for (timeout = 0; timeout < FEC_MII_TIMEOUT; timeout++)
    {
        if (MCF_FEC_EIR & MCF_FEC_EIR_MII)
            break;
    }
    if(timeout == FEC_MII_TIMEOUT)
        return 0;

    /* Clear the MII interrupt bit */
    MCF_FEC_EIR = MCF_FEC_EIR_MII;

    /* Restore the EIMR */
    MCF_FEC_EIMR = eimr;

    return 1;
}
/********************************************************************/
/*
 * Read a value from a PHY's MII register.
 *
 * Parameters:
 *  phy_addr    Address of the PHY.
 *  reg_addr    Address of the register in the PHY.
 *  data        Pointer to storage for the Data to be read
 *              from the PHY register (passed by reference)
 *
 * Return Values:
 *  0 on failure
 *  1 on success.
 *
 * Please refer to your PHY manual for registers and their meanings.
 * mii_read() polls for the FEC's MII interrupt event and clears it. 
 * If after a suitable amount of time the event isn't triggered, a 
 * value of 0 is returned.
 */
int
fec_mii_read(int phy_addr, int reg_addr, uint16* data)
{
    int timeout;
    uint32 eimr;

    /* Clear the MII interrupt bit */
    MCF_FEC_EIR = MCF_FEC_EIR_MII;

    /* Mask the MII interrupt */
    eimr = MCF_FEC_EIMR;
    MCF_FEC_EIMR &= ~MCF_FEC_EIMR_MII;

    /* Write to the MII Management Frame Register to kick-off the MII read */
    MCF_FEC_MMFR = (vuint32)(0
        | MCF_FEC_MMFR_ST_01
        | MCF_FEC_MMFR_OP_READ
        | MCF_FEC_MMFR_PA(phy_addr)
        | MCF_FEC_MMFR_RA(reg_addr)
        | MCF_FEC_MMFR_TA_10);

    /* Poll for the MII interrupt (interrupt should be masked) */
    for (timeout = 0; timeout < FEC_MII_TIMEOUT; timeout++)
    {
        if (MCF_FEC_EIR & MCF_FEC_EIR_MII)
            break;
    }

    if(timeout == FEC_MII_TIMEOUT)
        return 0;

    /* Clear the MII interrupt bit */
    MCF_FEC_EIR = MCF_FEC_EIR_MII;

    /* Restore the EIMR */
    MCF_FEC_EIMR = eimr;

    *data = (uint16)(MCF_FEC_MMFR & 0x0000FFFF);

    return 1;
}
/********************************************************************/

#else		//FSL Code for MCF5275 port
	
/********************************************************************/
/*
 * Write a value to a PHY's MII register.
 *
 * Parameters:
 *  phy_addr    Address of the PHY.
 *  reg_addr    Address of the register in the PHY.
 *  data        Data to be written to the PHY register.
 *
 * Return Values:
 *  0 on failure
 *  1 on success.
 *
 * Please refer to your PHY manual for registers and their meanings.
 * mii_write() polls for the FEC's MII interrupt event and clears it. 
 * If after a suitable amount of time the event isn't triggered, a 
 * value of 0 is returned.
 */
int
fec_mii_write(int phy_addr, int reg_addr, int data)
{
    int timeout;
    uint32 eimr;

    /* Clear the MII interrupt bit */
    MCF_FEC_EIR(ETH_PORT) = MCF_FEC_EIR_MII;

    /* Mask the MII interrupt */
    eimr = MCF_FEC_EIMR(ETH_PORT);
    MCF_FEC_EIMR(ETH_PORT) &= ~MCF_FEC_EIMR_MII;

    /* Write to the MII Management Frame Register to kick-off the MII write */
    MCF_FEC_MMFR(ETH_PORT) = (vuint32)(0
        | MCF_FEC_MMFR_ST_01
        | MCF_FEC_MMFR_OP_WRITE
        | MCF_FEC_MMFR_PA(phy_addr)
        | MCF_FEC_MMFR_RA(reg_addr)
        | MCF_FEC_MMFR_TA_10
        | MCF_FEC_MMFR_DATA(data));

    /* Poll for the MII interrupt (interrupt should be masked) */
    for (timeout = 0; timeout < FEC_MII_TIMEOUT; timeout++)
    {
        if (MCF_FEC_EIR(ETH_PORT) & MCF_FEC_EIR_MII)
            break;
    }
    if(timeout == FEC_MII_TIMEOUT)
        return 0;

    /* Clear the MII interrupt bit */
    MCF_FEC_EIR(ETH_PORT) = MCF_FEC_EIR_MII;

    /* Restore the EIMR */
    MCF_FEC_EIMR(ETH_PORT) = eimr;

    return 1;
}
/********************************************************************/
/*
 * Read a value from a PHY's MII register.
 *
 * Parameters:
 *  phy_addr    Address of the PHY.
 *  reg_addr    Address of the register in the PHY.
 *  data        Pointer to storage for the Data to be read
 *              from the PHY register (passed by reference)
 *
 * Return Values:
 *  0 on failure
 *  1 on success.
 *
 * Please refer to your PHY manual for registers and their meanings.
 * mii_read() polls for the FEC's MII interrupt event and clears it. 
 * If after a suitable amount of time the event isn't triggered, a 
 * value of 0 is returned.
 */
int
fec_mii_read(int phy_addr, int reg_addr, uint16* data)
{
    int timeout;
    uint32 eimr;

    /* Clear the MII interrupt bit */
    MCF_FEC_EIR(ETH_PORT) = MCF_FEC_EIR_MII;

    /* Mask the MII interrupt */
    eimr = MCF_FEC_EIMR(ETH_PORT);
    MCF_FEC_EIMR(ETH_PORT) &= ~MCF_FEC_EIMR_MII;

    /* Write to the MII Management Frame Register to kick-off the MII read */
    MCF_FEC_MMFR(ETH_PORT) = (vuint32)(0
        | MCF_FEC_MMFR_ST_01
        | MCF_FEC_MMFR_OP_READ
        | MCF_FEC_MMFR_PA(phy_addr)
        | MCF_FEC_MMFR_RA(reg_addr)
        | MCF_FEC_MMFR_TA_10);

    /* Poll for the MII interrupt (interrupt should be masked) */
    for (timeout = 0; timeout < FEC_MII_TIMEOUT; timeout++)
    {
        if (MCF_FEC_EIR(ETH_PORT) & MCF_FEC_EIR_MII)
            break;
    }

    if(timeout == FEC_MII_TIMEOUT)
        return 0;

    /* Clear the MII interrupt bit */
    MCF_FEC_EIR(ETH_PORT) = MCF_FEC_EIR_MII;

    /* Restore the EIMR */
    MCF_FEC_EIMR(ETH_PORT) = eimr;

    *data = (uint16)(MCF_FEC_MMFR(ETH_PORT) & 0x0000FFFF);

    return 1;
}
/********************************************************************/
#endif

#define FEC_LEVEL				4

/********************************************************************/
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

/********************************************************************/
//FSL configure the FEC IMRs in processor specific file
void 
FEC_IMR_init()			
{
	MCF_INTC0_IMRL &= 	~(MCF_INTC_IMRL_INT_MASK23); // TXF
	MCF_INTC0_IMRL &= 	~(MCF_INTC_IMRL_INT_MASK24); // TXB
	MCF_INTC0_IMRL &= 	~(MCF_INTC_IMRL_INT_MASK27); // RXF
	MCF_INTC0_IMRL &= 	~(MCF_INTC_IMRL_INT_MASK29); // MII
}

/********************************************************************/
// Init ePHY
/********************************************************************/
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

