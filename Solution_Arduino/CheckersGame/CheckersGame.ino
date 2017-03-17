/**
 * This file contains all the code needed to run the Checkers game on
 * either a PC using an ASCII display, or on an arduino board using an
 * LED matrix display.
 *
 * Uncomment the line:
 *       #define arduino
 * immediately following this comment block to run as a sketch on an arduino.
 * Otherwise, this code is set to run as a stand alone C program with ASCII display on the PC.
 */

#define arduino


#ifndef arduino
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
// For ASCII display - 1 to show move-by-move, 0 to show only game summary
int verbose = 0;
#else
#include "Arduino.h"
// Define the pin output leads for arduino board.
// int powerPin = 5V   		// Red, 	+5V to the power bus
// int groundPin = Gnd 		// White,	Gnd to the ground bus
const int dataPin = 8;// Yellow,	Pin 14 on the 75HC595 #1, Data pin
const int clockPin = 9;// Green,	Pin 11 on the 75HC595 #1, Clock pin
const int latchPin = 10;// Blue,	Pin 12 on the 75HC595 #1, Latch pin
const int player1Pin = 12;// Tan,		Red LED anode
const int player2Pin = 13;// Brown,	Yellow LED anode
// 75HC595 #1 - Green Matrix LEDs
// 75HC595 #2 - Red Matrix LEDs
// 75HC595 #3 - LED rows

// Toggle for LED to blink to indicate a king
int blinkOn;
// Game speed - 1 is fastest, 10 is about 1 move/second
const int speed = 1;
#endif

// The randomization seed for the game
int seed = 1;
// The maximum number of moves in a game before calling it a draw
const int MaxMoves = 150;

// Representation of the board and checkers
// Each unsigned char represents a row on the board.
unsigned char checkers[8];
unsigned char colors[8];
unsigned char kings[8];

// Active player, 1 for X or Red, 0 for O or Green
int player;
// The winner of the game (0 for O/Green, 1 for X/Red, 2 for tie and -1 for no winner yet)
int winner;
// The number of moves taken in the current game
int numMoves;

// Declare the elements that define a move
typedef struct {
	int row;
	int col;
	int diag;
	int dir;
	int score;
} Move_t;

// declare the interfaces to be implemented by students.
int autoTurn();
int autoMakeMove(Move_t move, int attemptOnly);
int autoMoveChecker(Move_t move, int moveToRow, int moveToCol, int attemptOnly);
int autoJumpChecker(Move_t move, int moveToRow, int moveToCol, int score,
		int attemptOnly);

/******************************************************************/
/*********************** Basic Methods ****************************/
/***********************    Core.c     ****************************/
/******************************************************************/

/**
 * Add two numbers together
 */
int add(int x, int y) {
	int a, b;
	do {
		a = x & y;
		b = x ^ y;
		x = a << 1;
		y = b;
	} while (a);
	return b;
}

/**
 * Negate a number.
 */
int negate(int x) {
	return add(~x, 1);
}

/**
 * Subtract one number from another.
 */
int subtract(int x, int y) {
	return add(x, negate(y));
}

#ifndef arduino
/**
 * Multiply one number by another
 */
int multiply(int x, int y) {
	int m = 1, z = 0;
	if (x < 0) {
		x = negate(x);
		y = negate(y);
	}
	do {
		if (x & m) {
			z = add(y, z);
		}
		y <<= 1;
		m <<= 1;
	} while (x >= m && y);
	return z;
}

/**
 * Divide one number by another.
 * The integer value is returned and the remainder is ignored.
 */
int divide(int x, int y) {
	int c = 0, sign = 0;
	if (x < 0) {
		x = negate(x);
		sign ^= 1;
	}
	if (y < 0) {
		y = negate(y);
		sign ^= 1;
	}
	if (y != 0) {
		while (x >= y) {
			x = subtract(x, y);
			++c;
		}
	}
	if (sign) {
		c = negate(c);
	}
	return c;
}
#endif

/**
 *  if x>y, then return 1, else return 0
 */
unsigned int isGreaterThan(int x, int y) {
	int byteLength = sizeof(int);
	int bitLength = byteLength * 8;
	return add(y, add(~x, 1)) >> subtract(bitLength, 1) & 1;
}

/**
 *  if x<y, then return 1, else return 0
 */
int isLessThan(int x, int y) {
	return !isGreaterThan(x, y);
}

/**
 * if x is even, return non-zero, else return 0
 */
int isEven(int x) {
	return !(x & 1);
}

/******************************************************************/
/********************** Bit Helper Methods ************************/
/***********************    Core.c     ****************************/
/******************************************************************/
/**
 * Set the specified bit in the given byte.
 */
int setBit(int byte, int bit) {
	return byte | (1 << bit);
}

/**
 * Clear the specified bit in the given byte.
 */
int clearBit(int byte, int bit) {
	return byte & ~(1 << bit);
}

/**
 * Is the specified bit in the given byte set.
 */
int isBit(int byte, int bit) {
	return (byte & (1 << bit)) > 0;
}

/******************************************************************/
/********************* Validation Methods *************************/
/*********************     Checkers.c     *************************/
/******************************************************************/

/**
 * Determine if a square is valid to play on.
 * Verify that the column and row are both on the board
 * and that the square is a playable one.
 */
