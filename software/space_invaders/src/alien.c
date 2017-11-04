// Include Header
#include "alien.h"

// States for the controller state machine.
enum alien_st_t {
	init_st, // Start here, transition out of this state on the first tick.
	wait_st, // Wait here until we reach the timer to move
	move_st, // Move the aliens and then wait for the next move
	shoot_st // The aliens will shoot a random bullet in this state
} alienCurrentState = init_st;

static uint32_t moveTimer; // Timer to control when aliens should move
static int32_t shootTimer1; // Timer to control how often an alien shoots
static bool shootBullet; // Global bool to control when a bullet should be shot
static point_t explosion; // Keeps track of where an exposion is to erase later
static bool exploded; // Keeps track if there is an explosion on the screen
static uint32_t explosionTimer; // Controls how long the explosion should show.

void alienDebugStatePrint();

// Funciton to set the shoot timer
void alien_setShootTimer(){
	shootTimer1 = getRand8()%SHOOT_TIMER_MAX + SHOOT_TIMER_MIN; 
}

// Function to control how often the aliens move
// How quickly they move are based off of the ALIEN_TIMER_MAX
void alien_tickAliens(){
    switch(alienCurrentState){
        case init_st: // Initialize necessary things
            moveTimer = 0;
            explosionTimer = EXPLOXION_TIMER; // init the explosion timer. this counts down
            alien_setShootTimer(); // init the shoot timer. this counts down
            alienCurrentState = wait_st; // Go immediately into the wait state
            shootBullet = false; // init shoot bullet to false
            break;
        case wait_st: // Transition out of this state after ALIEN_TIMER_MAX
        	//Erase an explosion
			if(exploded){
				explosionTimer--; // Decrement the explosion timer
				if(explosionTimer <= 0){
					explosionTimer = EXPLOXION_TIMER; // Reset the explosion timer
					exploded = false;
					// Erase the explosion based on the coordinates
					render_explodeAlien(explosion.x, explosion.y, BLACK);
				}
			}
        	moveTimer++; // Increment the timer each tick in the wait st
            shootTimer1--; // Decrement the shoot timer
            if(shootTimer1 <= 0) {
            	alien_fireAlien(); // Shoot a bullet from an alien
            	alien_setShootTimer(); // Reset the shoot timer
            }
            if(moveTimer >= ALIEN_TIMER_MAX) {
                moveTimer = 0; // Reset the move timer
                alienCurrentState = move_st; // Move the aliens
            }
            break;

        case move_st: // Transition out of this state after one tick
        	//Erase an explosion
			if(exploded){
				explosionTimer--; // Decrement the explosion timer
				if(explosionTimer <= 0){
					explosionTimer = EXPLOXION_TIMER; // Reset the timer
					exploded = false;
					// Erase the explosion based on the coordinates
					render_explodeAlien(explosion.x, explosion.y, BLACK);
				}
			}
			sound_playFastinvaderSound(); // Play the sound for the aliens moving
        	alien_moveAliens(); // Move the aliens
            render_eraseAliens(); // Erase the old aliens before drawing the new ones to the screen
            // First move the aliens

            render_drawAliens(); // Draw the aliens on the screen
            // Check for collisions before drawing to the screen
            alienCurrentState = wait_st; // Go back to wait state to be moved again
            break;
        case shoot_st:
        	alien_fireAlien(); // Create an alien bullet
        	alienCurrentState = wait_st; // Go back to the wait st
        	break;
        default: // We should never hit this
            xil_printf("Reached an invalid state, %d, in the alien state machine.\r\n", alienCurrentState);
    }    
}

/**
 * FUNCTION
 * Reset Aliens
 * 
 * FUNCTIONALITY
 * Resets the 'aliens' structure to its initial values.
 */
