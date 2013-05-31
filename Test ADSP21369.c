

/*****************************************************************************
 * Test ADSP21369.c
 *****************************************************************************/
#define LED6 DAI_PB16_I
#include ".\h\general.h" 

/////////////////////////////////////////////////
// 			GLOBALS
/////////////////////////////////////////////////

int gStatus,gStatus2;
int gFreq;
int gChangeFreq;
char gPhase;

unsigned char gDDS_word[5];



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


void dai_Interrupt(int sig_int);


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
	//DDS_init();
	// Double Reset and INIT - Makes no sense but works...
	//DDS_init();
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
	if(gStatus2 %2== 1){
	//	gStatus = 0;
	//	LED5_off;
	}else{//
	//	gStatus = 1;
	//	LED5_on;
	}

	gStatus2++;
	gChangeFreq = 1;
	switch(gStatus2%4){
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
	//*pTXSP3A = 0x80000001;

/*	    // Configure the DMA
	*pSPCTL1 = 0;
	while(*pCSP1A!=0);
    *pIISP1A =  (unsigned int)	gDDS_word;  // Internal DMA memory address
   // *pIMSP1A = sizeof(gDDS_word[0]);			// Address modifier
    *pCSP1A  = 5; 			// word count 4 bytes 
//    *pCPSP3B = 0;
    *pSPCTL1 = (SPTRAN | FSR | LAFS | IFS | LSBF | ICLK | CKRE | SLEN8 | SPEN_A | SDEN_A);
*/


}

//////////////////////////////////////////////////////////////////////////////
// void SP0_Interrupt(int sig_int)
//
// PURPOSE:		ISR For sp0 done interrupt!
//
//////////////////////////////////////////////////////////////////////////////
void SP1_Interrupt(int sig_int)
{
	int i;
	LED6_on;
	//for(i=0;i<10000000;i++);
	LED6_off;

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


    (*pDAI_IRPTL_PRI) = (SRU_EXTMISCA1_INT  | SRU_EXTMISCA2_INT | SRU_EXTMISCB0_INT);    //unmask individual interrupts
    (*pDAI_IRPTL_RE) = (SRU_EXTMISCA1_INT  | SRU_EXTMISCA2_INT | SRU_EXTMISCB0_INT);    //make sure interrupts latch on the rising edge

	//Set up interrupt priorities
    sysreg_bit_set(sysreg_IMASK, DAIHI); //make DAI interrupts high priority

    (*pSYSCTL) |= (IRQ0EN|IRQ1EN);

    sysreg_bit_set(sysreg_MODE2, (IRQ0E|IRQ1E) );
    sysreg_bit_clr(sysreg_IRPTL, (IRQ1I|IRQ0I) );
    sysreg_bit_set(sysreg_IMASK, (IRQ1I|IRQ0I) ); //enable IRQ interrupts
    sysreg_bit_set(sysreg_MODE1, IRPTEN ); 		  //enable global interrupts

  //  interrupt(SIG_DAIH,DAIroutine);
    interrupt (SIG_SP1,IRQ_DDS_SP1);

    interrupt(SIG_IRQ0,IRQ0_routine);
    interrupt(SIG_IRQ1,IRQ1_routine);
}







void IRQ_timer(int sigint)
{
	int i;
	*pTMSTAT &= TIM0IRQ;
	SRU(LOW, DAI_PB17_I);	
	//for(i=0;i<10;i++);
	SRU(HIGH, DAI_PB17_I);
//	IRQ_ADC_SampleReady(0);

}

void initTimer0 (void)
{
// timer7 is the 1mSec tick timer
  
	SRU (HIGH, DPI_PBEN06_I);
	SRU (TIMER0_O, DPI_PB06_I);
	SRU(TIMER0_O, TIMER0_I);

    *pTM0CTL = (TIMODEPWM | PULSE | PRDCNT | IRQEN);
    *pTM0PRD = CNV_uSEC * TICKS_PER_uSEC;
    *pTM0W = (CNV_uSEC * TICKS_PER_uSEC-3); // 10% pulse
	*pTM0STAT = TIM0EN;

	
    *pDAI_IRPTL_PRI |= SRU_EXTMISCB0_INT;
    *pDAI_IRPTL_RE |= SRU_EXTMISCB0_INT;
   	interrupts(SIG_P0,IRQ_ADC_SampleReady);
    interruptf(SIG_SP3,IRQ_ADC_SampleDone);
	interrupts(SIG_GPTMR0, IRQ_timer);
}



void main( void )
{
	/* Begin adding your custom code here */
	int i,k = 0,nr=0;
	int j=0;
	
	int temp;
	
	char *aux_ptr;
	int usbdata,usbdata2,usbdata3,usbdata4;
	unsigned char w4 = 0x8E;// 0x8E;
	unsigned char w3 = 0xE3;
	unsigned char w2 = 0x38;//0x38;
	unsigned char w1 = 0x0E>>0;//0x0E;
	unsigned char w0 = 0x09; // phase, power down, REF Multiplier
	
	unsigned short packetSize;
	
	char memaux[] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09};
