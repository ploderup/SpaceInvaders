/*
 * sound.c
 *
 *  Created on: Nov 1, 2017
 *      Author: superman
 */
#include "sound.h"

// The following externs are variables for the sounds
// generated from WAV files
// It consists of:
//	1) An array with the sound data
//	2) The number of samples for the sound
//	3) The rate at which the sound is sampled

// Data for the explosion sound
extern uint32_t explosion_soundData[];
extern uint32_t explosion_numberOfSamples;
extern uint32_t explosion_sampleRate;

// Data for the highest of the 4 alien move sounds
extern uint32_t fastinvader1_soundData[];
extern uint32_t fastinvader1_numberOfSamples;
extern uint32_t fastinvader1_sampleRate;

// Data for the mid-highest of the 4 alien move sounds
extern uint32_t fastinvader2_soundData[];
extern uint32_t fastinvader2_numberOfSamples;
extern uint32_t fastinvader2_sampleRate;

// Data for the mid-lowest of the 4 alien move sounds
extern uint32_t fastinvader3_soundData[];
extern uint32_t fastinvader3_numberOfSamples;
extern uint32_t fastinvader3_sampleRate;

// Data for the lowest of the 4 alien move sounds
extern uint32_t fastinvader4_soundData[];
extern uint32_t fastinvader4_numberOfSamples;
extern uint32_t fastinvader4_sampleRate;

// Data for the mid-highest of the 4 alien move sounds
extern uint32_t invaderkilled_soundData[];
extern uint32_t invaderkilled_numberOfSamples;
extern uint32_t invaderkilled_sampleRate;

// Data for the tank shooting sound
extern uint32_t shoot_soundData[];
extern uint32_t shoot_numberOfSamples;
extern uint32_t shoot_sampleRate;

// Data for the ufo explosion sound
extern uint32_t ufo_highpitch_soundData[];
extern uint32_t ufo_highpitch_numberOfSamples;
extern uint32_t ufo_highpitch_sampleRate;

// Data for the ufo moving sound
extern uint32_t ufo_lowpitch_soundData[];
extern uint32_t ufo_lowpitch_numberOfSamples;
extern uint32_t ufo_lowpitch_sampleRate;
extern uint32_t ufo_lowpitch_soundData_original[];

/**
 * BYTES LEFT TO PLAY:
 * The following counters track the number of bytes left to play (BLTP)
 * per sound.
 */
uint32_t tank_fire_sound_BLTP;
uint32_t tank_explode_sound_BLTP;
uint32_t alien_explode_sound_BLTP;
uint32_t alien_march_sound_BLTP;
uint32_t saucer_flight_sound_BLTP;
uint32_t saucer_explode_sound_BLTP;

// Used to keep track of which alien sound should be playing
uint8_t invaderSoundCounter = HIGH_PITCH;

// Used to point to one of the four different alien sound arrays
uint32_t* alienSound_data; 

Xuint32* sound_ptr; // Points to the specific sound in the sound data.

static uint32_t volume;

// Function to decrement all of the BLTP each time sound gets
// pushed into the FIFO
void decrementAllBLTP() {
	if(tank_fire_sound_BLTP) tank_fire_sound_BLTP--;
	if(tank_explode_sound_BLTP) tank_explode_sound_BLTP--;
	if(alien_explode_sound_BLTP) alien_explode_sound_BLTP--;
	if(alien_march_sound_BLTP) alien_march_sound_BLTP--;
	if(saucer_flight_sound_BLTP) saucer_flight_sound_BLTP--;
	if(saucer_explode_sound_BLTP) saucer_explode_sound_BLTP--;
}

// Function to put sound data into the FIFO
void sound_pumpIntoFifo(uint32_t* BLTP, uint32_t numSamples, uint32_t* sound_data) {
	int x;
	Xuint32 sample;

	/** Wait for the ready signal **/
	XAC97_AwaitCodecReady(XPAR_AXI_AC97_0_BASEADDR);

	// Put 100 samples from the sound data into the FIFO
	for(x = 0; x < SAMPLES_FOR_FIFO; x++){
		if(*BLTP > 0) { // Only put in sound data if there is more data to put
			sample = sound_data[numSamples - *BLTP]; // Point to the data
			XAC97_WriteFifo(XPAR_AXI_AC97_0_BASEADDR, sample); 
			decrementAllBLTP();
		} else {
			// check if saucer flight sound
			if(numSamples == ufo_lowpitch_numberOfSamples) {
				*BLTP = ufo_lowpitch_numberOfSamples;
			} else { // Clear the fifos if we've reached the end of the of a sound
				XAC97_ClearFifos(XPAR_AXI_AC97_0_BASEADDR);
				XAC97_mSetControl(XPAR_AXI_AC97_0_BASEADDR, AC97_ENABLE_IN_FIFO_INTERRUPT);
				return;
			}
		}
	}
}

