/*
 * Checkers.c
 *
 * Basic validation and query methods for the current status of the checker board.
 */

#include "Core.h"
#include "Checkers.h"

// the maximum number of moves in a game before calling it a draw
const int MaxMoves = 150;

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
int squareHasKing(int row, int col) {
	return isBit(kings[subtract(row, 1)], subtract(col, 1));
}

/**
 * Determine if the checker a the given row/column is the player's checker
 * and has been crowned a king.
 */
int squareHasPlayerKing(int row, int col, int player) {
	return squareHasPlayerChecker(row, col, player)
			&& squareHasKing(row, col);
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
