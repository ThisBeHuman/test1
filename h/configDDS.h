/***************************************************************
	Filename:	configDDS.h (DDS Configuration Functions)
	Author:		Diogo Aguiam - diogo.aguiam@ist.utl.pt
	Date:		April 2013
	Version:	v1.0
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

#ifndef _CONFIGDDS_H
#define _CONFIGDDS_H


#include "..\h\general.h"


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
#define DDS_FREQUENCY_MULTIPLIER	143	// For 30 MHz oscillator

//#define DDS_FREQUENCY_MULTIPLIER	24	// For 180 MHz oscillator

//  For system clock of 30 MHz
#define DDS_10MHz 	1431655765
#define DDS_1MHz 	143165576
#define DDS_100kHz	14316557
#define DDS_10kHz 	1431655
#define DDS_0Hz 	0
#define DDS_90kHz	12884901
#define DDS_99kHz	14173392
    

#define DDS_PHASE_0	0
#define DDS_PHASE_45	4
#define DDS_PHASE_90	8
#define DDS_PHASE_180	16


#define DDS_CURRENT_100		0
#define DDS_CURRENT_200		1
#define DDS_CURRENT_500		2
#define DDS_CURRENT_1000	3

// DDS channel defines
#define DDS_ch1		1
#define DDS_ch2		2
#define DDS_ch3		3

// DDS configuration definitions
#define DDS_CONFIG_SIZE 5

#define DDS_SPORT_CLK_DIV	0x00000008


// External Variables
extern char DDS_x6multiplier;
extern char DDS_powerdown;
extern int DDS1_frequency;
extern char DDS1_phase;
extern int DDS2_frequency;
extern char DDS2_phase;
extern int DDS3_frequency;
extern char DDS3_phase;



// Function Prototypes
void DDS_set_DMA(char channel);
void DDS_set_SRU(char channel);
void DDS_start_SPORT(void);
void DDS_update_frequency(void);
void DDS_reset(void);
void DDS_init(void);
void DDS_current_scale(char scale);
void InitDDS_IO(void);

void IRQ_DDS_SP1(int sig_int);

unsigned char DDS_WriteData(int frequency, char phase, char powerdown, char channel);
//unsigned char DDS_WriteByte(char byte, char channel);


#endif