int squareIsValid(int row, int col) {
	// verify a valid row on board
	// if row is <0 or >8 or col is <0 or > 8
	// or if row+column is odd, square is not playable
	return !(((row < 1) | (row > 8)) || ((col < 1) | (col > 8))
			|| !isEven(add(row, col)));
}

/**
 * Determine if a square has a checker on it.
 */
int squareHasChecker(int row, int col) {
	return isBit(checkers[subtract(row, 1)], subtract(col, 1));	// bit for col is set
}

/**
 * Determine if a square has a checker for the specified player on it.
 */
int squareHasPlayerChecker(int row, int col, int player) {
	return squareHasChecker(row, col)
			&& !((isBit(colors[subtract(row, 1)], subtract(col, 1)) & 1)
					^ player);	// bit for color is set
}

/**
 * Determine if the checker a the given row/column has been crowned a king.
 */
int squareHasPlayerKing(int row, int col, int player) {
	return squareHasPlayerChecker(row, col, player)
			&& isBit(kings[subtract(row, 1)], subtract(col, 1));
}

/**
 * Does the player's opponent have any checkers remaining?
 */
int boardHasOpponentCheckers(int playerIsX) {
	int row, col;
	int opponent = !playerIsX;
	// For each row...
	for (row = 1; row <= 8; row++) {
		// For each column...
		for (col = 1; col <= 8; col++) {
			// if the square has an opponent's checker, return 1
			if (squareHasPlayerChecker(row, col, opponent)) {
				return 1;
			}
		}
	}
	return 0;
}

/******************************************************************/
/*********************** Display Methods **************************/
/***********************    Display.c    **************************/
/******************************************************************/

/**
 * Display the current player to indicate who's turn it is.
 */
void displayPlayer() {
#ifdef arduino
	// set current player LED and clear other player LED for arduino
	if (player) {
		digitalWrite(player1Pin, HIGH);
		digitalWrite(player2Pin, LOW);
	}
	else {
		digitalWrite(player1Pin, LOW);
		digitalWrite(player2Pin, HIGH);
	}
#else
	// print the player (X or O) and the turn number for PC
	if (verbose) {
		printf("Player %s: Turn %i - ", player ? "X" : "O", numMoves);
		fflush(stdout);
	}
#endif
}

#ifdef arduino
/**
 * Clear the player LEDs (arduino only)
 */
void clearPlayer() {
	digitalWrite(player1Pin, LOW);
	digitalWrite(player2Pin, LOW);
}

/**
 * Clear the LED matrix (arduino only)
 */
void clearLEDBoard() {
	unsigned char allOff = 0x00;
	int i = 0;
	for(i = 0; i < 8; i++) {
		unsigned char row = 0xFF; // use FF for common cathode and 00 for common anode.
		bitWrite(row, i, LOW);// use LOW for common cathode, HIGH for common anode.
		digitalWrite(latchPin, LOW);// hold LEDs and load new data pulse
		// use Most Significant Bit First (MSBFIRST) so that lowest LED row corresponds to first byte in
		shiftOut(dataPin, clockPin, MSBFIRST, row);// row, IC1
		// use Least Significant Bit First (LSBFIRST) so that left-most LED corresponds to lowest bit
		shiftOut(dataPin, clockPin, LSBFIRST, 0);// red, IC2
		shiftOut(dataPin, clockPin, LSBFIRST, 0);// green, IC3
		digitalWrite(latchPin, HIGH);// fire off new data pulse to LEDs
	}
}

/**
 * Display the current board state on the LED matrix (arduino only)
 */
void displayLEDBoard() {
	// Display rows (push checker, color and king bytes into 74595's)
	// For each row...
	int i=0;
	for(i = 0; i < 8; i++) {
		// create a bit mask for this row
		unsigned char allOn = 0xFF;// use FF for common cathode and 00 for common anode.
		bitWrite(allOn, i, LOW);// use LOW for common cathode, HIGH for common anode.

		digitalWrite(latchPin, LOW);// hold LEDs and load new data pulse
		// mechanism for representing a king is to blink that checker
		// first show all colored checkers (blinks kings on)
		// the first byte sent is pushed through the first through to the last IC
		// in the series of three ICs.  The last byte sent is pushed to the first IC.
		if (blinkOn) {
			shiftOut(dataPin, clockPin, MSBFIRST, allOn);            // row, IC3
			// if checker bit is set and color bit is not set, light red
			shiftOut(dataPin, clockPin, LSBFIRST, checkers[i] & colors[i]);// red, IC2
			// if checker bit is set and color bit is also set, light green
			shiftOut(dataPin, clockPin, LSBFIRST, checkers[i] ^ colors[i]);// green, IC1
		}
		// then show only non-king colored checkers (blinks kings off)
		else {
			shiftOut(dataPin, clockPin, MSBFIRST, allOn);
			// if checker bit is set and color bit is not set and king bit is not set, light red
			shiftOut(dataPin, clockPin, LSBFIRST, ((checkers[i] & colors[i]) ^ kings[i]) & (checkers[i] & colors[i]));
			// if checker bit is set and color bit is set and king bit is not set, light green
			shiftOut(dataPin, clockPin, LSBFIRST, ((checkers[i] ^ colors[i]) ^ kings[i]) & (checkers[i] ^ colors[i]));
		}
		digitalWrite(latchPin, HIGH);  // fire off new data pulse to LEDs
	}
	// We clear the board after the last row so that each row is displayed for the same
	// amount of time, and therefore has the same brightness.  Otherwise, the last row
	// will appear brighter because it continues to display while the game logic is processed.
	clearLEDBoard();
}
#else

