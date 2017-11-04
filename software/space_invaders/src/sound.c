/*
 * sound.c
 *
 *  Created on: Nov 1, 2017
 *      Author: superman
 */
#include "sound.h"

extern uint32_t explosion_soundData[];
extern uint32_t explosion_numberOfSamples;
extern uint32_t explosion_sampleRate;

extern uint32_t fastinvader1_soundData[];
extern uint32_t fastinvader1_numberOfSamples;
extern uint32_t fastinvader1_sampleRate;

extern uint32_t fastinvader2_soundData[];
extern uint32_t fastinvader2_numberOfSamples;
extern uint32_t fastinvader2_sampleRate;

extern uint32_t fastinvader3_soundData[];
extern uint32_t fastinvader3_numberOfSamples;
extern uint32_t fastinvader3_sampleRate;

extern uint32_t fastinvader4_soundData[];
extern uint32_t fastinvader4_numberOfSamples;
extern uint32_t fastinvader4_sampleRate;

extern uint32_t invaderkilled_soundData[];
extern uint32_t invaderkilled_numberOfSamples;
extern uint32_t invaderkilled_sampleRate;

extern uint32_t shoot_soundData[];
extern uint32_t shoot_numberOfSamples;
extern uint32_t shoot_sampleRate;

extern uint32_t ufo_highpitch_soundData[];
extern uint32_t ufo_highpitch_numberOfSamples;
extern uint32_t ufo_highpitch_sampleRate;

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

uint8_t invaderSoundCounter = HIGH_PITCH;

uint32_t* alienSound_data;

Xuint32* sound_ptr;

static uint32_t volume;

void decrementAllBLTP() {
	if(tank_fire_sound_BLTP) tank_fire_sound_BLTP--;
	if(tank_explode_sound_BLTP) tank_explode_sound_BLTP--;
	if(alien_explode_sound_BLTP) alien_explode_sound_BLTP--;
	if(alien_march_sound_BLTP) alien_march_sound_BLTP--;
	if(saucer_flight_sound_BLTP) saucer_flight_sound_BLTP--;
	if(saucer_explode_sound_BLTP) saucer_explode_sound_BLTP--;
}

void sound_pumpIntoFifo(uint32_t* BLTP, uint32_t numSamples, uint32_t* sound_data) {
	int x;
	Xuint32 sample;

	/** Wait for the ready signal **/
	XAC97_AwaitCodecReady(XPAR_AXI_AC97_0_BASEADDR);

	for(x = 0; x < SAMPLES_FOR_FIFO; x++){
		if(*BLTP > 0) {
			sample = sound_data[numSamples - *BLTP];
			XAC97_WriteFifo(XPAR_AXI_AC97_0_BASEADDR, sample);
			decrementAllBLTP();
		} else {
			// check if saucer flight sound
			if(numSamples == ufo_lowpitch_numberOfSamples) {
				*BLTP = ufo_lowpitch_numberOfSamples;
			} else {
				XAC97_ClearFifos(XPAR_AXI_AC97_0_BASEADDR);
				XAC97_mSetControl(XPAR_AXI_AC97_0_BASEADDR, AC97_ENABLE_IN_FIFO_INTERRUPT);
				return;
			}
		}
	}
}

// This function gets called by the interrupt handler
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
			case HIGH_PITCH:
				numSamples = fastinvader1_numberOfSamples;
				//invaderSoundCounter = MED_HEIGH_PITCH;
				break;
			case MED_HEIGH_PITCH:
				numSamples = fastinvader2_numberOfSamples;
				//invaderSoundCounter = MED_LOW_PITCH;
				break;
			case MED_LOW_PITCH:
				numSamples = fastinvader3_numberOfSamples;
				//invaderSoundCounter = LOW_PITCH;
				break;
			case LOW_PITCH:
				numSamples = fastinvader4_numberOfSamples;
				//invaderSoundCounter = HIGH_PITCH;
				break;
			default:
				xil_printf("Hit the default in invaderSoundCounter and shouldn't have");
			break;
		}
		sound_pumpIntoFifo(&alien_march_sound_BLTP, numSamples, alienSound_data);

	} else if(saucer_flight_sound_BLTP && saucer_getSaucerAlive()) {
		sound_pumpIntoFifo(&saucer_flight_sound_BLTP, ufo_lowpitch_numberOfSamples, ufo_lowpitch_soundData);
	}
}

