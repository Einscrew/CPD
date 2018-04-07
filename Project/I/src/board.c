#include "board.h"

/******************************************
*    Allocs a game board and its masks    *
*                                         *
* Returns: 0 on success                   *
*          -1 on error                    *
******************************************/

int allocBoard(Board *b)
{
	/* Allocs memory for the game board and the masks */
    b->gameBoard = (Cell*)malloc(b->size*b->size*sizeof(Cell));
    b->rowMask = (int *)malloc(b->size*b->size * sizeof(int));
    b->colMask = (int *)malloc(b->size*b->size * sizeof(int));
    b->boxMask = (int *)malloc(b->size*b->size * sizeof(int));

    /* Initializes all masks with 0 */
    for (int i = 0; i < b->size; ++i)
    {
        b->rowMask[i] = 0;
        b->colMask[i] = 0;
        b->boxMask[i] = 0;
    }   

    if(b->gameBoard == NULL)
    {
        printf("Error creating board!\n");
        return -1;
    }

    return 0;
}

/*****************************************************
*    Allocs and copies a game board and its masks    *
*                                                    *
* Returns: a game board                              *
*****************************************************/

Board *copyBoard(Board * original)
{
    Board * new = (Board *)malloc(sizeof(Board));

    new->size = original->size;
    new->squareSize = original->squareSize;

    /* Allocs memory to the game board and masks */
    allocBoard(new);
    
    /* Copies the original board to the new board */
    for(int i = 0 ; i < original->size*original->size ; i++)
    {
        new->gameBoard[i].value = original->gameBoard[i].value;
        new->gameBoard[i].fixed = original->gameBoard[i].fixed;
        
        new->rowMask[row(i, original->size)] = original->rowMask[row(i, original->size)];
        new->colMask[col(i, original->size)] = original->colMask[col(i, original->size)];
        new->boxMask[box(i, original->squareSize)] = original->boxMask[box(i, original->squareSize)];
    }

    return new;
}

/*****************************************************
*       Removes a mask in a row, column and box      *                           *
*****************************************************/

void removeMasks(Board * b, int i)
{
    int value = b->gameBoard[i].value;

    if(value == 0)
    {
        return;
    }

    int row = row(i,b->size), col = col(i,b->size), box = box(i,b->squareSize);
    
    /* Removes masks in the respectvie row, column and box */
    b->rowMask[row] = removemask((b->rowMask[row]), value);
    b->colMask[col] = removemask((b->colMask[col]), value);
    b->boxMask[box] = removemask((b->boxMask[box]), value);
}

/*****************************************************
*       Updates a mask in a row, column and box      *                           *
*****************************************************/

void updateMasks(Board * b, int i)
{
    int row = row(i,b->size), col = col(i,b->size), box = box(i,b->squareSize);
    int value = b->gameBoard[i].value;

     if(value == 0)
     {
        return;
    }
    
    /* Updates masks in the respectvie row, column and box */
    b->rowMask[row] = addmask(b->rowMask[row], value);
    b->colMask[col] = addmask(b->colMask[col], value);
    b->boxMask[box] = addmask(b->boxMask[box], value);
}
            
/*************************************************************************************
*       Checks if a certain value is valid in a given index at the given board       *
*																					 *
* Returns: 1 if it's valid 															 *
*		   0 otherwise 																 *
*************************************************************************************/

int checkValidity(Board *b, int index, int value){

    int l = b->squareSize, totalMask, ret;
    int r = row(index,(l*l)), c = col(index,(l*l)), bo = box(index,l);

    /* Mask of row, column and box*/
    int mr  = b->rowMask[r];
    int mc = b->colMask[c];
    int mb = b->boxMask[bo];

    /* OR between masks */
    totalMask = ((mr | mc) | mb);

    /* Checks if that is a valid number for that index based on masks */
    ret = valid(value, totalMask);

    return ret;
}

/***********************************************************************
*       Fills the given game board according to the input file         *
*																	   *
* Returns: 0 on success												   *
*		   -1 on error   											   *
***********************************************************************/

int fillGameBoard(Board *board, char const* file)
{
    int i = 0, j = 0, index = 0, box = 0;
    char *line = NULL;
    char *aux = NULL;
    FILE *fptr = fopen(file, "r");

    if(fptr == NULL)
    {
        printf("Error reading load file!\n");
        return -1;
    }
    else
    {
        /* Gets the board size */
        if(fscanf(fptr, "%d\n", (int*)&(board->squareSize)) != 1){
            printf("Board size not specified in the file!\n");
            fclose(fptr);
            return -1;
        }
        else if(board->squareSize <= MAX_SQUARE_SIZE && board->squareSize >= MIN_SQUARE_SIZE) /* Checks if the square size is between the limits */
        {
            board->size = board->squareSize*board->squareSize;

            if(allocBoard(board) != 0)
            {
                fclose(fptr);
                return -1;
            }
        }
        else
        {
            printf("Invalid size of board game, please check if the file is correct!\n");
            fclose(fptr);
            return -1;
        }

        line = (char*)malloc(MAX * sizeof(char));
        if(line == NULL)
        {
            printf("Error on malloc!\n");
            fclose(fptr);
            return -1;
        }

        /* Get every line from the file and assign it to a vector of integers, then fill the board */
        while(fgets(line, MAX, fptr) != NULL)
        {
        	/* Gets each number of each cell */
            aux = strtok(line, " "); 

            /* Reads until the \n */
            while(aux && j < (board->size))
            {
                if(atoi(aux) >= 0 && atoi(aux) <= (board->size))
                {
                    index = no(j,i,board->size);
                    box = box(index, board->squareSize);

                    /* Checks if it's a fixed given value (!= from 0) */
                    if(atoi(aux) == 0)
                    {
                        board->gameBoard[index].fixed = FALSE;
                    }
                    else
                    {
                    	/* Add masks to the game board */
                        board->gameBoard[index].fixed = TRUE;
                        board->rowMask[i] = addmask(board->rowMask[i], atoi(aux));
                        board->colMask[j] = addmask(board->colMask[j], atoi(aux));
                        board->boxMask[box] = addmask(board->boxMask[box], atoi(aux));
                    }

                    board->gameBoard[index].value = atoi(aux);
                    j++;
                }
                else
                {
                    printf("There is an invalid number on the board game, check the file!\n");
                    fclose(fptr);
                    return -1;
                }

                aux = strtok(NULL, " ");
            }
            j=0;
            i++;
        }
    }

    fclose(fptr);
    free(line);
    return 0;
}

/**********************************
*       Prints a game board       *
**********************************/

void printBoard(Board *b)
{
    int i = 0;
    
    for(i = 0; i < b->size*b->size; i++)
    {
        printf("%d ", b->gameBoard[i].value);

        if((i+1)%(b->size) == 0){

            printf("\n");
        }
    }
}

/*********************************************
*       Frees a game board and its masks     *
*********************************************/

void freeBoard(Board *b)
{
    free(b->gameBoard);
    free(b->rowMask);
    free(b->colMask);
    free(b->boxMask);
}