/**
 *  Print out a simple ASCII representation of the current state of the board (PC only)
 */
void displayASCIIBoard() {
	int row;
	// Row 1 is the bottom row and Row 8 is the top row
	// Column 1 is the left-most column and column 8 is the right-most column
	for (row = 8; row >= 1; row--) {		// for each row on the board
		int col;
		for (col = 1; col <= 8; col++) {
			if (squareHasChecker(row, col)) {// if board square has checker bit set...
				if (squareHasPlayerChecker(row, col, 1)) {// if color bit is set, checker is black X
					if (squareHasPlayerKing(row, col, 1)) {
						printf("X ");	// if king bit is set, checker is an X
					} else {
						printf("x ");// if king bit is not set, checker is an x
					}
				} else {			// if color bit is not set,checker is white
					if (squareHasPlayerKing(row, col, 0)) {
						printf("O ");	// if king bit is set, checker is an O
					} else {
						printf("o ");// if king bit is not set, checker is an o
					}
				}
			} else {
				printf("- ");
			}
		}
		printf("\n");	// newline and go to the next row
	}
	printf("\n");
}
#endif

void displayBoard() {
#ifdef arduino
	// The inner loop holds LED state so that a king blinks
	// instead of looking more like a flicker.
	int i, j;
	for (i=0;i<speed;i++) {
		for (j=0;j<speed;j++) {
			displayLEDBoard();
		}
		blinkOn = !blinkOn;
	}
#else
	if (verbose) {
		displayASCIIBoard();
	}
#endif
}

/******************************************************************/
/********************* Application Methods ************************/
/******************************************************************/

// 1 for a completely random game with each game,
// 0 for the same random game each time.
void autoSetRandomizeSeed(int rseed) {
	seed = rseed;
	srand(seed);
}

/**
 * Jump a checker (if possible).
 * This method returns 0 for no jump possible, 1 for a single jump
 * and 2 or more for additional jumps.
 */
int autoJumpChecker(Move_t move, int moveToRow, int moveToCol, int score,
		int attemptOnly) {
	// calculate the move-to and jump-to row/column indices
	int jumpToRow, jumpToCol;
	if ((player && move.dir) || (!player && !move.dir)) {
		jumpToRow = add(move.row, 2);
	} else {
		jumpToRow = subtract(move.row, 2);
	}
	if (move.diag == 0) {
		jumpToCol = subtract(move.col, 2);
	} else {
		jumpToCol = add(move.col, 2);
	}

	// if player's checker is in move-to square or...
	if (squareHasPlayerChecker(moveToRow, moveToCol, player) |
	// if jump-to square is not playable square on board or...
			!squareIsValid(jumpToRow, jumpToCol) |
			// if jump-to square has checker
			squareHasChecker(jumpToRow, jumpToCol)) {
		// we can't jump
		return 0;
	}
	// Jump!
#ifndef arduino
	if (!attemptOnly && verbose) {
		printf("Jump %s from %i %i to %i %i\n", move.diag ? "right" : "left",
				move.row, move.col, jumpToRow, jumpToCol);
	}
#endif
	// remove opponent's checker from move-to square
	checkers[subtract(moveToRow, 1)] = clearBit(
			checkers[subtract(moveToRow, 1)], subtract(moveToCol, 1));
	colors[subtract(moveToRow, 1)] = clearBit(colors[subtract(moveToRow, 1)],
			subtract(moveToCol, 1));
	// add player's checker to jump-to square
	checkers[subtract(jumpToRow, 1)] = setBit(checkers[subtract(jumpToRow, 1)],
			subtract(jumpToCol, 1));
	// set the color and king bit for the checker
	if (player) {
		colors[subtract(jumpToRow, 1)] = setBit(colors[subtract(jumpToRow, 1)],
				subtract(jumpToCol, 1));
		if (squareHasPlayerKing(move.row, move.col, player) || jumpToRow == 8) {
			kings[subtract(jumpToRow, 1)] = setBit(
					kings[subtract(jumpToRow, 1)], subtract(jumpToCol, 1));
		}
	} else {
		colors[subtract(jumpToRow, 1)] = clearBit(
				colors[subtract(jumpToRow, 1)], subtract(jumpToCol, 1));
		if (squareHasPlayerKing(move.row, move.col, player) || jumpToRow == 1) {
			kings[subtract(jumpToRow, 1)] = setBit(
					kings[subtract(jumpToRow, 1)], subtract(jumpToCol, 1));
		}
	}
	// remove player's checker from move-from square(row, col)
	checkers[subtract(move.row, 1)] = clearBit(checkers[subtract(move.row, 1)],
			subtract(move.col, 1));
	colors[subtract(move.row, 1)] = clearBit(colors[subtract(move.row, 1)],
			subtract(move.col, 1));
	kings[subtract(move.row, 1)] = clearBit(kings[subtract(move.row, 1)],
			subtract(move.col, 1));
	// increment the score for the move
	score++;
	// reset location of checker and see if we can jump again.
	// calculate the move-to and jump-to row/column indices
	move.row = jumpToRow;
	move.col = jumpToCol;

	// try to continue jumping
	// first try to jump forward then try backward
	int jumpScore = 0;
	for (move.dir = 1; move.dir >= 0; move.dir--) {
		// cannot move backward unless checker is crowned
		if (move.dir == 0 && !squareHasPlayerKing(move.row, move.col, player)) {
			continue;
		}
		// first try to jump right, then try left
		for (move.diag = 1; move.diag >= 0; move.diag--) {
			// if move-to square has opponent's checker, try to jump
			// calculate the move-to row/column indices
			if ((player && move.dir) || (!player && !move.dir)) {
				moveToRow = add(move.row, 1);
			} else {
				moveToRow = subtract(move.row, 1);
			}
			if (move.diag == 0) {
				moveToCol = subtract(move.col, 1);
			} else {
				moveToCol = add(move.col, 1);
			}
			if (squareHasPlayerChecker(moveToRow, moveToCol, !player)) {
				jumpScore = autoJumpChecker(move, moveToRow, moveToCol,
						jumpScore, attemptOnly);
				// if jump was successful, increment move score
				if (isGreaterThan(jumpScore, 0)) {
					return add(score, jumpScore);
				}
			}

		}
	}
	return score;
}

