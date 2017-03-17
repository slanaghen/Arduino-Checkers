/*
 * Core.h
 */

#ifndef CORE_H_
#define CORE_H_

#include <stdlib.h>

// Basic function declarations
int add(int x, int y);
int subtract(int x, int y);
int multiply(int x, int y);
int divide(int x, int y);
int negate(int x);

int isGreaterThan(int x, int y);
int isLessThan(int x, int y);
int isEven(int x);

// Bit Helper function declarations
int setBit(int word, int bit);
int clearBit(int word, int bit);
int isBit(int word, int bit);

#endif /* CORE_H_ */
