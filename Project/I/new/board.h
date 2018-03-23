#ifndef BOARD
#define BOARD

#define MAX_SQUARE_SIZE 9 /* Max value of l */
#define MIN_SQUARE_SIZE 2 /* Min value of l */
#define MAX 100
#define TRUE 1
#define FALSE 0

#include "utils.h"

#define no(x,y,l) (y*l+x)

typedef struct Cell{
    int value;
    int fixed;
}Cell;


typedef struct Board{

    Cell *gameBoard;
    int size;
    int squareSize;

}Board;


int allocBoard(Board * b);
int checkValidity(Board * b, int size);
int checkAllBoard(Board *b);
void freeBoard(Board *b);


int fillGameBoard(Board *b, char const* file);
void printBoard(Board * b);

#endif