void alien_resetAliens() {
    // variables
    uint8_t i;
    
    // Initialize all of the positions
    aliens.stop_y = ALIEN_START_Y;
    aliens.sbtm_y = ALIEN_START_Y + ALIEN_BLK_HIG;
    aliens.slft_x = ALIEN_START_X;
    aliens.srit_x = ALIEN_START_X + ALIEN_BLK_WID;
    aliens.dtop_y = ALIEN_START_Y;
    aliens.dbtm_y = ALIEN_START_Y + ALIEN_BLK_HIG;
    aliens.dlft_x = ALIEN_START_X;
    aliens.drit_x = ALIEN_START_X + ALIEN_BLK_WID;
    
    // set all alive
    for(i = 0; i < NUM_ALIENS; i++) {
        aliens.aliens_alive[i] = ALIEN_ALIVE;
    }
    
    // set columns alive
    for(i = 0; i < NUM_ALIEN_COLS; i++) {
        aliens.cols_alive[i] = ALIEN_ALIVE;
    }
    
    // set rows alive
    for(i = 0; i < NUM_ALIEN_ROWS; i++) {
        aliens.rows_alive[i] = ALIEN_ALIVE;
    }
    
    // set sprite state
    aliens.sprite_state = ALIEN_SPRITE_OUT;
}

// Score increments as follows:
// +10 for bottom 2 rows
// +20 for middle 2 rows
// +40 for top row
void alien_updateScore(uint8_t index){
	uint32_t score;
	//render_drawScore(score, BLACK); // First erase the old score
    if(index < ALIEN_SECOND_ROW_INDEX){
        globals_incScore(TOP_ALIEN_SCORE); // add 40 to the score
    } else if(index < ALIEN_FOURTH_ROW_INDEX){
        globals_incScore(MID_ALIEN_SCORE); // add 20 to the score
    } else {
        globals_incScore(BOT_ALIEN_SCORE); // add 10 to the score
    }
    score = globals_getScore();
    render_drawScore(score, GREEN); // Draw the new score
}

/**
 * FUNCTION
 * Kill Alien
 * 
 * FUNCTIONALITY
 * Removes an alien from the index specified. If the alien removed is the last
 * of its column and the column is the right- or left-most column, or if the
 * alien is the last of its row and the row is the top- or bottom-most row, then
 * the corresponding edge of the 'aliens' is adjusted accordingly.
 * 
 * PARAMETERS
 * @param index, an unsigned 8-bit integer representing the index of the alien
 *               to be removed; note, aliens are indexed in row-major order,
 *               from 0 (at the top left of the block) to 54 (at the bottom
 *               right)
 */
void alien_killAlien(uint8_t index) {
    // last in row?
    if(alien_lastInRow(index)) {
        // *add some code here
        
    // last in column?
    } else if(alien_lastInColumn(index)) {
        // index of column
        uint8_t col_i = index % NUM_ALIEN_COLS;
        
        // in left-most column?
        if(alien_inLeftColumn(index)) {
            aliens.dlft_x += (ALIEN_WID+ALIEN_SIDE_PAD);
            
        // in right-most column?
        } else if(alien_inRightColumn(index)) {
            aliens.drit_x -= (ALIEN_WID+ALIEN_SIDE_PAD);
        }
        
        // in middle column
        aliens.cols_alive[col_i] = ALIEN_DEAD;
    }
    
    // kill alien
    aliens.aliens_alive[index] = ALIEN_DEAD;
    // update the score now that an alien has been killed
    alien_updateScore(index);
}

// This is for the erase funciton to check if the aliens will move down
bool alien_willMoveDown(){
	if(aliens.dlft_x < ALIEN_HRZ_SPD+ALIEN_WALL_PAD && alienDirection == Left){
		return true;
	}
	if(aliens.drit_x > ((SCREEN_WIDTH-OFFSET_CORRECTLY)-ALIEN_HRZ_SPD-ALIEN_WALL_PAD) && alienDirection == Right){
		return true;
	}
	return false;
}

/**
 * FUNCTION
 * Move Aliens
 * 
 * FUNCTIONALITY
 * Moves the alien block in the given direction, unless doing so would move
 * the aliens off screen; in this case, the aliens are moved down and then in
 * the opposite direction on the next call. This function also toggles the
 * sprite state.
 */
