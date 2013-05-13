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



#define USBADDR (int*)(0x08000001)	// USB ADDR, could be any address of external bank 2 
#define STATUS (0x1)		// Read USB STATUS register mask
#define DATA (0x0)			// Read USB DATA mask


#define DATA_AVAI (0x0101)	// USB STATUS data available mask
#define SPACE_AVAI (0x0202)  // USB STATUS space available mask




#endif

