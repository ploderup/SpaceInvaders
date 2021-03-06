#include "saucer.h"

#define SAUCER_WIDTH 16 // Width of the saucer image
#define SAUCER_HEIGHT 7 // Height of the saucer image
#define SHIP_TIMER_MAX 6 // Timer for the ship moving
#define SAUCER_STARTING_Y 75 // Ship starting 7 position
#define MIN_SAUCER_WAIT	1500	// 15 seconds

// States for the controller state machine.
enum saucer_st_t {
	init_st, // Start here, transition out of this state on the first tick.
	dead_st, // The saucer will stay here until it is marked as alive
	wait_st, // Wait here until we reach the timer to move
	move_st, // Move the aliens and then wait for the next move
	score_st // Erase the score from the screen
} saucerCurrentState = init_st;

static int32_t appearTimer; // Timer for the saucer to appear on the screen
static uint32_t moveTimer; // Timer for the saucer to move
static uint32_t eraseTimer; // Timer to erase the points of the saucer
static bool saucerAlive; // Bool to keep track of the saucer being alive
bool saucer_getSaucerAlive() { return saucerAlive; }
// Globals to be accessed by other files
static point_t saucerPosition; // Variable to keep track of upper leftmost corner of the block
static direction_t saucerDirection; // Keep track of the saucer direction
static uint32_t bonusPoints; // Variable for the bonus points

// Initialize the saucer off the screen
void saucer_initSaucer(){
	uint8_t randNum;
	// returns a random number either 0 or 1
	randNum = getRand8()% NUM_DIRECTIONS;
	if(randNum == TRUE) {
		saucerPosition.x = -(SAUCER_WIDTH); // Have the saucer start off the screen left
		saucerPosition.y = SAUCER_STARTING_Y;
		saucerDirection = Right; // Initialize the saucer to go right
	} else {
		saucerPosition.x = SCREEN_WIDTH+SAUCER_WIDTH; // Saucer start off the screen right
		saucerPosition.y = SAUCER_STARTING_Y;
		saucerDirection = Left; // initialize the saucer to go left
	}
}

// Function to control how often the aliens move
// How quickly they move are based off of the ALIEN_TIMER_MAX
void saucer_tickSaucer(){
	//saucerDebugStatePrint();
    switch(saucerCurrentState){
        case init_st:
        	saucer_initSaucer(); // initialize the saucer
        	moveTimer = 0; // Init the timer
        	bonusPoints = 0; // Init the timer
        	eraseTimer = ERASE_TIMER_MAX; // init the erase timer. It will count down
        	appearTimer = getRand8()%(MAX_SAUCER_WAIT-MIN_SAUCER_WAIT) + MIN_SAUCER_WAIT;
        	saucerAlive = false;
        	saucerCurrentState = dead_st; // Go immediately into the wait state
        break;
        
        case dead_st:
        	appearTimer--;
        	if(appearTimer <= 0) { // Show the saucer now that the appear timer is 0
        		saucerAlive = true;
        		saucerCurrentState = wait_st;
        	}
        break;
        
        case wait_st: // Transition out of this state after ALIEN_TIMER_MAX
        	if(!saucerAlive){ // If the saucer is dead go to the score st
        		saucerCurrentState = score_st;
        	}
            moveTimer++; // Increment the timer each tick in the wait st
            if(moveTimer >= SHIP_TIMER_MAX){
                moveTimer = 0; // Reset the move timer
                saucerCurrentState = move_st;
            }
        break;
        
        case move_st: // Transition out of this state after one tick
        	if(!saucerAlive){ // If the saucer is dead go to the score st
				    saucerCurrentState = score_st;
		    	} else {
			    	render_eraseSaucer();
	        		saucer_moveSaucer();
	        		render_drawSaucer(RED); // Draw the saucer to the screen
	        		sound_playUfoSound(); // Play the sound for the saucer
        	}
          // Check for collisions before drawing to the screen
          saucerCurrentState = wait_st; // Go back to wait state to be moved again
        break;
        
        case score_st:
        	if(eraseTimer <= 0){ // Once erase timer expires erase the score
        		render_drawSaucerPoints(bonusPoints, BLACK);
        		saucerCurrentState = init_st; // Go back to init the saucer after erasing score
        	} else {
        		eraseTimer--; // Dec the timer
        	}
        break;
        
        default:
            xil_printf("Reached an invalid state, %d, in the saucer state machine.\r\n", saucerCurrentState);
        break;
    }
}

// Move the saucer left of right
void saucer_moveSaucer(){
    switch(saucerDirection){
      case Left: // Check to see if direction is left
        saucerPosition.x -= MOVE_SAUCER_PX;
        if((saucerPosition.x + SAUCER_WIDTH*SCALE) < 0){
        	saucerAlive = false; // Saucer is dead once it is off the screen
        	sound_resetSaucer();
		}
      break;
      case Right: // Check to see if direction is right
        saucerPosition.x += MOVE_SAUCER_PX;
        if(saucerPosition.x > SCREEN_WIDTH){
        	saucerAlive = false; // Saucer is dead once it is off the screen
        	sound_resetSaucer();
        }
      break;
      default:
          xil_printf("The ship is not trying to move left or right");
    }
}

// Set the saucer position
void saucer_setSaucerPosition(point_t point) {
  saucerPosition = point;
}

// Get the saucer position
point_t saucer_getSaucerPosition() {
  return saucerPosition;
}

// Get the saucer direction
direction_t saucer_getSaucerDirection(){
	 return saucerDirection;
}

// This function returns the random point value for the saucer
void saucer_updateScore(){
  uint16_t multiplier;
  // returns a random number between 1 and 3
  multiplier = getRand8()%POINT_MULTIPLIER + TRUE;
  bonusPoints = multiplier * SAUCER_POINTS; // Multiply 50 by random number
  globals_incScore(bonusPoints); // Increment the global score
  uint32_t score = globals_getScore(); // Get the score to draw
  render_drawScore(score, GREEN); // Draw the score
}

// This funciton gets called when the saucer gets hit
void saucer_hitSaucer(){
	sound_playUFOExplosion();
	saucer_updateScore(); // update the score
	render_drawSaucer(BLACK); // Erase the saucer(draw it BLACK)
	render_drawSaucerPoints(bonusPoints, WHITE); // Draw the saucer points on the screen
	saucerAlive = false; // Flag that the saucer is dead
}

// This is a debug state print routine. It will print the names of the states each
// time tick() is called. It only prints states if they are different than the
// previous state.
void saucerDebugStatePrint() {
  static enum saucer_st_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != saucerCurrentState - this prevents reprinting the same state name over and over.
  if (previousState != saucerCurrentState || firstPass) {
    firstPass = false;                // previousState will be defined, firstPass is false.
    previousState = saucerCurrentState;     // keep track of the last state that you were in.
    //printf("msCounter:%d\n\r", msCounter);
    switch(saucerCurrentState) {            // This prints messages based upon the state that you were in.
      case init_st:
        printf("init_st\n\r");
        break;
      case wait_st:
        printf("wait_st\n\r");
        break;
      case move_st:
        printf("move_st\n\r");
        break;

      default:
      break;
    }
  }
}
