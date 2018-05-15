#ifndef SUDOKU_MPI
#define SUDOKU_MPI

#include "board.h"


extern int emptyCells;

int bruteForce(Board *b, int start, int id, int p, int askWork);

#endif
