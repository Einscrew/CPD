#ifndef STRATEGIES
#define STRATEGIES

#include "board.h"

void taskBruteForce(Board *b, int start, int numThreads, int threshold);
void printMasks(long int ** m, int index);
int solver(Board *b);

#endif
