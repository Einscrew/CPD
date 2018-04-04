#ifndef STRATEGIES
#define STRATEGIES

#include "board.h"
#include "queue.h"


int solver(Board *b);
void createTask(Board *b, int level, int bottom);
void search(Board *b, int index);
int bruteforce(Board *b, int start);

#endif
