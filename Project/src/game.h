#ifndef GAME
#define GAME

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aux1.h"
#include "strategies.h"

int fillGameBoard(Board *board, char const* file);
void printBoard(Cell **board, int size);

#endif