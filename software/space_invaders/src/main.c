/******************************************************************************
 * main.c
 * ==========================================================================
 * COURSE INFO
 * ECEn 427: Embedded Programming
 * Prof. Brad Hutchings
 *
 * AUTHORS
 * Dr. Hutchings (Original Code)
 * Davis Crabb
 * Peter Loderup
 *
 * UPDATE LOG
 * 28 Sep 2017 | Started file. Basic modifications made to allow for user
 * 				 input with 'getchar()'.
 * 06 Oct 2017 | Removed all 'printf's for Lab 3 passoff.
 *****************************************************************************/
// Includes
#include <stdio.h>
#include "platform.h"
#include "render.h"
#include "alien.h"
#include "bullet.h"
#include "tank.h"
#include "globals.h"
#include "sound.h"
#include "xgpio.h"          // Provides access to PB GPIO driver.
#include "mb_interface.h"   // provides the microblaze interrupt enables, etc.
#include "xintc_l.h"        // Provides handy macros for the interrupt controller.
#include "pit_driver.h"

XGpio gpLED;  // This is a handle for the LED GPIO block.
XGpio gpPB;   // This is a handle for the push-button GPIO block.

// This is going to call all of our tick functions
// It gets called on every timer interrupt (10ms)
void timer_interrupt_handler() {
	alien_tickAliens();   // Enter the alien state machine
	tank_FSM();           // Enter the tank state machine
	bullet_FSM();         // Enter the bullet state machine
	saucer_tickSaucer();  // Enter the saucer state machine
}

// This is invoked each time there is a change in the controller state
// (result of a push of the button or move of the joystick)
void controller_interrupt_handler() {

}

// This is invoked each time there is a change in the button state (result of a push or a bounce).
void pb_interrupt_handler() {
  // Clear the GPIO interrupt.
  XGpio_InterruptGlobalDisable(&gpPB);      // Turn off all PB interrupts for now.
  curr_btn = XGpio_DiscreteRead(&gpPB, 1);  // Get the current state of the buttons.
  XGpio_InterruptClear(&gpPB, 0xFFFFFFFF);  // Ack the PB interrupt.
  XGpio_InterruptGlobalEnable(&gpPB);       // Re-enable PB interrupts.

  if(curr_btn & UP_BTN_MASK){
	  sounds_increaseVolume();
  }
  if(curr_btn & DOWN_BTN_MASK){
	  sounds_decreaseVolume();
  }
}

void sound_interrupt_handler() {
  // pump sound values into AC97
  sound_addSoundValues();
}

// Main interrupt handler, queries the interrupt controller to see what peripheral
// fired the interrupt and then dispatches the corresponding interrupt handler.
// This routine acks the interrupt at the controller level but the peripheral
// interrupt must be ack'd by the dispatched interrupt handler.
// Question: Why is the timer_interrupt_handler() called after ack'ing the interrupt controller
// but pb_interrupt_handler() is called before ack'ing the interrupt controller?
void interrupt_handler_dispatcher(void* ptr) {
	int intc_status = XIntc_GetIntrStatus(XPAR_INTC_0_BASEADDR);
	// Check the PIT interrupt first.
	if (intc_status & /*XPAR_FIT_TIMER_0_INTERRUPT_MASK*/XPAR_PIT_0_INTERRUPT_MASK){
		//xil_printf("Got an interrupt from the pit\r\n");
		XIntc_AckIntr(XPAR_INTC_0_BASEADDR, /*XPAR_FIT_TIMER_0_INTERRUPT_MASK*/XPAR_PIT_0_INTERRUPT_MASK);
		timer_interrupt_handler();
	}
	// Check the push buttons.
	if (intc_status & XPAR_PUSH_BUTTONS_5BITS_IP2INTC_IRPT_MASK){
		pb_interrupt_handler();
		XIntc_AckIntr(XPAR_INTC_0_BASEADDR, XPAR_PUSH_BUTTONS_5BITS_IP2INTC_IRPT_MASK);
	}

	// Check the controller joystick and push button

	// check the sound
	if(intc_status & XPAR_AXI_AC97_0_INTERRUPT_MASK) {
		XIntc_AckIntr(XPAR_INTC_0_BASEADDR, XPAR_AXI_AC97_0_INTERRUPT_MASK);
		sound_interrupt_handler();
	}
}

int main (void) {
    init_platform();
    render_initScreen(); // Draw all initial items onto the screen
    sound_initSound(); // Initial the AC-97
    xil_printf("Initialize the pit\r\n");
    pit_Init(); // Initialize the pit
    xil_printf("Delay Reg: %d\r\n", pit_getDelayReg());
    xil_printf("Control Reg: %d\r\n", pit_getControlReg());
    // Initialize the GPIO peripherals.
    int success;
    success = XGpio_Initialize(&gpPB, XPAR_PUSH_BUTTONS_5BITS_DEVICE_ID);
    // Set the push button peripheral to be inputs.
    XGpio_SetDataDirection(&gpPB, 1, 0x0000001F);
    // Enable the global GPIO interrupt for push buttons.
    XGpio_InterruptGlobalEnable(&gpPB);
    // Enable all interrupts in the push button peripheral.
    XGpio_InterruptEnable(&gpPB, 0xFFFFFFFF);
    // Enable interrupts in the AC97
    XAC97_EnableInput(XPAR_AXI_AC97_0_BASEADDR, XPAR_AXI_AC97_0_INTERRUPT_MASK);

    microblaze_register_handler(interrupt_handler_dispatcher, NULL);
    XIntc_EnableIntr(XPAR_INTC_0_BASEADDR,
    		(/*XPAR_FIT_TIMER_0_INTERRUPT_MASK*/XPAR_PIT_0_INTERRUPT_MASK | XPAR_PUSH_BUTTONS_5BITS_IP2INTC_IRPT_MASK | XPAR_AXI_AC97_0_INTERRUPT_MASK));
    XIntc_MasterEnable(XPAR_INTC_0_BASEADDR);
    microblaze_enable_interrupts();
    globals_setGameOver(false);         // Set the game over to be false
    uint32_t counter = 0;
    while(!globals_getIsGameOver()){ // play until game over
    	uint32_t tempInt = 0;
    	//xil_printf("Going to get char\r\n");
    	char tempChar = getchar();
    	while(tempChar != '\r'){
    		tempInt *= 10;
    		tempInt += atoi(&tempChar);
    		tempChar = getchar();
    	}
    	xil_printf("New value: %d\r\n", tempInt);
    	pit_setDelay(tempInt);
    }
    xil_printf("Counter: %d", counter);
    render_drawGameOver();
    cleanup_platform();

    return 0;
}
