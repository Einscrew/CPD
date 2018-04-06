#ifndef STRATEGIES
#define STRATEGIES

#include "board.h"

int taskBruteForce(Board *b, int start, int numThreads, int threshold);
int solver(Board *b);
int bruteforce(Board *b, int start);

#endif
