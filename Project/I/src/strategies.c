#include <omp.h>
#include "strategies.h"

int activeThreads = 0;
int END = 0;
int flagStart = 0;
int flagINI = 0;
int numIniActiveThreads = 0;

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

int workNeeded(int numThreads)
{   
    return activeThreads <= numThreads;
}

int taskBruteForce(Board *b, int start, int numThreads, int threshold)
{
    int i = 0, valid = TRUE;

    while(flagStart != b->size)
    {}

    if(flagINI == 0)
    {
        activeThreads = numIniActiveThreads;
        flagINI = 1;
    }

    //printf("ACTIVE THREADS ENTRA: %d therad num: %d\n", activeThreads, omp_get_thread_num());
    /*#pragma omp atomic
        activeThreads++;*/
    //printf("ACTIVE ENTRA: %d\n", activeThreads);  

    for(i = start; i < b->size*b->size; i++)
    {
        if(END == TRUE)
        {
            //printf("ABORTED SAI: %d\n", omp_get_thread_num());
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
            if((((i - start) % b->squareSize) == threshold) && workNeeded(numThreads))
            {
                if((valid = makeGuess(b, i)) == TRUE)
                {
                    Board *new = copyBoard(b);

                    #pragma omp atomic
                    activeThreads++;

                    #pragma omp task
                    {
                        taskBruteForce(new, i+1, numThreads, 0);
                        freeBoard(new);
                        free(new);
                    }
                }
            } 
            
            if((valid = makeGuess(b, i)) == FALSE)
            {
                // Backtrack
                do{

                    // Erasing supposed values in the way back
                    if(b->gameBoard[i].fixed == FALSE)
                    {
                        // Erase guess & masks
                        removeMasks(b, i);
                        b->gameBoard[i].value = 0;
                    }
                    else
                    {
                        threshold--;
                    }

                    i--;

                }while (i >= start && (b->gameBoard[i].fixed == TRUE || b->gameBoard[i].value == b->size));

                if(i < start)
                { 
                    #pragma omp atomic
                    activeThreads--;
                    printf("ACTIVE SAI: %d therad num: %d\n", activeThreads, omp_get_thread_num());
                    
                    return FALSE;
                }
            }
        }
        else
        {
            threshold++;
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
    #pragma omp parallel shared(activeThreads, b, tabs)
    {
        /*Board *new = copyBoard(b);
        int threshold = 0;
        int numThreads = omp_get_num_threads();

        #pragma omp single
            taskBruteForce(new, 0, numThreads, threshold);*/

        int threshold = 0;
        int numThreads = omp_get_num_threads();
        
        #pragma omp for schedule(dynamic)
        for (int i = 1; i <= b->size; i++)
        {
            Board *new = copyBoard(b);

             if(checkValidity(new, 0, i) == TRUE)
             {
                #pragma omp atomic
                numIniActiveThreads++;

                printf("[%d]->%d@%d\n", omp_get_thread_num(), b->gameBoard[0].value, i);
                new->gameBoard[0].value = i;
                updateMasks(new, 0);

                #pragma omp task
                {
                    taskBruteForce(new, 1, numThreads, threshold);
                    freeBoard(new);
                    free(new);
                }
            }
            else
            {
                printf("[%d]->X\n", omp_get_thread_num());
            }

            #pragma omp atomic
            flagStart++;
            //printf("flagStart solver: %d\n", flagStart);
        }            
    }

    if(END == FALSE)
    {
        printf("No solution\n");
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



