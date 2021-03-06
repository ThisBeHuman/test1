/***************************************************************
	Filename:	configADC.h (ADC and Amplification chain Functions)
	Author:		Diogo Aguiam - diogo.aguiam@ist.utl.pt
	Date:		May 2013
	Version:	v1.0
	Purpose:	Set the Gain of the amplification chain.
				Initiate analog to digital conversion and read
			the data.
			
	Usage:	Both ADCs (AD7685) are configurated in a Chained Mode 
		with Busy Indicator (Daisy Chain with interrupt p.23 Rev.C
		of datasheet).
			Amplification chain consists of two 20 dB pre-amplifiers
		in series (AD8421), followed by a -8 dB to +32 dB analog
		variable gain amplifier (VCA821). The analog gain voltage
		of this amplifier is set with the AD5621 DAC, programmed
		through an SPI interface.
				
		Board DAI SRU assignements:
		
			GAIN_!CS	-		DAI_PB13
			GAIN_DATA	-		DAI_PB14
			GAIN_CLK	-		DAI_PB16
			
			ADC_CNV		-		DAI_PB17
			ADC_TRIG	-		DAI_PB19
			ADC_DATA	-		DAI_PB18
			ADC_CLK		-		DAI_PB20
	
	
	Extra:		
		GAIN DAC	-	SPORT4A	 	-	Gain setting voltage
		



***************************************************************/

#ifndef _CONFIGADC_H
#define _CONFIGADC_H


#include "..\h\general.h"



/* Defines for ADC IO

	Gain DAC !Sync (chip select) 	-	DAI 13
	Gain DAC Data					-	DAI 14
	Gain DAC Clock					-	DAI 16
	
	ADC Convert						-	DAI 17
	ADC Trigger						-	DAI 19
	ADC Data						-	DAI 18
	ADC Clock						-	DAI 20

*/
#define GAIN_CS_H		SRU(HIGH,DAI_PB13_I)
#define GAIN_CS_L		SRU(LOW,DAI_PB13_I)
#define GAIN_DATA_H		SRU(HIGH,DAI_PB14_I)
#define GAIN_DATA_L		SRU(LOW,DAI_PB14_I)
#define GAIN_CLK_H		SRU(HIGH,DAI_PB16_I)
#define GAIN_CLK_L		SRU(LOW,DAI_PB16_I)

#define ADC_CNV_H		SRU(HIGH,DAI_PB17_I)
#define ADC_CNV_L		SRU(LOW,DAI_PB17_I)
#define ADC_TRIG_H		SRU(HIGH,DAI_PB19_I)
#define ADC_TRIG_L		SRU(LOW,DAI_PB19_I)
#define ADC_DATA_H		SRU(HIGH,DAI_PB18_I)
#define ADC_DATA_L		SRU(LOW,DAI_PB18_I)
#define ADC_CLK_H		SRU(HIGH,DAI_PB20_I)
#define ADC_CLK_L		SRU(LOW,DAI_PB20_I)



// DAC values to set a specific GAIN

#define GAIN_32dB		0xA6

#define GAIN_40VV		2482
#define GAIN_33VV		1250
#define GAIN_20VV		963	
#define GAIN_10VV		869	
#define GAIN_1VV		700
#define GAIN_default	496
#define GAIN_infdB		0x0

#define GAIN_PD_ON		0
#define GAIN_PD_1kGND	1
#define GAIN_PD_100kGND	2
#define GAIN_PD_HiZ		3

#define GAIN_SPORT_CLK_DIV	0x00000080


// ADC defines
#define MAXSAMPLES		1024
#define ADC_SPORT_CLK_DIV	0x00000002

#define TICKS_PER_uSEC	99
#define CNV_uSEC		10



//ADC Configuration Defines
#define ADC_NULL 				0 
#define ADC_CONTINUOUS_SAMPLING	1


// ADC Samples Memory Buffer
//extern unsigned int SAMPLES_MEMORY[MAXSAMPLES];
extern unsigned int * SAMPLES_MEMORY;




// ADC Global Variables
unsigned char adc_end_of_sampling;	// Signals end of Acquisition
extern unsigned int adc_number_of_samples;	// Total number of samples in acquisition

unsigned char adc_continuous_sampling;	// if 1 - restarts sampling after stopsampling
unsigned int * adc_buffer_to_send;
unsigned int adc_number_of_samples_to_send;
unsigned char adc_send_continuous_samples; 	// Flag to send continuously acquired data

// ADC Swap Buffer Signals
unsigned char adc_sample_buffer_full;
unsigned int * adc_sample_buffer_full_ptr;
unsigned int adc_sample_buffer_full_number_of_samples;



// Function prototypes

void InitGAIN_IO(void);
void GAIN_set_voltage(int gain_value, char power_down_mode);
void GAIN_init(void);

void IRQ_ADC_SampleReady(int sig_int);
void IRQ_ADC_SampleDone(int sig_int);
void IRQ_ADC_AssertConversion(int sigint);

void ADC_StopSampling();


char adc_sample_irq;

#endif