// This function gets called by the interrupt handler and gets called
// whenever the FIFO gets half empty
// These sounds are placed in order of priority where the highest priority will play
// Priority goes as:
//	1) Tank Explosion sound
//	2) Tank fire sound
//	3) Saucer explosion sound
//	4) Alien explosion sound
//	5) Alien marching sound
//	6) Saucer flight sound
// The priority sound will call the function to pump in values to the FIFO and play the sound
void sound_addSoundValues(){
	if(tank_explode_sound_BLTP) {
		sound_pumpIntoFifo(&tank_explode_sound_BLTP, explosion_numberOfSamples, explosion_soundData);
	} else if(tank_fire_sound_BLTP) {
		sound_pumpIntoFifo(&tank_fire_sound_BLTP, shoot_numberOfSamples, shoot_soundData);
	} else if(saucer_explode_sound_BLTP) {
		sound_pumpIntoFifo(&saucer_explode_sound_BLTP, ufo_highpitch_numberOfSamples, ufo_highpitch_soundData);
	} else if(alien_explode_sound_BLTP) {
		sound_pumpIntoFifo(&alien_explode_sound_BLTP, explosion_numberOfSamples, explosion_soundData);
	} else if(alien_march_sound_BLTP) {
		uint32_t numSamples;
		switch(invaderSoundCounter){
			case HIGH_PITCH: // Get samles for high pitch
				numSamples = fastinvader1_numberOfSamples;
				break;
			case MED_HEIGH_PITCH: // Get samles for medium high pitch
				numSamples = fastinvader2_numberOfSamples;
				break;
			case MED_LOW_PITCH: // Get samles for medium low pitch
				numSamples = fastinvader3_numberOfSamples;
				break;
			case LOW_PITCH: // Get samles for low pitch
				numSamples = fastinvader4_numberOfSamples;
				break;
			default: // If the default is hit then there is an error
				xil_printf("Hit the default in invaderSoundCounter and shouldn't have");
		}
		sound_pumpIntoFifo(&alien_march_sound_BLTP, numSamples, alienSound_data);
	} else if(saucer_flight_sound_BLTP && saucer_getSaucerAlive()) {
		// Only pump the sound for the saucer if the saucer is alive
		sound_pumpIntoFifo(&saucer_flight_sound_BLTP, ufo_lowpitch_numberOfSamples, ufo_lowpitch_soundData);
	}
}

// Initialize sound in the main
void sound_initSound(){
	volume = AC97_VOL_MAX;
	// Reset the AC97 Core
	XAC97_HardReset(XPAR_AXI_AC97_0_BASEADDR);
	XAC97_InitAudio(XPAR_AXI_AC97_0_BASEADDR, 0);
	// Await codec
	XAC97_AwaitCodecReady(XPAR_AXI_AC97_0_BASEADDR);
	// Enable VRA
	XAC97_WriteReg(XPAR_AXI_AC97_0_BASEADDR, AC97_ExtendedAudioStat, AC97_EXTENDED_AUDIO_CONTROL_VRA);
	// Set frequency DAC
	XAC97_WriteReg(XPAR_AXI_AC97_0_BASEADDR, AC97_PCM_DAC_Rate, explosion_sampleRate);
	// Clear FIFO
	XAC97_ClearFifos(XPAR_AXI_AC97_0_BASEADDR);
	// Set control bit to cause IN_FIFO interrupts
	XAC97_mSetControl(XPAR_AXI_AC97_0_BASEADDR, AC97_ENABLE_IN_FIFO_INTERRUPT);

	/** Reset All Counters **/
	tank_fire_sound_BLTP = 0;
	tank_explode_sound_BLTP = 0;
	alien_explode_sound_BLTP = 0;
	alien_march_sound_BLTP = 0;
	saucer_flight_sound_BLTP = 0;
	saucer_explode_sound_BLTP = 0;

	/** Play Volume Settings **/
	// Initialize all sound to be the max volume
	XAC97_WriteReg(XPAR_AXI_AC97_0_BASEADDR, AC97_MasterVol, volume);
	XAC97_WriteReg(XPAR_AXI_AC97_0_BASEADDR, AC97_AuxOutVol, volume);
	XAC97_WriteReg(XPAR_AXI_AC97_0_BASEADDR, AC97_MasterVolMono, volume);
	XAC97_WriteReg(XPAR_AXI_AC97_0_BASEADDR, AC97_PCMOutVol, volume);
}

