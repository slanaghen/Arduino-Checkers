/*
 * CheckersEngine.c
 */

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "Core.h"
#include "Checkers.h"
#include "Display.h"
#include "CheckersEngine.h"

void playGame();
void setup();

/**
 * Test basic methods
 */
void testBasics() {
	printf("Test Basic Methods\n");
	int err = 0;

	// test isEven()
	if (isEven(1)) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%i is Even = %i\n", 1, isEven(1));
	if (!isEven(2)) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%i is Even = %i\n", 2, isEven(2));

	// test negate()
	if (!negate(1) == -1) {
		printf("ERROR: ");
		err = 1;
	}
	printf("- %2i = %2i\n", 1, negate(1));
	if (!negate(-1) == 1) {
		printf("ERROR: ");
		err = 1;
	}
	printf("- %2i = %2i\n", -1, negate(-1));

	// test isGreaterThan()
	if (isGreaterThan(1, 2)) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i > %2i = %2i\n", 1, 2, isGreaterThan(1, 2));
	if (!isGreaterThan(2, 1)) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i > %2i = %2i\n", 2, 1, isGreaterThan(2, 1));
	if (!isGreaterThan(1, -1)) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i > %2i = %2i\n", 1, -1, isGreaterThan(1, -1));
	if (isGreaterThan(-1, 1)) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i > %2i = %2i\n", -1, 1, isGreaterThan(-1, 1));

	// test isLessThan()
	if (!isLessThan(1, 2)) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i < %2i = %2i\n", 1, 2, isLessThan(1, 2));
	if (isLessThan(2, 1)) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i < %2i = %2i\n", 2, 1, isLessThan(2, 1));
	if (isLessThan(1, -1)) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i < %2i = %2i\n", 1, -1, isLessThan(1, -1));
	if (!isLessThan(-1, 1)) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i < %2i = %2i\n", -1, 1, isLessThan(-1, 1));

	// test add()
	if (!add(1, 2) == 3) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i + %2i = %2i\n", 1, 2, add(1, 2));
	if (!add(2, 3) == 5) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i + %2i = %2i\n", 2, 3, add(2, 3));
	if (!add(1, -1) == 0) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i + %2i = %2i\n", 1, -1, add(1, -1));
	if (!add(3, -1) == 2) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i + %2i = %2i\n", 3, -1, add(3, -1));
	if (!add(0, -1) == -1) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i + %2i = %2i\n", 0, -1, add(0, -1));

	// test subtract()
	if (!subtract(2, 1) == 1) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i - %2i = %2i\n", 2, 1, subtract(2, 1));
	if (!subtract(3, 2) == 1) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i - %2i = %2i\n", 3, 2, subtract(3, 2));
	if (!subtract(1, -1) == 2) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i - %2i = %2i\n", 1, -1, subtract(1, -1));
	if (!subtract(3, -1) == 4) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i - %2i = %2i\n", 3, -1, subtract(3, -1));
	if (!subtract(-2, -1) == -3) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i - %2i = %2i\n", -2, -1, subtract(-2, -1));
	if (!subtract(-3, 1) == 2) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i - %2i = %2i\n", -3, 1, subtract(-3, 1));

	// test multiply()
	if (!multiply(2, 1) == 2) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i * %2i = %2i\n", 2, 1, multiply(2, 1));
	if (!multiply(3, 2) == 6) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i * %2i = %2i\n", 3, 2, multiply(3, 2));
	if (!multiply(1, -1) == -1) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i * %2i = %2i\n", 1, -1, multiply(1, -1));
	if (!multiply(3, -1) == -3) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i * %2i = %2i\n", 3, -1, multiply(3, -1));
	if (!multiply(0, 1) == 0) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i * %2i = %2i\n", 0, 1, multiply(0, 1));

	// test divide()
	if (!divide(2, 1) == 2) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i / %2i = %2i\n", 2, 1, divide(2, 1));
	if (!divide(3, 2) == 1) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i / %2i = %2i\n", 3, 2, divide(3, 2));
	if (!divide(1, -1) == -1) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i / %2i = %2i\n", 1, -1, divide(1, -1));
	if (!divide(3, -1) == -3) {
		printf("ERROR: ");
		err = 1;
	}
	printf("%2i / %2i = %2i\n", 3, -1, divide(3, -1));

	printf("End Test\n\n");
	if (err) {
		exit(1);
	}
}

