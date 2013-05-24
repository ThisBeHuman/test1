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

void A0_HIGH(void){ SRU(HIGH, DAI_PB11_I);}
void A0_LOW(void){ SRU(LOW, DAI_PB11_I);}   

void CSUSB_HIGH(void){ SRU(HIGH, DAI_PB12_I);}
void CSUSB_LOW(void){ SRU(LOW, DAI_PB12_I);}



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
    copy >>=1;
    return copy;	
}


int decode16(char data){
	int mask = 0x01;
	char aux = 0,aux1,aux2;
	int i;
	for(i=0;i<16;i++){
		aux1 = data>> i;
		aux2 = aux1&mask;
		aux |= (((data>>i)&mask)<<(15-i));
		//mask = mask >>1;
			
	}	
	return aux;
	
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
		
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
	//	val = decode16(val);
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
		if(val == 1)
			A0_HIGH(); // A0

		CSUSB_LOW(); // CS_FTDI
		
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;

		data = *USBADDR;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;

		CSUSB_HIGH(); // CS_FTDI
		A0_LOW();	// A0
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		
		return data;//decode16(data);
	}

}


/************************************************************
	Function:		InitUSB_IO (void)
	Argument:	
	Description:	Initializes USB IOs
	Action:		
				DAI_PB11	-	A0
				DAI_PB12	-	!CS
				DATA0-15	-	DA0-7 & DB0-7
				USBADDRESS 	-	Memory Bank 2
				
************************************************************/
void InitUSB_IO(void){	
	SRU(HIGH, PBEN11_I);	// A0
	SRU(HIGH, PBEN12_I);	// !CS
	
	SRU(HIGH, DAI_PB12_I);
	SRU(HIGH, DAI_PB11_I);

}


/************************************************************
	Function:		USB_init (void)
	Argument:	
	Description:	Configures the AMI bus to communicate with
			the FT2232H.
	Action:		
				EPCTL2
				AMICTLx
				AMISTAT
				
************************************************************/
void USB_init(void)
{
	
	*pSYSCTL |= MSEN;
	*pEPCTL &= ~B2SD;
	*pAMICTL2 = AMIEN | BW16 | WS20 |PREDIS | IC5 | RHC5 | HC5 | PKDIS | AMIFLSH;
	// Bus width = 16
	// HC5 Bus Hold Cycle
	// IC5 Bus Idle Cycle
	// RHC5 Read Hold Cycle at the end of Read Access
	// FLSH - buffer holds data? #!
	
}

/************************************************************
	Function:	char	USB_access (char access, char readwrite, char data)
	Argument:		char access - USB_STATUS or USB_DATA_PIPE
					char access - USB_READ or USB_WRITE
					char data - byte to send
					 	// #! add data as a pointer
	Return:		In case of Read Access, returns the read byte
	Description:	Reads USB Status
	Action:		
	
************************************************************/
int USB_access(char access, char readwrite, char data)
{
	int byte;
	
	if( access == USB_STATUS){
		A0_HIGH(); 
	} else {
		A0_LOW();	
	}
	
	// Chip Selects the USB IC
	CSUSB_LOW(); // CS_FTDI

/*		
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
*/	
	NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
	NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
	
	if( readwrite == USB_READ) {
		byte = *USBADDR;
	} else {
		*USBADDR = data;	
		
	}	
	NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
	NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
	
//	NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
//	NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
	
	//Deasserts the Chip Select
	CSUSB_HIGH(); // CS_FTDI
	A0_LOW();	// A0
/*
	NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
	NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
	
	NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
	NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
*/	
	return byte;
}


/************************************************************
	Function:	USB_isPacketStart (char data)
	Argument:		char data - Byte to check if is start of packet
	Return:		True or False
	Description:	Verifies if the data read is a USB_START_OF_PACKET;
	Action:		
	
************************************************************/
bool USB_isPacketStart(char data)
{
	return (data == USB_START_OF_PACKET);	
}





/************************************************************
	Function:	bool USB_pollingDataAvailable ()
	Argument:		
	Return:		returns True if there is Data Available or False
		if a timeout ocurred.
		
	Description:	Continuously polls the USB to check if 
		there is data available in its fifo.
		Should only be used when reading packets after a 
		USB_START_OF_PACKET 
	Action:		
	
************************************************************/
bool USB_pollDataAvailable(void)
{
	int temp;
	// Timeout for first byte
	temp = USB_READ_TIMEOUT;
	while(!(USB_access(USB_STATUS, USB_READ, USB_NULL) & USB_DATA_AVAILABLE)){
		temp--;
		if(temp==0){
			return FALSE;
		}
	}
	return TRUE;
}

/************************************************************
	Function:	short USB_readPacketSize ()
	Argument:		
	Return:		signed short with size of packet or -1 if given an
			error
			
	Description:	Reads two consecutive bytes and formats 
		them as a signed short. Max Packet Size is 2^15 = 32kB.
		If there was an error reading the bytes it returns 
		USB_ERROR_FLAG
	Action:		
	
************************************************************/
unsigned short USB_readPacketSize(void)
{
	char usbdata;
	char valid;
	unsigned short packetsize=0;
	
	int temp;
	
	// MSByte of Packet Size
	if( USB_pollDataAvailable() == FALSE ){
		return USB_ERROR_FLAG;
	}
	usbdata = USB_access(USB_DATA_PIPE, USB_READ, USB_NULL);
	
	packetsize = usbdata<<8;
	
	// LSByte of Packet Size
	if( USB_pollDataAvailable() == FALSE ){
		return USB_ERROR_FLAG;
	}
	usbdata = USB_access(USB_DATA_PIPE, USB_READ, USB_NULL);
	
	packetsize |= usbdata;
	
	return packetsize;	
}



/************************************************************
	Function:	USB_write_memory (char * memory, int size_of_memory)
	Argument:		char memory - Byte buffer with data to send
					char size_of_memory - number of bytes in memory
	
	Description:	Reads each byte in the buffer memory and
		sends it to the USB FIFO.
	Action:		
	
************************************************************/
void USB_write_memory ( char* memory, int size_of_memory){
	int i=0;
	for(i=0;i<size_of_memory;i++){
		printf ("mem %d: %x\n",i,memory[i]);
	}
	
}

