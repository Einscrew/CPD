#include "aux1.h"

/*void verifyMalloc()
{
    
}*/

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