/**
 * Test bit manipulation helper methods.
 */
void testBitHelperMethods() {

	printf("Test Bit Helper Methods\n");
	// test clearBit(), setBit() and isBit()
	int i = 0, err = 0;

	// Print the size of an int.  This will vary based on processor.
	int byteLength = sizeof(int);
	int bitLength = byteLength * 8;
	printf("Size = %i bytes (%i bits)\n", byteLength, bitLength);
	// all we care about is the first 16 bits
	bitLength = 16;
	int word = pow(2, bitLength) - 1;

	for (i = 0; i < bitLength; i++) {
		if (clearBit(word, i) != word - pow(2, i)) {
			printf("ERROR: ");
			err = 1;
		}
		printf("%i clearBit %i -> %i\n", word, i, clearBit(word, i));
	}
	for (i = 0; i < bitLength; i++) {
		if (setBit(word, i) != word) {
			printf("ERROR: ");
			err = 1;
		}
		printf("%i setBit %i -> %i\n", word, i, setBit(word, i));
	}
	for (i = 0; i < bitLength; i++) {
		if (!isBit(word, i)) {
			printf("ERROR: ");
			err = 1;
		}
		printf("%i isBit %i -> %i\n", word, i, isBit(word, i));
	}
	word = 0;
	for (i = 0; i < bitLength; i++) {
		if (clearBit(word, i) != word) {
			printf("ERROR: ");
			err = 1;
		}
		printf("%i clearBit %i -> %i\n", word, i, clearBit(word, i));
	}
	for (i = 0; i < bitLength; i++) {
		if (setBit(word, i) != word + pow(2, i)) {
			printf("ERROR: ");
			err = 1;
		}
		printf("%i setBit %i -> %i\n", word, i, setBit(word, i));
	}
	for (i = 0; i < bitLength; i++) {
		if (isBit(word, i)) {
			printf("ERROR: ");
			err = 1;
		}
		printf("%i isBit %i -> %i\n", word, i, isBit(word, i));
	}

	printf("End Test\n\n");
	if (err) {
		exit(1);
	}
}

/**
 * Validate that various invalid moves are caught and dealt with appropriately.
 */
