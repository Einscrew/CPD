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