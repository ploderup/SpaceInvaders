// Include Header
#include "bullet.h"

// Global Vars
// Enum to keep track of the state for the state machine
enum        state_type {INIT, IDLE, MOVE_ALL, MOVE_TBTS, MOVE_ABTS} state;
bullet_t    bullets[MAX_NUM_BTS]; // Array to hold all of the bullets
uint32_t    a_counter; // alien bullet counter
uint32_t    t_counter; // Tank bullet counter
uint32_t    abullet_delay; // delay for the alien bullets
uint32_t    tbullet_delay; // delay for the tank bullets

// Accessors
// Used to get the array of alien bullets
bullet_t* bullet_getBullets()             { return bullets; }

// Funciton to get a specific bullet in the array of bullets
bullet_t bullet_getBullet(uint32_t i)    { return bullets[i]; }

// Funciton to set a bullet as active
void bullet_setActive(uint32_t i, bool active)
{ bullets[i].active = active; }

// Function to set the coordinates of a bullet
// Bullet is based on the index that is passed in
void bullet_setCoords(uint32_t i, uint32_t x, uint32_t y)
{ bullets[i].x = x; bullets[i].y = y; }

// Increase ABT Speed
void bullet_incAlienBulletSpeed() {
    if(abullet_delay-ABT_SPD_DIFF > ABT_MAX_SPD) {
        abullet_delay -= ABT_SPD_DIFF;
    }
}

/**
 * Bullet FSM:
 * A finite state machine to govern the movement of all bullets.
 */
void bullet_FSM() {
    switch(state) {
        case INIT:
        	a_counter = 0; // init the timer
        	t_counter = 0; // init the timer
        	abullet_delay = ABT_INIT_SPD; // init the delay
        	tbullet_delay = TBT_INIT_SPD; // init the delay
            // reset bullets positions and status
            uint32_t i;
            for(i = 0; i < MAX_NUM_BTS; i++) {
                // set tank bullet type
                if(i >= TBT0_I && i < (TBT0_I+MAX_NUM_TBTS))
                    bullets[i].btype = tank;
                // set alien bullet type
                else if(i % 2 == 0)
                    bullets[i].btype = diag_up;
                else
                    bullets[i].btype = cross_up;
                    
                // clear other members
                bullets[i].x        = 0; // reset the x
                bullets[i].y        = 0; // reset the y
                bullets[i].active   = false; // reset to false
            }
            state = IDLE;
        break;
        
        case IDLE:
            // increment counters
            t_counter++;
            a_counter++;
                
            // move alien bullets?
            if(a_counter >= abullet_delay) {
                a_counter = 0;
                state = MOVE_ABTS;
                
            // move tank bullet?
            } else if(t_counter >= tbullet_delay) {
                t_counter = 0;
                state = MOVE_TBTS;
            }
        break;
        // Move alien bullets 
        case MOVE_ABTS:
            bullet_eraseBullets(ABT0_I, MAX_NUM_ABTS); // Erase where the bullet is
            bullet_moveBullets(ABT0_I, MAX_NUM_ABTS); // Move the bullets posiition
            bullet_drawBullets(ABT0_I, MAX_NUM_ABTS); // Draw the bullet at the new position
            
            // check tank counter
            if(t_counter >= tbullet_delay) {
                t_counter = 0;
                state = MOVE_TBTS;
            } else {
            	state = IDLE;
            }
        break;
        // Move tank bullets 
        case MOVE_TBTS:
            bullet_eraseBullets(TBT0_I, MAX_NUM_TBTS); // Erase where the bullet is
            bullet_moveBullets(TBT0_I, MAX_NUM_TBTS); // Move the bullets posiition
            bullet_drawBullets(TBT0_I, MAX_NUM_TBTS); // Draw the bullet at the new position
            state = IDLE; 
        break;
        // Error state
        default: // We should never hit this
        break;
    }
}


/**
 * Erase Bullets:
 * Calls 'render_eraseBullet()' on all active bullets from index a for b
 * iterations.
 * 
 * @param a, index at which to start for loop
 * @param b, number of iterations for which to loop
 */
void bullet_eraseBullets(uint32_t a, uint32_t b) {
    // declare iterator
    uint32_t i;
    
    // iterate through bullets
    for(i = a; i < (a+b); i++) {
        if(bullets[i].active) {
            render_eraseBullet(bullets[i].x, bullets[i].y, bullets[i].btype);
        }
    }
}


/**
 * Move Bullets:
 * Update the position of all active bullets from index a for b iterations.
 * If the bullet is moved out of bounds, it is deactivated. If the bullet
 * collides with a bunker, alien, or saucer...
 * 
 * @param a, index at which to start for loop
 * @param b, number of iterations for which to loop
 */
void bullet_moveBullets(uint32_t a, uint32_t b) {
    // declare iterator
    uint32_t i;
    
    // iterate through bullets
    for(i = a; i < (a+b); i++) {
        if(bullets[i].active) {
            if(bullets[i].btype == tank) { // Check if it is a tank bullet
                bullets[i].y -= TBT_PX_TO_MOVE;
                if(bullet_outOfBounds(bullets[i].y)) { // Check if the bullet is out of bounds
                	bullets[i].active = false;
                	toggleType(i); // Switch to the other bitmap of the bullet
                } else if(bullet_TBTCollision(bullets[i].x, bullets[i].y)) {
                	bullets[i].active = false;
                	toggleType(i); // Switch to the other bitmap of the bullet
                }
            } else { // It must be an alien bullet in this case
                bullets[i].y += ABT_PX_TO_MOVE;
                if(bullet_outOfBounds(bullets[i].y)) { // Check if the bullet is out of bounds
                	bullets[i].active = false;
                	toggleType(i); // Switch to the other bitmap of the bullet
                } else if(bullet_ABTCollision(bullets[i].x, bullets[i].y)) {
                	bullets[i].active = false;
                	toggleType(i); // Switch to the other bitmap of the bullet
                }
            }
        }
    }
}


