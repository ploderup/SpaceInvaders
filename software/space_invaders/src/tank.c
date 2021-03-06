// Header
#include "tank.h"

// Definitions
#define WALL_BUFFER     5 // Buffer for the wall so the tank doesn't go off screen
#define COUNTER_MAX     2 // Max the counter
#define X_INIT          95 // Tank x position on the screen
#define Y_INIT          385 // Tank y position on the screen
#define EXPLOSION_TIME  50 // Timer for the explosions to happen

// Controller Mask
#define CNTLR_LEFT_MASK  4
#define CNTLR_RIGHT_MASK 8

#define CNTLR_BTN_MASK	 16

// Global Vars
uint32_t counter;
static int32_t	x_pos; // Global for x position of the tank
static int32_t	y_pos; // Global for the y position of the tank
static int32_t explosionTimer; // Global for the explosion of the tank
bool sound_flag;				// for playing explosion sound just once

// Accessors
// This is to set the tank position
void tank_setXPos(uint32_t x) { x_pos = x; }

// This is to get the tank x position
uint32_t tank_getXPos() { return x_pos; }

// Use this to set the tank y position
void tank_setYPos(uint32_t y) { y_pos = y; }

// Use this to get the tank y position
uint32_t tank_getYPos() { return y_pos; }

// Enums
// Use this to keep track of the various states of the tank
typedef enum {INIT, IDLE, MOVE, FIRE, EXPLODE, ALT_EXPLODE} state_type;
state_type curr_state = INIT; // Initialize the tank
/**
 * Finite-State Machine
 */
void tank_FSM() {
    // check state
    switch(curr_state) {
        // init state
        case INIT:
            // reset counter
            counter = 0;
            explosionTimer = EXPLOSION_TIME;
            // reset position
            x_pos = X_INIT;
            y_pos = Y_INIT;
            // draw tank
            render_drawTank(x_pos, y_pos, TANK_GREEN);
            sound_flag = false;
            // move to idle
            curr_state = IDLE;
        break;
        // idle state
        case IDLE:
            counter++;
            // move tank?
            if(counter >= COUNTER_MAX) {
                curr_state = MOVE;
                counter = 0;
            }
        break;
        // move state
        case MOVE:
            // ~(both L and R pressed)?
            if(!((curr_cntlr & CNTLR_LEFT_MASK) && (curr_cntlr & CNTLR_RIGHT_MASK))) {
                // check if the left button was pushed
                if(curr_cntlr & CNTLR_LEFT_MASK) {
                    if(!tank_tooCloseToEdge(LEFT)) { // Check if too close to edge
                        render_drawTank(x_pos, y_pos, BLACK); // erase
                        tank_moveTank(LEFT);
                        render_drawTank(x_pos, y_pos, TANK_GREEN); // draw
                    }
                }
                // // check if the right button was pushed
                if(curr_cntlr & CNTLR_RIGHT_MASK) {
                    if(!tank_tooCloseToEdge(RIGHT)) { // Check if too close to edge
                        render_drawTank(x_pos, y_pos, BLACK); // erase
                        tank_moveTank(RIGHT);
                        render_drawTank(x_pos, y_pos, TANK_GREEN); // draw
                    }
                }
            }
            // fire tank?
            if(curr_cntlr & CNTLR_BTN_MASK) {
                curr_state = FIRE;
            // return to idle
            } else {
                curr_state = IDLE;
            }
        break;
        
        // fire state
        case FIRE:
        	// fire if not active
        	if(!bullet_getBullet(TBT0_I).active) {
				// activate tank bullet
				bullet_setActive(TBT0_I, true);
				bullet_setCoords(TBT0_I, x_pos+TBT_X_OFST, y_pos+TBT_Y_OFST);

				// draw tank bullet
				render_drawBullet(bullet_getBullet(TBT0_I).x, bullet_getBullet(TBT0_I).y, bullet_getBullet(TBT0_I).btype, WHITE);
        	    sound_playShootSound(); // Play the bullet sound
        	}
        	curr_state = IDLE; // Go to the idle state
		break;
		// The next two states are for the tank explosion
		// There are two bitmaps to alternate between for the explosion
		// This state if for the tank explosion
        case EXPLODE:
        	if(!sound_flag) {
        		sound_playExplosionSound();
        		sound_flag = true;
        	}

        	if(explosionTimer > 0){
        		explosionTimer--; // dec the timer
        		render_explodeTank(x_pos, y_pos, TANK_GREEN, tank_explosion1_15x8); // draw the explosion
        		curr_state = ALT_EXPLODE;
        	} else {
        		render_explodeTank(x_pos, y_pos, BLACK, tank_explosion2_15x8); // Erase the explosion
        		curr_state = INIT; // If the timer expired reset the tank position
        	}
        	break;
        case ALT_EXPLODE:
        	if(explosionTimer > 0){
        		explosionTimer--;
        		render_explodeTank(x_pos, y_pos, TANK_GREEN, tank_explosion2_15x8); // Check if too close to edge
        		curr_state = EXPLODE;
        	} else {
        		render_explodeTank(x_pos, y_pos, BLACK, tank_explosion1_15x8); // Erase the explosion
        		curr_state = INIT; // If the timer expired reset the tank position
        	}
        	break;
        // error state
        default:
            xil_printf("E: Default state in Tank FSM reached.\n");
        break;
    }
}


/**
 * Too Close to Edge
 * 
 * Checks whether the tank is to close to the edge of the screen.
 * 
 * @param d, the direction the tank is currently moving
 * @return too_close, whether the tank is too close to the edge of the screen to
 * move any further in its current direction, d
 */
bool tank_tooCloseToEdge(direction d) {
    // declarations
    bool output;
    
    // moving left?
    if(d == LEFT) {
        // too close?
        if(x_pos-TANK_MOVEMENT_WIDTH-WALL_BUFFER < 0) {
            output = true;
        } else {
            output = false;
        }
    // moving right
    } else {
        // too close?
        if(x_pos+TANK_MOVEMENT_WIDTH+TANK_GRAPHIC_WIDTH+WALL_BUFFER > SCREEN_WIDTH) {
            output = true;
        } else {
            output = false;
        }
    }
    return output;
}

/**
 * Move Tank
 * Given a direction, this function updates the x-coordinates of the tank
 * accordingly.
 * 
 * @param d, direction in which the tank is currently moving
 */
void tank_moveTank(direction d) {
    switch(d) {
        case LEFT:
            x_pos -= TANK_MOVEMENT_WIDTH;
        break;
        case RIGHT:
            x_pos += TANK_MOVEMENT_WIDTH;
        break;
    }
}

// This function is called when the tank is hit
void tank_hitTank(){
	globals_decLives(); // Decrement the number of lives
	uint32_t lives = globals_getLives(); // Get the number of lives
	if(lives == 0){ // If the lives are 0 then we need to end the game
		globals_setGameOver(true); // Set game over to true so main knows
	}
	render_eraseLife(); // Erase the life
	curr_state = EXPLODE; // Show the explosion on the screen

}

