/*******************************************************************************
 * functions.h
 * ==========================================================================
 * COURSE INFO
 * ECEn 427: Embedded Programming
 * Prof. Brad Hutchings
 *
 * AUTHORS
 * Davis Crabb
 * Peter Loderup
 *
 * UPDATE LOG
 * 04 Oct 2017 | Started file. Added random number generator.
 ******************************************************************************/
// Includes
#pragma once
#include <stdint.h>
#include <stdlib.h>

// Function Prototypes
void seedRand();
uint8_t getRand8();

