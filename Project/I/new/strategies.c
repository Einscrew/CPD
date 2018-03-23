#include "strategies.h"

int bruteforce(Board *b){
    int i, valid;
    for(i = 0; i < b->size*b->size; i++){
        // For non fixed values:
        if(b->gameBoard[i].fixed == FALSE){
            while(b->gameBoard[i].value <= b->size){

                if((valid = checkValidity(b, i))){
                    break;
                }

                b->gameBoard[i].value++;
            }
            if(valid == FALSE){
                b->gameBoard[i].value = 0;
                //backtrack
                do {
                    i--;
                }while (b->gameBoard[i].fixed == TRUE && i > 0);
                if(i == 0 && b->gameBoard[i].value == b->size){
                    return FALSE;
                }

            }
        }
    }
    return TRUE;
}
