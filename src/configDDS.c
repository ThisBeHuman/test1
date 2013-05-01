/***************************************************************
	Filename:	configDDS.c (DDS Configuration Functions)
	Author:		Diogo Aguiam - diogo.aguiam@ist.utl.pt
	Date:		April 2013
	Version:	v1.0
	
	Dependecies:	configDDS.h
					
	Purpose:	Configuration of the Direct Digital Synthesizers
			in the Signal Generation Board.
			
	Usage:	Each DDS has its own SPORTxA channel assigned and 
		dedicated clock line. The DATA wire is shared between
		the DDS but the master SPORT takes over that output through
		SRU reassignment of the pins.
		
		Board DAI SRU assignements:
		
			W_CLK_1		-		DAI_PB10
			W_CLK_2		-		DAI_PB08
			W_CLK_3		-		DAI_PB06
			DATA(SDI)	-		DAI_PB07
			FQ_UD		-		DAI_PB09
			RESET		-		DAI_PB05
			
			SCALE_B0	-		DAI_PB03
			SCALE_B1	-		DAI_PB01
			OSC_EN		-		DAI_PB04
	
	
	
	Extra:		
		DDS 1	-	SPORT1A	 	-	Excitation Signal
		DDS 2	-	SPORT2A		-	Local Oscillator Ch1
		DDS 3 	- 	SPORT3A		- 	Local Oscillator Ch2
		



***************************************************************/


#include "../h/configDDS.h"

/**************************************************************
			EXTERNAL DDS GLOBAL VARIABLES
***************************************************************/

#define DDS_x6multiplier 0;

char DDS_powerdown = 0;
int DDS1_frequency = 0;
char DDS1_phase = 0;
int DDS2_frequency = 0;
char DDS2_phase = 0;
int DDS3_frequency = 0;
char DDS3_phase = 0;


/**************************************************************
			LOCAL DDS GLOBAL VARIABLES
***************************************************************/

// Semaphore for DDS data line busy by a SPORTx
char SEM_DDS_data_busy = 0;	


//DDS configuration words DMA buffer
unsigned char DDS_DMA_buffer[DDS_CONFIG_SIZE];





/************************************************************
	Function:		InitDDS_IO (void)
	Argument:	
	Description:	Initializes DDS and Current Scale IO pins
	Action:		Sets all of the DDS pins as outputs.
				Oscillator starts High and the rest starts 
				Low.	
************************************************************/
void InitDDS_IO(void){	
// Starting value of the Output Pins.
	
	// DDS Oscillator Enable (default: HIGH)
	DDS_OSC_EN_ON;	
	// Driver Current Scale (default: minimum scale 00)
	DDS_SCALE_b1_L;	// b1
	DDS_SCALE_b0_L;	// b0

	// DDS Reset	
	DDS_RESET_L;	
	// DDS Data
	DDS_DATA_H;	
	// DDS Frequency Update
	DDS_FQ_UD_L;	
	// DDS #1 Write Clock
	DDS_W_CLK1_L;	
	// DDS #2 Write Clock
	DDS_W_CLK2_L;	
	// DDS #3 Write Clock
	DDS_W_CLK3_L;	

	
	// DAI 02 Frame synce Register
	SRU(LOW,DAI_PB02_I);	

	
//Enabling pins as Outputs. High -> Output, Low -> Input
	// DDS Oscillator Enable 
	SRU(HIGH,PBEN04_I);	
	// Driver Current Scale
	SRU(HIGH,PBEN01_I);
	SRU(HIGH,PBEN03_I);	

	// DDS Reset	
	SRU(HIGH,PBEN05_I);	
	// DDS Data
	SRU(HIGH,PBEN07_I);	
	// DDS Frequency Update
	SRU(HIGH,PBEN09_I);	
	// DDS #1 Write Clock
	SRU(HIGH,PBEN10_I);	
	// DDS #2 Write Clock
	SRU(HIGH,PBEN08_I);	
	// DDS #3 Write Clock
	SRU(HIGH,PBEN06_I);

	// DAI 02 Frame sync Register
	SRU(HIGH,PBEN02_I);
	
}