// Have a global functions for the different state machines to call
// Function to be called by state machines to play the explosion sound
void sound_playExplosionSound(){
	tank_explode_sound_BLTP = explosion_numberOfSamples;
}

// The alien sound has 4 different pitches
// Each time this function gets called the pitch should change
void sound_playFastinvaderSound(){
	invaderSoundCounter++; // Alternate the sound
	// After hitting the low pitch cycle back to the high pitch again
	if(invaderSoundCounter > LOW_PITCH){ 
		invaderSoundCounter = HIGH_PITCH;
	}
	switch(invaderSoundCounter){
		case HIGH_PITCH: // Get samles for high pitch
			alien_march_sound_BLTP = fastinvader1_numberOfSamples;
			alienSound_data = fastinvader1_soundData;
			break;
		case MED_HEIGH_PITCH: // Get samles for medium high pitch
			alien_march_sound_BLTP = fastinvader2_numberOfSamples;
			alienSound_data = fastinvader2_soundData;
			break;
		case MED_LOW_PITCH: // Get samles for medium low pitch
			alien_march_sound_BLTP = fastinvader3_numberOfSamples;
			alienSound_data = fastinvader3_soundData;
			break;
		case LOW_PITCH: // Get samles for low pitch
			alien_march_sound_BLTP = fastinvader4_numberOfSamples;
			alienSound_data = fastinvader4_soundData;
			break;
		default: // If the default is hit then there is an error
			xil_printf("Hit the default in invaderSoundCounter and shouldn't have");
	}
}

/**
 * PLAY INVADER KILLED SOUND:
 * This function resets the appropriate BLTP counter.
 */
void sound_playInvaderkilledSound(){
	alien_explode_sound_BLTP = invaderkilled_numberOfSamples;
}

/**
 * PLAY SHOOT SOUND:
 * This function resets the appropriate BLTP counter.
 */
void sound_playShootSound(){
	tank_fire_sound_BLTP = shoot_numberOfSamples;
}

/**
 * PLAY UFO SOUND:
 * This function resets the appropriate BLTP counter.
 */
void sound_playUfoSound(){
	if(saucer_flight_sound_BLTP == 0) saucer_flight_sound_BLTP = ufo_lowpitch_numberOfSamples;
}

/**
 * PLAY UFO EXPLOSION SOUND:
 * This function resets the appropriate BLTP counter.
 */
void sound_playUFOExplosion() {
	saucer_flight_sound_BLTP = 0;
	saucer_explode_sound_BLTP = ufo_highpitch_numberOfSamples;
}

// Use this function to set the volume to whatever the variable
// volume is currently at
void sounds_setVolume(){
	XAC97_WriteReg(XPAR_AXI_AC97_0_BASEADDR, AC97_MasterVol, volume);
	XAC97_WriteReg(XPAR_AXI_AC97_0_BASEADDR, AC97_AuxOutVol, volume);
	XAC97_WriteReg(XPAR_AXI_AC97_0_BASEADDR, AC97_MasterVolMono, volume);
	XAC97_WriteReg(XPAR_AXI_AC97_0_BASEADDR, AC97_PCMOutVol, volume);
}

// Function to increase the volume
// Decreasing the variable volume will increase the sound
void sounds_increaseVolume(){
	// Only increase if the volume isn't at the max already
	if(volume > AC97_VOL_MAX && volume <= AC97_VOL_MIN){
		volume -= CHANGE_VOLUME_BY;
		sounds_setVolume(); // Set the volume after increasing the values
	}
}

// Function to decrease the volume
// Increasing the variable volume will decrease the sound
void sounds_decreaseVolume(){
	// Only decrease if the volume isn't at the min already
	if(volume >= AC97_VOL_MAX && volume < AC97_VOL_MIN){
		volume += CHANGE_VOLUME_BY;
		sounds_setVolume(); // Set the volume after decreasing the values
	}
}

// Reset the saucer so the sound doesn't keep playing once
// it has gone off of the screen
void sound_resetSaucer() {
	saucer_flight_sound_BLTP = 0; // Reset bits to play
	XAC97_ClearFifos(XPAR_AXI_AC97_0_BASEADDR); // Clear the sound from the FIFO
	XAC97_mSetControl(XPAR_AXI_AC97_0_BASEADDR, AC97_ENABLE_IN_FIFO_INTERRUPT);
}
