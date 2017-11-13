// pit_driver.c
#include "pit_driver.h"

void pit_Init(){
	Pit_mSetPitControlRegisterData(XPAR_PIT_0_BASEADDR, CONTROL_INIT_VALUE);
	Pit_mSetPitDelayRegisterData(XPAR_PIT_0_BASEADDR, DELAY_INIT_VALUE);
	//pit_WriteReg(XPAR_PIT_0_BASEADDR, PIT_CONTROL_REG, CONTROL_INIT_VALUE);
}
/*
void pit_WriteReg(Xuint32 baseaddr, Xuint32 reg_addr, Xuint32 value) {
    PIT_mSetPITRegisterData(baseaddr, value);
    PIT_mSetPITRegisterAccessCommand(baseaddr, reg_addr);
    //while (!PIT_isRegisterAccessFinished(baseaddr));
}
*/
/*
Xuint32 pit_ReadReg(Xuint32 baseaddr, Xuint32 reg_addr) {
    XAC97_mSetAC97RegisterAccessCommand(baseaddr, reg_addr  | 0x80);
    while (!XAC97_isRegisterAccessFinished(baseaddr));
    return XAC97_mGetAC97RegisterData(baseaddr);
}
*/

void pit_setDelay(Xuint32 value){
	Pit_mSetPitDelayRegisterData(XPAR_PIT_0_BASEADDR, value);
}
