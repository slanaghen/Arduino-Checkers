/*
 * Checkers.h
 */

#ifndef CHECKERS_H_
#define CHECKERS_H_

#include <stdlib.h>
#include <stdio.h>
#include "Core.h"

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
// the maximum number of moves in a game before calling it a draw
const int MaxMoves;

// Declare the elements that define a move
typedef struct {
	int row;
	int col;
	int diag;
	int dir;
	int score;
} Move_t;

int squareIsValid(int row, int col);
int squareHasChecker(int row, int col);
int squareHasPlayerChecker(int row, int col, int player);
int squareHasKing(int row, int col);
int squareHasPlayerKing(int row, int col, int player);
int boardHasOpponentCheckers(int playerIsX);

#endif /* CHECKERS_H_ */
