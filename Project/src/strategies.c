#include "strategies.h"

/* Update vector of possibilities when a number is inserted */
void updateBoardValues(Board *board, int row, int col, int value)
{
    int indexValue = value - 1; /* used to update the possibilites of other cells */
    int i = 0;
    int boxStartRow = (row / board->size) * board->size;
    int boxStartCol = (col / board->size) * board->size;

    if(value == 0)
    {
        return;
    }

    for(i = 0; i < (board->size*board->size); i++)
    {
        /* Update cells in the same row, if the cell is empty */
        if(board->gameBoard[row][i].value == 0)
        {
            if(board->gameBoard[row][i].possibleValues[indexValue] == TRUE)
            {
                board->gameBoard[row][i].possibleValues[indexValue] = FALSE;
                board->gameBoard[row][i].countPossibilities --;
            }
        }

        /* Update cells in the same column, if the cell is empty */
        if(board->gameBoard[i][col].value == 0)
        {
            if(board->gameBoard[i][col].possibleValues[indexValue] == TRUE)
            {
                board->gameBoard[i][col].possibleValues[indexValue] = FALSE;
                board->gameBoard[i][col].countPossibilities --;
            }
        }
        
        /* Update cells in the same box, if the cell is empty */
        if(board->gameBoard[boxStartRow + i / board->size][boxStartCol + i % board->size].value == 0)
        {
            if(board->gameBoard[boxStartRow + i / board->size][boxStartCol + i % board->size].possibleValues[indexValue] == TRUE)
            {
                board->gameBoard[boxStartRow + i / board->size][boxStartCol + i % board->size].possibleValues[indexValue] = FALSE;
                board->gameBoard[boxStartRow + i / board->size][boxStartCol + i % board->size].countPossibilities --;
            }
        }
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

/* Checks if a value it's the only possible solution */ 
void deleteStrategy(Board *board)
{
    int changed = 0, row = 0, col = 0, i = 0;

    while(changed == 0)
    {
        changed = -1;
        for(row = 0; row < board->size * board->size; row++)
        {
            for(col = 0; col < board->size * board->size; col++)
            {
                if(board->gameBoard[row][col].value != 0 || board->gameBoard[row][col].countPossibilities != 1)
                {
                    continue;
                }

                for(i = 0; i < board->size * board->size; i++)
                {
                    /* Place in the game boar the only possible solution */
                    if(board->gameBoard[row][col].possibleValues[i] == TRUE)
                    {
                        board->gameBoard[row][col].value = i + 1; /* the possible value is the index +1 */
                        break;
                    }
                }
                updateBoardValues(board, row, col, board->gameBoard[row][col].value); /* updates the other cells */
                changed = 0;
            }
        }

        if(changed == -1)
        {
            break;
        }
    }
}