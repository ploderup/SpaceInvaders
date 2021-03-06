// Includes
#include "render.h"

#define FRAME_BUFFER_0_ADDR 0xC1000000  // Starting location in DDR where we will store the images that we display.
unsigned int* framePointer = (unsigned int *) FRAME_BUFFER_0_ADDR;

// This function will return the color of the pixel in the frame buffer
// Necessary because only render has access to the frame buffer
uint32_t render_getPixelColor(uint32_t x, uint32_t y){
	return framePointer[y*SCREEN_WIDTH + x];
}

// This function will draw a color to the entire screen
// Can be used to initialize the screen to black or flash the screen
void render_drawEntireScreen(uint32_t color){
	uint32_t row,col;
	for( row=0; row<SCREEN_HEIGHT; row++) {
		for(col=0; col<SCREEN_WIDTH; col++) {
			framePointer[row*SCREEN_WIDTH + col] = color;
		}
	}
}

// Draw this when the game is over
// The goal of this right now is just to flash the screen
// At some point this will actually draw Game Over to the screen
void render_drawGameOver(){
	render_drawEntireScreen(BLACK);
	render_drawEntireScreen(WHITE);
	render_drawEntireScreen(RED);
	render_drawEntireScreen(BLACK);
	render_drawEntireScreen(WHITE);
	render_drawEntireScreen(RED);
}

// This will draw the score where the saucer was
// It allows the user to see what random score they got
void render_drawSaucerPoints(uint32_t points, uint32_t color){
	uint16_t digits[SCORE_MAX_DIGITS]; // This makes it so that the max digits in the score is 10
	point_t saucerPos = saucer_getSaucerPosition(); // Get the position of the saucer
	uint32_t xPos = saucerPos.x;
	uint8_t digit = 0; // Init the digit
	int32_t counter = 0; // Init the counter
	// We will get the digits in reverse order by modding, so we will need to
	// Put them in an array and print them in reverse order
	while(points){
		digit = points % SCORE_MOD; // Mod points by 10 to get a digit
		digits[counter] = digit; // Store the digit in the array
		points /= SCORE_MOD;
		counter++; // Incremeent counter
	}
	counter--; // Dec by 1 because it is 1 too big
	// ensure score on screen
	if(xPos < 0)
		xPos = 0;
	else if(xPos + (LETTER_SIZE+LETTER_SPACING)*SCALE*counter > SCREEN_WIDTH)
		xPos = SCREEN_WIDTH - (LETTER_SIZE+LETTER_SPACING)*SCALE*counter;
	while(counter >= 0){
		// Draw the digits in reverse order that they were put into the array
		xPos = render_drawDigit(digits[counter], color, xPos, saucerPos.y);
		counter--;
	}
}

// This function draws the saucer
// This draws the saucer in a way that part of it can be off of the screen
// The position can go off of the screen, but only the part on the screen will be drawn
void render_drawSaucer(uint32_t color){
	int32_t row,col; // used to keep track of the row and column in the bitmap
	point_t saucerPosition;
	saucerPosition = saucer_getSaucerPosition();
	for(row = saucerPosition.y; row < (saucerPosition.y + (SAUCER_HEIGHT*SCALE)); row++){ // iterate through the row
		for(col = saucerPosition.x; col < ((saucerPosition.x) + (SAUCER_WIDTH*SCALE)); col++){ // iterate through the column
			if((col >= 0) && (col < SCREEN_WIDTH)) { // Only draw the parts on the screen
				framePointer[row*SCREEN_WIDTH + col] = ((saucer_16x7[(row - saucerPosition.y)/SCALE])>>((col - saucerPosition.x)/SCALE)&1)*color;
			}
		}
	}
}

// This function is only going to erase the back part of the saucer
void render_eraseSaucer(){
	int32_t row,col; // used to keep track of the row and column in the bitmap
	point_t saucerPosition = saucer_getSaucerPosition(); // Get the saucer position
	direction_t saucerDirection = saucer_getSaucerDirection();
	for(row = saucerPosition.y; row < (saucerPosition.y + (SAUCER_HEIGHT*SCALE)); row++){
		if(saucerDirection == Right){ // If the ship is moving right, erase the left part of the saucer
			for(col = saucerPosition.x; col < (saucerPosition.x + (MOVE_SAUCER_PX)); col++){
				if((col >= 0) && (col < SCREEN_WIDTH)){ // only erase the parts on the screen
					framePointer[row*SCREEN_WIDTH + col] = ((saucer_16x7[(row - saucerPosition.y)/SCALE])>>((col - saucerPosition.x)/SCALE)&1)*BLACK;
				}
			}

		} else { // If the ship is moving left, erase the right part of the saucer
			for(col = (saucerPosition.x + (SAUCER_WIDTH*SCALE)); col >= ((saucerPosition.x + (SAUCER_WIDTH*SCALE)) - MOVE_SAUCER_PX); col--){
				if((col >= 0) && (col < SCREEN_WIDTH)){ // Only draw the parts on the screen
					framePointer[row*SCREEN_WIDTH + col] = ((saucer_16x7[(row - saucerPosition.y)/SCALE])>>((col - saucerPosition.x)/SCALE)&1)*BLACK;
				}
			}
		}
	}
}

