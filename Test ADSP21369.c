

/*****************************************************************************
 * Test ADSP21369.c
 *****************************************************************************/
#include <sru.h>

#ifdef __ADSP21371__
	#include <Cdef21371.h>
	#include <def21371.h>
#elif __ADSP21375__
	#include <Cdef21375.h>
	#include <def21375.h>
#elif __ADSP21369__
	#include <Cdef21369.h>
	#include <def21369.h>
#endif

#include <sysreg.h>
#include <signal.h>


/////////////////////////////////////////////////
// 			GLOBALS
/////////////////////////////////////////////////

int gStatus;
int gFreq;
int gChangeFreq;
char gPhase;


void Config_SRU_LEDS(void)
{
//Setting the SRU and route so that Flag pins connects to DPI pin buffers.
//Use Flags 4 to 15 only. Flags 0 to 3 are reserved
	SRU(FLAG4_O,DPI_PB06_I);	//DPI => We can use flags.
	SRU(FLAG5_O,DPI_PB07_I);	//DPI => We can use flags.
	SRU(FLAG6_O,DPI_PB08_I);	//DPI => We can use flags.
	SRU(FLAG7_O,DPI_PB13_I);	//DPI => We can use flags.
	SRU(FLAG8_O,DPI_PB14_I);	//DPI => We can use flags.
	SRU(LOW,DAI_PB15_I);		//DAI => We can't use flags. Value has to be set low or high.
	SRU(LOW,DAI_PB16_I);		//DAI => We can't use flags. Value has to be set low or high.

//Enabling the Buffer using the following sequence: High -> Output, Low -> Input
	SRU(HIGH,DPI_PBEN06_I);
	SRU(HIGH,DPI_PBEN07_I);
	SRU(HIGH,DPI_PBEN08_I);
	SRU(HIGH,DPI_PBEN13_I);
	SRU(HIGH,DPI_PBEN14_I);
	SRU(HIGH,PBEN15_I);		//default format to enable the buffer using DAI
	SRU(HIGH,PBEN16_I);
}

void Setup_leds(void)
{
	Config_SRU_LEDS();

    sysreg_bit_set(sysreg_FLAGS, (FLG3O|FLG4O|FLG5O|FLG6O|FLG7O|FLG8O) ); 	//Setting flag pins as outputs
	sysreg_bit_clr(sysreg_FLAGS, (FLG3|FLG4|FLG5|FLG6|FLG7|FLG8) );			//Clearing flag pins
}


#define LED6_off SRU(LOW,DAI_PB16_I)
#define LED6_on SRU(HIGH,DAI_PB16_I)
#define LED5_off SRU(LOW,DAI_PB15_I)
#define LED5_on SRU(HIGH,DAI_PB15_I)
/* Defines for DDS IO

	Oscillator Enable 	-	DAI 04
	Scale bit 0			-	DAI 03
	Scale bit 1			-	DAI 01
	Reset				-	DAI 05
	Frequency Update	-	DAI 09
	Data				-	DAI 07
	Write Clock DDS #1	-	DAI 10
	Write Clock DDS #2	-	DAI 8
	Write Clock DDS #3	-	DAI 6


*/
#define DDS_OSC_EN_ON 	SRU(HIGH,DAI_PB04_I)
#define DDS_OSC_EN_OFF 	SRU(LOW,DAI_PB04_I)


#define DDS_SCALE_b0_H 	SRU(HIGH,DAI_PB03_I)
#define DDS_SCALE_b0_L 	SRU(LOW,DAI_PB03_I)
#define DDS_SCALE_b1_H 	SRU(HIGH,DAI_PB01_I)
#define DDS_SCALE_b1_L 	SRU(LOW,DAI_PB01_I)

#define DDS_RESET_H 	SRU(HIGH,DAI_PB05_I)
#define DDS_RESET_L 	SRU(LOW,DAI_PB05_I)

#define DDS_FQ_UD_H 	SRU(HIGH,DAI_PB09_I)
#define DDS_FQ_UD_L 	SRU(LOW,DAI_PB09_I)

#define DDS_DATA_H 		SRU(HIGH,DAI_PB07_I)
#define DDS_DATA_L 		SRU(LOW,DAI_PB07_I)

#define DDS_W_CLK1_H 	SRU(HIGH,DAI_PB10_I)
#define DDS_W_CLK1_L 	SRU(LOW,DAI_PB10_I)