//
/*	union dds_freq freq;
	freq.freq_byte[0]=0x0E;
	freq.freq_byte[1]=0x38;
	freq.freq_byte[2]=0xE3;
	freq.freq_byte[3]=0x8E;
*/	
	gStatus = 0;
	gChangeFreq =0;
	gPhase = 0;
	adc_end_of_sampling = 0;
	adc_continuous_sampling = 1;
	
		// Enable pull-up resistors on unused DAI pins
	* (volatile int *)DAI_PIN_PULLUP = 0x9ffff;//0x9ffff;

	// Enable pull-up resistors on unused DPI pins
	* (volatile int *)DPI_PIN_PULLUP = 0x3fff;
	
	//initPLL_SDRAM();
	//InitPLL_SDRAM();
	
	
	Setup_leds();
	
	//SRU(HIGH,PIN_SCALE_b0);	
	//SRU(HIGH,PIN_SCALE_b1);	
	//InitSRU();

	//InitSPORT();
	
	
	//DDS_init_io();

	//DDS_current_scale(DDS_CURRENT_100);
	
	InitDDS_IO();
	InitGAIN_IO();
	InitADC_IO();
	InitUSB_IO();
	
	
	Setup_Ints();
	
	DDS_init();
	// Double Reset and INIT - Makes no sense but works...
	DDS_init();
	GAIN_init();
	//ADC_init();
	USB_init();
	//Setup_AMI();
	
	
	// Enable DAI interrupt on falling edge of PCG_FS
    //(*pDAI_IRPTL_PRI) = (SRU_EXTMISCA1_INT  | SRU_EXTMISCA2_INT | SRU_EXTMISCB0_INT);    //unmask individual interrupts
    //(*pDAI_IRPTL_RE) = (SRU_EXTMISCA1_INT  | SRU_EXTMISCA2_INT | SRU_EXTMISCB0_INT);    //make sure interrupts latch on the rising edge
	
//	*pDAI_IRPTL_PRI = SRU_EXTMISCB0_INT ;
//	*pDAI_IRPTL_RE = SRU_EXTMISCB0_INT ;
//	*pDAI_IRPTL_FE = 0; 
//	interrupt(SIG_P0,dai_Interrupt);
//	SRU (DAI_PB19_O, DAI_INT_22_I); 

