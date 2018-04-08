/**************************************************************************
 *                           sudoku-omp.h                                 *
 **************************************************************************                                                                   
 * Declaration of functions used in sudoku-omp.c                          *
 *************************************************************************/

#ifndef STRATEGIES
#define STRATEGIES

#include "board.h"

void taskBruteForce(Board *b, int start, int numThreads, int threshold);
void printMasks(long int ** m, int index);
void solver(Board *b);

#endif
