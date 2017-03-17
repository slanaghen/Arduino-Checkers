/*
 * CheckersEngine.h
 */

#ifndef CHECKERSENGINE_H_
#define CHECKERSENGINE_H_

// The identifier for the game
int gameNumber;

void setGameNumber(int gameNo);
int makeMove(Move_t move, int attemptOnly);
int takeTurn();

#endif /* CHECKERSENGINE_H_ */
