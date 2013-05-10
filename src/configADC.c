/***************************************************************
	Filename:	configADC.c (ADC and Amplification chain Functions)
	Author:		Diogo Aguiam - diogo.aguiam@ist.utl.pt
	Date:		May 2013
	Version:	v1.0
	
	Dependecies:	configADC.h
					
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


#include "../h/configADC.h"

/**************************************************************
			EXTERNAL ADC GLOBAL VARIABLES
***************************************************************/



/**************************************************************
			LOCAL ADC GLOBAL VARIABLES
***************************************************************/

unsigned int SAMPLES_MEMORY[MAXSAMPLES];





/************************************************************
	Function:		InitGAIN_IO (void)
	Argument:	
	Description:	Initializes Gain DAC IO pins
	Action:		All	pins are set as Outputs and default HIGH.
				Connect the SPORT2 to communicate with the 
				GAIN DAC.
				
					GAIN_CS -> Frame Sync -> DAI_13
					GAIN_DATA -> SPORT DA -> DAI_16
					GAIN_CLK -> SPORT CLK	-> DAI_14
************************************************************/
void InitGAIN_IO(void){	
    // SPORTx does not provide a gated clock. An internal
    // buffer must be used to supply the GAIN_CLK output.
    // The enable of this buffer corresponds to the 
    // SPORTx frame sync output. This frame sync is
    // configured to be high only during transmission.
    
    // SPORT4 frame sync used to enable the clock output
    // and as chip select
    // Must invert Chip select through MISCA5
    SRU(SPORT2_FS_O, MISCA4_I);
    SRU(SPORT2_FS_O, MISCA3_I);
    //SRU(HIGH, INV_MISCA4_I);
    SRU(LOW, DAI_PB13_I);
    
    // MISC buffer 3 implements a gated clock that depends on frame sync.
    // The clock output enable is supplied by this buffer.
    SRU(MISCA3_O, PBEN13_I);
    SRU(MISCA3_O, PBEN14_I);
	    
    // SPORT4 Data channel A
    SRU(SPORT2_DA_O, DAI_PB16_I);
    
    //SRU(SPORT2_FS_O, DAI_PB13_I);
    SRU(SPORT2_CLK_O, DAI_PB14_I);


//Enabling pins as Outputs. High -> Output, Low -> Input
	// GAIN DAC Chip Select SPORT2_DA_O SPORT2_FS_O
	//SRU(HIGH,PBEN13_I);	
	// GAIN DAC Data
	SRU(HIGH,PBEN16_I);
	// GAIN DAC Clock
	//SRU(HIGH,PBEN14_I);	
	
}





/**********************************************************
	Function:		GAIN_set_Voltage(void)
	Argument:	gain_value - Voltage Value to bet set
				power_down_mode - Which power down mode
	Description:	Masks configuration word and
			starts SPORT2 transfer to GAIN DAC.
	Action:		
			Configures the DAC configuration word:
			[ 2bit power down | 12 bit value | 2 bit X]
			
			SPORT modes: Standard Serial
			Data Transfer type: Standard
			
	Assumptions: Value to be transfered is the gain_value,
		
		
************************************************************/
void GAIN_set_voltage(int gain_value, char power_down_mode)
{
	int GAIN_config_word=0;
	// Masking of the configuration word
	
	GAIN_config_word = (power_down_mode & 0x03)<<14 | (gain_value & 0x0FFF)<<2;
	
	// Waits for free buffer #! Should have another implementation
    while (*pSPCTL2 & DXS1_A);
	// Sets SPORT transmit buffer
	*pTXSP2A = GAIN_config_word; //<- data trasnmist buffer
    
    
}



/**********************************************************
	Function:		GAIN_init()
	Argument:	
	Description:	Configures SPORT2 to be used for DAC
	Action:	Configures the SPORT2 as standard serial and 
		configures the DAC with the default GAIN word.
			
			SPORT modes: Standard Serial
			Data Transfer type: Standard
			
		
		
************************************************************/
void GAIN_init(void)
{	
    //Clear SPORT4 configuration register
    *pSPCTL2 = 0 ;

	    // Clock and frame sync divisor. According to DDS timings.
    *pDIV2 = GAIN_SPORT_CLK_DIV;
    // Configure and enable SPORT 4.
    // #! this config could be set during initialization and new words are added
    // to the transmit buffer
    *pSPCTL2 = (SPTRAN | FSR | LAFS |  IFS  | ICLK  | SLEN16 | SPEN_A );
    
    // Transmit mode
    // Frame Sync Required, Late FS and Internal FS
    // Internal Clock
    // Falling Edge sampling 
    // 16 bit configuration
    // SPORT enable and 

    	// Waits for free buffer #! Should have another implementation
    while (*pSPCTL2 & DXS1_A);

	*pTXSP2A = (GAIN_PD_ON & 0x03)<<14 | (GAIN_default & 0x0FFF)<<2;; //<- data trasnmist buffer
    
    
}