void testInvalidMoves() {
	printf("Test Invalid Moves\n");
	setup();

	int err = 0;
	Move_t move;
	// demonstrate bad row condition (row < 1)
	move.row = 0;
	move.col = 1;
	move.diag = 1;
	move.dir = 1;
	if (makeMove(move, 0)) {
		printf("ERROR: Previous invalid move not detected.\n");
		err = 1;
	}
	// demonstrate bad row condition (row > 8)
	move.row = 9;
	move.col = 1;
	move.diag = 1;
	move.dir = 1;
	if (makeMove(move, 0)) {
		printf("ERROR: Previous invalid move not detected.\n");
		err = 1;
	}
	// demonstrate bad column condition (col < 1)
	move.row = 1;
	move.col = 0;
	move.diag = 1;
	move.dir = 1;
	if (makeMove(move, 0)) {
		printf("ERROR: Previous invalid move not detected.\n");
		err = 1;
	}
	// demonstrate bad column condition (col > 8)
	move.row = 1;
	move.col = 9;
	move.diag = 1;
	move.dir = 1;
	if (makeMove(move, 0)) {
		printf("ERROR: Previous invalid move not detected.\n");
		err = 1;
	}
	// demonstrate unplayable start square condition
	move.row = 6;
	move.col = 1;
	move.diag = 1;
	move.dir = 1;
	if (makeMove(move, 0)) {
		printf("ERROR: Previous invalid move not detected.\n");
		err = 1;
	}
	// demonstrate unplayable start square condition
	move.row = 6;
	move.col = 7;
	move.diag = 1;
	move.dir = 1;
	if (makeMove(move, 0)) {
		printf("ERROR: Previous invalid move not detected.\n");
		err = 1;
	}
	// demonstrate no checker in start square condition
	move.row = 5;
	move.col = 1;
	move.diag = 1;
	move.dir = 1;
	if (makeMove(move, 0)) {
		printf("ERROR: Previous invalid move not detected.\n");
		err = 1;
	}
	// demonstrate no checker in start square condition
	move.row = 5;
	move.col = 7;
	move.diag = 1;
	move.dir = 1;
	if (makeMove(move, 0)) {
		printf("ERROR: Previous invalid move not detected.\n");
		err = 1;
	}
	// demonstrate move off the board
	move.row = 3;
	move.col = 1;
	move.diag = 0;
	move.dir = 1;
	if (makeMove(move, 0)) {
		printf("ERROR: Previous invalid move not detected.\n");
		err = 1;
	}
	// demonstrate move off the board
	move.row = 2;
	move.col = 8;
	move.diag = 1;
	move.dir = 1;
	if (makeMove(move, 0)) {
		printf("ERROR: Previous invalid move not detected.\n");
		err = 1;
	}
	// demonstrate wrong color checker in start square condition
	move.row = 7;
	move.col = 1;
	move.diag = 1;
	move.dir = 1;
	if (makeMove(move, 0)) {
		printf("ERROR: Previous invalid move not detected.\n");
		err = 1;
	}
	// demonstrate wrong color checker in start square condition
	move.row = 7;
	move.col = 7;
	move.diag = 1;
	move.dir = 1;
	if (makeMove(move, 0)) {
		printf("ERROR: Previous invalid move not detected.\n");
		err = 1;
	}
	printf("End Test\n");
	if (err) {
		printf("FAIL\n\n");
		exit(1);
	} else {
		printf("PASS\n\n");
	}
}

/**
 * Test a series of pre-defined games and verify consistent output.
 */
void testGameOutcomes() {
	playGame(1);
	printf(
			"Verify:\nPlayer O has no more checkers.\nPlayer X wins Game 001 in 37 moves!\n\n");

	playGame(2);
	printf(
			"Verify:\nPlayer X has no more checkers.\nPlayer O wins Game 002 in 88 moves!\n\n");

	playGame(14);
	printf(
			"Verify:\nPlayer O can't move.\nPlayer X wins Game 014 in 50 moves!\n\n");

	playGame(37);
	printf(
			"Verify:\nPlayer X can't move.\nPlayer O wins Game 037 in 43 moves!\n\n");

	playGame(4);
	printf(
			"Verify:\n100 moves taken with no winner.\nGame 004 is a draw!!\n\n");
}

/**
 * Play a set of pre-defined random games and print a summary
 * of the game wins and execution times.
 * Note that some game variables will affect the results.  For example,
 * increasing the maxMoves would result in fewer draws,
 * but would also make average game time longer.  Running in verbose mode
 * will result in longer game times as well.
 * During game competition, all game variables should be held constant
 * to produce good results.
 */
void evaluateGames() {
	int i, xWins, oWins, draws, numGames;
	xWins = oWins = draws = 0;
	numGames = 500;
	clock_t start = clock();
	for (i = 1; i <= numGames; i++) {
		playGame(i);
		if (winner == 1) {
			xWins++;
		} else if (winner == 0) {
			oWins++;
		} else if (winner == 2) {
			draws++;
		} else {
			printf("ERROR: Unknown game end status\n");
			exit(1);
		}
	}
	clock_t stop = clock();
	float duration = (stop - start);
	duration = (duration * 1000) / (CLOCKS_PER_SEC * numGames);
	printf(
			"Out of %i games, X won %i games, Y won %i games and %i games ended in a draw.\n",
			numGames, xWins, oWins, draws);
	printf("Games took an average of %0.3f ms each to play in %s mode\n\n",
			duration, verbose ? "verbose" : "summary");
}

/**
 * Run the test harness tests.
 */
void runTests() {
	testBasics();
	testBitHelperMethods();
	testInvalidMoves();
	testGameOutcomes();
}
