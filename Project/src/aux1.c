#include "aux1.h"

/*void verifyMalloc()
{
    
}*/


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

    for(i = 0; i < board->size * board->size;, i++)
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