/************************************************************
	Function:		DDS_reset()
	Argument:		
	Description:	Resets all DDS to a output low state.
	Action:	
			
				RESET		-		DAI_PB05
************************************************************/
void DDS_reset(void){
		char k;
		// Reset AD9851
		//DDS_RESET_H;
		
		DDS_RESET_H;
		for(k=0;k<100;k++);
		DDS_RESET_L;
		
		//DDS_RESET_L;
		//for(k=0;k<100;k++);
}

/************************************************************
	Function:		DDS_init()
	Argument:		
	Description:	Initializes serial mode on all AD9851 DDS.
	Action:	
		Must be run after InitDDS_IO();				
			W_CLK_1		-		DAI_PB10
			W_CLK_2		-		DAI_PB08
			W_CLK_3		-		DAI_PB06
			DATA(SDI)	-		DAI_PB07
			FQ_UD		-		DAI_PB09
			RESET		-		DAI_PB05

	AD9851 Initialization sequence:
		1. Reset all DDS.
		2. Assert W_CLK to load Serial enable sequence
		3. Update Frequency
		4. -- Serial Mode is Loaded --
		5. Write a valid 40-bit word to each DDS 
				(F=0Hz, Phase = 0)
		6. Update Frequency
		
	Extra: Figure 17, p.15 , AD9851 Datasheet
			 tDS Data setup time
			Hardwired serial mode - Just send W_CLK# and FQ_UD
************************************************************/
void DDS_init(void){
		char k,i;
		
		// 1. Reset all DDS.
		DDS_reset();
		
		// 2. Assert W_CLK to load Serial enable sequence
		DDS_W_CLK1_H;
		DDS_W_CLK2_H;
		DDS_W_CLK3_H;

		for(k=0;k<100;k++);
				
		DDS_W_CLK1_L;
		DDS_W_CLK2_L;
		DDS_W_CLK3_L;
	
		for(k=0;k<100;k++);
	
		// 3. Update Frequency
		DDS_update_frequency();
		// 4. -- Serial Mode is Loaded --
		
		// 5. Write a valid 40-bit word to each DDS 

	/*   USING SPORTs INTERFACE:
		DDS1_frequency = DDS2_frequency = DDS3_frequency = DDS_0Hz;
		DDS1_phase = DDS2_phase = DDS3_phase = DDS_PHASE_0;
		
		DDS_set_DMA(DDS_ch1);
		DDS_set_SRU(DDS_ch1);
		DDS_start_SPORT();
		
		DDS_set_DMA(DDS_ch2);
		DDS_set_SRU(DDS_ch2);
		DDS_start_SPORT();
		
		DDS_set_DMA(DDS_ch3);
		DDS_set_SRU(DDS_ch3);
		DDS_start_SPORT();
	*/
	/* USING BIT BANGING SOFTWARE MODE */
		DDS_WriteData(DDS_0Hz, DDS_PHASE_0, 0, DDS_ch1);
		DDS_WriteData(DDS_0Hz, DDS_PHASE_0, 0, DDS_ch2);
		DDS_WriteData(DDS_0Hz, DDS_PHASE_0, 0, DDS_ch3);

		DDS_update_frequency();			
			
			

		// 6. Update Frequency
		DDS_update_frequency();
		for(k=0;k<100;k++);
					
}


/************************************************************
	Function:		DDS_update_frequency()
	Argument:		
	Description:	Updates the frequency stored in the
				 registers of all AD9851 DDS
	Action:	
			Must be run after every DDS has been configurated.
				FQ_UD		-		DAI_PB09

			Depends on clear SEM_DDS_data_busy semaphore
************************************************************/
void DDS_update_frequency(void){
		char k;
		//DDS_FQ_UD_H;	
		
	
	while(SEM_DDS_data_busy);
	while(((*pDAI_PIN_STAT)& DAI_PB02));
		for(k=0;k<100;k++);
		DDS_FQ_UD_H;
		for(k=0;k<100;k++);
		DDS_FQ_UD_L;
		//DDS_FQ_UD_L;
}

/* DDS_powerdown()

	Powers down all DDS by shutting off the oscillator.
	Another way could be to program the power down of each DDS.
	
*/
void DDS_osc_off(void){
			// char k;
	DDS_OSC_EN_OFF;
	//DDS_OSC_EN_OFF;	
			//for(k=0;k<100;k++);
		
	
}

