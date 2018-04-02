#include "strategies.h"

int bruteforce(Board *b){

    int i, valid = TRUE;

    for(i = 0; i < b->size*b->size; i++){

        if(valid == FALSE)
        {
            i--;
            valid = TRUE;
        }
        // For non fixed values:
        if(b->gameBoard[i].fixed == FALSE){

            do
            {
                b->gameBoard[i].value++;

                valid = checkValidity(b, i);
                if(valid == TRUE){
                    break;
                }

            }while(b->gameBoard[i].value < b->size);

            if(valid == FALSE){

                //backtrack
                do
                {
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

