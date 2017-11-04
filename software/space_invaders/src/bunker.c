// Includes
#include "bunker.h"

// Declarations
Bunker bunkers[NUM_BUNKS];

// Definitions
#define OFFSET_CORRECTLY 	1
#define OUT_OF_BOUNDS		-1

// Function so other files can get the bunker section hp
uint32_t bunker_getBunkerSectionHP(uint32_t index, uint32_t section) {
	return bunkers[index].hp[section];
}

/**
 * FUNCTION
 * Reset Bunkers
 *
 * FUNCTIONALITY
 * Restores all bunkers to initial HP.
 */
void bunker_resetBunkers() {
    // iterator
    uint8_t i, j;

    // set hp
    for(i = 0; i < NUM_BUNKS; i++) {
    	bunkers[i].index = i;

		for(j = 0; j < NUM_SECTS; j++) {
			bunkers[i].hp[j] = BUNK_MAX_HP;
		}
    }
}


/**
 * FUNCTION
 * Damage Bunker
 *
 * FUNCTIONALITY
 * Damages a given section in a bunker. Note, this function does not re-draw
 * the bunker.
 * 
 * PARAMETERS
 * @param index, an 8-bit unsigned integer value representing the index of the
 *               bunker to damage; see header for note on indexing bunkers
 * @param bunker, a pointer to a Bunker struct
 */
void bunker_damageSection(uint32_t index, uint32_t section) {
	bunkers[index].hp[section]--;
}


/**
 * Coordinates to Index
 * Convert screen coordinates to corresponding bunker index upon which they
 * fall.
 * 
 * @param x, x-coordinate
 * @param y, y-coordinate
 * @return index, bunker index; -1 if no bunker exists
 */
uint32_t bunker_coordsToIndex(uint32_t x, uint32_t y) {
    // variables
    uint32_t index;
    
    // check y-coord
    if(y < BUNKS_Y_POS || y > BUNKS_Y_POS+BUNK_HIG) {
        return -1;
    }
    
    // which bunker?
    if(x >= BUNK0_X_POS && x < BUNK0_X_POS+BUNK_WID) {
        index = BUNK_0;
    } else if(x >= BUNK1_X_POS && x < BUNK1_X_POS+BUNK_WID) {
        index = BUNK_1;
    } else if(x >= BUNK2_X_POS && x < BUNK2_X_POS+BUNK_WID) {
        index = BUNK_2;
    } else if(x >= BUNK3_X_POS && x < BUNK3_X_POS+BUNK_WID) {
        index = BUNK_3;
    } else {
    	return -1;
    }
    
    // complete
    return index;
}


/**
 * Coordinates to Section
 * Convert screen coordinates to corresponding bunker section upon which they
 * fall. Note, that if the coordinates given relate to a valid
 * section position, then it is assumed that that section has health greater
 * than zero.
 * 
 * @param x, x-coordinate
 * @param y, y-coordinate
 * @return section, bunker section; -1 if no section exists
 */
uint32_t bunker_coordsToSection(uint32_t x, uint32_t y) {
	// variables
	uint32_t i, j;
	uint32_t index;

	// normalize x-coord to bunker 0
    while(x >= BUNK1_X_POS) {
        x -= BUNK0_X_POS+BUNK_WID;
    }

    // check x-coord
    if(x < BUNK0_X_POS || x > BUNK0_X_POS+BUNK_WID) {
    	return OUT_OF_BOUNDS;
    }

    // check y-coord
    if(y < BUNKS_Y_POS) {
    	return OUT_OF_BOUNDS;
    }

    // iterate the sections of the bunker
    for(i = OFFSET_CORRECTLY; i <= NUM_Y_SECTS; i++) {
    	for(j = OFFSET_CORRECTLY; j <= NUM_X_SECTS; j++) {
    		if(i == NUM_Y_SECTS && j == NUM_X_SECTS) {
    			j += JUMP_SECTION;
    		}
    		if(x < j*BUNK_SEC_WID+BUNK0_X_POS && y < i*BUNK_SEC_HIG+BUNKS_Y_POS) {
    			index = (i-OFFSET_CORRECTLY)*NUM_X_SECTS + (j-OFFSET_CORRECTLY);
    			if(index > NUM_SECTS) index = NUM_SECTS-OFFSET_CORRECTLY;
    			return index;
    		}
    	}
    }
}