/* DDS_powerup()

	Powers up all DDS by turning the oscillator back on
	Requires reconfiguration of the DDS frequencies.
	
*/
void DDS_osc_on(void){
	DDS_OSC_EN_ON;
	//DDS_OSC_EN_ON;	
		
	
}


/************************************************************
	Function:		DDS_current_scale(char scale)
	Argument:		scale
	Description:	Sets the driver output current according 
				to scale.
	Action:	
		DDS_CURRENT_100 	-	100 mA
		DDS_CURRENT_200		-	200 mA
		DDS_CURRENT_500		-	500 mA
		DDS_CURRENT_1000	-	1 A
************************************************************/
void DDS_current_scale(char scale){
	if(scale&0x1){
		DDS_SCALE_b0_H;
		//DDS_SCALE_b0_H;
	}
	else{
		DDS_SCALE_b0_L;
		//DDS_SCALE_b0_L;
	}
	
	if((scale>>1)&0x1){
		DDS_SCALE_b1_H;
		//DDS_SCALE_b1_H;
	}
	else{
		DDS_SCALE_b1_L;
		//DDS_SCALE_b1_L;
	}

}





/******************OBSOLETE**********************************
	Function:		DDS_start_SPORT(void)
	Argument:	
	Description:	Starts SPORT1 DMA transfer to DDS.
	Action:		
			Configures SPORT1 DMA memory index, memory size 
			and word count. Starts DMA transfer.
			Must be run after DDS_set_SRU() and DDS_set_DMA.
			
			SPORT modes: Standard Serial
			Data Transfer type: Standard DMA
			
	Assumptions: Data to be transfered is in DDS_DMA_buffer,
		and data word size is DDS_CONFIG_SIZE
		
		Depends on clear SEM_DDS_data_busy semaphore
************************************************************/
void DDS_start_SPORT(void)
{

	// waits if a DMA transfer is occuring
	while(SEM_DDS_data_busy);
	while(((*pDAI_PIN_STAT)& DAI_PB02));
    //Clear SPORT1 configuration register
    *pSPCTL1 = 0 ;

    // Configuration the DMA
    *pIISP1A =  (unsigned int)	DDS_DMA_buffer;	// Internal DMA memory address
    *pIMSP1A = sizeof(DDS_DMA_buffer[0]);		// Address modifier
    *pCSP1A  = DDS_CONFIG_SIZE; 				// word count 5 bytes 

    // Clock and frame sync divisor. According to DDS timings.
    *pDIV1 = DDS_SPORT_CLK_DIV;
    
    // Posts to SEM_DDS_data_busy semaphore
    SEM_DDS_data_busy = 1;
    // Configure and enable SPORT 1 and DMA
    *pSPCTL1 = (SPTRAN | FSR | LAFS | IFS | LSBF | ICLK | CKRE | SLEN8 | SPEN_A | SDEN_A);
    
    // Transmit mode
    // Frame Sync Required, Late FS and Internal FS
    // Last bit First
    // Internal Clock
    // Rising Edge sampling (In transmit mode, data is sent on falling edge)
    // 8 Bit words (between 3 and 32. Since DDS configuration is 40 bits, 5x 8bit chosen)
    // SPORT enable and DMA enable.

}



