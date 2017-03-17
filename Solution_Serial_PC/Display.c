/*
 * CheckersEngine.c
 */

#include "Core.h"
#include "Checkers.h"
#include "arduino/arduino.h"
#include "arduino/rs232.h"

// Define the pin output leads for arduino board.
// int powerPin = 5V   		// Red, 	+5V to the power bus
// int groundPin = Gnd 		// White,	Gnd to the ground bus
const int dataPin = 8; // Yellow,	Pin 14 on the 75HC595 #1, Data pin
const int clockPin = 9; // Green,	Pin 11 on the 75HC595 #1, Clock pin
const int latchPin = 10; // Blue,	Pin 12 on the 75HC595 #1, Latch pin
const int player1Pin = 12; // Tan,		Red LED anode
const int player2Pin = 13; // Brown,	Yellow LED anode
// 75HC595 #1 - Green Matrix LEDs
// 75HC595 #2 - Red Matrix LEDs
// 75HC595 #3 - LED rows

char *port = "COM3";
//	  char *port = "/dev/ttyACM0"; // Linux and Mac serial devices
int bdrate = 9600; // port baud
// speed of arduino board display
const int speed = 1;
// mechanism to make king checkers blink
int blinkOn = 0;

// 1 to show move-by-move, 0 to show only game summary
int verbose = 1;

/**
 * Display the current player to indicate who's turn it is.
 */
void displayPlayer() {
	// set current player LED and clear other player LED for arduino
	if (player) {
		digitalWrite(player1Pin, HIGH);
		digitalWrite(player2Pin, LOW);
	} else {
		digitalWrite(player1Pin, LOW);
		digitalWrite(player2Pin, HIGH);
	}
	// print the player (X or O) and the turn number for PC
	if (verbose) {
		printf("Player %s: Turn %i - ", player ? "X" : "O", numMoves);
		fflush(stdout);
	}
}

/**
 * Clear the current player indicator.
 */
void clearPlayer() {
	// clear both player LEDs
	digitalWrite(player1Pin, LOW);
	digitalWrite(player2Pin, LOW);
}

/**
 * Initialize the connection to the arduino board.
 */
void initializeLEDBoard() {
	if (OpenComport(port, bdrate)) {
		printf(
				"Could not establish connection on %s.\nCheck your connection. Exiting.\n",
				port);
		exit(1);
	}
}

/**
 * Terminate the connection to the arduino board.
 */
void terminateLEDBoard() {
	CloseComport();
}

/**
 * Display the current status of the checker board on the LED matrix.
 */
void displayLEDBoard() {
	initializeLEDBoard();
	// send board data over serial connection to arduino board
	unsigned char checkers[8], colors[8], kings[8];
	int row, col;
	// for each row
	for (row = 0; row < 8; row++) {
		// for each column
		checkers[row] = colors[row] = kings[row] = 0;
		for (col = 0; col < 8; col++) {
			if (squareHasChecker(row + 1, col + 1) && isEven(row + col)) {
				checkers[row] = setBit(checkers[row], col);
			}
			if (squareHasPlayerChecker(row + 1, col + 1, 1)
					&& isEven(row + col)) {
				colors[row] = setBit(colors[row], col);
			}
			if (squareHasKing(row + 1, col + 1)
					&& isEven(row + col)) {
				kings[row] = setBit(kings[row], col);
			}
		}
	}

	SendByte(player);

	for (row = 0; row < 8; row++) {
		SendByte(checkers[row]);
		SendByte(colors[row]);
		SendByte(kings[row]);
	}
	// handshake
	char buf[256];
	int bitsRead = 0;
	while (bitsRead == 0) {
		bitsRead = PollResponse(buf);
	}
	terminateLEDBoard();
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
	fflush(stdout);
}

/**
 *  Print out a simple representation of the current state of the board.
 */
void displayBoard() {
	if (verbose == 1) {
		displayLEDBoard();
		displayASCIIBoard();
	}
}