void alien_moveAliens() {
    // toggle sprites
    if(aliens.sprite_state == ALIEN_SPRITE_IN) {
        aliens.sprite_state = ALIEN_SPRITE_OUT;
    } else {
        aliens.sprite_state = ALIEN_SPRITE_IN;
    }
    // look at direction
    switch(alienDirection) {
        case Left:
            // at screen edge?
            if(aliens.dlft_x < ALIEN_HRZ_SPD+ALIEN_WALL_PAD) {
                // move down
                alienDirection = Down;
                
                // shift the alien block down
                aliens.stop_y += ALIEN_VRT_SPD;
                aliens.dtop_y += ALIEN_VRT_SPD;
                aliens.sbtm_y += ALIEN_VRT_SPD;
                aliens.dbtm_y += ALIEN_VRT_SPD;
                
            } else {
                // shift the alien block left
                aliens.slft_x -= ALIEN_HRZ_SPD;
                aliens.dlft_x -= ALIEN_HRZ_SPD;
                aliens.srit_x -= ALIEN_HRZ_SPD;
                aliens.drit_x -= ALIEN_HRZ_SPD;
                
            }
        
        break;
        case Right:
            // at screen edge?
            if(aliens.drit_x > ((SCREEN_WIDTH-OFFSET_CORRECTLY)-ALIEN_HRZ_SPD-ALIEN_WALL_PAD)) {
                // move down
                alienDirection = Down;
                
                // shift the alien block down
                aliens.stop_y += ALIEN_VRT_SPD;
                aliens.dtop_y += ALIEN_VRT_SPD;
                aliens.sbtm_y += ALIEN_VRT_SPD;
                aliens.dbtm_y += ALIEN_VRT_SPD;
                
            } else {
                // shift the alien block right
                aliens.slft_x += ALIEN_HRZ_SPD;
                aliens.dlft_x += ALIEN_HRZ_SPD;
                aliens.srit_x += ALIEN_HRZ_SPD;
                aliens.drit_x += ALIEN_HRZ_SPD;
            }
        
        break;
        // moving down
        default:
            // on left side?
            if(aliens.dlft_x < ALIEN_HRZ_SPD+ALIEN_WALL_PAD) {
                // move right
                alienDirection = Right;
                
                // shift block right
                aliens.slft_x += ALIEN_HRZ_SPD;
                aliens.dlft_x += ALIEN_HRZ_SPD;
                aliens.srit_x += ALIEN_HRZ_SPD;
                aliens.drit_x += ALIEN_HRZ_SPD;
                
            // on right side
            } else {
                // move right
                alienDirection = Left;
                
                // shift block left
                aliens.slft_x -= ALIEN_HRZ_SPD;
                aliens.dlft_x -= ALIEN_HRZ_SPD;
                aliens.srit_x -= ALIEN_HRZ_SPD;
                aliens.drit_x -= ALIEN_HRZ_SPD;
            }
        break;
    } 
}


/**
 * FUNCTION
 * In Top Row?
 * 
 * FUNCTIONALITY
 * Checks whether an alien is in the top-most row in the block.
 * 
 * PARAMETERS
 * @param index, an unsigned 8-bit integer representing the index of the alien
 *               under question; note, aliens are indexed in row-major order,
 *               from 0 (at the top left of the block) to 54 (at the bottom
 *               right)
 * 
 * RETURN
 * @return last, boolean representing whether the alien specified is the top-most row
 *               in the block
 */

bool alien_inTopRow(uint8_t index){
    // *add some code here
}

/**
 * FUNCTION
 * In Bottom Row?
 * 
 * FUNCTIONALITY
 * Checks whether an alien is in the bottom-most row in the block.
 * 
 * PARAMETERS
 * @param index, an unsigned 8-bit integer representing the index of the alien
 *               under question; note, aliens are indexed in row-major order,
 *               from 0 (at the top left of the block) to 54 (at the bottom
 *               right)
 * 
 * RETURN
 * @return last, boolean representing whether the alien specified is the bottom-most
 *               row in the block
 */
bool alien_inBottomRow(uint8_t index){
    // *add some code here
}


/**
 * FUNCTION
 * In Left-Most Column?
 * 
 * FUNCTIONALITY
 * Checks whether an alien is in the left-most column in the block.
 * 
 * PARAMETERS
 * @param index, an unsigned 8-bit integer representing the index of the alien
 *               under question; note, aliens are indexed in row-major order,
 *               from 0 (at the top left of the block) to 54 (at the bottom
 *               right)
 * 
 * RETURN
 * @return last, boolean representing whether the alien specified is the left-most
 *               column in the block
 */