// This function draws a letter to the screen
// Parameters: starting x position, letter height, letter width, and the bitmap to be drawn from render.h
uint32_t render_drawLetter(uint32_t xPos, uint32_t yPos, uint32_t letterHeight, uint32_t letterWidth, const uint32_t bitmap[], uint32_t color){
	uint16_t row, col; // used to keep track of the row and column in the bitmap
	for(row = yPos; row < (yPos + letterHeight); row++){
		for(col = xPos; col < (xPos + letterWidth); col++){
			framePointer[row*SCREEN_WIDTH + col] = ((bitmap[(row - yPos)]>>((letterWidth-1)-(col - xPos)))&1)*color;
		}
	}
	xPos += (LETTER_SPACING + letterWidth); // Increment the x position for drawing letters
	return xPos;
}

// This function is to initialize the score at the top of the screen
void render_initScore() {
	uint32_t xPos = SCORE_START_X;
	// First we are going to draw SCORE on the screen
	xPos = render_drawLetter(xPos, LETTER_POS_Y, LETTER_SIZE, LETTER_SIZE, letter_S_13x13, WHITE); // Draw the S
	xPos = render_drawLetter(xPos, LETTER_POS_Y, LETTER_SIZE, LETTER_SIZE, letter_C_13x13, WHITE); // Draw the C
	xPos = render_drawLetter(xPos, LETTER_POS_Y, LETTER_SIZE, LETTER_SIZE, letter_O_13x13, WHITE); // Draw the O
	xPos = render_drawLetter(xPos, LETTER_POS_Y, LETTER_SIZE, LETTER_SIZE, letter_R_13x13, WHITE); // Draw the R
	xPos = render_drawLetter(xPos, LETTER_POS_Y, LETTER_SIZE, LETTER_SIZE, letter_E_13x13, WHITE); // Draw the E

	xPos = render_drawLetter(SCORE_NUM_START, LETTER_POS_Y, LETTER_SIZE, LETTER_SIZE, number_0_13x13, GREEN); // Draw the 0

	xPos = render_drawLetter(LIVES_START_X, LETTER_POS_Y, LETTER_SIZE, LETTER_L_WIDTH, letter_L_10x13, WHITE); // Draw the L
	xPos = render_drawLetter(xPos, LETTER_POS_Y, LETTER_SIZE, LETTER_I_WIDTH, letter_I_3x13, WHITE); // Draw the I
	xPos = render_drawLetter(xPos, LETTER_POS_Y, LETTER_SIZE, LETTER_SIZE, letter_V_13x13, WHITE); // Draw the V
	xPos = render_drawLetter(xPos, LETTER_POS_Y, LETTER_SIZE, LETTER_SIZE, letter_E_13x13, WHITE); // Draw the E
	xPos = render_drawLetter(xPos, LETTER_POS_Y, LETTER_SIZE, LETTER_SIZE, letter_S_13x13, WHITE); // Draw the S
}

// This function is to initialize the lives at the top of the screen
// Draw 3 tanks for starting out at 3 lives
void render_initLives() {
	uint32_t x = LIVES_TANK_START_X;
	globals_setLives(MAX_LIVES); // initialize the lives to max lives
	uint32_t i;
	for(i = 0; i < MAX_LIVES; i++){
		render_drawTank(x, LETTER_POS_Y, GREEN); // Draw the tank
		x += ((TANK_GRAPHIC_WIDTH) + LIFE_SPACING); // Space out the tanks
	}
}

void render_eraseLife() {
	uint32_t x = LIVES_TANK_START_X + ((TANK_GRAPHIC_WIDTH) + LIFE_SPACING)*globals_getLives();
	render_drawTank(x, LETTER_POS_Y, BLACK);
}