/**
 * Move a checker (if possible).
 * The move is assumed to have been validated prior to this method.
 */
int autoMoveChecker(Move_t move, int moveToRow, int moveToCol, int attemptOnly) {
	// Move
#ifndef arduino
	if (!attemptOnly && verbose) {
		printf("Move %s from %i %i to %i %i\n", move.diag ? "right" : "left",
				move.row, move.col, moveToRow, moveToCol);
	}
#endif
	// add player's checker to move-to square
	checkers[subtract(moveToRow, 1)] = setBit(checkers[subtract(moveToRow, 1)],
			subtract(moveToCol, 1));
	if (player) {
		colors[subtract(moveToRow, 1)] = setBit(colors[subtract(moveToRow, 1)],
				subtract(moveToCol, 1));
		if (squareHasPlayerKing(move.row, move.col, player) || moveToRow == 8) {
			kings[subtract(moveToRow, 1)] = setBit(
					kings[subtract(moveToRow, 1)], subtract(moveToCol, 1));
		}
	} else {
		colors[subtract(moveToRow, 1)] = clearBit(
				colors[subtract(moveToRow, 1)], subtract(moveToCol, 1));
		if (squareHasPlayerKing(move.row, move.col, player) || moveToRow == 1) {
			kings[subtract(moveToRow, 1)] = setBit(
					kings[subtract(moveToRow, 1)], subtract(moveToCol, 1));
		}
	}
	// remove player's checker from move-from square
	checkers[subtract(move.row, 1)] = clearBit(checkers[subtract(move.row, 1)],
			subtract(move.col, 1));
	colors[subtract(move.row, 1)] = clearBit(colors[subtract(move.row, 1)],
			subtract(move.col, 1));
	kings[subtract(move.row, 1)] = clearBit(kings[subtract(move.row, 1)],
			subtract(move.col, 1));
	return 1;
}

/**
 * Move a player's checker from the specified square in the specified direction (if possible).
 */
int autoMakeMove(Move_t move, int attemptOnly) {
	// Save original board state in case move doesn't work.
	unsigned char originalCheckers[8];
	unsigned char originalColors[8];
	unsigned char originalKings[8];
	int i;
	if (attemptOnly) {
		for (i = 0; i < 8; i++) {
			originalCheckers[i] = checkers[i];
			originalColors[i] = colors[i];
			originalKings[i] = kings[i];
		}
	}
	// verify move-from square is playable square on board
	if (!squareIsValid(move.row, move.col)) {
#ifndef arduino
		if (!attemptOnly) {
			printf("Move from square %i %i is not valid.\n", move.row,
					move.col);
		}
#endif
		return 0;
	}
	// verify checker in move-from square
	if (!squareHasChecker(move.row, move.col)) {
#ifndef arduino
		if (!attemptOnly) {
			printf("Move from square %i %i does not have a checker.\n",
					move.row, move.col);
		}
#endif
		return 0;
	}
	// verify correct checker in move-from square
	if (!squareHasPlayerChecker(move.row, move.col, player)) {
#ifndef arduino
		if (!attemptOnly) {
			printf("Move from square %i %i does not have an %s checker.\n",
					move.row, move.col, player ? "X" : "O");
		}
#endif
		return 0;
	}

	// calculate the move-to row/column indices
	int moveToRow, moveToCol;
	if ((player && move.dir) || (!player && !move.dir)) {
		moveToRow = add(move.row, 1);
	} else {
		moveToRow = subtract(move.row, 1);
	}
	if (move.diag == 0) {
		moveToCol = subtract(move.col, 1);
	} else {
		moveToCol = add(move.col, 1);
	}

	int score = 0;
	// if move-to square is not playable move fails
	if (!squareIsValid(moveToRow, moveToCol)) {
#ifndef arduino
		if (!attemptOnly) {
			printf("Move from square %i %i to square %i %i is not valid.\n",
					move.row, move.col, moveToRow, moveToCol);
		}
#endif
		return 0;
	}
	// if move-to square has opponent's checker, try to jump
	if (squareHasPlayerChecker(moveToRow, moveToCol, !player)) {
		score = autoJumpChecker(move, moveToRow, moveToCol, score, attemptOnly);
		if (isGreaterThan(score, 0)) {
			// return score+1 so that a jump scores higher than a simple move.
			return add(score, 1);
		}
	}
	// if move-to square has no checker, move
	if (!squareHasChecker(moveToRow, moveToCol)) {
		score = autoMoveChecker(move, moveToRow, moveToCol, attemptOnly);
		if (isGreaterThan(score, 0)) {
			return score;
		}
	}

	if (!attemptOnly) {
#ifndef arduino
		printf("Move %s from %i %i to %i %i is blocked.\n",
				move.diag ? "right" : "left", move.row, move.col, moveToRow,
				moveToCol);
#endif
	} else {
		for (i = 0; i < 8; i++) {
			checkers[i] = originalCheckers[i];
			colors[i] = originalColors[i];
			kings[i] = originalKings[i];
		}
	}
	return 0;
}

