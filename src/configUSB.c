/***************************************************************
	Filename:	configUSB.c (USB communicat)ion
	Author:		Diogo Aguiam - diogo.aguiam@ist.utl.pt
	Date:		May 2013
	Version:	v1.0
	
	Dependecies:	configUSB.h
					
	Purpose:	Configuration and control functions for USB
		communication with the PC computer.
			
	Usage:	

***************************************************************/


#include "../h/configUSB.h"

/**************************************************************
			EXTERNAL USB GLOBAL VARIABLES
***************************************************************/



/**************************************************************
			LOCAL USB GLOBAL VARIABLES
***************************************************************/

#define NOP asm("nop;")

void A0_HIGH(void){ SRU(HIGH, DAI_PB13_I);}
void A0_LOW(void){ SRU(LOW, DAI_PB13_I);}   

void CSUSB_HIGH(void){ SRU(HIGH, DAI_PB14_I);}
void CSUSB_LOW(void){ SRU(LOW, DAI_PB14_I);}
/************************************************************/
/* Function: Setup_AMI										*/
/* Description: Configure AMI bus to comunicate with FT2232H*/
/* Argument : None			 								*/
/* Action: EPCTL											*/
/*		   AMICTLx    										*/
/*		   DMACx											*/
/*		   AMISTAT											*/
/************************************************************/

void Setup_AMI(void)
{
	
	*pSYSCTL |= MSEN;
	*pEPCTL &= ~B2SD;
	*pAMICTL2 = AMIEN | BW16 | WS20 | IC5 | RHC2 | HC2 | PKDIS | AMIFLSH;

}

int decode_data(int data)
{
	int copy = 0;
	int bit = 0;
	int I;
	copy = (data>>1)&0x2000;
	bit = (data<<1)&0x4000;
	data &= 0x9FFF;
	data |= copy | bit;
	copy = 0;
	for (I = 0; I < 16; I++){
        bit = data & 0x01;
        copy |= bit;
        data >>= 1;
        copy <<= 1;
    }
    return copy >> 1;	
}
int usb_access(bool op,int val)
{	//op = 1 write
	// op = 0 read
	
	int data = 0x00;
	int a;
	
	if(op)
	{
		CSUSB_LOW(); // CS_FTDI
		A0_LOW();	// A0
		
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		
		*USBADDR = val;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		
		
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		
		CSUSB_HIGH(); // CS_FTDI
		A0_HIGH();	// A0	
		
		return 0;
	}	
	else{
		A0_LOW();	// A0
		CSUSB_LOW(); // CS_FTDI
		
		if(val == 1)
			A0_HIGH(); // A0
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;

		data = *USBADDR;
		A0_LOW();	// A0
		CSUSB_HIGH(); // CS_FTDI
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		
		return decode_data(data);
	}

}


int usb_access2(bool op,int val)
{	//op = 1 write
	// op = 0 read
	
	int data = 0x00;
	int a;
	
	if(op)
	{
		CSUSB_LOW(); // CS_FTDI
		A0_LOW();	// A0
		
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		
		*USBADDR = val;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		
		
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		
		CSUSB_HIGH(); // CS_FTDI
		A0_HIGH();	// A0	
		
		return 0;
	}	
	else{
		A0_LOW();	// A0
		CSUSB_LOW(); // CS_FTDI
		
		if(val == 1)
			A0_HIGH(); // A0
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;

		data = *USBADDR;
		A0_LOW();	// A0
		CSUSB_HIGH(); // CS_FTDI
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		
		return data;
	}

}
