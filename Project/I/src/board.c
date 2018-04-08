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
    b->rowMask = (long int **)malloc(sizeof(long int *) * b->size);
    b->colMask = (long int **)malloc(sizeof(long int *) * b->size);
    b->boxMask = (long int **)malloc(sizeof(long int *) * b->size);


    
    if(b->gameBoard == NULL || b->rowMask == NULL || b->colMask == NULL || b->boxMask == NULL)
    {
        printf("Error creating board!\n");
        exit(EXIT_FAILURE);
    }

    /* Initializes all masks with 0 */
    for (int i = 0; i < b->size; ++i)
    {
        b->rowMask[i] = (long int *)malloc(sizeof(long int) * 3);
        b->colMask[i] = (long int *)malloc(sizeof(long int) * 3);
        b->boxMask[i] = (long int *)malloc(sizeof(long int) * 3);

        if( b->rowMask[i] == NULL || b->colMask[i] == NULL || b->boxMask[i] == NULL){
            printf("Error creating board!\n");
            exit(EXIT_FAILURE);
        }

        b->rowMask[i][0]=0;
        b->rowMask[i][1]=0;
        b->rowMask[i][2]=0;

        b->colMask[i][0]=0;
        b->colMask[i][1]=0;
        b->colMask[i][2]=0;

        b->boxMask[i][0]=0;
        b->boxMask[i][1]=0;
        b->boxMask[i][2]=0;
    }

    return 0;
}

/*****************************************************
*    Allocs and copies a game board and its masks    *
*                                                    *
* Returns: a game board                              *
*****************************************************/

