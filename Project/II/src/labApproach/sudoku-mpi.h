#ifndef SUDOKU_SERIAL
#define SUDOKU_SERIAL

#include "board.h"

int bruteForce(Board *b, int start, int id, int p, char * fid, MPI_Request * r);

#endif