uint32_t render_drawDigit(uint8_t digit, uint32_t color, uint32_t xPos, uint32_t yPos){
	switch(digit){
		case SCORE_DIGIT_ZERO:
			xPos = render_drawLetter(xPos, yPos, LETTER_SIZE, LETTER_SIZE, number_0_13x13, color); // Draw the number 0
		break;
		case SCORE_DIGIT_ONE:
			xPos = render_drawLetter(xPos, yPos, LETTER_SIZE, LETTER_SIZE, number_1_13x13, color); // Draw the number 1
		break;
		case SCORE_DIGIT_TWO:
			xPos = render_drawLetter(xPos, yPos, LETTER_SIZE, LETTER_SIZE, number_2_13x13, color); // Draw the number 2
		break;
		case SCORE_DIGIT_THREE:
			xPos = render_drawLetter(xPos, yPos, LETTER_SIZE, LETTER_SIZE, number_3_13x13, color); // Draw the number 3
		break;
		case SCORE_DIGIT_FOUR:
			xPos = render_drawLetter(xPos, yPos, LETTER_SIZE, LETTER_SIZE, number_4_13x13, color); // Draw the number 4
		break;
		case SCORE_DIGIT_FIVE:
			xPos = render_drawLetter(xPos, yPos, LETTER_SIZE, LETTER_SIZE, number_5_13x13, color); // Draw the number 5
		break;
		case SCORE_DIGIT_SIX:
			xPos = render_drawLetter(xPos, yPos, LETTER_SIZE, LETTER_SIZE, number_6_13x13, color); // Draw the number 6
		break;
		case SCORE_DIGIT_SEVEN:
			xPos = render_drawLetter(xPos, yPos, LETTER_SIZE, LETTER_SIZE, number_7_13x13, color); // Draw the number 7
		break;
		case SCORE_DIGIT_EIGHT:
			xPos = render_drawLetter(xPos, yPos, LETTER_SIZE, LETTER_SIZE, number_8_13x13, color); // Draw the number 8
		break;
		case SCORE_DIGIT_NINE:
			xPos = render_drawLetter(xPos, yPos, LETTER_SIZE, LETTER_SIZE, number_9_13x13, color); // Draw the number 9
		break;
		default: // We should never hit the default
			xil_printf("Invalid character in the score: %d\r\n", digit);
		break;
	}
	return xPos;
}

// Function to draw the current score of the game
// This is based off of the score in Globals
void render_drawScore(uint32_t currentScore, uint32_t color) {

	uint16_t digits[SCORE_MAX_DIGITS]; // This makes it so that the max digits in the score is 10
	//uint32_t currentScore = globals_getScore();
	uint32_t xPos = SCORE_NUM_START;
	uint8_t digit = 0;
	int32_t counter = 0;
	// Get one digit at a time from the score
	// They will get stored in reverse
	while(currentScore){
		digit = currentScore % SCORE_MOD; 
		digits[counter] = digit;
		currentScore /= SCORE_MOD;
		counter++; // Increase the counter
	}
	counter--;
	while(counter >= 0){ // Print out each digit to the screen in reverse order
		xPos = render_drawDigit(digits[counter], color, xPos, LETTER_POS_Y);
		counter--;
	}
}

/**
 * FUNCTION
 * Initialize Line
 *
 * FUNCTIONALITY
 * Draws line beneath tank.
 */
void render_initLine() {
	// variables
	uint16_t x_pos;	// x iterator
	uint16_t y_pos;	// y iterator

	// draw line
	for(y_pos = LINE_Y_BEG; y_pos < (LINE_Y_BEG+LINE_Y_LEN); y_pos++) {
		for(x_pos = LINE_X_BEG; x_pos < (LINE_X_BEG+LINE_X_LEN); x_pos++) {
			framePointer[y_pos*SCREEN_WIDTH + x_pos] = GREEN;
		}
	}
}

// Initialize the alien block at the beginning of the game/level
void render_initAliens() {
	// reset aliens
	alien_resetAliens();
	
	// draw aliens
	render_drawAliens();
	
	// set direction
	alienDirection = Right;
}

/**
 * FUNCTION
 * Erase Aliens
 * 
 * FUNCTIONALITY
 * Erases all living aliens.
 */
void render_eraseAliens() {
	// variables
	uint16_t x_init;
	uint16_t x;
	uint16_t y;
	uint8_t i;
	uint8_t j;
	// get coords of block
	x_init	= aliens.slft_x;
	y		= aliens.stop_y;
	// iterate rows
	for(i = 0; i < NUM_ALIEN_ROWS; i++) {
		// row alive?
		if(aliens.rows_alive[i]) {
			// set x_pos
			x = x_init;
			// iterate cols
			for(j = 0; j < NUM_ALIEN_COLS; j++) {
				// alien alive?
				if(aliens.aliens_alive[i*NUM_ALIEN_COLS+j]) {
					//render_eraseAlien(x, y);
					render_erasePartialAlien(x, y);
				}
				// increment x_pos
				x += (ALIEN_WID+ALIEN_SIDE_PAD);
			}
		}
		// increment y_pos
		y += (ALIEN_HIG+ALIEN_BTM_PAD);
	}
}
	
