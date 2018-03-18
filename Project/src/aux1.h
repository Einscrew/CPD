#ifndef AUX1
#define AUX1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SIZE 9 /* Max value of l */
#define MIN_SIZE 2 /* Min value of l */
#define MAX 100
#define TRUE 1
#define FALSE 0


typedef struct Cell{

    int value;
    int *possibleValues;
    int countPossibilities;

}Cell;

typedef struct Board{

    Cell **gameBoard;
    int size;

}Board;

int checkValidity(Cell *aux, int size);
int checkAllBoard(Board *board);
int checkBoardComplete(Board *board);
void freeBoard(Board *board);


#endif