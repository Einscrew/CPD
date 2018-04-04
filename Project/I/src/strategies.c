#include <omp.h>
#include "strategies.h"

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
        fillQueue(q, b, bottom, level+1);   
    }                 
}

int solver(Board *b)
{
    Queue *mainQ = create();
    int solFound = FALSE;
    
    fillQueue(mainQ, b, 1, 0);
    printf("Size: %d\n", mainQ->size);

    omp_set_num_threads(4);

    #pragma omp parallel shared(mainQ, solFound)
    {
        #pragma omp master
            printf("THREADS: %d\n", omp_get_num_threads());

        Board *currBoard = NULL;
        Queue *privQ = create();
        int result = FALSE;
        int index = 0, i= 0, valid;  
        int threshold = 5;
        int initialSize = mainQ->size;

        #pragma omp for
        for (int i = 1; i <= initialSize; ++i)
        {
            #pragma omp critical (getBoard)
            {
                currBoard = pop(mainQ, &index);
            }

            fillQueue(privQ, currBoard, threshold, index);
            free(currBoard);
        }

        if(privQ->size > 0){
            #pragma omp critical (updateQueue)
            {
                merge(mainQ, privQ);
            }
        }
       
        while(solFound == FALSE)
        {
            #pragma omp critical (updateQueue)
            {
                currBoard = pop(mainQ, &index);
                
                if(currBoard != NULL && index >= threshold){
                    printf("[%d]Will bruteforce, %d Boards left\n", omp_get_thread_num(), mainQ->size);
                    /*#pragma omp critical
                    {
                            printf("[%d] popped\n", omp_get_thread_num());// (currBoard == NULL)?'Y':'N');
                            printBoard(currBoard); 
                    }*/
                }
            }

            if(currBoard != NULL){
            
                if(index >= threshold){
                    /*#pragma omp critical
                    {
                        printf("brut %d\n", omp_get_thread_num());
                        printBoard(currBoard);
                    }*/
                    //result = bruteforce(currBoard, index+1);
                    index++;


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

                                if((i <= index && currBoard->gameBoard[i].value == currBoard->size) || solFound == TRUE){
                                    //free(currBoard);
                                    result = FALSE;
                                    break;
                                }
                            }
                        }
                    }
                    if(i == currBoard->size*currBoard->size){
                        result = TRUE;
                    }
                    
                    solFound = (result == TRUE)? TRUE: solFound;

                    if(result == TRUE){
                        #pragma omp critical
                        {
                            printf("EHEHEHindex:%d-%d [%d]\n",index ,threshold , omp_get_thread_num());
                            printBoard(currBoard);
                            free(currBoard);
                        }
                        break;
                    }else{
                        //free(currBoard);
                    }
                }

            }
        }
    }

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



