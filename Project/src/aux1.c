#include "aux1.h"

/*void verifyMalloc()
{
    
}*/

/* Allocates memory to the game board */
int allocBoard(Board *board)
{
    int j = 0;

    board->gameBoard = (Cell**)malloc((board->size*board->size)*sizeof(Cell*));

    if(board->gameBoard == NULL)
    {
        printf("Error on malloc!\n");
        return -1;
    }
    for(j = 0; j < board->size * board->size; j++)
    {
        board->gameBoard[j] = (Cell*)malloc((board->size*board->size)*sizeof(Cell));
        board->gameBoard[j]->possibleValues = NULL;
        if(board->gameBoard[j] == NULL)
        {
            printf("Error on malloc!\n");
            return -1;
        }
    }
    return 0;
}

/* Creates a vector of possibilities for each cell that has a value != 0 */
void createVectorPossibilities(Cell *aux, int size)
{
    int i = 0;
    aux->possibleValues = (int*)malloc(size*sizeof(int));

    if(aux->possibleValues == NULL)
    {
        printf("Error on malloc!\n");
        exit(-1);
    }
    /* Initializes the vector of possibilities of each empty cell with all possible values */
    for(i = 0; i < size; i++)
    {
        aux->possibleValues[i] = TRUE;
    }
}

/* Checks each row, column or box */
int checkValidity(Cell *aux, int size)
{
    int exist[size];
    int i = 0;

    for(i = 0; i < size; i++)
    {
        exist[i] = FALSE;
    }

    for(i = 0; i < size; i++)
    {
        if(aux[i].value == 0)
        {
            continue;
        }
        
        if(exist[aux[i].value - 1] == TRUE)
        {
            return FALSE;
        }
        exist[aux[i].value - 1] = TRUE;
    }
    return TRUE;
}

/* Checks if there aren't any duplicate values in each row, column or box */
int checkAllBoard(Board *board)
{
    int i = 0, k = 0, w = 0, z = 0;
    int aux1 = 0, aux2 = 0;

    /* Used to compare */
    Cell rows[board->size*board->size];
    Cell cols[board->size*board->size];
    Cell boxes[board->size*board->size];

    for(i = 0; i < board->size * board->size; i++)
    {
        if(aux1 == board->size*board->size)
        {
            aux1 = 0; /* Resets the value when reaches the last cell in a row */
            aux2 = aux2 + board->size; /* Increments k so that the box starts at the left corner up cell */
        }
    
        w = 0;
        for(k = aux2; k < board->size + aux2; k++)
        {
            for(z = aux1; z < board->size + aux1; z++)
            {
                boxes[w] = board->gameBoard[k][z];
                w++;
            }
        }
        aux1 = aux1 + board->size; /* Increments z so that the box end at the right corner up cell */
        
        /* Check duplicates inside a box */ 
        if(checkValidity(boxes, board->size*board->size) == FALSE)
        {
            return FALSE;
        }

        /* Check duplicates inside a column */
        for(w = 0; w < board->size * board->size; w++)
        {
            cols[w] = board->gameBoard[w][i];
        }

        if(checkValidity(cols, board->size*board->size) == FALSE)
        {
            return FALSE;
        }

        /* Check duplicates inside a row */
        for(w = 0; w < board->size * board->size; w++)
        {
            rows[w] = board->gameBoard[i][w];
            //printf("rows[%d]: %d\n", j, board->gameBoard[i][j].value);
        }

        if(checkValidity(rows, board->size*board->size) == FALSE)
        {
            return FALSE;
        }
    }
    return TRUE;
}

/* Checks if the board is complete and if it's valid */
int checkBoardComplete(Board *board)
{
    int i = 0, j = 0;

    for(i = 0; i < board->size * board->size; i++)
    {
        for(j = 0; j < board->size * board->size; j++)
        {
            if(board->gameBoard[i][j].value == 0)
            {
                return FALSE;
            }
        }
    }

    if(checkAllBoard(board) == FALSE)
    {
        return FALSE;
    }

    return TRUE;
}

/* Finds the cell that has the less number of possibilities */
int minimumPossibilities(Board *board, MinCell *minPosCell)
{
    int i = 0, j = 0, minCount = 0;
    Cell *minCell = NULL;

    for(i = 0; i < board->size * board->size; i++)
    {
        for(j = 0; j < board->size * board->size; j++)
        {
            /* The ones that are already filled don't matter */
            if(board->gameBoard[i][j].value  != 0)
            {
                continue;
            }

            /* Updates the cell with the mininum value of possibilities */
            if(minCount < board->gameBoard[i][j].countPossibilities || minCell == NULL)
            {
                minCell = &(board->gameBoard[i][j]);
                minPosCell->row = i;
                minPosCell->col = j;
            }
        }
    }

    if(minCell == NULL)
    {
        return FALSE;
    }

    return TRUE;
}

/* Makes a copy of the board to use in brute force approach */
void copyBoard(Board *board, Board *copy, int option)
{
    int i = 0, j = 0, k = 0;

    if(option == TRUE)
    {
        copy->size = board->size;
        allocBoard(copy);            
    }

    for(i = 0; i < copy->size * copy->size; i++)
    {
        for(j = 0; j < copy->size * copy->size; j++)
        {
            copy->gameBoard[i][j].value = board->gameBoard[i][j].value;
            
            if(copy->gameBoard[i][j].value == 0)
            {
                if(option == TRUE)
                {
                    createVectorPossibilities(&(copy->gameBoard[i][j]), (copy->size*copy->size));
                }
                
                for(k = 0; k < copy->size * copy->size; k++)
                {
                    copy->gameBoard[i][j].possibleValues[k] = board->gameBoard[i][j].possibleValues[k];
                }
            }
            copy->gameBoard[i][j].countPossibilities = board->gameBoard[i][j].countPossibilities;
        }
    }
}

/* Prints the game board */
void printBoard(Cell **board, int size)
{
    int i = 0, j = 0;

    for(i = 0; i < size; i++)
    {
        for(j = 0; j < size; j++)
        {
            printf("%d ", board[i][j].value);
        }
        printf("\n");
    }
}

void freeBoard(Board *board)
{
    int i = 0, j = 0;

    for(i = 0; i < board->size; i++)
    {
        for(j = 0; j < board->size; j++)
        {
            if(board->gameBoard[i][j].possibleValues != NULL)
            {
                free(board->gameBoard[i][j].possibleValues);
            }
        }
        free(board->gameBoard[i]);
    }
    free(board->gameBoard);
}