#define DDS_W_CLK2_H 	SRU(HIGH,DAI_PB08_I)
#define DDS_W_CLK2_L 	SRU(LOW,DAI_PB08_I)

#define DDS_W_CLK3_H 	SRU(HIGH,DAI_PB06_I)
#define DDS_W_CLK3_L 	SRU(LOW,DAI_PB06_I)

/*

Fout = (DPhase * System clock)/2^32

Dphase = (Fout*2^32)/SystemClock

*/

// Frequencies for system clock of 180MHz - Won't work!
//		DDS jitters frequency when out of range.

/*
#define DDS_10MHz 	238609294
#define DDS_1MHz 	23860929
#define DDS_100kHz	2386092
#define DDS_10kHz 	238609
#define DDS_0Hz 	0
*/
//  For system clock of 120 MHz
/*
#define DDS_10MHz 	357913941
#define DDS_1MHz 	35791394
#define DDS_100kHz	3579139
#define DDS_10kHz 	357913
#define DDS_0Hz 	0
*/

//  For system clock of 30 MHz
#define DDS_10MHz 	1431655765
#define DDS_1MHz 	143165576
#define DDS_100kHz	14316557
#define DDS_10kHz 	1431655
#define DDS_0Hz 	0



#define DDS_PHASE_0	0
#define DDS_PHASE_45	4
#define DDS_PHASE_90	8
#define DDS_PHASE_180	16


#define DDS_CURRENT_100		0
#define DDS_CURRENT_200		1
#define DDS_CURRENT_500		2
#define DDS_CURRENT_1000	3

#define DDS_ch1		1
#define DDS_ch2		2
#define DDS_ch3		3




#define PIN_OSC_EN 			DAI_PB04_I
#define PIN_OSC_EN_REG 		DAI_PB04_I_REG
#define PBEN_OSC_EN_REG  	PBEN04_I_REG 
#define PBEN_OSC_EN 		PBEN04_I


#define PIN_SCALE_b0 		DAI_PB03_I
#define PIN_SCALE_b1 		DAI_PB01_I
#define PIN_SCALE_b0_REG 	DAI_PB03_I_REG
#define PIN_SCALE_b1_REG 	DAI_PB01_I_REG
#define PBEN_SCALE_b0_REG  	PBEN03_I_REG 
#define PBEN_SCALE_b1_REG 	PBEN01_I_REG 
#define PBEN_SCALE_b0 		PBEN03_I
#define PBEN_SCALE_b1		PBEN01_I


#define PIN_RESET 			DAI_PB05_I
#define PIN_RESET_REG 		DAI_PB05_I_REG
#define PBEN_RESET_REG  	PBEN05_I_REG 
#define PBEN_RESET 			PBEN05_I




#define PIN_DATA 			DAI_PB07_I
#define PIN_DATA_REG 		DAI_PB07_I_REG
#define PBEN_DATA_REG  		PBEN07_I_REG 
#define PBEN_DATA 			PBEN07_I

#define PIN_FQ_UD			DAI_PB09_I
#define PIN_FQ_UD_REG		DAI_PB09_I_REG
#define PBEN_FQ_UD_REG  	PBEN09_I_REG 
#define PBEN_FQ_UD 			PBEN09_I

#define PIN_W_CLK3 			DAI_PB06_I
#define PIN_W_CLK2 			DAI_PB08_I
#define PIN_W_CLK1 			DAI_PB10_I
#define PIN_W_CLK3_REG 		DAI_PB06_I_REG
#define PIN_W_CLK2_REG 		DAI_PB08_I_REG
#define PIN_W_CLK1_REG 		DAI_PB10_I_REG
#define PBEN_W_CLK3_REG 	PBEN06_I_REG 
#define PBEN_W_CLK2_REG 	PBEN08_I_REG 
#define PBEN_W_CLK1_REG 	PBEN10_I_REG 
#define PBEN_W_CLK3		 	PBEN06_I
#define PBEN_W_CLK2		 	PBEN08_I
#define PBEN_W_CLK1		 	PBEN10_I
// Avoid cc0020: error, where adding _REG screws everything




union dds_freq {
	int freq_int;
	char freq_byte[4];
		
};







