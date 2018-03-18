#ifndef STRATEGIES
#define STRATEGIES

#include "aux1.h"

void updateBoardValues(Board *board, int row, int col, int value);
void deleteStrategy(Board *board);
int bruteForceStrategy(Board *board);
int solveSudoku(Board *board);

#endif