bool alien_inLeftColumn(uint8_t index){
    // variables
    uint8_t lm_col; // index of left-most column
    bool    inlmc;  // in left-most column?
    uint8_t i;  // iterator
    
    // assume !inlmc
    inlmc = false;
    
    // get 'lm_col'
    for(i = 0; i < NUM_ALIEN_COLS; i++) {
        if(aliens.cols_alive[i]) {
            lm_col = i;
            break;
        }
    }
    
    // alien in 'lm_col'?
    for(i = 0; i < NUM_ALIEN_ROWS; i++) {
        // temp index
        uint8_t temp = lm_col + i*NUM_ALIEN_COLS;
        
        if(temp == index) {
            inlmc = true;
            break;
        }
    }
    
    // complete
    return inlmc;
}


/**
 * FUNCTION
 * In Right-Most Column?
 * 
 * FUNCTIONALITY
 * Checks whether an alien is in the right-most column in the block.
 * 
 * PARAMETERS
 * @param index, an unsigned 8-bit integer representing the index of the alien
 *               under question; note, aliens are indexed in row-major order,
 *               from 0 (at the top left of the block) to 54 (at the bottom
 *               right)
 * 
 * RETURN
 * @return last, boolean representing whether the alien specified is the right-most
 *               column in the block
 */
bool alien_inRightColumn(uint8_t index){
    // variables
    uint8_t rm_col;     // index of right-most column
    bool    inrmc;      // in right-most column?
    uint8_t i;          // iterator
    
    // assume !inrmc
    inrmc = false;
    
    // get 'rm_col'
    for(i = NUM_ALIEN_COLS; i > 0; i--) {
        if(aliens.cols_alive[i-OFFSET_CORRECTLY]) {
            rm_col = (i-OFFSET_CORRECTLY);
            break;
        }
    }
    
    // alien in 'rm_col'?
    for(i = 0; i < NUM_ALIEN_ROWS; i++) {
        // temp index
        uint8_t temp = rm_col + i*NUM_ALIEN_COLS;
        
        if(temp == index) {
            inrmc = true;
            break;
        }
    }
    
    // complete
    return inrmc;
}


/**
 * FUNCTION
 * Last in Row?
 * 
 * FUNCTIONALITY
 * Checks whether an alien is the last is its row.
 * 
 * PARAMETERS
 * @param index, an unsigned 8-bit integer representing the index of the alien
 *               under question; note, aliens are indexed in row-major order,
 *               from 0 (at the top left of the block) to 54 (at the bottom
 *               right)
 * 
 * RETURN
 * @last, boolean representing whether the alien specified is the last in its
 *        row
 */
bool alien_lastInRow(uint8_t index) {
    // variables
    uint8_t row;    // [0-4]
    bool    last;   // last?
    uint8_t i;      // iterator
    
    // get row number
    row = index / NUM_ALIEN_COLS; // 2
    
    // assume last
    last = true;
    
    // iterate through cols
    for(i = 0; i < NUM_ALIEN_COLS; i++) {
        // temp index
        uint8_t temp = row*NUM_ALIEN_COLS + i;
        
        // same as index?
        if(temp != index) {
            if(alien_isAlive(temp)) {
                last = false;
                break;
            }
        }
    }
    
    // complete
    return last;
}


/**
 * FUNCTION
 * Last in Column?
 * 
 * FUNCTIONALITY
 * Checks whether an alien is the last remaining in its column.
 * 
 * PARAMETERS
 * @param index, an unsigned 8-bit integer representing the index of the alien
 *               under query; note, aliens are indexed in row-major order, from
 *               0 (at the top left of the block) to 54 (at the bottom right)
 * 
 * RETURN
 * @last, boolean representing all other aliens in the column specified by
 *        'index' are dead
 */
