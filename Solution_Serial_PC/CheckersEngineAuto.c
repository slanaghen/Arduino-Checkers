/*
 * CheckersEngineAuto.c
 */

#include "Core.h"
#include "Checkers.h"
#include "CheckersEngine.h"

/**
 * Make an automatically generated move.
 */
int takeAutoTurn() {
	// the maximum number of "best moves" to pick from in automated play
	const int MaxBestMoves = 10;
	Move_t move;
	Move_t bestMoves[MaxBestMoves];
	int score = 0;
	int bestScore = 0;
	int i = 0;
	// save current board state
	unsigned char originalCheckers[8];
	unsigned char originalColors[8];
	unsigned char originalKings[8];
	for (i = 0; i < 8; i++) {
		originalCheckers[i] = checkers[i];
		originalColors[i] = colors[i];
		originalKings[i] = kings[i];
	}

	// NOTE: moving from least row/col to greatest, and greatest dir/diag to least will
	// produce a different game if these orders are changed.

	// for each of players' checkers.
	for (move.row = 8; move.row >= 0; move.row--) {	// for each row on the board
		for (move.col = 8; move.col >= 0; move.col--) {	// while there are any bit sets still to check...
			// if board square has player's checker...
			if (squareHasPlayerChecker(move.row, move.col, player)) {
				// try each direction of move, score it and save it if it is the best move.
				for (move.dir = 0; move.dir <= 1; move.dir++) {
					// cannot move backward unless checker is crowned
					if (move.dir == 0
							&& !squareHasPlayerKing(move.row, move.col,
									player)) {
						continue;
					}
					// try each diagonal of move
					for (move.diag = 1; move.diag >= 0; move.diag--) {
						// start with original board state
						for (i = 0; i < 8; i++) {
							checkers[i] = originalCheckers[i];
							colors[i] = originalColors[i];
							kings[i] = originalKings[i];
						}
						// score it and save it if it is a best move.
						score = makeMove(move, 1);
						if (score >= bestScore) {
							if (score > bestScore) {
								bestScore = score;
								// new best score, clear out all old best scores
								for (i = 0; i < MaxBestMoves; i++) {
									bestMoves[i].score = 0;
									bestMoves[i].row = 0;
									bestMoves[i].col = 0;
									bestMoves[i].diag = 0;
									bestMoves[i].dir = 0;
								}
							}
							// put this best move in the next available spot in the array of best moves
							for (i = 0; i < MaxBestMoves; i++) {
								if (bestMoves[i].score == 0) {
									bestMoves[i].score = score;
									bestMoves[i].row = move.row;
									bestMoves[i].col = move.col;
									bestMoves[i].diag = move.diag;
									bestMoves[i].dir = move.dir;
									break;
								}
							}
						}
					}
				}
			}
		}
	}
	// if any move is possible...
	if (bestScore > 0) {
		// pick a random move from the best moves identified
		for (i = 1; i <= MaxBestMoves; i++) {
			if (bestMoves[subtract(i, 1)].score == 0) {
				break;
			}
		}
		i--;
		srand(gameNumber);
		int index = rand() % i;
		// start with original board state
		for (i = 0; i < 8; i++) {
			checkers[i] = originalCheckers[i];
			colors[i] = originalColors[i];
			kings[i] = originalKings[i];
		}

		// actually take a highest scoring move
		score = makeMove(bestMoves[index], 0);

		// did this move remove all opponent's checkers and end the game?
		if (boardHasOpponentCheckers(player) == 0) {
			winner = player;
			printf("Player %s has no more checkers.\n", player ? "O" : "X");
		}
		// else if no move is possible
	} else {
		winner = !player;
		printf("Player %s can't move.\n", player ? "X" : "O");
	}
	return score;
}
