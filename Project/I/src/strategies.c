#include <omp.h>
#include "strategies.h"

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

int workNeeded()
{   
    int ret = FALSE;
    #pragma omp critical(work)
    {
        if(activeThreads < omp_get_num_threads()*2){
            activeThreads++;
            ret = TRUE;
        }
    }
    return ret;
}

int taskBruteForce(Board *b, int start)
{
    int i = 0, valid = TRUE;

    #pragma omp critical(print)
    {
        printf("%d@[%d]\n",omp_get_thread_num(),start);
        printBoard(b);
    }
    /*#pragma omp atomic
        activeThreads++;*/
    //printf("ACTIVE ENTRA: %d\n", activeThreads);  

    for(i = start; i < b->size*b->size; i++)
    {
        if(END == TRUE)
        {
            return TRUE;
        }

        if(valid == FALSE)
        {
            i--;
            valid = TRUE;
        }
        // For non fixed values:
        if(b->gameBoard[i].fixed == FALSE)
        {
            //check if there's work for all threads
            if(( (i % b->size) == 0 && workNeeded()))
            {
                if((valid = makeGuess(b, i)) == TRUE)
                {
                    #pragma omp critical (print)
                    {
                        printf("WOOOORk\n");
                        printf("%d->%d@%d\n", omp_get_thread_num(), b->gameBoard[i].value, i);
                    }
                    Board *new = copyBoard(b);
                    #pragma omp task
                    {
                        taskBruteForce(new, i+1);
                        freeBoard(new);
                        free(new);
                    }
                }
            } 
            
            if((valid = makeGuess(b, i)) == FALSE)
            {
                // Backtrack
                do{
                    if(END == TRUE)
                    {
                        return TRUE;
                    }

                    // Erasing supposed values in the way back
                    if(b->gameBoard[i].fixed == FALSE)
                    {
                        // Erase guess & masks
                        removeMasks(b, i);
                        b->gameBoard[i].value = 0;
                    }

                    i--;

                }while (i >= start && (b->gameBoard[i].fixed == TRUE || b->gameBoard[i].value == b->size));

                if(i < start)
                {
                    #pragma omp atomic
                    activeThreads--;
                    //printf("ACTIVE SAI: %d\n", activeThreads);
                    
                    return FALSE;
                }
            }
        }
    }

    #pragma omp critical (print)
    {
        END = TRUE;
        printf("Thread: %d\n", omp_get_thread_num());   
        printBoard(b);
    }
    
    return TRUE;
}

int solver(Board *b)
{    

    #pragma omp parallel shared(activeThreads, b)
    {
        #pragma omp master
            printf("THREADS: %d\n", omp_get_num_threads());

        Board *currBoard = copyBoard(b);

        #pragma omp single
        taskBruteForce(currBoard, 0);
    }

    return 0;

}

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