// This is to erase part of an alien so there is no flashing
// A black rectangle will be drawn the is the height of the alien
// and the width the number of pixels being moved
void render_erasePartialAlien(uint16_t x, uint16_t y){
	uint32_t i,j;
	// iterate height
	switch(alienDirection){
		case Right:
			for(i = y; i < y+ALIEN_HIG; i++) {
				// iterate width
				for(j = x-ALIEN_HRZ_SPD; j < x; j++) {
					// write to pixel
					framePointer[i*SCREEN_WIDTH + j] = BLACK;
				}
			}
		break;
		case Left:
			for(i = y; i < y+ALIEN_HIG; i++) {
				// iterate width
				for(j = x + ALIEN_WID; j < x+ALIEN_WID+ALIEN_HRZ_SPD; j++) {
					// write to pixel
					framePointer[i*SCREEN_WIDTH + j] = BLACK;
				}
			}
		break;
		case Down:
			for(i = y-ALIEN_VRT_SPD; i < y ; i++) {
				// iterate width
				for(j = x; j < x+ALIEN_WID; j++) {
					// write to pixel
					framePointer[i*SCREEN_WIDTH + j] = BLACK;
				}
			}
			break;
		default: // Hopefully we will never hit this default
			render_eraseAlien(x,y);
			xil_printf("Something went wrong in the erase partial alien switch statement");
			break;
	}
}

/**
 * FUNCTION
 * Erase Alien
 * 
 * FUNCTIONALITY
 * Erases an alien at the given coordinates.
 * 
 * PARAMETERS
 * @param x, unsigned 8-bit integer representing the x-coordinate of the upper-
 *			 left corner of the alien to be erased
 * @param y, unsigned 8-bit integer representing the y-coordinate of the upper-
 *			 left corner of the alien to be erased
 */
// Instead of drawing the alien we can just draw the explosion
void render_eraseAlien(uint16_t x, uint16_t y) {
	// variables
	uint16_t 	i;
	uint16_t 	j;
	// iterate height
	for(i = y; i < y+ALIEN_HIG; i++) {
		// iterate width
		for(j = x; j < x+ALIEN_WID; j++) {
			// write to pixel
			framePointer[i*SCREEN_WIDTH + j] = BLACK;
		}
	}
}

// This function draws the explosion to the screen
// You can pass in black or white to draw or erase the explosion
void render_explodeAlien(uint16_t x, uint16_t y, uint32_t color) {
	uint32_t row,col; // Variables to control iterations
	for(row = y; row < (y + ALIEN_EXPLOSION_HEIGHT*SCALE); row++){
		for(col = x; col < (x + ALIEN_EXPLOSION_WIDTH*SCALE); col++){
			framePointer[row*SCREEN_WIDTH + col] = ((alien_explosion_12x10[(row - y)/SCALE]>>((col - x)/SCALE))&1)*color;
		}
	}
}

/**
 * FUNCTION
 * Draw Aliens
 * 
 * FUNCTIONALITY
 * Draws all live aliens.
 */
void render_drawAliens() {
	// variables
	uint16_t x_init;
	uint16_t x;
	uint16_t y;
	uint8_t i;
	uint8_t j;
	uint8_t alien_type;
	// get coords of block
	x_init	= aliens.slft_x;
	y		= aliens.stop_y;
	// iterate rows
	for(i = 0; i < NUM_ALIEN_ROWS; i++) {
		// row alive?
		if(aliens.rows_alive[i]) {
			// get alien type
			if(i < ALIEN_TOP_TIER) {
				alien_type = ALIEN_SML;
			} else if(i < ALIEN_MID_TIER) {
				alien_type = ALIEN_MID;
			} else {
				alien_type = ALIEN_BIG;
			}
			// set x_pos
			x = x_init;
			// iterate cols
			for(j = 0; j < NUM_ALIEN_COLS; j++) {
				// alien alive?
				if(alien_isAlive(i*NUM_ALIEN_COLS+j)) {
					render_drawAlien(alien_type, x, y);
				}
				// increment x_pos
				x += (ALIEN_WID+ALIEN_SIDE_PAD);
			}
		}
		// increment y_pos
		y += (ALIEN_HIG+ALIEN_BTM_PAD);
	}
}
	

/**
 * FUNCTION
 * Draw Alien
 * 
 * FUNCTIONALITY
 * Draws an alien at the given coordinates.
 * 
 * PARAMETERS
 * @param a_type, an enumerated type (see alien.h) representing the type of
 *				  alien to be drawn (small, medium, big)
 * @param x, unsigned 8-bit integer representing the x-coordinate of the upper-
 *			 left corner of the alien to be drawn
 * @param y, unsigned 8-bit integer representing the y-coordinate of the upper-
 *			 left corner of the alien to be drawn0
 */