void DDS_init_io(void){
	
	
	//DAI => We can't use flags. Value has to be set low or high.
	
	
	// Oscillator starts active
	SRU(HIGH,PIN_OSC_EN);	
		
	SRU(LOW,PIN_SCALE_b0);	
	SRU(LOW,PIN_SCALE_b1);	
	
	SRU(LOW,PIN_RESET);		

	SRU(LOW,PIN_DATA);	
		
	SRU(LOW,PIN_FQ_UD);		

	SRU(LOW,PIN_W_CLK3);	
	SRU(LOW,PIN_W_CLK2);	
	SRU(LOW,PIN_W_CLK1);	

	SRU(LOW,DAI_PB02_I);	

	
		//Enabling the Buffer using the following sequence: High -> Output, Low -> Input
	SRU(HIGH,PBEN_OSC_EN);		//default format to enable the buffer using DAI
	SRU(HIGH,PBEN_SCALE_b0);
	SRU(HIGH,PBEN_SCALE_b1);		//default format to enable the buffer using DAI
	SRU(HIGH,PBEN_RESET);
	SRU(HIGH,PBEN_DATA);		//default format to enable the buffer using DAI
	SRU(HIGH,PBEN_FQ_UD);
	SRU(HIGH,PBEN_W_CLK3);		//default format to enable the buffer using DAI
	SRU(HIGH,PBEN_W_CLK2);
	SRU(HIGH,PBEN_W_CLK1);		//default format to enable the buffer using DAI
	
	SRU(HIGH,PBEN02_I);	
	

}


void DDS_update_frequency(void);

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
	union dds_freq dds_frequency;
	char lastbyte=0;
	

	unsigned char w4 = 0x8E;// 0x8E;
	unsigned char w3 = 0xE3;
	unsigned char w2 = 0x38;//0x38;
	unsigned char w1 = 0x0E>>0;//0x0E;
	unsigned char w0 = 0x09; // phase, power down, REF Multiplier
	
	char temp_freq;
		
	dds_frequency.freq_int = frequency;

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


//////////////////////////////////////////////////////////////////////////////
// void IRQ0_routine(int sig_int)
//
// PURPOSE:		ISR for pushbutton 2
//
//////////////////////////////////////////////////////////////////////////////
void IRQ0_routine(int sig_int)
{
	if(gStatus %2== 1){
	//	gStatus = 0;
		LED6_off;
	//	DDS_powerdown;
	}else{
	//	gStatus = 1;
		LED6_on;
	//	DDS_powerup;
	}
	DDS_init();
	// Double Reset and INIT - Makes no sense but works...
	DDS_init();
	DDS_current_scale(gStatus%4);
	gStatus++;

}

//////////////////////////////////////////////////////////////////////////////
// void IRQ1_routine(int sig_int)
//
// PURPOSE:		ISR for pushbutton 1
//
//////////////////////////////////////////////////////////////////////////////
void IRQ1_routine(int sig_int)
{
	int freq;
	if(gStatus %2== 1){
	//	gStatus = 0;
		LED5_off;
	}else{
	//	gStatus = 1;
		LED5_on;
	}

	gStatus++;
	gChangeFreq = 1;
	switch(gStatus%4){
		case 0:
			gFreq = DDS_10kHz;
			break;
		case 1:
			gFreq = DDS_100kHz;
			break;
		case 2:
			gFreq = DDS_1MHz;
			break;
		case 3:
			gFreq = DDS_10MHz;
			break;
		default:
			break;
	}
		//gFreq = DDS_10MHz;
	//for(k=0;k<100;k++);
	//DDS_update_frequency();

}



//////////////////////////////////////////////////////////////////////////////
// void Setup_Ints(void)
//
// PURPOSE:		Configure the system to accept the push buttons as inputs
//
//////////////////////////////////////////////////////////////////////////////
void Setup_Ints(void)
{
	//Config_SRU_INTS();


    (*pDAI_IRPTL_PRI) = (SRU_EXTMISCA1_INT  | SRU_EXTMISCA2_INT);    //unmask individual interrupts
    (*pDAI_IRPTL_RE) = (SRU_EXTMISCA1_INT  | SRU_EXTMISCA2_INT);    //make sure interrupts latch on the rising edge

	//Set up interrupt priorities
    sysreg_bit_set(sysreg_IMASK, DAIHI); //make DAI interrupts high priority

    (*pSYSCTL) |= (IRQ0EN|IRQ1EN);

    sysreg_bit_set(sysreg_MODE2, (IRQ0E|IRQ1E) );
    sysreg_bit_clr(sysreg_IRPTL, (IRQ1I|IRQ0I) );
    sysreg_bit_set(sysreg_IMASK, (IRQ1I|IRQ0I) ); //enable IRQ interrupts
    sysreg_bit_set(sysreg_MODE1, IRPTEN ); 		  //enable global interrupts

  //  interrupt(SIG_DAIH,DAIroutine);
    interrupt(SIG_IRQ0,IRQ0_routine);
    interrupt(SIG_IRQ1,IRQ1_routine);
}




