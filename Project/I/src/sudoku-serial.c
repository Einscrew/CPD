#include "sudoku-serial.h"

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

void bruteForce(Board *b){

	int i = 0,valid = TRUE;

	for(i = 0; i < b->size*b->size; i++){

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
				}while (i >= 0 && (b->gameBoard[i].fixed == TRUE || b->gameBoard[i].value == b->size));

				if(i < 0)
                {
					printf("No solution\n");
					return;
				}
			}
		}
	}
	printBoard(b);
}

int main(int argc, char const *argv[]) {

    Board *board = NULL;

    if(argv[1] != NULL)
    {
            board = (Board*)malloc(sizeof(Board));
            if((fillGameBoard(board, argv[1])) == 0)
            {
            	bruteForce(board);       
                freeBoard(board);
            }
            free(board);
    }
    else
    {
            printf("No file was specified!\n");
    }
    
    return 0;
}


