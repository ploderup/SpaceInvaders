/**
 * PIT DRIVER
 */
#ifndef PIT_H
#define PIT_H

// Read / Write Macros
#include <xbasic_types.h>
#include <xio.h>

#define PIT_mGetStatus(BaseAddress) \
            XIo_In32((BaseAddress + AC97_STATUS_OFFSET))
#define PIT_isRegisterAccessFinished(BaseAddress) \
            ((PIT_mGetStatus(BaseAddress) & AC97_REG_ACCESS_BUSY) == 0)
#define PIT_mSetPITRegisterAccessCommand(BaseAddress, value) \
            XIo_Out32((BaseAddress) + AC97_REG_CONTROL_OFFSET,(value))
#define PIT_mSetPITRegisterData(BaseAddress, value) \
            XIo_Out32((BaseAddress) + AC97_REG_WRITE_OFFSET,(value))

// Using code below here
#define Pit_mSetPitDelayRegisterData(BaseAddress, value) \
			XIo_Out32((BaseAddress) + PIT_DELAY_REG,(value))

#define Pit_mSetPitControlRegisterData(BaseAddress, value) \
			XIo_Out32((BaseAddress) + PIT_CONTROL_REG,(value))

#define XPAR_PIT_0_INTERRUPT_MASK 0X000001
#define XPAR_AXI_INTC_0_PIT_0_INTERRUPT_INTR 0
#define XPAR_PIT_0_BASEADDR 0x7BC00000
#define XPAR_PIT_0_HIGHADDR 0x7BC0FFFF

#define CONTROL_INIT_VALUE  0x7
#define DELAY_INIT_VALUE	0x1000

// Register
#define PIT_CONTROL_REG 0x00
#define PIT_DELAY_REG 0x04

void pit_Init();
void pit_WriteReg(Xuint32 baseaddr, Xuint32 reg_addr, Xuint32 value);
Xuint32 pit_ReadReg(Xuint32 baseaddr, Xuint32 reg_addr);
void pit_setDelay();

#endif
