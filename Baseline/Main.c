/*
 * Main.c
 *
 * A simple test harness to validate core game logic,
 * validate basic checkers game logic and
 * interactively simulate a basic two player checkers game.
 *
 *  Created on: Nov 8,2012
 *      Author: sgl
 */

#include <stdlib.h>
#include <stdio.h>
#include "Core.h"
#include "Checkers.h"
#include "Display.h"
#include "CheckersEngine.h"
#include "CheckersEngineTest.h"

/******************************************************************/
/************************* Main Methods ***************************/
/******************************************************************/

/**
 * This method calls the checkers engine method to take a turn for player 1.
 * The autoTurn() method can be replaced with a call to another
 * checkers engine so that different engines may compete against each other.
 */
void takeTurnPlayer1() {
	takeTurn();
}

/**
 * This method calls the checkers engine method to take a turn for player 2.
 * The autoTurn() method can be replaced with a call to another
 * checkers engine so that different engines may compete against each other.
 */
void takeTurnPlayer2() {
	takeTurn();
}

/**
 * Set initial conditions.
 * This method (re)sets the initial conditions for the game.
 */
void setup() {

	if (verbose) {
		printf("Welcome to checkers!\n");
	}

	player = 1;
	numMoves = 1;
	winner = -1;

	checkers[0] = checkers[2] = checkers[6] = 85;
	checkers[3] = checkers[4] = 0;
	checkers[5] = checkers[1] = checkers[7] = 170;
	colors[0] = colors[2] = 85;
	colors[1] = 170;
	colors[3] = colors[4] = colors[5] = colors[6] = colors[7] = 0;
	kings[0] = kings[1] = kings[2] = kings[3] = kings[4] = kings[5] = kings[6] =
			kings[7] = 0;

	displayBoard();
	displayPlayer();
}

/**
 * This method loops over each player's turn.
 * When a game finishes control is returned to the calling routine.
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
		if (winner == 2) {
			printf("%i moves taken with no winner.\nGame %03i is a draw!\n\n",
					MaxMoves, gameNumber);
		} else {
			printf("Player %s wins Game %03i in %i moves!\n\n",
					winner ? "X" : "O", gameNumber, numMoves);
			fflush(stdout);
		}
		displayBoard();
		return;
	}
	// switch players, increment the number of moves taken and display the current board state
	player = !player;
	numMoves++;
	displayBoard();
	displayPlayer();
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
