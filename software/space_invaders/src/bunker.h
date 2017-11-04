/*******************************************************************************
 * bunker.h
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
 * 04 Oct 2017 | Started file.
 ******************************************************************************/
// Includes
#include <stdint.h>
#include <stdio.h>
#pragma once

// graphical definitions
#define NUM_BUNKS       4       // number of bunkers
#define NUM_SECTS       10      // number of sections /bunker
#define NUM_X_SECTS		4
#define NUM_Y_SECTS		3
#define BUNK_WID        48      // bunker width (pixels)
#define BUNK_HIG        36      // bunker height (pixels)
#define BUNK_SEC_WID    12      // bunker section width (pixels)
#define BUNK_SEC_HIG    12      // bunker section height (pixels)
#define BUNK0_X_POS     89      // x-position of bunk 0, 89 from screen
#define BUNK1_X_POS     226     // x-position of bunk 1, 90 from bunk 0
#define BUNK2_X_POS     363     // x-position of bunk 2, 90 from bunk 1
#define BUNK3_X_POS     500     // x-position of bunk 3, 90 from bunk 2
#define BUNKS_Y_POS     321     // y-position of all bunkers
#define BUNK_MAX_HP     4       // maximum hp of given bunker section
#define BUNK_ST1_HP     3       // hp of bunker section after being shot once
#define BUNK_ST2_HP     2       // hp of bunker section after being shot twice
#define BUNK_ST3_HP     1       // hp of bunker section after being shot thrice
#define BUNK_MIN_HP     0       // minimum hp of bunker section
#define BUNK_0          0       // bunker 0
#define BUNK_1          1       // bunker 1
#define BUNK_2          2       // bunker 2
#define BUNK_3          3       // bunker 3
#define JUMP_SECTION    2       // Used for when we need to jump over a section

/**
 * DESCRIPTION
 * Bunker Definitions
 * 
 * NOTES
 * These bunkers are initialized to bunker_24x18 from graphics.h and, like all
 * graphics from that header, need to be scaled up before drawing.
 */
// bunker struct
typedef struct Bunker {
    uint8_t hp[NUM_SECTS];
    uint8_t index;
} Bunker;

// function prototypes
void bunker_resetBunkers();
void bunker_damageSection(uint32_t index, uint32_t section);
uint32_t bunker_getBunkerSectionHP(uint32_t index, uint32_t section);
uint32_t bunker_coordsToIndex(uint32_t x, uint32_t y);
uint32_t bunker_coordsToSection(uint32_t x, uint32_t y);

