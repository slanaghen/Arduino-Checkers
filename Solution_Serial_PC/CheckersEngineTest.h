/*
 * CheckersEngineTest.h
 */

#ifndef CHECKERSENGINETEST_H_
#define CHECKERSENGINETEST_H_

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "Core.h"
#include "Checkers.h"
#include "CheckersEngine.h"

// INSTRUCTOR ONLY
#include <math.h>
#include <time.h>

void playGame(int gameNo);

void testBasics();
void testBitHelperMethods();
void testInvalidMoves();
void testGameOutcomes();
void evaluateGames();
void runTests();

#endif /* CHECKERSENGINETEST_H_ */
