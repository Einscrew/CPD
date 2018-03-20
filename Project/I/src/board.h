#ifndef BOARD
#define BOARD

#define MAX_SQUARE_SIZE 9 /* Max value of l */
#define MIN_SQUARE_SIZE 2 /* Min value of l */
#define MAX 100
#define TRUE 1
#define FALSE 0

#include "utils.h"


typedef struct Cell{

    int value;
    int *possibleValues;
    int countPossibilities;

}Cell;


typedef struct Board{

    Cell **gameBoard;
    int size;
    int squareSize;

}Board;


int allocBoard(Board * b);
int checkValidity(Cell *aux, int size);
int checkAllBoard(Board *board);
void freeBoard(Board *board);


void createVectorPossibilities(Cell *aux, int size);
int fillGameBoard(Board *board, char const* file);
void updateBoardValues(Board *board, int row, int col, int value);
void printBoard(Cell **board, int size);

#endif
