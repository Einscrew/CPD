#include <omp.h>
#include "strategies.h"

int activeThreads = 0;

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

void fillQueue(Queue *q, Board *b, int bottom, int level)
{
    int i = 0;

    if(level > bottom || level >= b->size*b->size){
        return;
    }

    if(level == bottom)
    {
        //ver copy
        if(push(b, q, level) == 0)
        {
            printf("Failed to push\n");
        }
        return;
    }

    if(b->gameBoard[level].fixed == FALSE)
    {
        for(i = 1; i <= b->size; i++)
        {
            if(checkValidity(b, level, i) == TRUE)
            {
                b->gameBoard[level].value = i;
                updateMasks(b, level);
                if(level < bottom)
                {
                    fillQueue(q, b, bottom, level+1);
                }
                removeMasks(b, level);
                b->gameBoard[level].value = 0;
            }
        }
    }
    else
    {
        fillQueue(q, b, bottom+1, level+1);   
    }                 
}

int workNeeded()
{
    int res = FALSE;

    #pragma omp critical (updateNumThreads)
    {
        if(activeThreads < omp_get_num_threads())
        {
            res = TRUE;
        }
    }

    return res;
}

int solver(Board *b)
{
    Queue *mainQ = create();
    int solFound = FALSE;
    
    //fillQueue(mainQ, b, 1, 0);
    push(b, mainQ, 0);
   
    //printf("Size: %d\n", mainQ->size);
    

    #pragma omp parallel shared(mainQ, solFound)
    {
        #pragma omp master
            printf("THREADS: %d\n", omp_get_num_threads());

        Board *currBoard = NULL;
        int result = TRUE;
        int index = 0, i = 0, valid = TRUE;

        do
        {
            valid = TRUE;
            result = TRUE;
            #pragma omp critical (updateQueue)
            {
                currBoard = pop(mainQ, &index);
                
                if(currBoard != NULL)
                {
                    #pragma omp atomic
                    activeThreads++;
                    
                    /*printf("Active after pop: %d\n", activeThreads);
                    printf("thread: %d\n", omp_get_thread_num());
                    printf("index: %d valid: %d\n", index, valid);
                    printBoard(currBoard);*/
                    /*#pragma omp critical
                    {
                            printf("[%d] popped\n", omp_get_thread_num());// (currBoard == NULL)?'Y':'N');
                            printBoard(currBoard); 
                    }*/
                }
            }

            if(currBoard != NULL){

                for(i = index; i < currBoard->size*currBoard->size ; i++){
                    //printf("%d\n", i);

                    if(valid == FALSE)
                    {
                        i--;
                        valid = TRUE;
                    }
                    // For non fixed values:
                    if(currBoard->gameBoard[i].fixed == FALSE){
                        
                        // No more alternatives
                        if((valid = makeGuess(currBoard, i)) == FALSE){

                            // Backtrack
                            do{
                                // Erasing supposed values in the way back
                                if(currBoard->gameBoard[i].fixed == FALSE)
                                {
                                    // Erase guess & masks
                                    removeMasks(currBoard, i);
                                    currBoard->gameBoard[i].value = 0;
                                }

                                i--;
                            }while (i >= index && (currBoard->gameBoard[i].fixed == TRUE || currBoard->gameBoard[i].value == currBoard->size));

                            if(i < index || solFound == TRUE){
                                result = FALSE;
                                break;
                            }
                        }
                        else if(workNeeded() && i < currBoard->size * currBoard->size - 1)
                        {
                            #pragma omp critical (updateQueue)
                            {
                                push(copyBoard(currBoard), mainQ, i+1);
                            }

                            if((valid = makeGuess(currBoard, i)) == FALSE){

                                do{
                                    // Erasing supposed values in the way back
                                    if(currBoard->gameBoard[i].fixed == FALSE)
                                    {
                                        // Erase guess & masks
                                        removeMasks(currBoard, i);
                                        currBoard->gameBoard[i].value = 0;
                                    }

                                    i--;
                                }while (i >= index && (currBoard->gameBoard[i].fixed == TRUE || currBoard->gameBoard[i].value == currBoard->size));

                                if(i < index || solFound == TRUE){
                                    result = FALSE;
                                    break;
                                }
                            }
                        }
                    }
                }

                if(result == FALSE)
                {
                    #pragma omp atomic
                    activeThreads--;
                    //printf("Active after false: %d\n", activeThreads);
                }
                else
                {
                    printf("ENCONTROU SOLUÇÃO\n");
                    #pragma omp critical (updateSol)
                    {
                        solFound = TRUE;
                        printBoard(currBoard);
                    }
                    
                }

                freeBoard(currBoard);
                free(currBoard);
            }

        }while(solFound == FALSE);
    }
    destroy(mainQ);    
    return 0;

}

/*
int parallelSolver(Board * b){

    //create Stack
	Queue * stack = create(9);
	int original;
	int i = 0;

	while(i < b->size*b->size && b->gameBoard[i].fixed == TRUE){
		i++;
	}

	if(b->gameBoard[i].fixed == FALSE){
		// Save original value??
		original = b->gameBoard[i].value;
		// Put Boards on Stack, only with this [i].value changed
		while(!full(stack) && b->gameBoard[i].value++ <= b->size ){

			push(b, stack, -1);
		}
	}
	printStack(stack);
	return 0;

}*/


int bruteforce(Board *b, int start){

	int i,valid = TRUE;

	for(i = start; i < b->size*b->size; i++){
		//printf("%d\n", i);

		if(valid == FALSE)
		{
			i--;
			valid = TRUE;
		}
        // For non fixed values:
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

				if(i <= start && b->gameBoard[i].value == b->size){
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}