/**
 * Make an automatically generated move.
 */
int autoTurn() {
	// the maximum number of "best moves" to pick from in automated play
	const int MaxBestMoves = 10;
	Move_t move;
	Move_t bestMoves[MaxBestMoves];
	int score = 0;
	int bestScore = 0;
	int i = 0;
	// save current board state
	unsigned char originalCheckers[8];
	unsigned char originalColors[8];
	unsigned char originalKings[8];
	for (i = 0; i < 8; i++) {
		originalCheckers[i] = checkers[i];
		originalColors[i] = colors[i];
		originalKings[i] = kings[i];
	}

	// NOTE: moving from least row/col to greatest, and greatest dir/diag to least will
	// produce a different game if these orders are changed.

	// for each of players' checkers.
	for (move.row = 8; move.row >= 0; move.row--) {	// for each row on the board
		for (move.col = 8; move.col >= 0; move.col--) {	// while there are any bit sets still to check...
			// if board square has player's checker...
			if (squareHasPlayerChecker(move.row, move.col, player)) {
				// try each direction of move, score it and save it if it is the best move.
				for (move.dir = 0; move.dir <= 1; move.dir++) {
					// cannot move backward unless checker is crowned
					if (move.dir == 0
							&& !squareHasPlayerKing(move.row, move.col,
									player)) {
						continue;
					}
					// try each diagonal of move
					for (move.diag = 1; move.diag >= 0; move.diag--) {
						// start with original board state
						for (i = 0; i < 8; i++) {
							checkers[i] = originalCheckers[i];
							colors[i] = originalColors[i];
							kings[i] = originalKings[i];
						}
						// score it and save it if it is a best move.
						score = autoMakeMove(move, 1);
						if (score >= bestScore) {
							if (score > bestScore) {
								bestScore = score;
								// new best score, clear out all old best scores
								for (i = 0; i < MaxBestMoves; i++) {
									bestMoves[i].score = 0;
									bestMoves[i].row = 0;
									bestMoves[i].col = 0;
									bestMoves[i].diag = 0;
									bestMoves[i].dir = 0;
								}
							}
							// put this best move in the next available spot in the array of best moves
							for (i = 0; i < MaxBestMoves; i++) {
								if (bestMoves[i].score == 0) {
									bestMoves[i].score = score;
									bestMoves[i].row = move.row;
									bestMoves[i].col = move.col;
									bestMoves[i].diag = move.diag;
									bestMoves[i].dir = move.dir;
									break;
								}
							}
						}
					}
				}
			}
		}
	}
	// if any move is possible...
	if (bestScore > 0) {
		// pick a random move from the best moves identified
		for (i = 1; i <= MaxBestMoves; i++) {
			if (bestMoves[subtract(i, 1)].score == 0) {
				break;
			}
		}
		i--;
		int index = rand() % i;
		// start with original board state
		for (i = 0; i < 8; i++) {
			checkers[i] = originalCheckers[i];
			colors[i] = originalColors[i];
			kings[i] = originalKings[i];
		}

		// actually take a highest scoring move
		score = autoMakeMove(bestMoves[index], 0);

		// did this move remove all opponent's checkers and end the game?
		if (boardHasOpponentCheckers(player) == 0) {
			winner = player;
#ifndef arduino
			printf("Player %s has no more checkers.\n", player ? "O" : "X");
#endif
		}
	// else if no move is possible
	} else {
		winner = !player;
#ifndef arduino
		printf("Player %s can't move.\n", player ? "X" : "O");
#endif
	}
	return score;
}

/******************************************************************/
/************************ Main Methods ****************************/
/************************    main.c    ****************************/
/******************************************************************/

/**
 * This method calls the checkers engine method to take a turn for player 1.
 * The autoTurn() method can be replaced with a call to another
 * checkers engine so that different engines may compete against each other.
 */
void takeTurnPlayer1() {
	autoTurn();
}

