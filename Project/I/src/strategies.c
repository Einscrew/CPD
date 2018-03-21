#include "strategies.h"

int backtrack(Board * b){

    int i, j;
    for( i=0; i < b->size; i++){
        for( j=0; j < b->size; j++){
            /*
            if ij valid cell
            | increment &&
            | remove itself this.possibilities &&
            | continue??
            |
            else
            | continue

            if not valid
            | backtrack(&i, &j)
            */
        }
    }

    return 1;
}


/* TRY */
int solve(Board * b)
{
    int i = 0, j = 0, k = 0, prev = 0;
    int state = 0;

    for (i = 0; i < b->size; i++) {
        for (j = 0; j < b->size; j++) {
            /* Means that the value it's not assigned yet */
            if (b->gameBoard[i][j].fixed == FALSE) {
                for (k = 0; k < b->size; k++) {

                    /* When the value is zero and has no more possibilities left */
                    if(b->gameBoard[i][j].countPossibilities == 0)
                    {
                        return FALSE;
                    }

                    /* Look for the possible values for the cell */
                    if (b->gameBoard[i][j].possibleValues[k] == TRUE){

                        prev = b->gameBoard[i][j].value;
                        b->gameBoard[i][j].value = k+1;

                        /* Means that it's the first time that we going to try a number and
                        there's only one possibility for this cell */
                        if(prev == 0 && b->gameBoard[i][j].countPossibilities == 1)
                        {
                            updateBoardValues(b, i, j, b->gameBoard[i][j].value);
                            b->gameBoard[i][j].fixed = TRUE;
                        }

                        /* Checks if the guess is a possible choice in each row, column and box.
                        If returns Fixed means that the colision was with a value that is already fixed*/
                        state = checkAllBoard(b);
                        if(state != TRUE){

                            /* Means that there's no possible solution */
                            if(b->gameBoard[i][j].fixed == TRUE && b->gameBoard[i][j].countPossibilities == 1)
                            {
                                return FALSE;
                            }

                            /* If the collision was with a fixed value then we can remove that possibility from the other cells
                             in the same row, column and box*/
                            if(state == FIXED)
                            {
                                updateBoardValues(b, i, j, b->gameBoard[i][j].value);
                            }
                            else
                            {
                                /* Returns to the previous value */
                                b->gameBoard[i][j].value = prev;
                            }
                        }
                        else
                        {
                            break;
                        }

                        /* If all the possibilities are tested and no one fits,
                        then we have to backtrack to the last element set in game board */
                        if(k == b->size - 1 || state != TRUE)
                        {
                            while(b->gameBoard[i][j].fixed == TRUE){

                                j--;

                                if(j == 0 && i > 0)
                                {
                                    /* If j reaches the first cell in a row,
                                    so it has to be set to the last cell of the previous row */
                                    j = 8;
                                    i--;
                                }
                                else if(i == 0)
                                {
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return TRUE;
}



