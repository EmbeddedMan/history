/*
 * File:		common.h
 * Purpose:		File to be included by all project files
 *
 * Notes:
 */

#ifndef _COMMON_H_
#define _COMMON_H_

/* 
 * Force functions to return values in D0 
 */
#pragma pointers_in_D0

/********************************************************************/

#define MCF5223x		1		/* This defines the processor being used */
#define M52233DEMO		1		/* This defines the evb being used */
#define DHCP			1		/* 0=DHCP off, 1=DHCP on */
#define CONSOLE_UART	0		/* Set to the respective UART number to use for console */
#define FULL			0
#define HALF			1
#define DUPLEX			HALF	//FSL enter HALF or FULL to select Ethernet duplex mode
#define ETH_PORT		0		//FSL the MCF5223x has one Ethernet FEC port FEC0

#include "MCF52235.h"					/* processor specific headers */

/********************************************************************/

#endif /* _COMMON_H_ */
