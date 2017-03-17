/*
 * CheckersEngine.c
 */

#include "Core.h"
#include "Checkers.h"
#include "CheckersEngine.h"
#include "Display.h"

// The identifier for the game
int gameNumber = 0;

/**
 * Set the game number.
 */
void setGameNumber(int gameNo) {
	gameNumber = gameNo;
}

/**
 * Jump a checker (if possible).
 * This method returns 0 for no jump possible, 1 for a single jump
 * and 2 or more for additional jumps.
 */
int jumpChecker(Move_t move, int moveToRow, int moveToCol, int score,
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
				jumpScore = jumpChecker(move, moveToRow, moveToCol, jumpScore,
						attemptOnly);
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
		if (!attemptOnly && verbose) {
			printf("Move from square %i %i is not valid.\n", move.row,
					move.col);
		}
		return 0;
	}
	// verify checker in move-from square
	if (!squareHasChecker(move.row, move.col)) {
		if (!attemptOnly && verbose) {
			printf("Move from square %i %i does not have a checker.\n",
					move.row, move.col);
		}
		return 0;
	}
	// verify correct checker in move-from square
	if (!squareHasPlayerChecker(move.row, move.col, player)) {
		if (!attemptOnly && verbose) {
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

	int score = 0;
	// if move-to square is not playable move fails
	if (!squareIsValid(moveToRow, moveToCol)) {
		if (!attemptOnly && verbose) {
			printf("Move from square %i %i to square %i %i is not valid.\n",
					move.row, move.col, moveToRow, moveToCol);
		}
		return 0;
	}
	// if move-to square has opponent's checker, try to jump
	if (squareHasPlayerChecker(moveToRow, moveToCol, !player)) {
		score = jumpChecker(move, moveToRow, moveToCol, score, attemptOnly);
		if (isGreaterThan(score, 0)) {
			// return score+1 so that a jump scores higher than a simple move.
			return add(score, 1);
		}
	}
	// if move-to square has no checker, move
	if (!squareHasChecker(moveToRow, moveToCol)) {
		score = moveChecker(move, moveToRow, moveToCol, attemptOnly);
		if (isGreaterThan(score, 0)) {
			return score;
		}
	}

	if (!attemptOnly && verbose) {
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