bool alien_lastInColumn(uint8_t index) {
    // variables
    uint8_t col;    // [0-10]
    bool    last;   // last?
    uint8_t i;      // iterator
    
    // get column number
    col = index % NUM_ALIEN_COLS;
    
    // assume last
    last = true;
    
    // iterate through cols
    for(i = 0; i < NUM_ALIEN_ROWS; i++) {
        // temp index
        uint8_t temp = col + i*NUM_ALIEN_COLS;

        // same as 'index'
        if(temp != index) {
            if(alien_isAlive(temp)) {
                last = false;
                break;
            }
        }
    }

    // complete
    return last;
}

// This function returns the point for an alien based on the index you pass in
point_t alien_getCoordinates(uint16_t index){
    point_t alien_xy; // This is a point to hold the xy of the alien
    //alienNum now contains the alien index we want to shoot from
    uint16_t col = index % NUM_ALIEN_COLS;
    // This gives the correct row because of integer division
    uint16_t row = index / NUM_ALIEN_COLS;
    int32_t x, y; // Keep track of positions
    int32_t i; // variable for iteration

    x = aliens.slft_x;
    y = aliens.stop_y;
    // Iterate through the alien block columns
    for(i = 0; i < col; i++) {
    	x += ALIEN_WID + ALIEN_SIDE_PAD;
    }
    // Iterate thorugh the alien block rows
    for(i = 0; i < row; i++) {
    	y += ALIEN_HIG + ALIEN_BTM_PAD;
    }
    alien_xy.x = x; // Set the x position
    alien_xy.y = y; // Set the y position
    
    return alien_xy; // Return the coordinates found
}

/**
 * FUNCTION
 * Get Bottom Alien
 * 
 * FUNCTIONALITY
 * Returns the location of a random alien from each alien located at the bottom
 * of their respective columns.
 * 
 * @return point, a point structure representing the x and y coordinates of
 *               an alien which exists at the bottom of its column.
 */
point_t alien_getBottomAlien() {
    // variables
    point_t alien_xy;
    //point_t aliens_xy[NUM_ALIEN_COLS];
    uint16_t alienNum;
    
    bool foundAlien = false;
    uint16_t randCol;
    while(!foundAlien){
        // returns a random number between 0 and 10
        randCol = getRand8()%NUM_ALIEN_COLS;
        if(aliens.cols_alive[randCol]){
            foundAlien = true; // See if there is an alien in the column
        }
    }
    alienNum = NUM_ALIENS - (NUM_ALIEN_COLS - randCol); // Get the index of a bottom alien
    // Iterate through the column until we find the alien still in the column
    uint16_t i;
    for(i = 0; i < NUM_ALIEN_ROWS; i++){
        if(aliens.aliens_alive[alienNum]){
            break;
        }
        alienNum -= NUM_ALIEN_COLS; // Decrement the index to the next in the col
    }
    alien_xy = alien_getCoordinates(alienNum); // Get the coordinates based off the index

    // complete
    return alien_xy;
}

// Find out if a point is in an alien
// This is used for collisions for bullets, but could possibly be used with other things
bool alien_pointInAlien(uint32_t xPos, uint32_t yPos, point_t alienPoint){
    // Check to see if the point is within the alien
    if((alienPoint.y-ALIEN_BTM_PAD/SCALE < yPos) && ((alienPoint.y + ALIEN_HIG+ALIEN_BTM_PAD/SCALE) > yPos)){
        if((alienPoint.x-ALIEN_SIDE_PAD/2 < xPos) && ((alienPoint.x + ALIEN_WID+ALIEN_SIDE_PAD/2) > xPos)){
            return true;
        }
    }
    return false;
}

// This function takes in an x and y value and figures out which alien it belongs to
bool alien_detectHitAlien(uint32_t xPos, uint32_t yPos){
    int32_t i;
    // We are going to iterate from the top down because aliens on the bottom row
    // will be getting hit before the higher rows (most likely). This will decrease
    // the time we have to stay in the loop
    for(i = NUM_ALIENS - OFFSET_CORRECTLY; i >= 0; i--){
        point_t alienPoint = alien_getCoordinates(i); // Get the coordinates of the alien
        if(aliens.aliens_alive[i]){
            if(alien_pointInAlien(xPos, yPos, alienPoint)){
                // If we've made it hear then we've hit an alien
                render_explodeAlien(alienPoint.x, alienPoint.y, WHITE);
                sound_playInvaderkilledSound();
                alien_killAlien(i); // Kill the hit alien
                explosion.x = alienPoint.x;
                explosion.y = alienPoint.y;
                exploded = true;
                //render_explodeAlien(alienPoint.x, alienPoint.y, BLACK);
                return true; // Return because we found the alien
            }
        }
    }
    return false;
}

