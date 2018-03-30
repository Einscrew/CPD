#ifndef BOARD
#define BOARD

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>
//#include "utils.h"


#define MAX_SQUARE_SIZE 9 /* Max value of l */
#define MIN_SQUARE_SIZE 2 /* Min value of l */
#define MAX 100
#define TRUE 1
#define FALSE 0

#define no(x,y,l) (y*l+x)

typedef struct Cell{

    int value;
    int fixed;
    int countPossibilities;
    int minPoss;
    //int numberGuesses;

}Cell;

typedef struct Board{

    Cell *gameBoard;
    int size;
    int squareSize;

}Board;

Board *allocBoard(Board *b);
int checkValidity(Board *b, int size);
int checkAllBoard(Board *b);
void freeBoard(Board *b);

int fillGameBoard(Board *b, char const* file);
int updatePossibilities(Board *board);
void printBoard(Board *b);
Board *copyBoard(Board *b, Board *solution);

#endif
