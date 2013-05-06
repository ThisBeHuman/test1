#include <sru.h>
#include <def21369.h>

// This function will setup the SRU Registers
void InitSRU(void)
{
	// Enable pull-up resistors on unused DAI pins
	* (volatile int *)DAI_PIN_PULLUP = 0xfffff;

	// Enable pull-up resistors on unused DPI pins
	* (volatile int *)DPI_PIN_PULLUP = 0x3fff;

	//Generating Code for connecting : SPORT2_FS to MISCA4
	SRU (SPORT2_FS_O, MISCA4_I); 

	//Ext MiscA4 is to be inverted 
	*(volatile int *)SRU_EXT_MISCA |= 0x40000000 ;


}
