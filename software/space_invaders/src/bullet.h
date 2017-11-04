/*******************************************************************************
 * bullet.h
 * ==========================================================================
 * COURSE INFO
 * ECEn 427: Embedded Programming
 * Prof. Brad Hutchings
 *
 * AUTHORS
 * Davis Crabb
 * Peter Loderup
 * 
 * FILE DESCRIPTION
 * Handles all calculation relating to moving of alien and tank bullets.
 *
 * UPDATE LOG
 * 10 Oct 2017 | Created file. Added definitions from "render.h".
 * 14 Oct 2017 | Added some #defines relating to bullet movement. Created
 *               'moveBullets', 'bullet_getBullets' and 'updatePosition'
 *               functions. Also moved enumerations and structures relating to
 *               bullets from 'globals.h' to this file.
 ******************************************************************************/
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "render.h"
#include "bunker.h"

// #Defines
#define MAX_NUM_ABTS    4       // maximum number of alien bullets
#define MAX_NUM_TBTS    1       // maximum number of tank bullets
#define MAX_NUM_BTS     MAX_NUM_ABTS + MAX_NUM_TBTS // maximum number of bullets
#define TBT_HEIGHT      10      // tank bullet height
#define TBT_WIDTH       2       // tank bullet width
#define TBT_X_OFST      14      // tank bullet x-offset
#define TBT_Y_OFST      -10     // tank bullet y-offset
#define TBT_PX_TO_MOVE  6       // number of pixels for (all) bullets to move
#define ABT_PX_TO_MOVE  10      // number of pixels for (all) bullets to move
#define TBT_INIT_SPD    1       // initial tank bullet speed
#define TBT0_I          0       // index of tank bullet in array
#define ABT_HEIGHT      10      // alien bullet height
#define ABT_WIDTH       6       // alien bullet width
#define ABT_X_OFST      4       // alien bullet x-offset
#define ABT0_I          1       // index of alien bullet 1 in array
#define ABT1_I          2       // index of alien bullet 2 in array
#define ABT2_I          3       // index of alien bullet 3 in array
#define ABT3_I          4       // index of alien bullet 4 in array
#define ABT_INIT_SPD    3       // initial speed of alien bullet
#define ABT_SPD_DIFF    1       // speed by which alien bullets increment
#define ABT_MAX_SPD     4       // maximum alien bullet speed
#define OUT_OF_BOUNDS   -1      // Used to see if something is out of bounds

// Get Functions
//bullet_t* bullet_getPositions(point_t* bullet_positions);

// State Machine
void bullet_FSM();

// Enums
typedef enum {tank, cross_up, cross_mid, cross_down, diag_up, diag_down} bullet_type;

// Bullet Struct
typedef struct {
    uint32_t    x;      // x coordinate
    uint32_t    y;      // y coordinate
    bool        active; // bullet active?
    bullet_type btype;  // bullet type
} bullet_t;

bullet_t*   bullet_getBullets();
bullet_t    bullet_getBullet();
void        bullet_setActive(uint32_t i, bool active);
void        bullet_setCoords(uint32_t i, uint32_t x, uint32_t y);
bool		bullet_ABTCollision(uint32_t x, uint32_t y);
void        bullet_eraseBullets(uint32_t a, uint32_t b);
void        bullet_moveBullets(uint32_t a, uint32_t b);
void        bullet_drawBullets(uint32_t a, uint32_t b);
bool        bullet_TBTCollision(uint32_t x, uint32_t y);
bool 		bullet_outOfBounds(uint32_t y);