/******************OBSOLETE**********************************
	Function:		DDS_set_SRU (char channel)
	Argument:		channel - which DDS 1-3 channel to configure
	Description:	Configures the SRU to prepare the 
				SPORT transfer for a specific channel.
				
	Action:		Must be called before DDS_start_SPORT.
				Sets the SRU to the corresponding DDS channel.
			SRU for the DATA line stays the same, and only 
			the corresponding W_CLK_# wire is configured to
			receive the SPORT_CLK_O.
			
					W_CLK_1 - DAI_PB10
					W_CLK_2 - DAI_PB08
					W_CLK_3 - DAI_PB06
					DATA 	- DAI_PB07
		
		Depends on clear SEM_DDS_data_busy semaphore DAI_PB02
************************************************************/
void DDS_set_SRU(char channel)
{
/*
		W_CLK_1 - DAI_PB10
		W_CLK_2 - DAI_PB08
		W_CLK_3 - DAI_PB06
		DATA 	- DAI_PB07
		FQ_UD	- DAI_PB09
		RESET	- DAI_PB05

*/    
	// waits for semaphore
	
	while(SEM_DDS_data_busy);
	while(((*pDAI_PIN_STAT)& DAI_PB02));
    // DATA pin is PB07 
    SRU(SPORT1_DA_O, DAI_PB07_I);
    //SRU(SPORT1_FS_O, DAI_PB08_I);
    
    // SPORTx does not provide a gated clock. An internal
    // buffer must be used to supply the W_CLK_# outputs.
    // The enable of this buffer corresponds to the 
    // SPORTx frame sync output. This frame sync is
    // configured to be high only during transmission.
    
    // SPORT1 frame sync used to enable the clock output
    SRU(SPORT1_FS_O, MISCA4_I);
    // MISC buffer 4 implements a gated clock that depends on frame sync.
    // All clock output enables are supplied by this buffer.
    SRU(MISCA4_O, PBEN10_I);
    SRU(MISCA4_O, PBEN08_I);
    SRU(MISCA4_O, PBEN06_I);
	
    //#!
    //SRU(SPORT1_FS_O, DAI_PB05_I);
    // frame sync sent to dai PB02 for status interrupt.
    SRU(SPORT1_FS_O, DAI_PB02_I);
    
    //SRU(SPORT1_FS_O,DAI_PB05_I);
    // SRU(SPORT1_CLK_O, DAI_PB10_I);

    
	// Set up of the corresponding W_CLK_# line.
	switch (channel){	
		case DDS_ch1:
				// W_CLK_2 and W_CLK_3 set HIGH
				SRU(HIGH, DAI_PB08_I);
				SRU(HIGH, DAI_PB06_I);
				//  Clock out on WCLK1
				SRU(SPORT1_CLK_O, DAI_PB10_I);
				break;
		case DDS_ch2:
				// W_CLK_1 and W_CLK_3 set HIGH
				SRU(HIGH, DAI_PB10_I);
				SRU(HIGH, DAI_PB06_I);
				//  Clock out on WCLK2
				SRU(SPORT1_CLK_O, DAI_PB08_I);
				break;
		case DDS_ch3:
				// W_CLK_2 and W_CLK_3 set HIGH
				SRU(HIGH, DAI_PB10_I);
				SRU(HIGH, DAI_PB08_I);
				//  Clock out on WCLK3
				SRU(SPORT1_CLK_O, DAI_PB06_I);
				break;
		default:
				// WRONG CHANNEL!
				break;
	}
    

}

/******************OBSOLETE**********************************
	Function:		DDS_set_DMA(channel)
	Argument:		channel - which DDS info to fill buffer with.
				Global Vars:	DDS#_frequency
								DDS#_phase
								DDS_x6multiplier
								DDS_powerdown
	Description:	Configures/fills memory space for DMA transfer..
	Action:		Prepares the DDS_DMA_buffer with the
		frequency, phase and powerdown/ x6multiplier information
		of the corresponding provided channel.
		
		First 4 bytes of DDS_DMA_buffer are the frequency, LSB first
		The last byte is 5 bits phase + powerdown + 0 + x6multiplier
		
		
		Depends on clear SEM_DDS_data_busy semaphore DAI_PB02
************************************************************/
void DDS_set_DMA(char channel)
{
	int frequency = 0;
	char phase = 0;

/*	DDS_DMA_buffer[0] = 0;
	DDS_DMA_buffer[1] = 0;
	DDS_DMA_buffer[2] = 0;
	DDS_DMA_buffer[3] = 0;
	DDS_DMA_buffer[4] = 0;
*/	
	
	// waits for semaphore
	while(SEM_DDS_data_busy);
	while(((*pDAI_PIN_STAT)& DAI_PB02));
	
	switch (channel){	
		case DDS_ch1:
				frequency = DDS1_frequency;
				phase = DDS1_phase;
				break;
		case DDS_ch2:
				frequency = DDS2_frequency;
				phase = DDS2_phase;
				break;
		case DDS_ch3:
				frequency = DDS3_frequency;
				phase = DDS3_phase;
				break;
		default:
				// WRONG CHANNEL!
				break;
	}
	
	DDS_DMA_buffer[0] = frequency&0xff;
	DDS_DMA_buffer[1] = (frequency>>8)&0xff;
	DDS_DMA_buffer[2] = (frequency>>16)&0xff;
	DDS_DMA_buffer[3] = (frequency>>24)&0xff;
	DDS_DMA_buffer[4] = (phase&0x1f)<<3 | DDS_powerdown<<2 | DDS_x6multiplier;
	// Sends a byte through DATA pins LSB first

	
	
}

