#ifndef STRATEGIES
#define STRATEGIES

#include <omp.h>
#include "board.h"
#include "stack.h"

Stack *bruteforce(Board *b, Stack *head);
Board *solveSudoku(Stack *head, Board *b, Board *solution);
int incPosition(Board *b, int index);
int decPosition(Board *b, int index);

#endif

