/*
 * Display.h
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

// For ASCII display - 1 to show move-by-move, 0 to show only game summary
int verbose;

#include "arduino/arduino.h"
// Define the pin output leads for arduino board.
// int powerPin = 5V   		// Red, 	+5V to the power bus
// int groundPin = Gnd 		// White,	Gnd to the ground bus
const int dataPin; // Yellow,	Pin 14 on the 75HC595 #1, Data pin
const int clockPin; // Green,	Pin 11 on the 75HC595 #1, Clock pin
const int latchPin; // Blue,	Pin 12 on the 75HC595 #1, Latch pin
const int player1Pin; // Tan,		Red LED anode
const int player2Pin; // Brown,	Yellow LED anode
// 75HC595 #1 - Green Matrix LEDs
// 75HC595 #2 - Red Matrix LEDs
// 75HC595 #3 - LED rows

char *port;
//	  char *port = "/dev/ttyACM0"; // Linux and Mac serial devices
const int bdrate; // port baud
// speed of arduino board display
const int speed;
// mechanism to make king checkers blink
int blinkOn;

void displayPlayer();
void clearPlayer();
void displayBoard();
void initializeLEDBoard();
void terminateLEDBoard();

#endif /* DISPLAY_H_ */
