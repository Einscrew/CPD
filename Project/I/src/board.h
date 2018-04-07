#ifndef BOARD
#define BOARD

#define MAX_SQUARE_SIZE 9 /* Max value of l */
#define MIN_SQUARE_SIZE 2 /* Min value of l */
#define MAX 100
#define TRUE 1
#define FALSE 0

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define no(x,y,l) (y*l+x)
#define row(n,l) (n/l)
#define col(n,l) (n%l)
#define off(i,sl) ((i / sl) * sl)
#define box(i, bs) (((row(i,(bs*bs))/bs) * bs) + (col(i,(bs*bs))/bs))

#define mask(n) ( 1 << (n - 1) )
#define addmask(m,a) ( m | mask(a) )
#define valid(v, m) (((mask(v) & m) == 0)?1:0)
#define removemask(m,r) ( m & (~(mask(r))) )


typedef struct Cell{
    char value;
    char fixed;
}Cell;


typedef struct Board{

    Cell *gameBoard;
    long int * rowMask;
	long int * colMask;
    long int * boxMask;	
    char size;
    char squareSize;

}Board;


int allocBoard(Board * b);
Board * copyBoard(Board* original);

void updateMasks(Board * b, int i);
void removeMasks(Board * b, int i);

int checkValidityMasks(Board * b, int size, int value);
int checkAllBoard(Board *b);
void freeBoard(Board *b);


int fillGameBoard(Board *b, char const* file);
void printBoard(Board * b);

#endif