void main( void )
{
	/* Begin adding your custom code here */
	int i,k = 0;
	
	unsigned char w4 = 0x8E;// 0x8E;
	unsigned char w3 = 0xE3;
	unsigned char w2 = 0x38;//0x38;
	unsigned char w1 = 0x0E>>0;//0x0E;
	unsigned char w0 = 0x09; // phase, power down, REF Multiplier
	union dds_freq freq;
	freq.freq_byte[0]=0x0E;
	freq.freq_byte[1]=0x38;
	freq.freq_byte[2]=0xE3;
	freq.freq_byte[3]=0x8E;
	
	gStatus = 0;
	gChangeFreq =0;
	gPhase = 0;
	Setup_leds();
	
	//SRU(HIGH,PIN_SCALE_b0);	
	//SRU(HIGH,PIN_SCALE_b1);	

	DDS_init_io();

	DDS_current_scale(DDS_CURRENT_100);
	
	
		
	Setup_Ints();
	
	DDS_init();
	// Double Reset and INIT - Makes no sense but works...
	DDS_init();
	
	//while(1){
		//	LED6_off;
		//	sysreg_bit_clr(sysreg_FLAGS, FLG4);
		//DATA_H
		//	for(i=0; i<4000000;i++);	
		//		for(i=0; i<400000000;i++);	
		//			for(i=0; i<4000000000;i++);	
		//	LED6_on;
		//DATA_L;
		//	sysreg_bit_set(sysreg_FLAGS, FLG4);
		//	for(i=0; i<4000000;i++);	
		
		//	DDS_SCALE_b0_H;
		//	SRU(HIGH,DAI_PB01_I);
		//	for(k=0;k<10000;k++);
		//	DDS_SCALE_b0_L;
		//	SRU(LOW,DAI_PB01_I);
		
		// Reset AD9851

		for(k=0;k<100;k++);
		// assert write 			
		//SRU(HIGH,PIN_RESET);	

		for(k=0;k<100;k++);
		//SRU(LOW,PIN_RESET);	
		for(k=0;k<100;k++);

		// Sets Scale		
		// Write frequency word for DDS ad9851
//		DDS_WriteData(DDS_100kHz,0,0,DDS_ch1);
//		DDS_WriteData(DDS_100kHz,0,0,DDS_ch2);
//		DDS_WriteData(DDS_100kHz,0,0,DDS_ch3);

		for(k=0;k<100;k++);
		//DDS_update_frequency();
/*		
		DDS_WriteByte(w4,3);
		DDS_WriteByte(w3,3);
		DDS_WriteByte(w2,3);
		DDS_WriteByte(w1,3);
		DDS_WriteByte(w0,3);
*/		
		
		// tDS Data setup time
		for(k=0;k<10000;k++);
		
			// assert write 
			
/*		SRU(HIGH,PIN_FQ_UD);	
		// tWH W_CLOCK High time
		for(k=0;k<100;k++);
		
		
		SRU(LOW,PIN_FQ_UD);	
		for(k=0;k<100;k++);
*/	while(1){
		if(gChangeFreq ==1){

			//DDS_init();

		
			for(k=0;k<100;k++);
			DDS_WriteData(gFreq,DDS_PHASE_0,0,DDS_ch1);
			DDS_WriteData(gFreq,DDS_PHASE_0,0,DDS_ch2);
			DDS_WriteData(gFreq,DDS_PHASE_90,0,DDS_ch3);
			for(k=0;k<100;k++);
			DDS_update_frequency();
			gChangeFreq = 0;
			gPhase++;
			gPhase = gPhase&0x1f;
		}
	//			DDS_update_frequency();

	}
//	return 0;
}
