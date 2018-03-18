#ifndef STRATEGIES
#define STRATEGIES

#include "aux1.h"

void updateBoardValues(Board *board, int row, int col, int value);
int checkValidity(Cell *aux, int size);
int checkAllBoard(Board *board);
void deleteStrategy(Board *board);

#endif