/************************************************************
	Function:		IRQ_DDS_SP1(int sig_int)
	Argument:		sig_int
	Description:	End of SPORT transfer interruption
	Action:		
			Clears the SEM_DDS_data_busy semaphore.
			
			DDS_set_DMA and DDS_set_SRU can execute.
			Another transfer can be initiated.
************************************************************/
void IRQ_DDS_SP1(int sig_int)
{
	// clears the semaphore
//	if(DAI_PB02==0)
		SEM_DDS_data_busy = 0;
//	if(((*pDAI_PIN_STAT)& DAI_PB02))
//		DDS_update_frequency();
}



/*  DDS_WriteByte(char byte, char channel)

	Writes a byte to the corresponding DDS channel.
	Based on software SPI source code form Microchip
*/
unsigned char DDS_WriteByte(char byte, char channel){

	char i,k;
	unsigned char data;
	//int timeout;

	data = byte;
	for(i=0;i<8;i++){
		//timeout = TIMEOUT;
		if( (data>>i)&0x1 ){
			DDS_DATA_H;
		}
		else{
			
			DDS_DATA_L;
		}
		
		// tDS Data setup time
		//for(k=0;k<100;k++);
		
			// assert write 
		switch (channel){
			case 1:
					DDS_W_CLK1_H;	
					// tWH W_CLOCK High time
					for(k=0;k<1;k++);
					DDS_W_CLK1_L;	
					//for(k=0;k<100;k++);
					break;
			case 2:
					DDS_W_CLK2_H;	
					// tWH W_CLOCK High time
					for(k=0;k<1;k++);
					DDS_W_CLK2_L;	
					//for(k=0;k<100;k++);
					break;
			case 3:
					DDS_W_CLK3_H;	
					// tWH W_CLOCK High time
					for(k=0;k<1;k++);
					DDS_W_CLK3_L;	
					//for(k=0;k<100;k++);
					break;
			default:
					// WRONG CHANNEL!
					break;
			
		}	

		
	}

	return 0;
}



/*  DDS_WriteData(int frequency, char phase, char powerdown, char channel)

	Writes a full frequency and phase configuration to the corresponding DDS channel.
	
*/
unsigned char DDS_WriteData(int frequency, char phase, char powerdown, char channel){
//	union dds_freq dds_frequency;
	char lastbyte=0;
	

	unsigned char w4 = 0x8E;// 0x8E;
	unsigned char w3 = 0xE3;
	unsigned char w2 = 0x38;//0x38;
	unsigned char w1 = 0x0E>>0;//0x0E;
	unsigned char w0 = 0x09; // phase, power down, REF Multiplier
	
	char temp_freq;
		
	//dds_frequency.freq_int = frequency;

	temp_freq = frequency&0xff;
	DDS_WriteByte(temp_freq,channel);
	temp_freq = (frequency>>8)&0xff;
	DDS_WriteByte(temp_freq,channel);
	temp_freq = (frequency>>16)&0xff;
	DDS_WriteByte(temp_freq,channel);
	temp_freq = (frequency>>24)&0xff;
	DDS_WriteByte(temp_freq,channel);

/*	DDS_WriteByte(w4,channel);
	DDS_WriteByte(w3,channel);
	DDS_WriteByte(w2,channel);
	DDS_WriteByte(w1,channel);
*/
	lastbyte = (phase&0x1f) <<3;
	powerdown = powerdown <<2;
	lastbyte = (phase&0x1f) <<3 | powerdown <<2 | DDS_x6multiplier;//1; // 1 is for the ref multiplier on LSB
	DDS_WriteByte(lastbyte,channel);
	// Sends a byte through DATA pins LSB first

	
	
		return 0;
}
