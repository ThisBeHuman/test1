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
#define USB_SPACE_AVAILABLE (0x0202)


// USB Packet Defines
#define USB_START_OF_PACKET	0xd3

#define USB_READ_TIMEOUT 100

#define USB_MAX_PAYLOAD_SIZE	256

// USB Error messages
#define USB_ERROR_FLAG 		-1
#define USB_WRONG_CMD		-10
#define USB_WRONG_CMD_SIZE	-11

// USB Message/Payload Headers
#define USB_MSG_CHANGE_FREQ		0
#define USB_MSG_SET_GAIN		1
#define USB_MSG_CURRENT_SCALE	2
#define USB_MSG_ADC_SAMPLING	3
#define USB_MSG_ADC_STOP_SAMPLING	4

#define USB_MSG_DDS_POWERDOWN	2

// USB Message/Payload sizes
#define USB_MSG_CHANGE_FREQ_SIZE		16
#define USB_MSG_SET_GAIN_SIZE		3
#define USB_MSG_CURRENT_SCALE_SIZE	2
#define USB_MSG_ADC_SAMPLING_SIZE	10
#define USB_MSG_ADC_STOP_SAMPLING_SIZE	1

#define USB_MSG_DDS_POWERDOWN_SIZE	12//#!

extern unsigned char USB_PAYLOAD_BUFFER[USB_MAX_PAYLOAD_SIZE];


// Function prototypes

unsigned short processDDSChangeFreq(unsigned short msg_size, unsigned char * msg_buffer);

unsigned short processSetGain(unsigned short msg_size, unsigned char * msg_buffer);
unsigned short processSetCurrentScale(unsigned short msg_size, unsigned char * msg_buffer);
unsigned short processADCStartSampling(unsigned short msg_size, unsigned char * msg_buffer);





#endif

