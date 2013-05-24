/***************************************************************
	Filename:	configUSB.h (USB communication)
	Author:		Diogo Aguiam - diogo.aguiam@ist.utl.pt
	Date:		May 2013
	Version:	v1.0
	Purpose:	
	Usage:
	
	Extra:		



***************************************************************/

#ifndef _CONFIGUSB_H
#define _CONFIGUSB_H


#include "..\h\general.h"

#define RHC3    (BIT_18|BIT_19)
#define RHC4    (BIT_20)
#define RHC5    (BIT_20|BIT_18)
#define RHC6    (BIT_20|BIT_19)
#define RHC7    (BIT_20|BIT_18|BIT_19)

#define HC3     (BIT_11|BIT_12)
#define HC4     (BIT_13)
#define HC5     (BIT_13|BIT_11)
#define HC6     (BIT_13)|(BIT_12)
#define HC7     (BIT_13|BIT_12|BIT_11)

#define USBADDR (int*)(0x08000001)	// USB ADDR, could be any address of external bank 2 
#define STATUS (0x1)		// Read USB STATUS register mask
#define DATA (0x0)			// Read USB DATA mask

#define USB_STATUS		1
#define USB_DATA_PIPE	0
#define USB_READ		1
#define USB_WRITE		0
#define	USB_NULL		0


#define DATA_AVAI (0x0101)	// USB STATUS data available mask
#define SPACE_AVAI (0x0202)  // USB STATUS space available mask

#define USB_DATA_AVAILABLE (0x0101)

// USB Packet Defines
#define USB_START_OF_PACKET	0xd3
#define USB_ERROR_FLAG -1

#define USB_READ_TIMEOUT 100

#endif

