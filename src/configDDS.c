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
			LOCAL DDS GLOBAL VARIABLES
***************************************************************/

// Semaphore for DDS data line busy by a SPORTx
char sem_dds_data_busy = 0;	





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
	SRU(HIGH,DAI_PB04_I);	
	// Driver Current Scale (default: minimum scale 00)
	SRU(LOW,DAI_PB01_I);
	SRU(LOW,DAI_PB03_I);	

	// DDS Reset	
	SRU(LOW,DAI_PB05_I);	
	// DDS Data
	SRU(LOW,DAI_PB07_I);	
	// DDS Frequency Update
	SRU(LOW,DAI_PB09_I);	
	// DDS #1 Write Clock
	SRU(LOW,DAI_PB10_I);	
	// DDS #2 Write Clock
	SRU(LOW,DAI_PB08_I);	
	// DDS #3 Write Clock
	SRU(LOW,DAI_PB06_I);	

//Enabling pins as Outputs. High -> Output, Low -> Input
	// DDS Oscillator Enable 
	SRU(HIGH,PBEN04_I);	
	// Driver Current Scale
	SRU(LOW,PBEN01_I);
	SRU(LOW,PBEN03_I);	

	// DDS Reset	
	SRU(LOW,PBEN05_I);	
	// DDS Data
	SRU(LOW,PBEN07_I);	
	// DDS Frequency Update
	SRU(LOW,PBEN09_I);	
	// DDS #1 Write Clock
	SRU(LOW,PBEN10_I);	
	// DDS #2 Write Clock
	SRU(LOW,PBEN08_I);	
	// DDS #3 Write Clock
	SRU(LOW,PBEN06_I);


}





void DDS_update_frequency(void);

/*  DDS_WriteByte(char byte, char channel)

	Writes a byte to the corresponding DDS channel.
	Based on software SPI source code form Microchip
*/

/************************************************************
	Function:		DDS_WriteByte (void)
	Argument:	
	Description:	
	Action:		
	.	#!
************************************************************/
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
					for(k=0;k<10;k++);
					DDS_W_CLK1_L;	
					//for(k=0;k<100;k++);
					break;
			case 2:
					DDS_W_CLK2_H;	
					// tWH W_CLOCK High time
					for(k=0;k<10;k++);
					DDS_W_CLK2_L;	
					//for(k=0;k<100;k++);
					break;
			case 3:
					DDS_W_CLK3_H;	
					// tWH W_CLOCK High time
					for(k=0;k<10;k++);
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
	lastbyte = lastbyte | powerdown | 0;//1; // 1 is for the ref multiplier on LSB
	DDS_WriteByte(lastbyte,channel);
	// Sends a byte through DATA pins LSB first

	
	
		return 0;
}
/*	DDS_reset()
	
	Resets all DDS to a output low state.

*/
void DDS_reset(void){
		char k;
		// Reset AD9851
		DDS_RESET_H;
		for(k=0;k<100;k++);
		DDS_RESET_L;
		for(k=0;k<100;k++);
}

/*	DDS_init()
		
		Initializes serial mode on all AD9851 DDS.

*/
void DDS_init(void){
		// Initialize serial mode ad9851
		// Hardwired serial mode - Just send W_CLK1 and FQ_UD
		// Figure 17, p.15 , AD9851 Datasheet
				// tDS Data setup time
		
		char k,i;
		//for(k=0;k<100;k++);
		
		// Initializes ADSP IO pins
		
		
		DDS_reset();
		
		//for(i=0;i<5;i++){
			DDS_W_CLK1_H;
			DDS_W_CLK2_H;
			DDS_W_CLK3_H;
		
		
			for(k=0;k<100;k++);
			DDS_W_CLK1_L;
			DDS_W_CLK2_L;
			DDS_W_CLK3_L;
		
			for(k=0;k<100;k++);
		//}
		
		// Program a valid word to the DDS registers
//		DDS_WriteData(0,0,0,DDS_ch1);
//		DDS_WriteData(0,0,0,DDS_ch2);
//		DDS_WriteData(0,0,0,DDS_ch3);
		
		
		
		// Activate serial mode
			//SRU(HIGH,PIN_FQ_UD);
		DDS_update_frequency();
			//for(k=0;k<1000;k++);		
			
		// Program a valid word to the DDS registers
		DDS_WriteData(DDS_0Hz,DDS_PHASE_0,0,DDS_ch1);
		DDS_WriteData(DDS_0Hz,DDS_PHASE_0,0,DDS_ch2);
		DDS_WriteData(DDS_0Hz,DDS_PHASE_0,0,DDS_ch3);
		DDS_update_frequency();
			
			for(k=0;k<100;k++);
					
}

/* DDS_update_frequency()

	Updates the frequency stored in the registers of all AD9851 DDS
	
*/
void DDS_update_frequency(void){
			 char k;
		DDS_FQ_UD_H;	
			for(k=0;k<100;k++);
		
		DDS_FQ_UD_L;
}

/* DDS_powerdown()

	Powers down all DDS by shutting off the oscillator.
	Another way could be to program the power down of each DDS.
	
*/
void DDS_powerdown(void){
			// char k;
		DDS_OSC_EN_OFF;	
			//for(k=0;k<100;k++);
		
	
}

/* DDS_powerup()

	Powers up all DDS by turning the oscillator back on
	Requires reconfiguration of the DDS frequencies.
	
*/
void DDS_powerup(void){
		DDS_OSC_EN_ON;	
		
	
}


/* DDS_current_scale(char scale)

	Sets the driver output current scale.
	0 	-	100 mA
	1	-	200 mA
	2	-	500 mA
	3	-	1 A
	
*/
void DDS_current_scale(char scale){
	if(scale&0x1){
		DDS_SCALE_b0_H;
	}
	else{
		DDS_SCALE_b0_L;
	}
	
	if((scale>>1)&0x1){
		DDS_SCALE_b1_H;
	}
	else{
		DDS_SCALE_b1_L;
	}

}