void render_drawAlien(uint8_t a_type, uint16_t x, uint16_t y) {
	// variables
	uint16_t 	i;
	uint16_t 	j;
	const uint32_t*	sprite;
	
	// get sprite
	switch(a_type) {
		// small invader
		case ALIEN_SML:
			if(aliens.sprite_state == ALIEN_SPRITE_IN) {
				sprite = alien_top_in_12x8;
			} else {
				sprite = alien_top_out_12x8;
			}
		break;
		
		// medium invader
		case ALIEN_MID:
			if(aliens.sprite_state == ALIEN_SPRITE_IN) {
				sprite = alien_middle_in_12x8;
			} else {
				sprite = alien_middle_out_12x8;
			}
		break;
		
		// large invader
		default:
			if(aliens.sprite_state == ALIEN_SPRITE_IN) {
				sprite = alien_bottom_in_12x8;
			} else {
				sprite = alien_bottom_out_12x8;
			}
		break;
	}
	// iterate height
	for(i = y; i < y+ALIEN_HIG; i++) {
		// iterate width
		for(j = x; j < x+ALIEN_WID; j++) {
			framePointer[i*SCREEN_WIDTH + j] = (((sprite[(i-y)/IMG_SCALE] >> (ALIEN_WID-(j-x)-1)/IMG_SCALE)) & 1)*ALIEN_WHITE;
		}
	}
}


/**
 * FUNCTION
 * Initialize Bunkers
 * 
 * FUNCTIONALITY
 * Resets HP of bunkers and draws them on the screen.
 */
void render_initBunkers() {
	// initialize bunkers
	bunker_resetBunkers();
	// draw bunkers
	render_drawBunkers();
}

/**
 * FUNCTION
 * Draw Bunkers
 * 
 * FUNCTIONALITY
 * Draws all four bunkers by iterative calls to 'drawBunkerSection()'.
 */
void render_drawBunkers() {
	// iterators
	uint8_t i;
	
	// iterate bunkers
	for(i = 0; i < NUM_BUNKS; i++) {
		render_drawBunker(i);
	}
}


/**
 * FUNCTION
 * Draw Bunker
 * 
 * FUNCTIONALITY
 * Draws a whole given bunker.
 * 
 * PARAMETERS
 * @param bunk_index, unsigned 8-bit integer representing index of bunker to be
 *					  drawn
 */
void render_drawBunker(uint8_t bunk_index) {
	// variables
	uint16_t x_init;
	uint16_t x_pos;
	uint16_t y_init;
	uint16_t y_pos;
	
	// get inits
	switch(bunk_index) {
		case BUNK_0: // bunker 0
			x_init = BUNK0_X_POS;
			y_init = BUNKS_Y_POS;
		break;
		case BUNK_1: // bunker 1
			x_init = BUNK1_X_POS;
			y_init = BUNKS_Y_POS;
		break;
		case BUNK_2: // bunker 2
			x_init = BUNK2_X_POS;
			y_init = BUNKS_Y_POS;
		break;
		default: // bunker 3
			x_init = BUNK3_X_POS;
			y_init = BUNKS_Y_POS;
		break;
	}

	// draw the bunker
	for(y_pos = y_init; y_pos < (y_init+BUNK_HIG); y_pos++) {
		for(x_pos = x_init;	x_pos < (x_init+BUNK_WID); x_pos++) {
			framePointer[y_pos*SCREEN_WIDTH + x_pos] = (((bunker_24x18[(y_pos-y_init)/IMG_SCALE] >>
				(BUNK_WID-(x_pos-x_init)-1)/IMG_SCALE)) & 1)*GREEN;
		}
	}
}


/**
 * FUNCTION
 * Erase Bunker Section
 * 
 * FUNCTIONALITY
 * Erases (damages) portions of a given bunker's section.
 * 
 * PARAMETERS
 * @param section, unsigned 8-bit integer representing section of bunker to be
 *				   damaged
 * @param bunker, Bunker structure representing bunker to be drawn
 */
