/*
 * sound.h
 *
 *  Created on: Nov 1, 2017
 *      Author: superman
 */

#ifndef SOUND_H_
#define SOUND_H_

#include "xac97_l.h"
#include <stdint.h>
#include <stdio.h>
#include "xparameters.h"
#include "saucer.h"

#define HIGH_PITCH 0
#define MED_HEIGH_PITCH 1
#define MED_LOW_PITCH 2
#define LOW_PITCH 3
#define SAMPLES_FOR_FIFO 100
#define CHANGE_VOLUME_BY 50

void sound_initSound();
void sound_addSoundValues();
void sound_playExplosionSound();
void sound_playFastinvaderSound();
void sound_playInvaderkilledSound();
void sound_playShootSound();
void sound_playUfoSound();
void sound_resetSaucer();

#endif /* SOUND_H_ */
