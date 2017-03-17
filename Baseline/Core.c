/*
 * Core.c
 *
 * This program only uses the bitwise operators !,~,&,^,|,<<,>>
 * in basic methods with the exception of add() and multiply().
 * The basic methods form the core game logic for classic board games.
 */

#include <stdlib.h>
#include "Core.h"

// use a looping method (do/while) for add() and multiply(), or
// use a recursive method for add() and multiply().
#define loop	// ERROR: infinite loop with faulty addByRecursing() will cause SEGV
#ifdef loop
#define ADDMETHOD addByLooping
#define MULTIPLYMETHOD multiplyByLooping
#else
#define ADDMETHOD addByRecursing
#define MULTIPLYMETHOD multiplyByRecursing
#endif

/******************************************************************/
/*********************** Basic Methods ****************************/
/******************************************************************/
/**
 * Add two numbers together.
 */
int addByLooping(int x, int y) {
	int a, b;
	do {
		a = x & y;
		b = x ^ y;
		x = a << 1;
		y = b;
	} while (a);
	return b;
}

/**
 * Add two numbers together using recursion
 */
int addByRecursing(int x, int y) {
	int next_x = x ^ y;
	int carry = (x & y) << 1;
//	next_x=addByRecursing(next_x,carry);	//ERROR: this causes infinite recursion
	if (carry)
		next_x = addByRecursing(next_x, carry); // SOLUTION: this fixes the infinite recursion
	return next_x;
}

/**
 * Add two numbers together
 */
int add(int x, int y) {
	return ADDMETHOD(x, y);
}

/**
 * Negate a number.
 */
int negate(int x) {
	return add(~x, 1);
}

/**
 * Subtract one number from another.
 */
int subtract(int x, int y) {
	return add(x, negate(y));
}

/**
 * Multiply one number by another by looping.
 */
int multiplyByLooping(int x, int y) {
	int m = 1, z = 0;
	if (x < 0) {
		x = negate(x);
		y = negate(y);
	}
	do {
		if (x & m)
			z = add(y, z);
		y <<= 1;
		m <<= 1;
	} while (x >= m && y);
	return z;
}

/**
 * Multiply one number by another by recursing.
 */
int multiplyByRecursing(int x, int y) {
	if (y > 0)
		return (add(x, multiplyByRecursing(x, subtract(y, 1))));
	if (y < 0)
		return negate(multiplyByRecursing(x, negate(y)));
	else
		return 0;
}

/**
 * Multiply one number by another
 */
int multiply(int x, int y) {
	return MULTIPLYMETHOD(x, y);
}

/**
 * Divide one number by another.
 * The integer value is returned and the remainder is ignored.
 */
int divide(int x, int y) {
	int c = 0, sign = 0;
	if (x < 0) {
		x = negate(x);
		sign ^= 1;
	}
	if (y < 0) {
		y = negate(y);
		sign ^= 1;
	}
	if (y != 0) {
		while (x >= y) {
			x = subtract(x, y);
			++c;
		}
	}
	if (sign) {
		c = negate(c);
	}
	return c;
}

/**
 *  if x>y, then return 1, else return 0
 */
int isGreaterThan(int x, int y) {
	int byteLength = sizeof(int);
	int bitLength = byteLength * 8;
	return add(y, add(~x, 1)) >> subtract(bitLength, 1) & 1;
}

/**
 *  if x<y, then return 1, else return 0
 */
int isLessThan(int x, int y) {
	return !isGreaterThan(x, y);
}

/**
 * if x is even, return non-zero, else return 0
 */
int isEven(int x) {
	return !(x & 1);
}

/******************************************************************/
/********************** Bit Helper Methods ************************/
/******************************************************************/
/**
 * Set the specified bit in the given byte.
 */
int setBit(int byte, int bit) {
	return byte | (1 << bit);
}

/**
 * Clear the specified bit in the given byte.
 */
int clearBit(int byte, int bit) {
	return byte & ~(1 << bit);
}

/**
 * Is the specified bit in the given byte set.
 */
int isBit(int byte, int bit) {
	return (byte & (1 << bit)) > 0;
}

