#include "strategies.h"

int bruteforce(Board *b){

    int i = 0, valid = TRUE;

    for(i = 0; i < b->size*b->size; i++){

        if(valid == FALSE)
        {
            i--;
            valid = TRUE;
        }
        // For non fixed values:
        if(b->gameBoard[i].fixed == FALSE){

            if(b->gameBoard[i].value == 0)
            {
                b->gameBoard[i].value = b->gameBoard[i].minPoss;
            }

            do
            {
                /* Starts by trying the first possible value for the cell */
                b->gameBoard[i].value++;

                valid = checkValidity(b, i);
                if(valid == TRUE){
                    break;
                }

            }while(b->gameBoard[i].value < b->size/*|| b->gameBoard[i].numberGuesses < b->gameBoard[i].countPossibilities*/);

            if(valid == FALSE){

                //backtrack
                do
                {
                    /* Resets the value  */
                    if(b->gameBoard[i].fixed == FALSE)
                    {
                        b->gameBoard[i].value = 0;
                    }

                    i--;

                }while ((b->gameBoard[i].fixed == TRUE && i > 0) || b->gameBoard[i].value == b->size);

                if(i == 0 && b->gameBoard[i].value == b->size){
                    return FALSE;
                }

            }
        }
    }
    return TRUE;
}