void render_eraseBunkerSection(uint32_t index, uint32_t section, uint32_t hp) {
	// variables
	uint32_t	sect_pic[BUNK_SEC_HIG];	// graphic of section to be written
	uint16_t	x_init;		// initial x-position at which to set 'y_pos'
	uint16_t	x_pos;		// x-position to be written to
	uint16_t	y_init;		// initial y-position at which to set 'y_pos'
	uint16_t	y_pos;		// y-position to be written to
	uint8_t		iter;		// iterator (used in combining arrays)
	
	// which bunker?
	switch(index) {
		// bunker 0
		case 0:
			x_init = BUNK0_X_POS;
			y_init = BUNKS_Y_POS;
		break;
		// bunker 1
		case 1:
			x_init = BUNK1_X_POS;
			y_init = BUNKS_Y_POS;
		break;
		// bunker 2
		case 2:
			x_init = BUNK2_X_POS;
			y_init = BUNKS_Y_POS;
		break;
		// bunker 3
		default:
			x_init = BUNK3_X_POS;
			y_init = BUNKS_Y_POS;
		break;
	}
	
	// which section?
	switch(section) {
		// section 0
		case 0:
			// do nothing
		break;
		// section 1
		case 1:
			x_init += BUNK_SEC_WID;
		break;
		// section 2
		case 2:
			x_init += (BUNK_SEC_WID + BUNK_SEC_WID);
		break;
		// section 3
		case 3:
			x_init += (BUNK_SEC_WID + BUNK_SEC_WID + BUNK_SEC_WID);
		break;
		// section 4
		case 4:
			y_init += BUNK_SEC_HIG;
		break;
		// section 5
		case 5:
			x_init += BUNK_SEC_WID;
			y_init += BUNK_SEC_HIG;
		break;
		// section 6
		case 6:
			x_init += (BUNK_SEC_WID + BUNK_SEC_WID);
			y_init += BUNK_SEC_HIG;
		break;
		// section 7
		case 7:
			x_init += (BUNK_SEC_WID + BUNK_SEC_WID + BUNK_SEC_WID);
			y_init += BUNK_SEC_HIG;
		break;
		// section 8
		case 8:
			y_init += (BUNK_SEC_HIG + BUNK_SEC_HIG);
		break;
		// section 9
		default:
			x_init += (BUNK_SEC_WID + BUNK_SEC_WID + BUNK_SEC_WID);
			y_init += (BUNK_SEC_HIG + BUNK_SEC_HIG);
		break;
	}
	
	// bitmask corresponding to hp?
	switch(hp) {
		// HP 4
		case BUNK_MAX_HP:
			for(iter = 0; iter < BUNK_SEC_HIG; iter++) {
				sect_pic[iter] = bunkerDamage3_6x6[iter];
			}
		break;
		// HP 3
		case BUNK_ST1_HP:
			for(iter = 0; iter < BUNK_SEC_HIG; iter++) {
				sect_pic[iter] = bunkerDamage2_6x6[iter];
			}
		break;
		// HP 2
		case BUNK_ST2_HP:
			for(iter = 0; iter < BUNK_SEC_HIG; iter++) {
				sect_pic[iter] = bunkerDamage1_6x6[iter];
			}
		break;
		// HP 1
		case BUNK_ST3_HP:
			for(iter = 0; iter < BUNK_SEC_HIG; iter++) {
				sect_pic[iter] = bunkerDamage0_6x6[iter];
			}
		break;
		// HP 0
		default:
			for(iter = 0; iter < BUNK_SEC_HIG; iter++) {
				sect_pic[iter] = 0;
			}
		break;
	}
	
	// draw section
	for(y_pos = y_init; y_pos < (y_init+BUNK_SEC_HIG); y_pos++) {
		for(x_pos = x_init; x_pos < (x_init+BUNK_SEC_WID); x_pos++) {
			if(framePointer[y_pos*SCREEN_WIDTH + x_pos] == GREEN) {
				framePointer[y_pos*SCREEN_WIDTH + x_pos] = (((sect_pic[(y_pos-y_init)/IMG_SCALE] >>
					(BUNK_SEC_WID-(x_pos-x_init)-1)/IMG_SCALE)) & 1)*GREEN;
			}
		}
	}
}

/**
 * DRAW TANK:
 * This function draws the tank at the coordinates given, in some specified
 * color.
 *
 * @param x, x-coords of top-left corner of tank
 * @param y, y-coords of top-left corner of tank
 * @param color, color in which to draw the tank
 */
void render_drawTank(uint32_t x, uint32_t y, uint32_t color) {
	uint32_t row, col;
	// Draw the tank where it should be on the screen
	for(row = y; row < (y + TANK_GRAPHIC_HEIGHT); row++){
		for(col = x; col < (x + TANK_GRAPHIC_WIDTH); col++){
			framePointer[row*SCREEN_WIDTH + col] = ((tank_15x8[(row - y)/IMG_SCALE]>>(col - x)/IMG_SCALE)&1)*color;
		}
	}
}


