/*
 * CheckersEngineTest.h
 *
 *  Created on: Feb 27, 2013
 *      Author: sgl
 */

#ifndef CHECKERSENGINETEST_H_
#define CHECKERSENGINETEST_H_

/*
 * CheckersEngine.c
 *
 *  Created on: Jan 11, 2013
 *      Author: sgl
 */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "Core.h"
#include "Checkers.h"
#include "CheckersEngine.h"

// INSTRUCTOR ONLY
#include <math.h>
#include <time.h>

void playGame();
void testBasics();
void testBitHelperMethods();
void testInvalidMoves();
void playGame(int seed);
void testGameOutcomes();
void evaluateGames();
void runTests();

#endif /* CHECKERSENGINETEST_H_ */
