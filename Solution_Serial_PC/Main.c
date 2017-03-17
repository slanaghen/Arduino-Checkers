/*
 * Main.c
 *
 * A simple test harness to validate core game logic,
 * validate basic checkers game logic and
 * simulate a basic two player checkers game.
 *
 * NOTE: This version runs the logic locally and runs
 * the Arduino LED display in parallel over a serial connection.
 * The synchronization between the two can be delicate.
 */

#include <stdlib.h>
#include "Core.h"
#include "Checkers.h"
#include "Display.h"
#include "CheckersEngine.h"
#include "CheckersEngineAuto.h"
#include "CheckersEngineInteractive.h"
#include "CheckersEngineTest.h"
#include "arduino/arduino.h"

int initial = 1;
/******************************************************************/
/************************* Main Methods ***************************/
/******************************************************************/

/**
 * This method calls the checkers engine method to take a turn for player 1.
 * The takeInteractiveTurn() method can be replaced with a call to another
 * checkers engine so that different engines may compete against each other.
 */
void takeTurnPlayer1() {
//	takeInteractiveTurn();
	takeAutoTurn();
}

/**
 * This method calls the checkers engine method to take a turn for player 2.
 * The takeInteractiveTurn() method can be replaced with a call to another
 * checkers engine so that different engines may compete against each other.
 */
void takeTurnPlayer2() {
//	takeInteractiveTurn();
	takeAutoTurn();
}

/**
 * Setup is required by arduino to set initial conditions.
 * This method (re)sets the initial conditions for the game.
 */
void setup() {

	if (initial) {
		pinMode(dataPin, OUTPUT);
		pinMode(clockPin, OUTPUT);
		pinMode(latchPin, OUTPUT);
		pinMode(player1Pin, OUTPUT);
		pinMode(player2Pin, OUTPUT);

		digitalWrite(latchPin, LOW);
		digitalWrite(dataPin, LOW);
		digitalWrite(clockPin, LOW);
		initial = 0;
	}

	blinkOn = 1;

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
 * This method is required by arduino to loop infinitely.
 * This method loops over each player's turn.
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

	displayBoard();
	// Summarize game results
	if (winner == 0   // player 2 wins
	|| winner == 1	// player 1 wins
	|| winner == 2) {  // tie
		if (verbose == 2) {
			if (winner < 2) {
				int i;
				for (i = 0; i < speed; i++) {
					delay(10 * speed);
					displayPlayer();
					delay(10 * speed);
					clearPlayer();
				}
			} else {
				clearPlayer();
				delay(20 * speed * speed);
			}
			setGameNumber(gameNumber + 1);
			// setup another and keep playing games.
			setup();
		}
		if (winner == 2) {
			printf("%i moves taken with no winner.\nGame %03i is a draw!\n\n",
					MaxMoves, gameNumber);
		} else {
			printf("Player %s wins Game %03i in %i moves!\n\n",
					winner ? "X" : "O", gameNumber, numMoves);
			fflush(stdout);
		}
		return;
	}
	// switch players, increment the number of moves taken and display the current board state
	player = !player;
	numMoves++;
	displayPlayer();
}

/**
 * This method is used to specify the game to be played
 * and to simulate the control of flow created automatically on the arduino,
 * except that instead of looping infinitely when a game completes
 * this method returns control to the calling routine.
 */
void playGame(int gameNo) {
	setGameNumber(gameNo);
	setup();
	while (winner < 0) {
		loop();
	}
}

/**
 * Main entry point for PC game execution.
 */
int main() {
//	runTests();
	playGame(0);
	evaluateGames();
	return 0;
}