/**
 * This randomly picks an alien in the bottom row and fires one of types of alien 
 * bullets from them. We will get the figure out where to initially show the bullet
 * based of off the alien position that we can get from alien.c
*/
void alien_fireAlien() {
    // Get all of the bullets
	bullet_t alienBullet1, alienBullet2, alienBullet3, alienBullet4;
    alienBullet1 = bullet_getBullet(ABT0_I);
    alienBullet2 = bullet_getBullet(ABT1_I);
    alienBullet3 = bullet_getBullet(ABT2_I);
    alienBullet4 = bullet_getBullet(ABT3_I);
    
	// Get random alien coordinates from Alien class/struct
	point_t alienPos = alien_getBottomAlien();

	alienPos.x += HALF_ALIEN_WIDTH; // this will be changed
	// Calculate where the bullet is going to be
	uint32_t x = alienPos.x + ALIEN_BULLET_OFFSET_X; // Add the x offset to put the bullet in the middle of the alien
	uint32_t y = alienPos.y + ALIEN_HIG; // Add the height of the alien to put the bullet at the bottom

	// Find an alien bullet that isn't currently active
	// If all bullets are active, then don't draw a new bullet
	if(!alienBullet1.active){ // If the alien bullet1 is active we'll draw it
		bullet_setActive(ABT0_I, true);
		bullet_setCoords(ABT0_I, x, y);
		render_drawBullet(x, y, alienBullet1.btype, WHITE); // Draw bullet to screen
	} else {
		if(!alienBullet2.active){ // If the alien bullet2 is active we'll draw it
			bullet_setActive(ABT1_I, true);
			bullet_setCoords(ABT1_I, x, y);
			render_drawBullet(x, y, alienBullet2.btype, WHITE);
		} else {
			if(!alienBullet3.active){ // If the alien bullet3 is active we'll draw it
				bullet_setActive(ABT2_I, true);
				bullet_setCoords(ABT2_I, x, y);
				render_drawBullet(x, y, alienBullet3.btype, WHITE);
			} else {
				if(!alienBullet4.active){ // If the alien bullet4 is active we'll draw it
					bullet_setActive(ABT3_I, true);
					bullet_setCoords(ABT3_I, x, y);
					render_drawBullet(x, y, alienBullet4.btype, WHITE);
				}
			}
		}
	}
}

/**
 * FUNCTION
 * Alien Alive?
 * 
 * FUNCTIONALITY
 * Returns whether or not the alien in question is still alive.
 * 
 * PARAMETERS
 * @param index, an unsigned 8-bit integer representing the index of the alien
 *               under query; note, aliens are indexed in row-major order, from
 *               0 (at the top left of the block) to 54 (at the bottom right)
 * 
 * RETURN
 * @param alive, a boolean representing whether the alien is alive or not
 */
bool alien_isAlive(uint8_t index) {
    // return alien status
    return aliens.aliens_alive[index];
}

// This is a debug state print routine. It will print the names of the states each
// time tick() is called. It only prints states if they are different than the
// previous state.
void alienDebugStatePrint() {
  static enum alien_st_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != alienCurrentState - this prevents reprinting the same state name over and over.
  if (previousState != alienCurrentState || firstPass) {
    firstPass = false;                // previousState will be defined, firstPass is false.
    previousState = alienCurrentState;     // keep track of the last state that you were in.
    //printf("msCounter:%d\n\r", msCounter);
    switch(alienCurrentState) {            // This prints messages based upon the state that you were in.
      case init_st:
        printf("init_st\n\r");
        break;
      case wait_st:
        printf("wait_st\n\r");
        break;
      case move_st:
        printf("move_st\n\r");
        break;
     }
  }
}

