/*
 * CheckersEngine.c
 *
 *  Created on: Jan 11, 2013
 *      Author: sgl
 */

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "Core.h"
#include "Checkers.h"

// For ASCII display - 1 to show move-by-move, 0 to show only game summary
int verbose = 1;

/**
 * Display the current player to indicate who's turn it is.
 */
void displayPlayer() {

	// print the player (X or O) and the turn number for PC
	if (verbose) {
		printf("Player %s: Turn %i - ", player ? "X" : "O", numMoves);
		fflush(stdout);
	}
}

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

/**
 *  Print out a simple representation of the current state of the board.
 */
void displayBoard(int* board, int playerIsX) {
	if (verbose > 0) {
		displayASCIIBoard(board);
	}
}