/**
 * ERASE TANK:
 * This function erases a portion of the tank corresponding to how fast its moving.
 *
 * @param x, x-coords of top-left corner of tank
 * @param y, y-coords of top-left corner of tank
 * @param direction the tank is moving
 */
void render_eraseTank(uint32_t x, uint32_t y, direction_t direction) {
	uint32_t row, col;

	switch(direction) {
		case Left: // Erase the block on the right side if moving left
			for(row = y; row < (y + TANK_GRAPHIC_HEIGHT); row++){
				for(col = x+TANK_GRAPHIC_WIDTH; col >= x+TANK_GRAPHIC_WIDTH-TANK_MOVEMENT_WIDTH; col--){
					framePointer[row*SCREEN_WIDTH + col] = ((tank_15x8[(row - y)/IMG_SCALE]>>(col - x)/IMG_SCALE)&1)*BLACK;
				}
			}
		break;
		case Right: // Erase the block on the left if moving right
			for(row = y; row < (y + TANK_GRAPHIC_HEIGHT); row++){
				for(col = x; col < (x + TANK_MOVEMENT_WIDTH); col++){
					framePointer[row*SCREEN_WIDTH + col] = ((tank_15x8[(row - y)/IMG_SCALE]>>(col - x)/IMG_SCALE)&1)*BLACK;
				}
			}
		break;
	}
}

// Explode the tank by passing it one of the tank guises
void render_explodeTank(uint32_t x, uint32_t y, uint32_t color, const uint32_t bitmap[]){
	uint32_t row, col;
	for(row = y; row < (y + TANK_GRAPHIC_HEIGHT); row++){
		for(col = x; col < (x + TANK_GRAPHIC_WIDTH); col++){
			framePointer[row*SCREEN_WIDTH + col] = ((bitmap[(row - y)/IMG_SCALE]>>(col - x)/IMG_SCALE)&1)*color;
		}
	}
}

/**
 * Draw Bullet:
 * This function draws a bullet on the screen.
 * 
 * @param x, the x-coord at which the top-left of the bullet should be drawn
 * @param y, the y-coord at which the top-left of the bullet should be drawn
 * @param bt, the type of bullet to draw
 * @param color, the color in which to draw the bullet
 */
void render_drawBullet(uint32_t x, uint32_t y, uint32_t bt, uint32_t color) {
	uint32_t row = 0;	// row to draw on
	uint32_t col = 0;	// column to draw on
	const uint32_t* img;		// alien bullet sprite to draw

	// tank bullet?
	if(bt == tank) {
		// draw bullet
		for(row = y; row < (y+TBT_HEIGHT); row++) {
			for(col = x; col < (x+TBT_WIDTH); col++) {
				if(row > TOP_BULLET_BORDER) {
					framePointer[row*SCREEN_WIDTH + col] = color;
				}
			}
		}
	// alien bullet
	} else {
		// which sprite?
		 switch(bt) {
		 	case diag_up:
		 		img = diagUp_3x5;
		 	break;
		 	case diag_down:
		 		img = diagDown_3x5;
		 	break;
		 	case cross_up:
		 		img = crossUp_3x5;
		 	break;
		 	case cross_mid:
		 		img = crossMid_3x5;
		 	break;
		 	case cross_down:
		 		img = crossDown_3x5;
		 	break;
		 	// error case
		 	default:
		 		xil_printf("E: Invalid bullet type passed to render_drawBullet().\r\n");
		 	break;
		 }
		// draw bullet
		for(row = y; row < (y+ABT_HEIGHT); row++) {
			for(col = x; col < (x+ABT_WIDTH); col++) {
				framePointer[row*SCREEN_WIDTH + col] = ((img[(row-y)/IMG_SCALE]>>(col-x)/IMG_SCALE)&1)*color;
			}
		}
	}
}


/**
 * Erase Bullet:
 * This function checks the type of bullet given it, and then erases it on the
 * screen accordingly.
 * 
 * @param bullet, a 'bullet_t' struct
 */
void render_eraseBullet(uint32_t x, uint32_t y, uint32_t bt) {
	uint16_t row = 0; // Row to draw on
	uint16_t col = 0; // Column to draw on

	switch(bt){
		// tank bullet
		case tank:
			for(row = y; row < (y+TBT_HEIGHT); row++){
				for(col = x; col < (x+TBT_WIDTH); col++){
					if(row > TOP_BULLET_BORDER) {
						framePointer[row*SCREEN_WIDTH + col] = BLACK;
					}
				}
			}
		break;
		
		// alien bullet
		default:
			for(row = y; row < (y+ABT_HEIGHT); row++) {
				for(col = x; col < (x + ABT_WIDTH); col++) {
					if(row < BTM_BULLET_BORDER) {
						framePointer[row*SCREEN_WIDTH + col] = BLACK;
					} 
				}
			}
		break;
	}
}