//	initTimer0();	
	while(1){
		if(gChangeFreq ==1){
	/*     	*/ 
			DDS1_frequency = gFreq;// 0x80FFFF01;//DDS_10kHz;
			DDS1_phase = DDS_PHASE_0;
//			DDS_set_DMA(DDS_ch1);
//			DDS_set_SRU(DDS_ch1);
//			DDS_start_SPORT();
			
			DDS_WriteData(DDS_100kHz, DDS_PHASE_0, 0, DDS_ch1);
	
			DDS2_frequency = gFreq;//DDS_100kHz;
			DDS2_phase = DDS_PHASE_0;
//			DDS_set_DMA(DDS_ch2);
//			DDS_set_SRU(DDS_ch2);
//			DDS_start_SPORT();

			DDS_WriteData(DDS_99kHz, DDS_PHASE_0, 0, DDS_ch2);

			DDS3_frequency = gFreq;
			DDS3_phase = DDS_PHASE_0;
//			DDS_set_DMA(DDS_ch3);
//			DDS_set_SRU(DDS_ch3);
//			DDS_start_SPORT();
		
			DDS_WriteData(DDS_99kHz, DDS_PHASE_90, 0, DDS_ch3);

		    //SRU(HIGH, DAI_PB13_I);

			DDS_update_frequency();	
			//SRU(LOW, DAI_PB13_I);
		
			//GAIN_32dB   GAIN_PD_ON
			GAIN_set_voltage(GAIN_10VV,GAIN_PD_ON);
			//ADC_init();
		//	SRU(LOW, DAI_PB17_I);
		//	*pSPCTL4 = 0;
		    //*pSPCTL4 = (FSR | ICLK | CKRE | SLEN32 | SPEN_A );


		// ASSERTS CLOCK HIGH UNTIL BUSY SIGNAL - then starts SPORT
   		//SRU(HIGH, DAI_PB20_I); ////#!

		//	for(i=0;i<10;i++);
		//	SRU(HIGH, DAI_PB17_I);
    		//(*pDAI_IRPTL_RE) = (SRU_EXTMISCA1_INT  | SRU_EXTMISCA2_INT | SRU_EXTMISCB0_INT);    //make sure interrupts latch on the rising edge
		//	ADC_StartSampling(200);
			
    		//ADC_StopSampling();

		//	for(i=0;i<10;i++);
			//while(!((*pDAI_PIN_STAT)& DAI_PB19));
//    		SRU(SPORT4_CLK_O, DAI_PB20_I); //#!
//		    *pSPCTL4 = (FSR | ICLK | CKRE | SLEN32 | SPEN_A | 0 );
			//*pTXSP3A=0xaaaaaaaa;
			
			gChangeFreq = 0;
//			gPhase++;
//			gPhase = gPhase&0x1f;
		}
		
/* USB	*/	
//			SRU(HIGH, PBEN14_I);
//			SRU(HIGH, PBEN13_I);
		
		
//			SRU(HIGH, DAI_PB14_I);
//			SRU(LOW, DAI_PB14_I);
//			SRU(HIGH, DAI_PB13_I);
//			SRU(LOW, DAI_PB13_I);
		//

					for(i=0;i<1000;i++);	
//			if((usb_access(0, STATUS) & SPACE_AVAI)){
//				usb_access(1, (k++)&0xff);
				//printf("escrito\n");
//			}USB_access(USB_DATA_PIPE, USB_WRITE, usbdata);

			
		// USB Polling
			usbdata = USB_access(USB_STATUS, USB_READ, USB_NULL);
			//usbdata = usb_access(0, STATUS);
			usbdata = usbdata& DATA_AVAI;
//				usb_access(1, j++);
			if(usbdata ){
				usbdata = USB_access(USB_DATA_PIPE, USB_READ, USB_NULL);
			//	usbdata = usb_access(0, DATA);
//				usbdata2 = usb_access(0, DATA);
//				usbdata3 = usb_access(0, DATA);
//				usbdata4 = usb_access(0, DATA);
				if(usbdata == 0x17){
			//		ADC_StartSampling(1024,CNV_uSEC, adc_continuous_sampling);
				}
				if(USB_isPacketStart(usbdata)){
			//		ADC_StartSampling(1024, CNV_uSEC,adc_continuous_sampling);
					packetSize = USB_readPacketSize();
	//#!				printf("size: %d\n",packetSize);
					if(packetSize > 0){
						if(USB_readPayload(packetSize, &USB_PAYLOAD_BUFFER) == USB_ERROR_FLAG){
							printf("USB Error: Error reading payload.\n");
						}else{
							for(i=0; i<packetSize;i++){
							//	printf("%x ", USB_PAYLOAD_BUFFER[i]);
							}
							temp = USB_processPayload(packetSize, &USB_PAYLOAD_BUFFER);
							if( temp == USB_ERROR_FLAG){
								printf("USB Error: Error processing payload.\n");
							}else if(temp == USB_WRONG_CMD_SIZE){
								printf("USB Error: Wrong Command Size\n",j++);
							}
						}	
					}
						
				}
				//while(!(usb_access(0, STATUS) & SPACE_AVAI));
				//USB_access(USB_DATA_PIPE, USB_WRITE, usbdata);
			//	usb_access(1, usbdata);
			//	printf("usb%d: %x \n",j++,usbdata);
			}

		if(adc_end_of_sampling){
		//	aux_ptr = (char*)SAMPLES_MEMORY;
			k = adc_number_of_samples*4;
			j=0;
			for (i = 0; i< adc_number_of_samples ; i++){
				for(k = 0; k < 4; k++){
//				printf("sample: %d %x\n",i,SAMPLES_MEMORY.Char[i]);
					while(!(USB_access(USB_STATUS, USB_READ, USB_NULL)&SPACE_AVAI));
			//		printf("usb samples %d: int: %x - %x\n",j++,SAMPLES_MEMORY[i], (SAMPLES_MEMORY[i]>>(k*8))&0xff);
				//	printf("usb samples %d: int: %x - %x\n",j++,SAMPLES_MEMORY.Int[i/4], SAMPLES_MEMORY.Char[i]);
					USB_access(USB_DATA_PIPE, USB_WRITE, (SAMPLES_MEMORY[i]>>(k*8))&0xff);
				}
			}	
			
		//	USB_write_memory(memaux, 10);
			adc_end_of_sampling = 0;
		}	
		if(adc_send_continuous_samples){

			USB_sendADCData(adc_number_of_samples_to_send,adc_buffer_to_send);

			adc_send_continuous_samples = 0;	
		}
						
//	 A0_HIGH();

//	A0_LOW();
/*		if(adc_sample_irq!=0){

			i= *pRXSP4A;
			if(i!=0x0a){
				for(i=0; i<10;i++);

				//*pSPCTL4 = 0;
				//*pSPCTL4 = (FSR | ICLK | CKRE | SLEN32 | SPEN_A );

				//*pSPCTL4 = (FSR | 0 | CKRE | SLEN32 | 0 );
				for(i=0; i<10;i++);
			}
			if ((*pSPCTL3 & DXS1_A)){
				adc_sample_irq= 0;
			
			//	IRQ_ADC_SampleDone(0);	
			}
			for(i=0;i<10000;i++);	
		}
*/			

		
	//			DDS_update_frequency();
		//ADC_init();
	//	k= *pRXSP3A;
	//		SRU(LOW, DAI_PB18_I);
	//		for(i=0;i<30;i++);
	//		SRU(HIGH, DAI_PB18_I);
	//		for(i=0;i<30;i++);

		//for(i=0;i<1000000;i++);
		//LED6_off;
		//for(i=0;i<10000000;i++);
		//LED6_on;
		
	//while ((*pSPCTL3 & DXS_A)!=0);  //wait DAC:
     	
    	//*pTXSP1B = 0;
		//*pTXSP1A=0xaa;
		
		//gDDS_word[0]=nr++;
		//InitSRU();

	}
//	return 0;
}
