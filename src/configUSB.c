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
unsigned char USB_PAYLOAD_BUFFER[USB_MAX_PAYLOAD_SIZE];



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
	Function:	bool USB_pollingSpaceAvailable ()
	Argument:		
	Return:		returns True if there is Space Available or False
		if a timeout ocurred.
		
	Description:	Continuously polls the USB to check if 
		there is space available in its fifo.
		Should only be used when writing packets to the USB FIFO
	Action:		
	
************************************************************/
bool USB_pollSpaceAvailable(void)
{
	int temp;
	// Timeout for first byte
	temp = USB_READ_TIMEOUT;
	while(!(USB_access(USB_STATUS, USB_READ, USB_NULL) & USB_SPACE_AVAILABLE)){
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
	Return:		signed short with size of packet or -1 if given ano
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
	Function:	short USB_readPayload (unsigned short usb_size, unsigned char * payload_buffer)
	Argument:	unsigned short usb_size - The payload byte count to be read is usb_size - 3 bytes (header+UsbSize bytes)	
				unsigned char * payload_buffer - pointer to the payload buffer
	Return:		TRUE if read all the bytes
				USB_ERROR_FLAG if there was an error
			
	Description:	Receives the USB SIZE of the packet and reads
		usb_size-3 bytes from the USB. These 3 bytes are the 
		header byte and the two usbSize bytes.
		The read bytes fill the payload_buffer buffer given as a pointer.
		
		If all the bytes were read, it returns True, otherwise,
		it returns a USB_ERROR_FLAG
	Action:		
	
************************************************************/
unsigned short USB_readPayload(unsigned short usb_size, unsigned char * payload_buffer)
{

	int temp, index;
	int payload_size = usb_size;
	
	for(index = 0; index < payload_size; index++){
		// Verifies if there is a byte waiting in the USB_FIFO
		if( USB_pollDataAvailable() == FALSE ){
			return USB_ERROR_FLAG;
		}
		payload_buffer[index] = USB_access(USB_DATA_PIPE, USB_READ, USB_NULL);	
	}
	
	return TRUE;	
}

/************************************************************
	Function:	short USB_processPayload (unsigned short payload_size, unsigned char * payload_buffer)
	Argument:	unsigned short payload_size - Payload message size for confirmation
 				unsigned char * payload_buffer - Payload buffer with message to process
	Return:		TRUE if message has been processed without errors.
				USB_ERROR_FLAG if there was an error
				USB_WRONG_CMD if the interpreted command was unrecognized
			
	Description:	Processes and executes the command from
		the read payload message received from the USB according 
		to its header byte.
		It confirms the payload_size with the appropriate command message size.
	Action:		
	
************************************************************/
unsigned short USB_processPayload(unsigned short payload_size, unsigned char * payload_buffer)
{
	int temp, index;	
//	printf("USB Header: %d\n",payload_buffer[0]);
	switch ( payload_buffer[0] ){
		case USB_MSG_CHANGE_FREQ:
			if(payload_size != USB_MSG_CHANGE_FREQ_SIZE) return USB_WRONG_CMD_SIZE;
			
			processDDSChangeFreq(payload_size, payload_buffer);
			break;
		case USB_MSG_SET_GAIN:
			if(payload_size != USB_MSG_SET_GAIN_SIZE) return USB_WRONG_CMD_SIZE;
			processSetGain(payload_size, payload_buffer);
			break;
		case USB_MSG_CURRENT_SCALE:
			if(payload_size != USB_MSG_CURRENT_SCALE_SIZE) return USB_WRONG_CMD_SIZE;
			
			processSetCurrentScale(payload_size, payload_buffer);
			break;
		case USB_MSG_ADC_SAMPLING:
//			printf("payload size:%d\n",payload_size);
			if(payload_size != USB_MSG_ADC_SAMPLING_SIZE) return USB_WRONG_CMD_SIZE;
			
			processADCStartSampling(payload_size, payload_buffer);
			break;
		case USB_MSG_ADC_STOP_SAMPLING:
//			printf("payload size:%d\n",payload_size);
			if(payload_size != USB_MSG_ADC_STOP_SAMPLING_SIZE) return USB_WRONG_CMD_SIZE;
			
			processADCStopSampling(payload_size, payload_buffer);
			break;
		default:
			return USB_ERROR_FLAG;
		
	}
	
	return TRUE;	
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


/************************************************************
	Function:	short processDDSChangeFreq (unsigned short msg_size, unsigned char * msg_buffer)
	Argument:	unsigned short msg_size - Payload message size for confirmation
 				unsigned char * msg_buffer - Payload buffer with message to process
	Return:		TRUE if message has been processed without errors.
				USB_ERROR_FLAG if there was an error
			
			
	Description: Processes a Change Frequency message.
		Verifies if the header byte corresponds to this function
		and if so updates the frequency of the DDS
	Extra:	The change frequency command message is as follows:
	[CHANGE_FREQ_HEADER - 1 byte (0)	|
	 DDS1	Freq - 4 bytes 	(1,2,3,4) 	| Phase	- 1 byte (5)
	 DDS2	Freq - 4 bytes 	(6,7,8,9)	| Phase	- 1 byte (10)
	 DDS3	Freq - 4 bytes (11,12,13,14)| Phase	- 1 byte (15)]
		
************************************************************/
unsigned short processDDSChangeFreq(unsigned short msg_size, unsigned char * msg_buffer)
{
	int temp, index;	
	
	int DDS1_Freq, DDS2_Freq, DDS3_Freq;
	char DDS1_Phase, DDS2_Phase, DDS3_Phase;
	
	// Checks if this message corresponds to a Change Frequency command
	if(msg_size != USB_MSG_CHANGE_FREQ_SIZE 
		&& msg_buffer[0] != USB_MSG_CHANGE_FREQ) {
			return USB_WRONG_CMD;
	}
	
//	printf("CHANGE FREQ\n");
	
	// DDS 1 Frequency
	DDS1_Freq = msg_buffer[1] <<24;
	DDS1_Freq |= msg_buffer[2] <<16;
	DDS1_Freq |= msg_buffer[3] <<8;
	DDS1_Freq |= msg_buffer[4];
	DDS1_Freq = DDS1_Freq * DDS_FREQUENCY_MULTIPLIER;
	
	DDS1_Phase = msg_buffer[5]&0x1f;

		// DDS 2 Frequency
	DDS2_Freq = msg_buffer[6] <<24;
	DDS2_Freq |= msg_buffer[7] <<16;
	DDS2_Freq |= msg_buffer[8] <<8;
	DDS2_Freq |= msg_buffer[9];
	DDS2_Freq = DDS2_Freq * DDS_FREQUENCY_MULTIPLIER;
	
	DDS2_Phase = msg_buffer[10]&0x1f;

		// DDS 3 Frequency
	DDS3_Freq = msg_buffer[11] <<24;
	DDS3_Freq |= msg_buffer[12] <<16;
	DDS3_Freq |= msg_buffer[13] <<8;
	DDS3_Freq |= msg_buffer[14];
	DDS3_Freq = DDS3_Freq * DDS_FREQUENCY_MULTIPLIER;
	DDS3_Phase = msg_buffer[15]&0x1f;
	
	// Reconfigure the DDS.

//		DDS_init();
		// Double Reset and INIT - Makes no sense but works...
		DDS_init();
		DDS_init();

		DDS_WriteData(DDS1_Freq, DDS1_Phase, 0, DDS_ch1);
		DDS_WriteData(DDS2_Freq, DDS2_Phase, 0, DDS_ch2);
		DDS_WriteData(DDS3_Freq, DDS3_Phase, 0, DDS_ch3);
		
		DDS_update_frequency();	

//		DDS_update_frequency();	

	return TRUE;
}


/************************************************************
	Function:	short processSetGain (unsigned short msg_size, unsigned char * msg_buffer)
	Argument:	unsigned short msg_size - Payload message size for confirmation
 				unsigned char * msg_buffer - Payload buffer with message to process
	Return:		TRUE if message has been processed without errors.
				USB_ERROR_FLAG if there was an error
			
			
	Description: Processes a Set Gain message.
		Verifies if the header byte corresponds to this function
		and if so updates the Gain of the Amplifier chain
	Extra:			
************************************************************/
unsigned short processSetGain(unsigned short msg_size, unsigned char * msg_buffer)
{
	int temp, index;	
	
	
	// Checks if this message corresponds to a Change Frequency command
	if(msg_size != USB_MSG_SET_GAIN_SIZE 
		&& msg_buffer[0] != USB_MSG_SET_GAIN) {
			return USB_WRONG_CMD;
	}
	 
	temp = (msg_buffer[1]<<8 | msg_buffer[2])&0x0fff;
	printf("gain: %d\n", temp);
	GAIN_set_voltage(temp,GAIN_PD_ON);
	

	return TRUE;
}


/************************************************************
	Function:	short processSetCurrentScale (unsigned short msg_size, unsigned char * msg_buffer)
	Argument:	unsigned short msg_size - Payload message size for confirmation
 				unsigned char * msg_buffer - Payload buffer with message to process
	Return:		TRUE if message has been processed without errors.
				USB_ERROR_FLAG if there was an error
			
			
	Description: Processes a and sets the current scale
		
	Extra:	Scale Value		Output current
				0				100 mA
				1				200 mA
				2				500 mA
				4				 1	A
************************************************************/
unsigned short processSetCurrentScale(unsigned short msg_size, unsigned char * msg_buffer)
{
	//int temp, index;	
	int current_scale;
	
	// Checks if this message corresponds to a Change Frequency command
	if(msg_size != USB_MSG_CURRENT_SCALE_SIZE 
		&& msg_buffer[0] != USB_MSG_CURRENT_SCALE) {
			return USB_WRONG_CMD;
	}
	 
	current_scale = msg_buffer[1]& 0xff;
	printf("scale: %d\n", current_scale);

	DDS_current_scale(current_scale);
	

	return TRUE;
}



/************************************************************
	Function:	short processADCStartSampling (unsigned short msg_size, unsigned char * msg_buffer)
	Argument:	unsigned short msg_size - Payload message size for confirmation
 				unsigned char * msg_buffer - Payload buffer with message to process
	Return:		TRUE if message has been processed without errors.
				USB_ERROR_FLAG if there was an error
			
			
	Description: Processes an ADC Sampling message
		
	Extra:	#!

************************************************************/
unsigned short processADCStartSampling(unsigned short msg_size, unsigned char * msg_buffer)
{
	//int temp, index;	
	int current_scale;
	unsigned int sampling_period;
	char continuous_sampling;
	unsigned int number_of_samples;
		
	// Checks if this message corresponds to a Change Frequency command
	if(msg_size != USB_MSG_ADC_SAMPLING_SIZE 
		&& msg_buffer[0] != USB_MSG_ADC_SAMPLING) {
			return USB_WRONG_CMD;
	}
	 
	sampling_period = msg_buffer[1] <<24;
	sampling_period |= msg_buffer[2] <<16;
	sampling_period |= msg_buffer[3] <<8;
	sampling_period |= msg_buffer[4];
	
	continuous_sampling = msg_buffer[5]&0x1;
	
	number_of_samples = msg_buffer[6] <<24;
	number_of_samples |= msg_buffer[7] <<16;
	number_of_samples |= msg_buffer[8] <<8;
	number_of_samples |= msg_buffer[9];
	
	ADC_StartSampling(number_of_samples, sampling_period, continuous_sampling);
	return TRUE;
}


/************************************************************
	Function:	short processADCStopSampling (unsigned short msg_size, unsigned char * msg_buffer)
	Argument:	unsigned short msg_size - Payload message size for confirmation
 				unsigned char * msg_buffer - Payload buffer with message to process
	Return:		TRUE if message has been processed without errors.
				USB_ERROR_FLAG if there was an error
			
			
	Description: Stops an ocurring ADC Sampling and sending to USB.
		
	Extra:	

************************************************************/
unsigned short processADCStopSampling(unsigned short msg_size, unsigned char * msg_buffer)
{
	//int temp, index;	
	int current_scale;
	unsigned int sampling_period;
	char continuous_sampling;
	unsigned int number_of_samples;
		
	// Checks if this message corresponds to a Change Frequency command
	if(msg_size != USB_MSG_ADC_STOP_SAMPLING_SIZE 
		&& msg_buffer[0] != USB_MSG_ADC_STOP_SAMPLING) {
			return USB_WRONG_CMD;
	}
	 
	ADC_StopSampling();


	return TRUE;
}


/************************************************************
	Function:	short USB_sendADCData(unsigned short msg_size, unsigned char * msg_buffer)
	Argument:	unsigned short buffer_size - Number of samples to send
 				unsigned char * buffer 
	Return:		TRUE if message has been processed without errors.
				USB_ERROR_FLAG if there was an error
			
			
	Description: Processes a Set Gain message.
		Verifies if the header byte corresponds to this function
		and if so updates the Gain of the Amplifier chain
	Extra:			
************************************************************/
unsigned short USB_sendADCData(int buffer_size, unsigned short * buffer)
{
	int temp, index;	
	int k;
	
	 //printf("send adc data! %d\n",buffer);
	//k = adc_number_of_samples*4;
	for (index = 0; index< adc_number_of_samples ; index++){
		for(k = 0; k < 4; k++){
	
			// Poll for space available
			if( USB_pollSpaceAvailable() == FALSE ){
				return USB_ERROR_FLAG;
			}
			//printf("buffer: %d\n",(buffer[index]>>(k*8))&0xff);
			USB_access(USB_DATA_PIPE, USB_WRITE, (buffer[index]>>(k*8))&0xff);
		}
	}	

	return TRUE;
}




