/******************************************************************************
 * globals.h
 * ==========================================================================
 * COURSE INFO
 * ECEn 427: Embedded Programming
 * Prof. Brad Hutchings
 *
 * AUTHORS
 * Davis Crabb
 * Peter Loderup
 *
 * This set of files is to control just the saucer specifically
 *****************************************************************************/

#ifndef SAUCER_H_
#define SAUCER_H_

#include <stdint.h>
#include <stdlib.h>
#include "sound.h" // Include to play sound
#include "globals.h" // Include for access to globals
#include "render.h" // Include to draw

#define MOVE_SAUCER_PX 4 // Move the saucer this number of pixels
#define SAUCER_POINTS 50 // Number of points
#define POINT_MULTIPLIER 2 // Way to get 50, 100, or 150 points with saucer_points
#define NUM_DIRECTIONS 2 // Only two kinds of directions
#define MAX_SAUCER_WAIT 4500 // The max time it will take for the saucer to appear is 45 seconds
#define TRUE 1 // Set a value for true
#define ERASE_TIMER_MAX 80 // Max of the erase timer

void saucer_moveSaucer();
enum direction_t saucer_getSaucerDirection();
void saucer_setSaucerPosition(point_t point);
point_t saucer_getSaucerPosition();
void saucer_tickSaucer();
bool saucer_getSaucerAlive();
void saucer_hitSaucer();


#endif /* SHIP_H_ */
