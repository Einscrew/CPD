#include "board.h"

int emptyCells=0;
int originalEmptyCells = 0;

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

    /* Initializes all masks with 0 */
    for (int i = 0; i < b->size; ++i)
    {
        b->rowMask[i] = (long int *)malloc(sizeof(long int ) * 3);
        b->rowMask[i][0]=0;
        b->rowMask[i][1]=0;
        b->rowMask[i][2]=0;

        b->colMask[i] = (long int *)malloc(sizeof(long int ) * 3);
        b->colMask[i][0]=0;
        b->colMask[i][1]=0;
        b->colMask[i][2]=0;

        b->boxMask[i] = (long int *)malloc(sizeof(long int ) * 3);
        b->boxMask[i][0]=0;
        b->boxMask[i][1]=0;
        b->boxMask[i][2]=0;
    }

    if(b->gameBoard == NULL)
    {
        printf("Error creating board!\n");
        return -1;
    }
    return 0;
}

int makeCopyBoard(Board* dst, Board * src){
    int i = 0;
    dst->size = src->size;
    dst->squareSize = src->squareSize;
    
    /* Copies the original board to the new board */
    for(i = 0 ; i < src->size*src->size ; i++){
        dst->gameBoard[i].value = src->gameBoard[i].value;
        dst->gameBoard[i].fixed = src->gameBoard[i].fixed;
    }

    for (i = 0; i < src->size; i++)
    {
        dst->rowMask[i][0] = src->rowMask[i][0];
        dst->rowMask[i][1] = src->rowMask[i][1];
        dst->rowMask[i][2] = src->rowMask[i][2];

        dst->colMask[i][0] = src->colMask[i][0];
        dst->colMask[i][1] = src->colMask[i][1];
        dst->colMask[i][2] = src->colMask[i][2];

        dst->boxMask[i][0] = src->boxMask[i][0];
        dst->boxMask[i][1] = src->boxMask[i][1];
        dst->boxMask[i][2] = src->boxMask[i][2];
    }

    return 1;

}


/*****************************************************
*    Allocs and copies a game board and its masks    *
*                                                    *
* Returns: a game board                              *
*****************************************************/

Board * copyBoard(Board * original){

    Board * new = (Board *)malloc(sizeof(Board));
    int i = 0;

    new->size = original->size;
    new->squareSize = original->squareSize;

    /* Allocs memory to the game board and masks */
    allocBoard(new);
    
    /* Copies the original board to the new board */
    for(i = 0 ; i < original->size*original->size ; i++){
        new->gameBoard[i].value = original->gameBoard[i].value;
        new->gameBoard[i].fixed = original->gameBoard[i].fixed;
    }
    for (i = 0; i < original->size; i++)
    {
        new->rowMask[i][0] = original->rowMask[i][0];
        new->rowMask[i][1] = original->rowMask[i][1];
        new->rowMask[i][2] = original->rowMask[i][2];

        new->colMask[i][0] = original->colMask[i][0];
        new->colMask[i][1] = original->colMask[i][1];
        new->colMask[i][2] = original->colMask[i][2];

        new->boxMask[i][0] = original->boxMask[i][0];
        new->boxMask[i][1] = original->boxMask[i][1];
        new->boxMask[i][2] = original->boxMask[i][2];
    }
    return new;
}

/*****************************************************
*       Removes a mask in a row, column and box      *                           
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
*       Updates a mask in a row, column and box      *                           
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
                        emptyCells++;
                        originalEmptyCells++;
                    }else
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




int compressBoard(Board * b, int fixed, int index, char ** r){
    int current = 0, area = b->size*b->size;

    char * comp = malloc( (sizeof(int)+sizeof(char)) * area);


    for (int i = 0; i < area; ++i)
    {
        if(!fixed && index < i)
            break;

        if((b->gameBoard[i].fixed && fixed) || (!b->gameBoard[i].fixed && !fixed)){
            memcpy(&comp[current], &i, sizeof(int));
            current+=sizeof(int);
            memcpy(&comp[current], &(b->gameBoard[i].value), sizeof(char));
            current+=sizeof(char);
        }
    }
    *r = comp;
    
    return current;
}


int decompressBoard(Board * b, char * r, int s, int fixed){
    int current = 0, index = 0, i , area=b->size * b->size, m_index, value;

    if(fixed)
        emptyCells = area;
    else
        emptyCells = originalEmptyCells;

    if(b->gameBoard == NULL){
        allocBoard(b);
        for ( i = 0; i < area; ++i)
        {
            b->gameBoard[i].value = 0;
            b->gameBoard[i].fixed = FALSE;
        }
    }
    
    
    while(current < s)
    {
        memcpy(&index, &r[current], sizeof(int));
        current += sizeof(int);
        value = r[current];
        memcpy(&(b->gameBoard[index].value), &value, sizeof(char));
        current += sizeof(char);
        b->gameBoard[index].fixed = fixed;

        emptyCells = emptyCells-1;
        
        //TODO: update masks <------------------------------
        m_index = value/32;
        value -= m_index*32;

        b->rowMask[row(index, b->size)][m_index] = addmask(b->rowMask[row(index, b->size)][m_index], value);
        b->colMask[col(index, b->size)][m_index] = addmask(b->colMask[col(index, b->size)][m_index], value);
        b->boxMask[box(index, b->squareSize)][m_index] = addmask(b->boxMask[box(index, b->squareSize)][m_index], value);

    }
    return ++index;
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

void printBoardT(Board *b, int id)
{

    int i = 0;
    printf("[%d]", id);
    for(i = 0; i < b->size*b->size; i++)
    {
        printf("%d ", b->gameBoard[i].value);

        if((i+1)%(b->size) == 0){

            printf("\n[%d]", id);
        }
    }
}

void printMask(int n, int c){
    int k = 0;
    c--;
    for (  ; c >= 0; c--)
    {
        if ((c+1)%4 == 0)
        {
            printf(" ");
        }
        k = n >> c;
        if (k & 1)
          printf("1");
        else
          printf("0");
    } 
} 


void printBM(Board *b){
    int i;

    for (i = 0; i < b->size; ++i)
    {
        printMask(b->rowMask[i][0], b->size);
        printf("\n");
    }
    printf("\n....\n");
    for (i = 0; i < b->size; ++i)
    {
        printMask(b->colMask[i][0], b->size);
        printf("\n");
    }
    printf("\n....\n");
    for (i = 0; i < b->size; ++i)
    {
        printMask(b->boxMask[i][0], b->size );
        printf("\n");
    }

    printf("\n....\n");
    printBoard(b);
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
