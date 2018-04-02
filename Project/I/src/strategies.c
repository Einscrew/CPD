#include "strategies.h"

void testStack(Board * b){
	Stack * s = create(4);

	push(b, s, -1);
	push(b, s, -1);
	push(b, s, -1);
	push(b, s, -1);
	push(b, s, -1);

	printStack(s);

    /*
    Board *e = pop(s);
    freeBoard(e);
    free(e);

    printStack(s);

    e = pop(s);
    freeBoard(e);
    free(e);

    printStack(s);
    */
	destroy(s);

	return;
}


int parallelSolver(Board * b){

    //create Stack
	Stack * stack = create(9);
	int original;
	int i = 0;

	while(i < b->size*b->size && b->gameBoard[i].fixed == TRUE){
		i++;
	}

	if( b->gameBoard[i].fixed == FALSE){
		// Save original value??
		original = b->gameBoard[i].value;
		// Put Boards on Stack, only with this [i].value changed
		while(!full(stack) && b->gameBoard[i].value++ <= b->size ){

			push(b, stack, -1);
		}
	}
	printStack(stack);
	return 0;

}

int makeGuess(Board * b, int i){
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


int bruteforce(Board *b, int s){

	int i,valid = TRUE;

	for(i = s; i < b->size*b->size; i++){
		printf("%d\n", i);

		if(valid == FALSE)
		{
			i--;
			valid = TRUE;
		}
        // For non fixed values:
		if(b->gameBoard[i].fixed == FALSE){
			
            // No more alternatives
			if((valid = makeGuess(b, i))== FALSE){

                // Backtrack
				do{
                    // Erasing supposed values in the way back
					if(b->gameBoard[i].fixed == FALSE)
					{
						// Erase guess & masks
                        b->gameBoard[i].value = 0;
						removeMasks(b, i);
					}
					i--;
				}while ((i >= 0 s && b->gameBoard[i].fixed == TRUE) || b->gameBoard[i].value == b->size);

				if(i <= s && b->gameBoard[i].value == b->size){
					return FALSE;
				}

			}

		}
	}
	return TRUE;
}