/************************************************************
	Function:		InitADC_IO (void)
	Argument:	
	Description:	Initializes ADC IO pins
	Action:		
				Connect the SPORT3 to communicate with the ADCs.
				
				ADC_CNV		-		DAI_PB17	Output
				ADC_TRIG	-		DAI_PB19	Input
			NC	ADC_DATA	-		DAI_PB18	Input
				ADC_CLK		-		DAI_PB20	Output
************************************************************/
void InitADC_IO(void){	

	
	// ADC_CNV is configured as an output by PWM?
    SRU(HIGH, DAI_PB17_I);
    // Data received from ADC_DATA goes to SPORT3 DA_I
    //SRU(DAI_PB18_O, SPORT4_DA_I);
    // When using SPORTx as a Receive Master, its internal
    // clock must be fed into its input.
    SRU(SPORT4_CLK_O, SPORT4_CLK_I);
    SRU(SPORT4_FS_O, SPORT4_FS_I);
    SRU(SPORT4_CLK_O, DAI_PB20_I);
	// The ADC_TRIG serves as External Frame Sync for the SPORT
    SRU(DAI_PB19_O, SPORT4_FS_I);
    SRU(DAI_PB19_O, SPORT4_DA_I);

//Enabling pins as Outputs. High -> Output, Low -> Input
	SRU(LOW, PBEN19_I);	// TRIG
    SRU(LOW, DAI_PB19_I); // just in case, tie it to low

	SRU(LOW, PBEN18_I);	// DATA
	SRU(HIGH, DAI_PB18_I); // just in case, tie it to low
	
	SRU(HIGH,PBEN17_I);	// CNV
	SRU(HIGH,PBEN20_I);	// CLK
}


/**********************************************************
	Function:		ADC_init()
	Argument:	
	Description:	Configures SPORT3 to be used for ADC
	Action:	Configures the SPORT3 as standard serial and
		Receive Master with DMA.			
		
************************************************************/
void ADC_init(void)
{	
    //Clear SPORT3 configuration register
    *pSPCTL4 = 0 ;

    // Configuration the DMA
    *pIISP3A =  (unsigned int)	SAMPLES_MEMORY;	// Internal DMA memory address
    *pIMSP3A = sizeof(SAMPLES_MEMORY[0]);		// Address modifier
    *pCSP3A  = MAXSAMPLES; 				// word count 5 bytes 
    
    
	    // Clock and frame sync divisor. According to DDS timings.
    *pDIV4 = ADC_SPORT_CLK_DIV;
    // Configure and enable SPORT 3.
    // #! this config could be set during initialization and new words are added
    // to the transmit buffer
    
    //####*pSPCTL4 = (FSR | ICLK | CKRE | SLEN32 | SPEN_A );
    
    
    
    // Receive Master mode
    // Frame Sync Required, Early FS and External FS
    // Internal Clock
    // Rising Edge sampling 
    // 2*16 = 32 bit configuration
    // SPORT enable  

    	// Waits for free buffer #! Should have another implementation
    //while (*pSPCTL3 & DXS1_A);

	//*pTXSP2A = (GAIN_PD_ON & 0x03)<<14 | (GAIN_default & 0x0FFF)<<2;; //<- data trasnmist buffer
    
    
}


/************************************************************
	Function:		IRQ_ADC_SP3(int sig_int)
	Argument:		sig_int
	Description:	End of SPORT DMA transfer interruption
	Action:		
			
************************************************************/
void IRQ_ADC_SP4(int sig_int)
{
	unsigned int k,i;
	k= *pRXSP4A;
//	for(i=0; i<10;i++);
	adc_sample_irq =1;
	*pSPCTL4 = 0;

	//i= *pRXSP4A;
//	printf("Int 1 : %d \n", i);
	//if(k!=0xffffffff){
//	for(i=0; i<10;i++);

	//*pSPCTL4 = 0;
	//*pSPCTL4 = (FSR | ICLK | CKRE | SLEN32 | SPEN_A );

	//*pSPCTL4 = (FSR | ICLK | CKRE | SLEN32 | 0 );
//	for(i=0; i<10;i++);
	//}
	*pSPCTL4 = 0;
//	*pSPCTL4 = (FSR | ICLK | CKRE | SLEN32 | 0 );
	printf("passou pela int SP4. ADC1 %d , ADC2 %d\n", k>>16, k&0xffff);

}


