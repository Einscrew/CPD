#ifndef STRATEGIES
#define STRATEGIES

#include "board.h"

void taskBruteForce(Board *b, int start, int numThreads, int threshold);
int solver(Board *b);
int bruteforce(Board *b, int start);

#endif
