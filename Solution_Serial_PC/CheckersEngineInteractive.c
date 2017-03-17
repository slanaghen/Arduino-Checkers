/*
 * CheckersEngineInteractive.c
 *
 * This file is intended to show how a different game engine can be used.
 * It uses some of the same methods in CheckersEngine and so includes that
 * file, but it could (and should, in practice) be completely independent of that engine.
 */

#include "Core.h"
#include "Checkers.h"
#include "CheckersEngine.h"

int takeInteractiveTurn() {

	int validInput = 0, moveComplete = 0;
	char *dir = NULL, *diag = NULL, *col = NULL, *row = NULL;
	do {
		validInput = 1;
		// prompt the user to enter a row number
		printf("\nEnter the checker row:");
		fflush(stdout);
		gets(row);
		if (*row < '1' || *row > '8') {
			printf("  Row must be a number between 1 and 8 (%c).\n", *row);
			validInput = 0;
			continue;
		}
		// prompt the user to enter a column number
		printf("Enter the checker column: ");
		fflush(stdout);
		gets(col);
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
			gets(dir);
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

		gets(diag);
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
	return moveComplete;
}

