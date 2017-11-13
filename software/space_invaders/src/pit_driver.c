// pit_driver.c
#include "pit_driver.h"

// Initializes the Pit
// Start off with the delay at 1,000,000
// Have the loop, interrupt, and decrement bits set
void pit_Init(){
	Pit_mSetPitDelayRegisterData(XPAR_PIT_0_BASEADDR, DELAY_INIT_VALUE);
	Pit_mSetPitControlRegisterData(XPAR_PIT_0_BASEADDR, CONTROL_INIT_VALUE);
}

// Function to change the delay of the driver
// 1,000,000 the original standard that the game is built to
void pit_setDelay(Xuint32 value){
	Pit_mSetPitDelayRegisterData(XPAR_PIT_0_BASEADDR, value);
}

// Gets the current delay that is in the delay register
Xuint32 pit_getDelayReg(){
	return Pit_mGetPitDelayData(XPAR_PIT_0_BASEADDR);
}

// Gets the current value in the control register
Xuint32 pit_getControlReg(){
	return Pit_mGetPitControlData(XPAR_PIT_0_BASEADDR);
}
