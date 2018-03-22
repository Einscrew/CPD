#ifndef BOARD
#define BOARD

#define MAX_SQUARE_SIZE 9 /* Max value of l */
#define MIN_SQUARE_SIZE 2 /* Min value of l */
#define MAX 100
#define TRUE 1
#define FALSE 0
#define FIXED 2

#include "utils.h"

typedef struct Cell{

    int value;
    int countPossibilities;
    int fixed; /* To know if it's a fixed value in the board game */
    int numberGuesses;
    int *possibleValues;

}Cell;

typedef struct Board{

    Cell **gameBoard;
    int size;
    int squareSize;

}Board;


int allocBoard(Board * b);
int fillGameBoard(Board *board, char const* file);
int checkValidity(Cell *aux, int size);
int checkAllBoard(Board *board);
void freeBoard(Board *board);


void createVectorPossibilities(Cell *aux, int size);
void updateBoardValues(Board *board, int row, int col, int value);
void printBoard(Cell **board, int size);

#endif
