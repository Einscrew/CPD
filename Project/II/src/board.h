#ifndef BOARD
#define BOARD


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_SQUARE_SIZE 9 /* Max value of l */
#define MIN_SQUARE_SIZE 2 /* Min value of l */
#define MAX 300
#define TRUE 1
#define FALSE 0


#define BOARD_FACTOR 1000000

/* Used to convert matrix indexes to unidimensional arrays */
#define no(x,y,l) (y*l+x)
#define row(n,l) (n/l)
#define col(n,l) (n%l)
#define off(i,sl) ((i / sl) * sl)
#define box(i, bs) (((row(i,(bs*bs))/bs) * bs) + (col(i,(bs*bs))/bs))

/* Used to add, remove, update and check masks */
#define mask(n) ( 1 << (n - 1) )
#define addmask(m,v) ( m | mask(v) )
#define valid(v, m) (((mask(v) & m) == 0)?1:0)
#define removemask(m,r) ( m & (~(mask(r))) )

typedef struct Cell{
    char value;
    char fixed;
}Cell;

typedef struct Board{

    Cell *gameBoard;
    long int ** rowMask;
	long int ** colMask;
    long int ** boxMask;	
    char size;
    char squareSize;
    int fixed;

}Board;

int allocBoard(Board * b);
Board * copyBoard(Board* original);
int makeCopyBoard(Board* dst, Board * src);
void updateMasks(Board * b, int i);
void removeMasks(Board * b, int i);
int checkValidityMasks(Board * b, int size, int value);
int fillGameBoard(Board *b, char const* file);
int compressBoard(Board * board, int fixed, int index, char ** r);
int decompressBoard(Board * b, char * r, int s, int fixed);
void printBoard(Board * b);
void printBoardT(Board *b, int id);
void printBM(Board *b);
void freeBoard(Board *b);

#endif