// Initialize the screen for drawing
void render_initScreen() {
	int Status;       // Keep track of success/failure of system function calls.
	XAxiVdma videoDMAController;
	// There are 3 steps to initializing the vdma driver and IP.
	// Step 1: lookup the memory structure that is used to access the vdma driver.
	XAxiVdma_Config * VideoDMAConfig = XAxiVdma_LookupConfig(XPAR_AXI_VDMA_0_DEVICE_ID);
	// Step 2: Initialize the memory structure and the hardware.
	if(XST_FAILURE == XAxiVdma_CfgInitialize(&videoDMAController, VideoDMAConfig,	XPAR_AXI_VDMA_0_BASEADDR)) {
		xil_printf("VideoDMA Did not initialize.\r\n");
	}
	// Step 3: (optional) set the frame store number.
	if(XST_FAILURE ==  XAxiVdma_SetFrmStore(&videoDMAController, 2, XAXIVDMA_READ)) {
		xil_printf("Set Frame Store Failed.");
	}
	// Initialization is complete at this point.

	// Setup the frame counter.
	XAxiVdma_FrameCounter myFrameConfig;
	myFrameConfig.ReadFrameCount = 2;
	myFrameConfig.ReadDelayTimerCount = 10;
	myFrameConfig.WriteFrameCount = 2;
	myFrameConfig.WriteDelayTimerCount = 10;
	Status = XAxiVdma_SetFrameCounter(&videoDMAController, &myFrameConfig);
	if (Status != XST_SUCCESS) {
	   xil_printf("Set frame counter failed %d\r\n", Status);
	   if(Status == XST_VDMA_MISMATCH_ERROR)
		   xil_printf("DMA Mismatch Error\r\n");
	}
	// Now we tell the driver about the geometry of our frame buffer and a few other things.
	// Our image is 480 x 640.
	XAxiVdma_DmaSetup myFrameBuffer;
	myFrameBuffer.VertSizeInput = 480;    // 480 vertical pixels.
	myFrameBuffer.HoriSizeInput = 640*4;  // 640 horizontal (32-bit pixels).
	myFrameBuffer.Stride = 640*4;         // Dont' worry about the rest of the values.
	myFrameBuffer.FrameDelay = 0;
	myFrameBuffer.EnableCircularBuf = 1;
	myFrameBuffer.EnableSync = 0;
	myFrameBuffer.PointNum = 0;
	myFrameBuffer.EnableFrameCounter = 0;
	myFrameBuffer.FixedFrameStoreAddr = 0;
	if(XST_FAILURE == XAxiVdma_DmaConfig(&videoDMAController, XAXIVDMA_READ, &myFrameBuffer)) {
		xil_printf("DMA Config Failed\r\n");
	 }
	// We need to give the frame buffer pointers to the memory that it will use. This memory
	// is where you will write your video data. The vdma IP/driver then streams it to the HDMI
	// IP.
	 myFrameBuffer.FrameStoreStartAddr[0] = FRAME_BUFFER_0_ADDR;

	 if(XST_FAILURE == XAxiVdma_DmaSetBufferAddr(&videoDMAController, XAXIVDMA_READ,
						   myFrameBuffer.FrameStoreStartAddr)) {
		 xil_printf("DMA Set Address Failed Failed\r\n");
	 }
	 // Now, let's get ready to start displaying some stuff on the screen.

	 // This tells the HDMI controller the resolution of your display (there must be a better way to do this).
	 XIo_Out32(XPAR_AXI_HDMI_0_BASEADDR, 640*480);

	 // This makes all the pixels on the screen black
	 int row=0, col=0;
	 for( row=0; row<SCREEN_HEIGHT; row++) {
		 for(col=0; col<SCREEN_WIDTH; col++) {
			framePointer[row*SCREEN_WIDTH + col] = BLACK;
		 }
	 }
	 // Start the DMA for the read channel only.
	 if(XST_FAILURE == XAxiVdma_DmaStart(&videoDMAController, XAXIVDMA_READ)){
		 xil_printf("DMA START FAILED\r\n");
	 }
	 int frameIndex = 0;
	  // We have two frames, let's park on frame 0. Use frameIndex to index them.
	  // Note that you have to start the DMA process before parking on a frame.
	  if (XST_FAILURE == XAxiVdma_StartParking(&videoDMAController, frameIndex,  XAXIVDMA_READ)) {
		 xil_printf("vdma parking failed\n\r");
	  }

	 // Print each initial position for the sprites
	 render_initScore();
	 render_initLives();
	 render_initLine();
	 render_initBunkers();
	 render_initAliens();
}

