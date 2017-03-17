/*
 * CheckersEngineInteractive.c
 *
 * This file is intended to show how a different game engine can be used.
 * It uses some of the same methods in CheckersEngine and so includes that
 * file, but it could (and should, in practice) be completely independent of that engine.
 */

#include <string.h>
#include <time.h>
#include "Checkers.h"
#include "Display.h"
#include "CheckersEngine.h"

/**
 * Jump a checker (if possible).
 * This method returns 0 for no jump possible, 1 for a single jump
 * and 2 or more for additional jumps.
 */
int jumpChecker(Move_t move, int moveToRow, int moveToCol, int attemptOnly) {
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
	if (!attemptOnly && verbose) {
		printf("Jump %s from %i %i to %i %i\n", move.diag ? "right" : "left",
				move.row, move.col, jumpToRow, jumpToCol);
	}
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
	// reset location of checker and see if we can jump again.
	// calculate the move-to and jump-to row/column indices
	move.row = jumpToRow;
	move.col = jumpToCol;

	// try to continue jumping
	// first try to jump forward then try backward
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
				jumpChecker(move, moveToRow, moveToCol, attemptOnly);
				// if jump was successful, return 1
				return 1;
			}

		}
	}
	return 1;
}

/**
 * Move a checker (if possible).
 * The move is assumed to have been validated prior to this method.
 */
int moveChecker(Move_t move, int moveToRow, int moveToCol, int attemptOnly) {
	// Move
	if (!attemptOnly && verbose) {
		printf("Move %s from %i %i to %i %i\n", move.diag ? "right" : "left",
				move.row, move.col, moveToRow, moveToCol);
	}
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
int makeMove(Move_t move, int attemptOnly) {
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
		if (!attemptOnly) {
			printf("Move from square %i %i is not valid.\n", move.row,
					move.col);
		}
		return 0;
	}
	// verify checker in move-from square
	if (!squareHasChecker(move.row, move.col)) {
		if (!attemptOnly) {
			printf("Move from square %i %i does not have a checker.\n",
					move.row, move.col);
		}
		return 0;
	}
	// verify correct checker in move-from square
	if (!squareHasPlayerChecker(move.row, move.col, player)) {
		if (!attemptOnly) {
			printf("Move from square %i %i does not have an %s checker.\n",
					move.row, move.col, player ? "X" : "O");
		}
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

	// if move-to square is not playable move fails
	if (!squareIsValid(moveToRow, moveToCol)) {
		if (!attemptOnly) {
			printf("Move from square %i %i to square %i %i is not valid.\n",
					move.row, move.col, moveToRow, moveToCol);
		}
		return 0;
	}
	// if move-to square has opponent's checker, try to jump
	if (squareHasPlayerChecker(moveToRow, moveToCol, !player)) {
		if (jumpChecker(move, moveToRow, moveToCol, attemptOnly)) {
			// return 1
			return 1;
		}
	}
	// if move-to square has no checker, move
	if (!squareHasChecker(moveToRow, moveToCol)) {
		if (moveChecker(move, moveToRow, moveToCol, attemptOnly)) {
			return 1;
		}
	}

	if (!attemptOnly) {
		printf("Move %s from %i %i to %i %i is blocked.\n",
				move.diag ? "right" : "left", move.row, move.col, moveToRow,
				moveToCol);
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
 * Set the game number and seed the move randomizer with it.
 */
void setGameNumber(int rseed) {
	gameNumber = rseed;
	srand(gameNumber);
}

int takeTurn() {

	int validInput = 0, moveComplete = 0;
	// ERROR: if no malloc is used and no initial value is given, get STATUS_ACCESS_VIOLATION
	// WARNING: if no initial value is set, get uninitialized value warning during compilation
	char *ch, *dir, *diag, *col, *row;
//	int *ch=NULL, *dir=NULL, *diag=NULL, *col=NULL, *row=NULL;
	ch = malloc(sizeof(int));
	row = malloc(sizeof(int));
	col = malloc(sizeof(int));
	diag = malloc(sizeof(int));
	dir = malloc(sizeof(int));
	do {
		validInput = 1;
		// prompt the user to enter a row number
		printf("\nEnter the checker row:");
		fflush(stdout);
//		fgets(row, 1, stdin);
		gets(row);
//		gets(ch);
//		strcpy(row,ch);		// ERROR: STATUS_ACCESS_VIOLATION if input is 5 intacters or longer
//			strncpy(row, ch, 1);// SOLUTION: this works, or you do "int row = atoi(ch)" as a another fix.
		// ERROR: this only evaluates the first intacter/digit and will almost always succeed,
		//  even if the int* evaluates to a multi-digit number
		if (*row < '1' || *row > '8') {
			printf("  Row must be a number between 1 and 8 (%c).\n", *row);
			validInput = 0;
			continue;
		}
		// prompt the user to enter a column number
		printf("Enter the checker column: ");
		fflush(stdout);
//		fgets(row, 1, stdin);
		gets(col);
//		gets(ch);
//			strcpy(col,ch);		// ERROR: STATUS_ACCESS_VIOLATION if input is 5 intacters or longer
//		strncpy(col, ch, 1);	// SOLUTION: works
		if (*col < '1' || *col > '8') {
			printf("  Column must be a number between 1 and 8 (%c).\n", *col);
			validInput = 0;
			continue;
		}
		if (squareHasPlayerKing(*row, *col, player)) {// if checker is a king, prompt for forward/backward
			// prompt user to enter a direction
			printf(
					"Enter the checker direction (1 for forward, 0 for backward): ");
			fflush(stdout);

//			fgets(row, 1, stdin);
			gets(dir);
//			gets(ch);
//			strcpy(dir,ch);		// ERROR: STATUS_ACCESS_VIOLATION if input is 5 intacters or longer
//			strncpy(dir, ch, 1);	// SOLUTION: works
			if (*dir < '0' || *dir > '1') {
				printf("  Direction must be a 0 or a l (%c).\n", *dir);
				validInput = 0;
				continue;
			}
		} else {
			*dir = '1';
		}
		// prompt the user to enter a direction
		printf("Enter the checker diagonal (1 for right, 0 for left): ");
		fflush(stdout);

//		fgets(row, 1, stdin);
		gets(diag);
//		gets(ch);
//			strcpy(diag,ch);		// ERROR: STATUS_ACCESS_VIOLATION if input is 5 intacters or longer
//		strncpy(diag, ch, 1);	// SOLUTION: works
		if (*diag < '0' || *diag > '1') {
			printf("  Diagonal must be a 0 or a l (%c).\n", *diag);
			validInput = 0;
			continue;
		}
		Move_t move;
		move.row = atoi(row);
		move.col = atoi(col);
		move.diag = atoi(diag);
		move.dir = atoi(dir);
		if (validInput) {
			moveComplete = makeMove(move, 0);
		}

	} while (!validInput || !moveComplete);
	// ERROR: memory leak if the free() statements are commented out.
	// Private Memory starts at <1M and as games are played gradually leaks memory.
	// To monitor memory usage, run resmon.exe (Windows Resource Monitor),
	// SOLUTION: free memory allocated before exiting this method
	free(ch);
	free(row);
	free(col);
	free(dir);
	free(diag);
	return moveComplete;
}

