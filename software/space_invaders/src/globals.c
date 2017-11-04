/******************************************************************************
 * globals.c
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
 * 28 Sep 2017 | Started file. Setup functions for setting tank and tank
 * 							 bullet positions.
 *****************************************************************************/

// Demonstrates one way to handle globals safely in C.
#include "globals.h"


#define TANK_START_POS 100 // Defines where on the screen the tank is going to start

// Here are the globals
static uint32_t score; // Lives will need to be kept track of globally
static uint16_t lives; // Score will need to be kept track of globally
static bool isGameOver; // Global to let main know when the game is over

// Set whether the game is over
void globals_setGameOver(bool gameOver){
	isGameOver = gameOver;
}
// Get whether the game is over
bool globals_getIsGameOver(){
	return isGameOver;
}
// Set the number of current lives
void globals_setLives(uint16_t currentLives){
	lives = currentLives;
}
// Decrement the lives when the tank is hit
void globals_decLives(){
	lives--;
}
// Get the number of current lives
uint16_t globals_getLives(){
	return lives;
}
// Add the number of points passed in to the score
void globals_incScore(uint32_t currentScore){
	score += currentScore;
}
// Get the current score
uint32_t globals_getScore(){
	return score;
}
