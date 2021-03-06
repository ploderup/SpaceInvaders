/*******************************************************************************
 * tank.h
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
 * 12 Oct 2017 | Created file. Added enumerated type for current state of
 *               state machine and code for state machine, as well.
 ******************************************************************************/
// Includes
#pragma  once
#include <stdbool.h>
#include <stdint.h>
#include "globals.h"
#include "render.h"
#include "bullet.h"
#include "sound.h"

// Definitions
#define TANK_GRAPHIC_HEIGHT  16 // Height of the tank graphic
#define TANK_GRAPHIC_WIDTH   30 // Width of the tank graphic
#define TANK_MOVEMENT_WIDTH  5 // Move the tank this number of pixels
typedef enum {LEFT, RIGHT} direction; // Enum for the tank direction

// Prototypes
void        tank_FSM();
void        tank_setXPos(uint32_t x);
uint32_t    tank_getXPos();
void        tank_setYPos(uint32_t y);
uint32_t    tank_getYPos();
void 		tank_hitTank();
bool        tank_tooCloseToEdge(direction d);
void        tank_moveTank(direction d);