void sound_initSound(){
	volume = AC97_VOL_MAX;
	// Reset the AC97 Core
	XAC97_HardReset(XPAR_AXI_AC97_0_BASEADDR);
	XAC97_InitAudio(XPAR_AXI_AC97_0_BASEADDR, 0);
	//await codec
	XAC97_AwaitCodecReady(XPAR_AXI_AC97_0_BASEADDR);
	//enable VRA
	XAC97_WriteReg(XPAR_AXI_AC97_0_BASEADDR, AC97_ExtendedAudioStat, AC97_EXTENDED_AUDIO_CONTROL_VRA);
	//set frequency DAC
	XAC97_WriteReg(XPAR_AXI_AC97_0_BASEADDR, AC97_PCM_DAC_Rate, explosion_sampleRate);
	//clear FIFO
	XAC97_ClearFifos(XPAR_AXI_AC97_0_BASEADDR);
	//set control bit to cause IN_FIFO interrupts
	XAC97_mSetControl(XPAR_AXI_AC97_0_BASEADDR, AC97_ENABLE_IN_FIFO_INTERRUPT);

	/** Reset Counters **/
	tank_fire_sound_BLTP = 0;
	tank_explode_sound_BLTP = 0;
	alien_explode_sound_BLTP = 0;
	alien_march_sound_BLTP = 0;
	saucer_flight_sound_BLTP = 0;
	saucer_explode_sound_BLTP = 0;

	/** Play Volume Settings **/
	XAC97_WriteReg(XPAR_AXI_AC97_0_BASEADDR, AC97_MasterVol, volume);
	XAC97_WriteReg(XPAR_AXI_AC97_0_BASEADDR, AC97_AuxOutVol, volume);
	XAC97_WriteReg(XPAR_AXI_AC97_0_BASEADDR, AC97_MasterVolMono, volume);
	//XAC97_WriteReg(XPAR_AXI_AC97_0_BASEADDR, AC97_PCBeepVol, AC97_VOL_MAX);
	XAC97_WriteReg(XPAR_AXI_AC97_0_BASEADDR, AC97_PCMOutVol, volume);
}

// Have a global function for the different state machines to call
void sound_playExplosionSound(){
	tank_explode_sound_BLTP = explosion_numberOfSamples;
}

// The alien sound has 4 different pitches
// Each time this function gets called the pitch should change
void sound_playFastinvaderSound(){
	invaderSoundCounter++;
	if(invaderSoundCounter > LOW_PITCH){
		invaderSoundCounter = HIGH_PITCH;
	}
	switch(invaderSoundCounter){
		case HIGH_PITCH:
			alien_march_sound_BLTP = fastinvader1_numberOfSamples;
			alienSound_data = fastinvader1_soundData;
			//invaderSoundCounter = MED_HEIGH_PITCH;
			break;
		case MED_HEIGH_PITCH:
			alien_march_sound_BLTP = fastinvader2_numberOfSamples;
			alienSound_data = fastinvader2_soundData;
			//invaderSoundCounter = MED_LOW_PITCH;
			break;
		case MED_LOW_PITCH:
			alien_march_sound_BLTP = fastinvader3_numberOfSamples;
			alienSound_data = fastinvader3_soundData;
			//invaderSoundCounter = LOW_PITCH;
			break;
		case LOW_PITCH:
			alien_march_sound_BLTP = fastinvader4_numberOfSamples;
			alienSound_data = fastinvader4_soundData;
			//invaderSoundCounter = HIGH_PITCH;
			break;
		default:
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

void sounds_setVolume(){
	XAC97_WriteReg(XPAR_AXI_AC97_0_BASEADDR, AC97_MasterVol, volume);
	XAC97_WriteReg(XPAR_AXI_AC97_0_BASEADDR, AC97_AuxOutVol, volume);
	XAC97_WriteReg(XPAR_AXI_AC97_0_BASEADDR, AC97_MasterVolMono, volume);
	XAC97_WriteReg(XPAR_AXI_AC97_0_BASEADDR, AC97_PCMOutVol, volume);
}

void sounds_increaseVolume(){
	if(volume > AC97_VOL_MAX && volume <= AC97_VOL_MIN){
		volume -= CHANGE_VOLUME_BY;
		sounds_setVolume();
	}
}

void sounds_decreaseVolume(){
	if(volume >= AC97_VOL_MAX && volume < AC97_VOL_MIN){
		volume += CHANGE_VOLUME_BY;
		sounds_setVolume();
	}
}

void sound_resetSaucer() {
	saucer_flight_sound_BLTP = 0;
	XAC97_ClearFifos(XPAR_AXI_AC97_0_BASEADDR);
	XAC97_mSetControl(XPAR_AXI_AC97_0_BASEADDR, AC97_ENABLE_IN_FIFO_INTERRUPT);
}
