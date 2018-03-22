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



int solve(Board * b)
{
    int i = 0, j = 0, k = 0;
    int state = 0;
    int conflitValue = 0;

    for (i = 0; i < b->size; i++) {
        for (j = 0; j < b->size; j++) {
            //printf("%d %d (%d/%d)\n", i, j, b->gameBoard[i][j].numberGuesses, b->gameBoard[i][j].countPossibilities);

            b->gameBoard[i][j].numberGuesses = 0;

            /* Means that the value it's not assigned yet */
            if (b->gameBoard[i][j].fixed == FALSE) {

                for (k = 0; k < b->size; k++){

                    /* When the value is zero and has no more possibilities left and there's no conflict in action */
                    if(b->gameBoard[i][j].countPossibilities == 0 /*&& state != TRUE*/)
                    {
                        return FALSE;
                    }

                    /* Look for the possible values for the cell */
                    if (b->gameBoard[i][j].possibleValues[k] == TRUE){

                        b->gameBoard[i][j].value = k+1;

                        /* Means that there's only one possibility for this cell */
                        if(b->gameBoard[i][j].countPossibilities == 1)
                        {
                            updateBoardValues(b, i, j, b->gameBoard[i][j].value);
                            b->gameBoard[i][j].fixed = TRUE;
                            conflitValue = b->gameBoard[i][j].value;
                        }

                        /* Checks if the guess is a possible choice in each row, column and box.
                        If returns Fixed means that the collision was with a value that is already fixed */
                        state = checkAllBoard(b);
                        if(state != TRUE)
                        {
                            /* If the value it's not fixed, the number of guesses already done are incremented */
                            if(b->gameBoard[i][j].fixed == FALSE)
                            {
                                b->gameBoard[i][j].numberGuesses++;
                            }

                            /* Means that there's no possible solution */
                            if(b->gameBoard[i][j].fixed == TRUE && b->gameBoard[i][j].countPossibilities == 1)
                            {
                                return FALSE;
                            }
                        }
                        else /* Means that the value is fine and we don't have to backtrack */
                        {
                            /* If the value it's not fixed, the number of guesses already done are incremented */
                            if(b->gameBoard[i][j].fixed == FALSE)
                            {
                                b->gameBoard[i][j].numberGuesses++;
                            }
                            break;
                        }
                    }

                    /* Means that we've tried every possible solutions, so we have to backtrack to the previous
                    unfixed cell */
                    if(b->gameBoard[i][j].numberGuesses == b->gameBoard[i][j].countPossibilities)
                    {
                        if(state == FALSE)
                        {
                            /* If the value it's not possible, the value of the cell is turned 0 again */
                            b->gameBoard[i][j].value = 0;

                            do
                            {
                                j--;

                                if(j + 1 == 0 && i > 0)
                                {
                                    /* If j reaches the first cell in a row,
                                    so it has to be set to the last cell of the previous row */
                                    j = b->size - 1;
                                    i--;
                                }
                                else if(j == 0 & i == 0)
                                {
                                    break;
                                }

                                /* If the value of the cell is of the size of the board and the value it's not fixed yet
                                 then we continue to backtrack once all possible values were already tried and so we continue
                                 searching for the next unfixed cell */
                                if(b->gameBoard[i][j].value == b->size && b->gameBoard[i][j].fixed == FALSE)
                                {
                                    b->gameBoard[i][j].value = 0;
                                }

                            }while(b->gameBoard[i][j].fixed == TRUE || b->gameBoard[i][j].value == 0);

                            /* The value k keeps the last possible try of that cell */
                            k = b->gameBoard[i][j].value - 1;
                        }
                        else if(state == FIXED)
                        {
                            do
                            {
                                j--;

                                if(j + 1 == 0 && i > 0)
                                {
                                    j = b->size - 1;
                                    i--;
                                }
                                else if(j == 0 && i == 0)
                                {
                                    break;
                                }

                                if(b->gameBoard[i][j].value == b->size && b->gameBoard[i][j].fixed == FALSE)
                                {
                                    printf("i: %d j: %d\n", i, j);
                                    b->gameBoard[i][j].value = 0;
                                }

                            }while(b->gameBoard[i][j].value != conflitValue);

                            /* If the value is the last possible one, then all the possible values are tested again */
                            if(b->gameBoard[i][j].value == b->size)
                            {
                                k = 0;
                            }
                            else /* The value k keeps the last possible try of that cell */
                            {
                                k = b->gameBoard[i][j].value - 1;
                            }

                        }
                    }
                }
            }
        }
    }

    return TRUE;
}



