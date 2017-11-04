// Includes
#include "functions.h"

/**
 * FUNCTION
 * Seed Random Generator
 * 
 * FUNCTIONALITY
 * Seeds the random number generator. This function *must* be called once
 * before any calls to getRand().
 */
void seedRand() {
    // plant seed
    srand(0);
}


/**
 * FUNCTION
 * Get Random Number (8)
 * 
 * FUNCTIONALITY
 * Gets a random 8-bit number.
 * 
 * RETURN
 * @return rand, a random 8-bit number.
 */
uint8_t getRand8() {
    return (uint8_t) rand();
}
