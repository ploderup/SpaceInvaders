/*******************************************************************************
 * alien.h
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
 * 05 Oct 2017 | Started file.
 * 06 Oct 2017 | Finalized changes for Lab 3 passoff. Had a hard time getting
 *               aliens to move and erase correctly; solved problem by adding
 *               static alien block frame coordinates in addition to dynamic
 *               ones. May want to modify this later as it may be a little too
 *               contrived.
 ******************************************************************************/
// Inclusions
#pragma once
#include <stdbool.h>
#include <stdio.h>
#include "globals.h"
#include "render.h"
#include "functions.h"
#include "sound.h"

// Definitions
#define NUM_ALIENS          55      // number of aliens at start of game
#define NUM_ALIEN_COLS      11      // initial number of alien columns
#define NUM_ALIEN_ROWS      5       // initial number of alien rows
#define ALIEN_HIG           16      // alien sprite height (pixels)
#define ALIEN_WID           24      // alien sprite width (pixels)
#define ALIEN_SIDE_PAD      8       // padding on each side of alien sprite
#define ALIEN_WALL_PAD      10      // padding btw alien block and screen walls
#define ALIEN_BTM_PAD       12      // padding on bottom of alien sprite
#define ALIEN_BLK_WID       (NUM_ALIEN_COLS*ALIEN_WID + (NUM_ALIEN_COLS-1)*ALIEN_SIDE_PAD)
#define ALIEN_BLK_HIG       (NUM_ALIEN_ROWS*ALIEN_HIG + (NUM_ALIEN_ROWS-1)*ALIEN_BTM_PAD)
#define ALIEN_START_X       118     // initial x-position of alien block
#define ALIEN_START_Y       92      // initial y-position of alien block
#define ALIEN_HRZ_SPD       4       // number of pixels to move alien at a time
#define ALIEN_VRT_SPD       10      // number of pixels to move alien at a time
#define ALIEN_ALIVE         1       // alien alive value, used as boolean
#define ALIEN_DEAD          0       // alien dead value, used as boolean
#define ALIEN_SPRITE_IN     0       // aliens displayed w/ 'in' sprite
#define ALIEN_SPRITE_OUT    1       // aliens displayed w/ 'out' sprite
#define ALIEN_TOP_TIER      1       // top sprite below row 1 (counting from top)
#define ALIEN_MID_TIER      3       // mid sprites below row 3 (counting from top)
#define ALIEN_SML           0       // small alien type
#define ALIEN_MID           1       // medium alien type
#define ALIEN_BIG           2       // big alien type
#define ALIEN_TIMER_MAX     75      // This means the aliens will move every 750ms
#define HALF_ALIEN_WIDTH 6          // Used for setting the bullet right below the alien
#define ALIEN_SECOND_ROW_INDEX 11   // Starting index of the second row
#define ALIEN_FOURTH_ROW_INDEX 33   // Starting index of the 4th row
#define TOP_ALIEN_SCORE 40          // Score of the top row of aliens
#define MID_ALIEN_SCORE 20          // Score of the middle two rows of aliens
#define BOT_ALIEN_SCORE 10          // Score of the bottom two rows of aliens
#define ALIEN_BULLET_OFFSET_X 7     // Helps with drawing the bullet
#define SHOOT_TIMER_MAX 2000        // Fire this interval at a max
#define SHOOT_TIMER_MIN 100         // Fire this interval at a min
#define EXPLOXION_TIMER 10          // Time that the explosion will show - 100ms
#define OFFSET_CORRECTLY 1          // Used for any adjustment with -1 we need to do
#define ALIEN_EXPLOSION_WIDTH	12
#define ALIEN_EXPLOSION_HEIGHT	10

// AlienBlock Struck
typedef struct AlienBlock {
    // block location (static)
    int32_t    stop_y;                 // x-coord of top of block
    int32_t    sbtm_y;                 // x-coord of bottom of block
    int32_t    slft_x;                 // y-coord of left of block
    int32_t    srit_x;                 // y-coord of right of block
    
    // block location (dynamic)
    uint16_t    dtop_y;                 // x-coord of top of block
    uint16_t    dbtm_y;                 // x-coord of bottom of block
    uint16_t    dlft_x;                 // y-coord of left of block
    uint16_t    drit_x;                 // y-coord of right of block
    
    // info on remaining aliens
    bool aliens_alive[NUM_ALIENS];      // aliens remaining
    bool cols_alive[NUM_ALIEN_COLS];	// cols w/ 1+ alien remaining
    bool rows_alive[NUM_ALIEN_ROWS];    // rows w/ 1+ alien remaining
    
    // sprite state
    uint8_t sprite_state;
} AlienBlock;

// Global Variables
AlienBlock aliens;

// Function Prototypes
void    alien_tickAliens(); // State Machine
void    alien_resetAliens();
void    alien_killAlien(uint8_t index);
void    alien_moveAliens();
bool    alien_inTopRow(uint8_t index);
bool    alien_inBottowRow(uint8_t index);
bool    alien_inLeftColumn(uint8_t index);
bool    alien_inRightColumn(uint8_t index);
bool    alien_lastInRow(uint8_t index);
bool    alien_lastInColumn(uint8_t index);
point_t alien_getCoordinates(uint16_t index);
point_t alien_getBottomAlien();
bool    alien_isAlive(uint8_t index);
bool    alien_detectHitAlien();
void    alien_fireAlien();
