/*
 * sound.h
 *
 *  Created on: Nov 1, 2017
 *      Author: superman
 */

#ifndef SOUND_H_
#define SOUND_H_

#include "xac97_l.h" // Include for access to AC97 variables
#include <stdint.h> // Include for various int values
#include <stdio.h> // Include for standard input and output
#include "xparameters.h"
#include "saucer.h" // Inclue to see if the saucer is alive

#define HIGH_PITCH 0 // Value for if the counter is at the high pitch
#define MED_HEIGH_PITCH 1 // Value for if the counter is at the medium high pitch
#define MED_LOW_PITCH 2 // Value for if the counter is at the medium low pitch
#define LOW_PITCH 3 // Value for if the counter is at the low pitch
#define SAMPLES_FOR_FIFO 100 // Number of samples to put in the FIFO each interrupt of AC97
#define CHANGE_VOLUME_BY 50 // Number to change the volume by

void sound_initSound();
void sound_addSoundValues();
void sound_playExplosionSound();
void sound_playFastinvaderSound();
void sound_playInvaderkilledSound();
void sound_playShootSound();
void sound_playUfoSound();
void sound_resetSaucer();

#endif /* SOUND_H_ */