/**
 * Toggle Type
 * Switches the sprite associated with a given bullet from cross to diagonal,
 * or visa versa.
 */
void toggleType(uint32_t i) {
	if(bullets[i].btype != tank) {
		if(bullets[i].btype == diag_up || bullets[i].btype == diag_down) {
			bullets[i].btype = cross_up;
		} else {
			bullets[i].btype = diag_up;
		}
	}
}

/**
 * Draw Bullets:
 * Calls 'render_drawBullet()' on all active bullets from index a for b
 * iterations.
 * 
 * @param a, index at which to start for loop
 * @param b, number of iterations for which to loop
 */
void bullet_drawBullets(uint32_t a, uint32_t b) {
    // declare iterator
    uint32_t i;
    
    // iterate through bullets
    for(i = a; i < (a+b); i++) {
        if(bullets[i].active) {
            // draw bullet
        	render_drawBullet(bullets[i].x, bullets[i].y, bullets[i].btype, WHITE);

            // toggle sprite
        	switch(bullets[i].btype) {
        		case cross_up:
        			bullets[i].btype = cross_mid;
        		break;
        		case cross_mid:
        			bullets[i].btype = cross_down;
        		break;
        		case cross_down:
        			bullets[i].btype = cross_up;
        		break;
        		case diag_up:
        			bullets[i].btype = diag_down;
        		break;
        		case diag_down:
        			bullets[i].btype = diag_up;
        		break;

        		// TANK BULLET
        		default:
        			// do nothing
        		break;
        	}
        }
    }
}

/**
 * Out of Bounds:
 * Checks whether a bullet is out of bounds, given the top-left corner of
 * its sprite.
 *
 * @param y, y-coordinate of top-left corner of bullet sprite
 * @return oob, whether the bullet is out of bounds or not
 */
bool bullet_outOfBounds(uint32_t y) {
	bool oob;
	if(y+TBT_HEIGHT < TOP_BULLET_BORDER || y+ABT_HEIGHT > BTM_BULLET_BORDER) {
		oob = true;
	} else {
		oob = false;
	}
	return oob;
}

// Detect collisions with the alien bullets
bool bullet_ABTCollision(uint32_t x, uint32_t y){
	uint32_t i, j; // Used for iteration
	uint32_t index, section, hp;
	bool	 collision; // Used to keep track if there is a collision
	// Iterate through every pixel of the bullet to check for a collision
	for(i = y; i < y+ABT_HEIGHT; i++) {
		for(j = x; j < x+ABT_WIDTH; j++) {
			uint32_t pixel_color = render_getPixelColor(j, i); // Get the pixel color
			if(pixel_color == TANK_GREEN){
				tank_hitTank(); // collision logic with tank
				return true;
			} else if(pixel_color == GREEN) {
				index = bunker_coordsToIndex(j, i); // Get index of bunker that was hit
				section = bunker_coordsToSection(j, i); // Get the section of the hit bunker
				if(index != OUT_OF_BOUNDS && section != OUT_OF_BOUNDS) {
					bunker_damageSection(index, section); // Damage the hit bunker section
					hp = bunker_getBunkerSectionHP(index, section); // Get the HP
					render_eraseBunkerSection(index, section, hp); // Erase hit section
					return true;
				} else {
					collision = false;
				}
			} else {
				collision = false;
			}
		}
	}
	return collision;
}


/**
 * Detect Tank Bullet Collision:
 * This funciton is used to detect what the alien bullet hit. The white of the 
 * bullet is different than the alien white so we can differentiate based on 
 * the color
 *
 * @param pass int the x and y of the bullet
 */
bool bullet_TBTCollision(uint32_t x, uint32_t y) {
	// variables
	bool 		collision; // Keep track of collision
	uint32_t 	i, j; // Use for iteration
	uint32_t	index; // Used for the index of the bunker
	uint32_t	section; // Used for the section of the bunker
	uint32_t 	hp; // Variable to get the HP of the bunker

	for(i = y; i < y+TBT_HEIGHT; i++) {
		for(j = x; j < x+TBT_WIDTH; j++) {
			uint32_t pixel_color = render_getPixelColor(j, i);
	
			switch(pixel_color) {
				// hit a bunker
				case GREEN:
					index = bunker_coordsToIndex(j, i); // Get the index
					section = bunker_coordsToSection(j, i); // Get the coordinates
					if(index != OUT_OF_BOUNDS && section != OUT_OF_BOUNDS) {
						bunker_damageSection(index, section); // Damage the section
						hp = bunker_getBunkerSectionHP(index, section); // Get the HP
						render_eraseBunkerSection(index, section, hp); // Erase the section
						return true;
					} else {
						collision = false;
					}
				break;
				// See if the bullet hit an alien
				case ALIEN_WHITE:
					alien_detectHitAlien(x, y);
					return true;
				break;
				// See if the bullet hit the saucer
				case RED:
					saucer_hitSaucer();
					return true;
				break;
				// no collision
				default:
					collision = false;
				break;
			}
		}
	}
	return collision;
}


/**
 * Print Active Bullets:
 * Prints bitstream corresponding to activity status of each bullet. Used for
 * debugging purposed.
 */
void printActiveBullets() {
	uint32_t i;

	xil_printf("D: Active bullets [");

	for(i = 0; i < MAX_NUM_BTS; i++) {
		xil_printf("%d", bullets[i].active);
	}

	xil_printf("]\r\n");
}
