#ifndef STRATEGIES
#define STRATEGIES

#include "board.h"
#include "queue.h"
#include <omp.h>

void testQueue(Board *b);


int solver(Board *b);
int bruteforce(Board *b);

#endif