/**
 * This method calls the checkers engine method to take a turn for player 2.
 * The autoTurn() method can be replaced with a call to another
 * checkers engine so that different engines may compete against each other.
 */
void takeTurnPlayer2() {
	autoTurn();
}

/**
 * Setup is required by arduino to set initial conditions.
 * This method (re)sets the initial conditions for the game for both the
 * arduino and the PC games.
 */
void setup() {

#ifdef arduino
	pinMode(dataPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(latchPin, OUTPUT);
	pinMode(player1Pin, OUTPUT);
	pinMode(player2Pin, OUTPUT);

	digitalWrite(latchPin,LOW);
	digitalWrite(dataPin,LOW);
	digitalWrite(clockPin,LOW);

	blinkOn = 1;

#else
	if (verbose) {
		printf("Welcome to checkers!\n");
	}
#endif

	player = 1;
	numMoves = 1;
	winner = -1;

	checkers[0] = checkers[2] = checkers[6] = 85;
	checkers[3] = checkers[4] = 0;
	checkers[5] = checkers[1] = 	checkers[7] = 170;
	colors[0] = colors[2] = 85;
	colors[1] = 170;
	colors[3] = colors[4] = colors[5] = colors[6] = colors[7] = 0;
	kings[0] = kings[1] = kings[2] = kings[3] = kings[4] = kings[5] = kings[6] = kings[7] = 0;

	displayBoard();
	displayPlayer();
}

/**
 * This method is required by arduino to loop infinitely.
 * This method loops over each player's turn.
 * On the arduino, when a game finishes another game is immediately started.
 * On the PC, when a game finishes control is returned to the calling routine.
 */
void loop() {
	if (player) {
		takeTurnPlayer1();
	} else {
		takeTurnPlayer2();
	}
	if (numMoves == MaxMoves) {
		winner = 2;
	}

	// Summarize game results
	if (winner == 0   // player 2 wins
	|| winner == 1	// player 1 wins
	|| winner == 2) {  // tie
#ifdef arduino
			if (winner < 2) {
				int i,j;
                                // hold the winner display for a
                                // fixed number of cycles so we can 
                                // see who won even if game is being
                                // played very fast.
				for (i=0;i<10;i++ ) {
			                displayBoard();
                                        // blink the winner's LED
				        for (j=0;j<speed;j++ ) {
					        delay(10*speed);
					        displayPlayer();
					        delay(10*speed);
					        clearPlayer();
                                        }
				}
			} else {
				clearPlayer();
				delay(20*speed*speed);
			}
			autoSetRandomizeSeed(seed+1);
			// if on arduino, keep playing games.
			setup();
#else
		if (winner == 2) {
			printf("%i moves taken with no winner.\nGame %03i is a draw!\n\n",
					MaxMoves, seed);
		} else {
			printf("Player %s wins Game %03i in %i moves!\n\n",
					winner ? "X" : "O", seed, numMoves);
			fflush(stdout);
		}
		displayBoard();
		return;
#endif
	}
	// switch players, increment the number of moves taken and display the current board state
	player = !player;
	numMoves++;
	displayBoard();
	displayPlayer();
}

#ifndef arduino

/******************************************************************/
/************************ Test Harness  ***************************/
/******************** checkersEngineTest.c  ***********************/
/******************************************************************/
#include <math.h>
#include <time.h>

void playGame();

/**
 * Test basic methods
 */
void testBasics() {
	printf("Test Basic Methods\n");
	int err = 0;

	// test isEven()
	if (isEven(1)) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%i is Even = %i\n", 1, isEven(1));
	if (!isEven(2)) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%i is Even = %i\n", 2, isEven(2));

	// test negate()
	if (!negate(1) == -1) {
		printf("ERROR: ");
		err = 1;
	}
	printf("- %2i = %2i\n", 1, negate(1));
	if (!negate(-1) == 1) {
		printf("ERROR: ");
		err = 1;
	}
	printf("- %2i = %2i\n", -1, negate(-1));

	// test isGreaterThan()
	if (isGreaterThan(1, 2)) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i > %2i = %2i\n", 1, 2, isGreaterThan(1, 2));
	if (!isGreaterThan(2, 1)) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i > %2i = %2i\n", 2, 1, isGreaterThan(2, 1));
	if (!isGreaterThan(1, -1)) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i > %2i = %2i\n", 1, -1, isGreaterThan(1, -1));
	if (isGreaterThan(-1, 1)) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i > %2i = %2i\n", -1, 1, isGreaterThan(-1, 1));

	// test isLessThan()
	if (!isLessThan(1, 2)) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i < %2i = %2i\n", 1, 2, isLessThan(1, 2));
	if (isLessThan(2, 1)) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i < %2i = %2i\n", 2, 1, isLessThan(2, 1));
	if (isLessThan(1, -1)) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i < %2i = %2i\n", 1, -1, isLessThan(1, -1));
	if (!isLessThan(-1, 1)) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i < %2i = %2i\n", -1, 1, isLessThan(-1, 1));

	// test add()
	if (!add(1, 2) == 3) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i + %2i = %2i\n", 1, 2, add(1, 2));
	if (!add(2, 3) == 5) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i + %2i = %2i\n", 2, 3, add(2, 3));
	if (!add(1, -1) == 0) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i + %2i = %2i\n", 1, -1, add(1, -1));
	if (!add(3, -1) == 2) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i + %2i = %2i\n", 3, -1, add(3, -1));
	if (!add(0, -1) == -1) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i + %2i = %2i\n", 0, -1, add(0, -1));

	// test subtract()
	if (!subtract(2, 1) == 1) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i - %2i = %2i\n", 2, 1, subtract(2, 1));
	if (!subtract(3, 2) == 1) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i - %2i = %2i\n", 3, 2, subtract(3, 2));
	if (!subtract(1, -1) == 2) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i - %2i = %2i\n", 1, -1, subtract(1, -1));
	if (!subtract(3, -1) == 4) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i - %2i = %2i\n", 3, -1, subtract(3, -1));
	if (!subtract(-2, -1) == -3) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i - %2i = %2i\n", -2, -1, subtract(-2, -1));
	if (!subtract(-3, 1) == 2) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i - %2i = %2i\n", -3, 1, subtract(-3, 1));

	// test multiply()
	if (!multiply(2, 1) == 2) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i * %2i = %2i\n", 2, 1, multiply(2, 1));
	if (!multiply(3, 2) == 6) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i * %2i = %2i\n", 3, 2, multiply(3, 2));
	if (!multiply(1, -1) == -1) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i * %2i = %2i\n", 1, -1, multiply(1, -1));
	if (!multiply(3, -1) == -3) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i * %2i = %2i\n", 3, -1, multiply(3, -1));
	if (!multiply(0, 1) == 0) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i * %2i = %2i\n", 0, 1, multiply(0, 1));

	// test divide()
	if (!divide(2, 1) == 2) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i / %2i = %2i\n", 2, 1, divide(2, 1));
	if (!divide(3, 2) == 1) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i / %2i = %2i\n", 3, 2, divide(3, 2));
	if (!divide(1, -1) == -1) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i / %2i = %2i\n", 1, -1, divide(1, -1));
	if (!divide(3, -1) == -3) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i / %2i = %2i\n", 3, -1, divide(3, -1));

	printf("End Test\n\n");
	if (err) {
		exit(1);
	}
}

/**
 * Test bit manipulation helper methods.
 */
void testBitHelperMethods() {

	printf("Test Bit Helper Methods\n");
	// test clearBit(), setBit() and isBit()
	int i = 0, err = 0;

	// Print the size of an int.  This will vary based on processor.
	int byteLength = sizeof(int);
	int bitLength = byteLength * 8;
	printf("Size = %i bytes (%i bits)\n", byteLength, bitLength);
	// all we care about is the first 16 bits
	bitLength = 16;
	int word = pow(2, bitLength) - 1;

	for (i = 0; i < bitLength; i++) {
		if (clearBit(word, i) != word - pow(2, i)) {
			printf("ERROR: ");
			err = 1;
		}
		printf("%i clearBit %i -> %i\n", word, i, clearBit(word, i));
	}
	for (i = 0; i < bitLength; i++) {
		if (setBit(word, i) != word) {
			printf("ERROR: ");
			err = 1;
		}
		printf("%i setBit %i -> %i\n", word, i, setBit(word, i));
	}
	for (i = 0; i < bitLength; i++) {
		if (!isBit(word, i)) {
			printf("ERROR: ");
			err = 1;
		}
		printf("%i isBit %i -> %i\n", word, i, isBit(word, i));
	}
	word = 0;
	for (i = 0; i < bitLength; i++) {
		if (clearBit(word, i) != word) {
			printf("ERROR: ");
			err = 1;
		}
		printf("%i clearBit %i -> %i\n", word, i, clearBit(word, i));
	}
	for (i = 0; i < bitLength; i++) {
		if (setBit(word, i) != word + pow(2, i)) {
			printf("ERROR: ");
			err = 1;
		}
		printf("%i setBit %i -> %i\n", word, i, setBit(word, i));
	}
	for (i = 0; i < bitLength; i++) {
		if (isBit(word, i)) {
			printf("ERROR: ");
			err = 1;
		}
		printf("%i isBit %i -> %i\n", word, i, isBit(word, i));
	}

	printf("End Test\n\n");
	if (err) {
		exit(1);
	}
}

/**
 * Validate that various invalid moves are caught and dealt with appropriately.
 */
void testInvalidMoves() {
	printf("Test Invalid Moves\n");
	setup();

	int err = 0;
	Move_t move;
	// demonstrate bad row condition (row < 1)
	move.row = 0;
	move.col = 1;
	move.diag = 1;
	move.dir = 1;
	if (autoMakeMove(move, 0)) {
		printf("ERROR: Previous invalid move not detected.\n");
		err = 1;
	}
	// demonstrate bad row condition (row > 8)
	move.row = 9;
	move.col = 1;
	move.diag = 1;
	move.dir = 1;
	if (autoMakeMove(move, 0)) {
		printf("ERROR: Previous invalid move not detected.\n");
		err = 1;
	}
	// demonstrate bad column condition (col < 1)
	move.row = 1;
	move.col = 0;
	move.diag = 1;
	move.dir = 1;
	if (autoMakeMove(move, 0)) {
		printf("ERROR: Previous invalid move not detected.\n");
		err = 1;
	}
	// demonstrate bad column condition (col > 8)
	move.row = 1;
	move.col = 9;
	move.diag = 1;
	move.dir = 1;
	if (autoMakeMove(move, 0)) {
		printf("ERROR: Previous invalid move not detected.\n");
		err = 1;
	}
	// demonstrate unplayable start square condition
	move.row = 6;
	move.col = 1;
	move.diag = 1;
	move.dir = 1;
	if (autoMakeMove(move, 0)) {
		printf("ERROR: Previous invalid move not detected.\n");
		err = 1;
	}
	// demonstrate unplayable start square condition
	move.row = 6;
	move.col = 7;
	move.diag = 1;
	move.dir = 1;
	if (autoMakeMove(move, 0)) {
		printf("ERROR: Previous invalid move not detected.\n");
		err = 1;
	}
	// demonstrate no checker in start square condition
	move.row = 5;
	move.col = 1;
	move.diag = 1;
	move.dir = 1;
	if (autoMakeMove(move, 0)) {
		printf("ERROR: Previous invalid move not detected.\n");
		err = 1;
	}
	// demonstrate no checker in start square condition
	move.row = 5;
	move.col = 7;
	move.diag = 1;
	move.dir = 1;
	if (autoMakeMove(move, 0)) {
		printf("ERROR: Previous invalid move not detected.\n");
		err = 1;
	}
	// demonstrate move off the board
	move.row = 3;
	move.col = 1;
	move.diag = 0;
	move.dir = 1;
	if (autoMakeMove(move, 0)) {
		printf("ERROR: Previous invalid move not detected.\n");
		err = 1;
	}
	// demonstrate move off the board
	move.row = 2;
	move.col = 8;
	move.diag = 1;
	move.dir = 1;
	if (autoMakeMove(move, 0)) {
		printf("ERROR: Previous invalid move not detected.\n");
		err = 1;
	}
	// demonstrate wrong color checker in start square condition
	move.row = 7;
	move.col = 1;
	move.diag = 1;
	move.dir = 1;
	if (autoMakeMove(move, 0)) {
		printf("ERROR: Previous invalid move not detected.\n");
		err = 1;
	}
	// demonstrate wrong color checker in start square condition
	move.row = 7;
	move.col = 7;
	move.diag = 1;
	move.dir = 1;
	if (autoMakeMove(move, 0)) {
		printf("ERROR: Previous invalid move not detected.\n");
		err = 1;
	}
	printf("End Test\n");
	if (err) {
		printf("FAIL\n\n");
		exit(1);
	} else {
		printf("PASS\n\n");
	}
}

/**
 * This method is used to specify the game to be played
 * and to simulate the control of flow created automatically on the arduino,
 * except that instead of looping infinitely when a game completes
 * this method returns control to the calling routine.
 */
void playGame(int seed) {
	autoSetRandomizeSeed(seed);
	setup();
	while (winner < 0) {
		loop();
	}
}

/**
 * Test a series of pre-defined games and verify consistent output.
 */
void testGameOutcomes() {
	playGame(1);
	printf(
			"Verify:\nPlayer O has no more checkers.\nPlayer X wins Game 001 in 37 moves!\n\n");

	playGame(2);
	printf(
			"Verify:\nPlayer X has no more checkers.\nPlayer O wins Game 002 in 88 moves!\n\n");

	playGame(14);
	printf(
			"Verify:\nPlayer O can't move.\nPlayer X wins Game 014 in 50 moves!\n\n");

	playGame(37);
	printf(
			"Verify:\nPlayer X can't move.\nPlayer O wins Game 037 in 43 moves!\n\n");

	playGame(4);
	printf(
			"Verify:\n100 moves taken with no winner.\nGame 004 is a draw!!\n\n");
}

/**
 * Play a set of pre-defined random games and print a summary
 * of the game wins and execution times.
 * Note that some game variables will affect the results.  For example,
 * increasing the maxMoves would result in fewer draws,
 * but would also make average game time longer.  Running in verbose mode
 * will result in longer game times as well.
 * During game competition, all game variables should be held constant
 * to produce good results.
 */
void evaluateGames() {
	int i, xWins, oWins, draws, numGames;
	xWins = oWins = draws = 0;
	numGames = 500;
	clock_t start = clock();
	for (i = 1; i <= numGames; i++) {
		playGame(i);
		if (winner == 1) {
			xWins++;
		} else if (winner == 0) {
			oWins++;
		} else if (winner == 2) {
			draws++;
		} else {
			printf("ERROR: Unknown game end status\n");
			exit(1);
		}
	}
	clock_t stop = clock();
	float duration = (stop - start);
	duration = (duration*1000)/(CLOCKS_PER_SEC*numGames);
	printf("Out of %i games, X won %i games, Y won %i games and %i games ended in a draw.\n",
			numGames, xWins, oWins, draws);
	printf("Games took an average of %0.3f ms each to play in %s mode\n\n",
			duration, verbose ? "verbose":"summary");
}

/**
 * Run the test harness tests.
 */
void runTests() {
	testBasics();
	testBitHelperMethods();
	testInvalidMoves();
	testGameOutcomes();
}

/**
 * Main entry point for PC game execution.
 */
int main() {
	runTests();
	playGame(0);
	evaluateGames();
	return 0;
}
#endif