Board *copyBoard(Board * original){

    Board * new = (Board *)malloc(sizeof(Board));
    if(new == NULL){
        printf("Error copying board \n");
        exit(EXIT_FAILURE);
    }

    new->size = original->size;
    new->squareSize = original->squareSize;

    /* Allocs memory to the game board and masks */
    allocBoard(new);
    
    /* Copies the original board to the new board */
    for(int i = 0 ; i < original->size*original->size ; i++){
        new->gameBoard[i].value = original->gameBoard[i].value;
        new->gameBoard[i].fixed = original->gameBoard[i].fixed;
        
        new->rowMask[row(i, original->size)][0] = original->rowMask[row(i, original->size)][0];
        new->rowMask[row(i, original->size)][1] = original->rowMask[row(i, original->size)][1];
        new->rowMask[row(i, original->size)][2] = original->rowMask[row(i, original->size)][2];

        new->colMask[col(i, original->size)][0] = original->colMask[col(i, original->size)][0];
        new->colMask[col(i, original->size)][1] = original->colMask[col(i, original->size)][1];
        new->colMask[col(i, original->size)][2] = original->colMask[col(i, original->size)][2];

        new->boxMask[box(i, original->squareSize)][0] = original->boxMask[box(i, original->squareSize)][0];
        new->boxMask[box(i, original->squareSize)][1] = original->boxMask[box(i, original->squareSize)][1];
        new->boxMask[box(i, original->squareSize)][2] = original->boxMask[box(i, original->squareSize)][2];
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

    int m_index = b->gameBoard[i].value/32;
    value -= (m_index*32);

    int row = row(i,b->size), col = col(i,b->size), box = box(i,b->squareSize);

    /* Removes masks in the respectvie row, column and box */
    b->rowMask[row][m_index] = removemask((b->rowMask[row][m_index]), value);
    b->colMask[col][m_index] = removemask((b->colMask[col][m_index]), value);
    b->boxMask[box][m_index] = removemask((b->boxMask[box][m_index]), value);
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

    int m_index = b->gameBoard[i].value/32;
    value -= (m_index*32);
    
    /* Adds masks in the respectvie row, column and box */
    b->rowMask[row][m_index] = addmask(b->rowMask[row][m_index], value);
    b->colMask[col][m_index] = addmask(b->colMask[col][m_index], value);
    b->boxMask[box][m_index] = addmask(b->boxMask[box][m_index], value);
}

/************************************************************************************
*       Checks if a certain value is valid in a given index at the given board       *
*                                                                                    *
* Returns: 1 if it's valid                                                           *
*          0 otherwise                                                               *
*************************************************************************************/

int checkValidityMasks(Board *b, int index, int value){

    int l = b->squareSize, totalMask, ret;
    int r = row(index,(l*l)), c = col(index,(l*l)), bo = box(index,l);
    
    int m_index = value/32;

    /* Mask of row, column and box*/
    long int mr  = b->rowMask[r][m_index];
    long int mc = b->colMask[c][m_index];
    long int mb = b->boxMask[bo][m_index];

    /* OR between masks */
    totalMask = ((mr | mc) | mb);

    /* Checks if that is a valid number for that index based on masks */
    ret = valid(value, totalMask);

    return ret;
}

/**************************************************************************
*       Checks if there are duplicates in each row, column and box        *
*                                                                         *
* Returns: TRUE if there are no duplicates                                *
*          FALSE otherwise                                                *
**************************************************************************/

int checkEachCell(Board *b, int index)
{
    int i = 0, l = row(index, b->size), c = col(index, b->size), result = TRUE;
    int bline = off(l, b->squareSize), bcol = off(c, b->squareSize);

    int currb = 0, currl = no(0, l, b->size), currc = no(c, 0, b->size);

    int *existInRow = (int*)malloc(b->size * sizeof(int));
    int *existInCol = (int*)malloc(b->size * sizeof(int));
    int *existInBox = (int*)malloc(b->size * sizeof(int));

    if(existInRow == NULL || existInCol == NULL || existInBox  == NULL){
        printf("Error checking board \n");
        exit(EXIT_FAILURE);
    }
    for(i = 0; i < b->size; i++)
    {
        existInRow[i] = FALSE;
        existInCol[i] = FALSE;
        existInBox[i] = FALSE;
    }

    for(i = 0; i < b->size; i++)
    {
        currc = no(c, i, b->size);
        currl = no(i, l, b->size);
        currb = no(bcol+i, bline, b->size);

        /* Checks if are duplciates in the index row */
        if(b->gameBoard[currl].value != 0)
        {
            if(existInRow[b->gameBoard[currl].value-1] == TRUE)
            {
                result = FALSE;
                break;
            }
            existInRow[b->gameBoard[currl].value-1] = TRUE;
        }

        /* Checks if are duplciates in the index column */
        if(b->gameBoard[currc].value != 0)
        {
            if(existInCol[b->gameBoard[currc].value-1] == TRUE)
            {
                result = FALSE;
                break;
            }
            existInCol[b->gameBoard[currc].value-1] = TRUE;
        }

        /* Checks if are duplciates in the index box */
        if(b->gameBoard[currb].value != 0)
        {
            if(existInBox[b->gameBoard[currb].value -1] == TRUE)
            {
                result = FALSE;
                break;
            }
            existInBox[b->gameBoard[currb].value -1] = TRUE;
        }

        if(((i+1)%b->squareSize) == 0)
        {
            bcol -= b->squareSize;
            bline++;
        }
    }

    free(existInRow);
    free(existInCol);
    free(existInBox);

    return result;
}

/*********************************************************************************************************
*        Checks for each cell in a given game board has no duplicates in each row, column and box        *
*                                                                                                        *
* Returns: TRUE if there are no duplicates                                                               *
*          FALSE otherwise                                                                               *
*********************************************************************************************************/

int checkAllBoard(Board *b)
{
    int i = 0;

    for(i = 0; i < b->size * b->size; i++)
    {
        if(checkEachCell(b, i) == FALSE)
        {
            return FALSE;
        }
    }

    return TRUE;
}

/***********************************************************************
*       Fills the given game board according to the input file         *
*                                                                      *
* Returns: 0 on success                                                *
*          -1 on error                                                 *
***********************************************************************/

int fillGameBoard(Board *board, char const* file)
{
    int i = 0, j = 0, index = 0, box = 0, m_index = 0, value = 0;
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
        if(fscanf(fptr, "%d\n", (int*)&(board->squareSize)) != 1)
        {
            printf("Board size not specified in the file!\n");
            fclose(fptr);
            return -1;
        }
        else if(board->squareSize <= MAX_SQUARE_SIZE && board->squareSize >= MIN_SQUARE_SIZE)
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
            freeBoard(board);
            fclose(fptr);
            return -1;
        }

        /* Get every line from the file and assign it to a vector of integers, then fill the board */
        while(fgets(line, MAX, fptr) != NULL)
        {
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
                        board->gameBoard[index].fixed = TRUE;
                        value = atoi(aux);
                        m_index = value/32;
                        value -= m_index*32;

                        /* Add masks to the game board */
                        board->rowMask[i][m_index] = addmask(board->rowMask[i][m_index], value);
                        board->colMask[j][m_index] = addmask(board->colMask[j][m_index], value);
                        board->boxMask[box][m_index] = addmask(board->boxMask[box][m_index], value);
                    }

                    board->gameBoard[index].value = atoi(aux);
                    j++;
                }
                else
                {
                    printf("There is an invalid number on the board game, check the file!\n");
                    free(line);
                    freeBoard(board);
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
    
    for (int i = 0; i < b->size; ++i)
    {        
        free(b->rowMask[i]);
        free(b->colMask[i]);
        free(b->boxMask[i]);
    }
    
    free(b->rowMask);
    free(b->colMask);
    free(b->boxMask);
}
