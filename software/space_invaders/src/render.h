/*******************************************************************************
 * render.h
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
 * 28 Sep 2017 | Started file. Wrote prototype for 'drawAll' function.
 * 04 Oct 2017 | Wrote all current function prototypes.
 * 06 Oct 2017 | Finalized changes for Lab 3 passoff. Added plenty of
 *               functionality to functions already existing, especially the
 *               bullet drawing and alien moving functions. Also, might need
 *               to optimize erasing and drawing of aliens as there is
 *               currently a minor flash upon movement (especially quick ~).
 ******************************************************************************/
// Includes
#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "alien.h"
#include "bunker.h"
#include "functions.h"
#include "globals.h"
#include "graphics.h"
#include "time.h"
#include "unistd.h"
#include "xaxivdma.h"
#include "xio.h"
#include "xparameters.h"
#include "bullet.h"
#include "tank.h"
#include "saucer.h"

// Screen dimensions
#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480
#define IMG_SCALE		2
#define SCALE 2

// Saucer drawing stuff
#define SAUCER_HEIGHT 7
#define SAUCER_WIDTH 16

// Letter definitions
#define LETTER_SIZE 13
#define LETTER_L_WIDTH 10
#define LETTER_I_WIDTH 3
#define LETTER_POS_Y 40
#define LETTER_SPACING 2
#define LIVES_START_X 300
#define LIVES_TANK_START_X 400

#define TANK_BULLET_OFFSET 7 // This is the pixels over from the edge of the tank to fire the bullet
#define MAX_LIVES 3
#define LIFE_SPACING 10 // This is the space between tanks that represents lives

#define DEC                 10

// Line Definitions
#define LINE_X_BEG  5       // where the line beneath the tank begins (x)
#define LINE_X_LEN  630     // the length of the line beneath the tank (x)
#define LINE_Y_BEG  424     // where the line beneath the tank begins (y)
#define LINE_Y_LEN  2       // the length of the line beneath the tank (y)
#define TOP_BULLET_BORDER 70			// top border where bullets should erase
#define BTM_BULLET_BORDER LINE_Y_BEG

// Color Definitions
#define GREEN 0x0000FF00
#define WHITE 0x00FFFFFF
#define ALIEN_WHITE 0x00FFFFFE
#define BLACK 0x00000000
#define RED 0xA90000
#define TANK_GREEN 0x0000FE00

// Key Definitions
#define MOVE_TANK_R		'6'
#define MOVE_TANK_L		'4'
#define UPDATE_ALIENS	'8'
#define KILL_ALIEN		'2'
#define FIRE_TANK		'5'
#define FIRE_ALIEN		'3'
#define UPDATE_BULLETS	'9'
#define ERODE_BUNKER	'7'

// Number/Score definitions
#define SCORE_START_X 70
#define SCORE_NUM_START 160
#define SCORE_MAX_DIGITS 10
#define SCORE_MOD 10
#define SCORE_DIGIT_ZERO 0
#define SCORE_DIGIT_ONE 1
#define SCORE_DIGIT_TWO 2
#define SCORE_DIGIT_THREE 3
#define SCORE_DIGIT_FOUR 4
#define SCORE_DIGIT_FIVE 5
#define SCORE_DIGIT_SIX 6
#define SCORE_DIGIT_SEVEN 7
#define SCORE_DIGIT_EIGHT 8
#define SCORE_DIGIT_NINE 9

typedef enum direction_t {Left, Right, Down} direction_t;
direction_t alienDirection;

bool drawTankFlag;
bool drawAliensFlag;
bool drawBulletsFlag;

// Function Prototypes
void render_drawGameOver();
uint32_t render_getPixelColor(uint32_t yPos, uint32_t xPos);
void render_initScore();
void render_initLives();
void render_initLine();
void render_drawScore();
uint32_t render_drawDigit(uint8_t digit, uint32_t color, uint32_t xPos, uint32_t yPos);

// saucer prototype
void render_eraseSaucer();
void render_drawSaucer();
void render_drawSaucerPoints();

void render_initAliens();
void render_moveAliens();
void render_eraseAliens();
void render_erasePartialAlien(uint16_t x, uint16_t y);
void render_eraseAlien(uint16_t x, uint16_t y);
void render_explodeAlien(uint16_t x, uint16_t y, uint32_t color);
void render_drawAliens();
void render_drawAlien(uint8_t at, uint16_t x, uint16_t y);

void render_initBunkers();
void render_drawBunkers();
void render_drawBunker(uint8_t bunk_index);
void render_eraseBunkerSection(uint32_t index, uint32_t section, uint32_t hp);

void render_initTank();
void render_drawTank(uint32_t x, uint32_t y, uint32_t color);
void render_eraseLife();
void render_explodeTank(uint32_t x, uint32_t y, uint32_t color, const uint32_t bitmap[]);

void render_initAlienBullets();
//void render_fireAlien();
//void render_fireTank();
void render_drawBullet(uint32_t x, uint32_t y, uint32_t bt, uint32_t color);
void render_eraseBullet(uint32_t x, uint32_t y, uint32_t bt);

void render_drawAll();
void render_initScreen();
