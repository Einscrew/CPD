#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include "sudoku-serial.h"

int activeThreads = 0;
int END = 0;

int makeGuess(Board *b, int i){

    removeMasks(b, i);
    int value = b->gameBoard[i].value + 1;

    while(value <= b->size){

        if(checkValidity(b, i, value) == TRUE){
            b->gameBoard[i].value = value;
            updateMasks(b, i);
            return TRUE;
        }
        
        value++;
    }
    return FALSE;
}

int  bruteforce(Board *b, int start){

	int i,valid = TRUE;

	for(i = start; i < b->size*b->size; i++){
		//printf("%d\n", i);

		if(valid == FALSE)
		{
			i--;
			valid = TRUE;
		}
        // For non fixed values:sudoku-ser
		if(b->gameBoard[i].fixed == FALSE){
			
            // No more alternatives
			if((valid = makeGuess(b, i)) == FALSE){

                // Backtrack
				do{
                    // Erasing supposed values in the way back
					if(b->gameBoard[i].fixed == FALSE)
					{
						// Erase guess & masks
                        removeMasks(b, i);
                        b->gameBoard[i].value = 0;
					}

					i--;
				}while (i >= start && (b->gameBoard[i].fixed == TRUE || b->gameBoard[i].value == b->size));

				if(i < start){
					printf("Sudoku has no solution.\n");
					return FALSE;
				}
			}
		}
	}
	printBoard(b);
	return TRUE;
}

int main(int argc, char const *argv[]) {

    Board *board = NULL;

    if(argv[1] != NULL)
    {
            board = (Board*)malloc(sizeof(Board));
            if((fillGameBoard(board, argv[1])) == 0)
            {
            	bruteforce(board , 0);       
                freeBoard(board);
            }
            free(board);
    }
    else
    {
            printf("No file was specified!\n");
    }
    printf("THE END\n" );
    return 0;
}


