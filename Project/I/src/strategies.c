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
        fillQueue(q, b, bottom+1, level+1);   
    }                 
}

int solver(Board *b)
{
    Queue *mainQ = create();
    int solFound = FALSE;
    
    //fillQueue(mainQ, b, 1, 0);
    push(b, mainQ, 0);
   
    printQueue(mainQ);
    printf("Size: %d\n", mainQ->size);
    
    omp_set_num_threads(8);

    int finished = 0, flagStop = 0;    

    #pragma omp parallel shared(mainQ, solFound, finished, flagStop)
    {
        #pragma omp master
            printf("THREADS: %d\n", omp_get_num_threads());

        Board *currBoard = NULL;
        Queue *privQ = create();
        int result = FALSE;
        int index = 0, i = 0, valid = FALSE;  
        int threshold = 2;
        int numThreads = omp_get_num_threads();

        while(solFound == FALSE && finished < numThreads)
        {
            
            #pragma omp critical (updateQueue)
            {
                currBoard = pop(mainQ, &index);
                //printf("Size pop: %d\n", mainQ->size);
            }
 
            if(currBoard != NULL)
            {
                if(index >= threshold)
                {
                    if(mainQ->size == numThreads - 1)
                    {
                        printf("size: %d\n", mainQ->size);
                        printf("Pôs a flag a 1\n");
                        flagStop = 1;
                    }

                    /*#pragma omp critical
                    {
                        printf("brut %d\n", omp_get_thread_num());
                        printBoard(currBoard);
                    }*/
                    //result = bruteforce(currBoard, index+1);
                    index++;

                    for(i = index; i < currBoard->size*currBoard->size ; i++)
                    {
                        if(valid == FALSE)
                        {
                            i--;
                            valid = TRUE;
                        }
                        // For non fixed values:
                        if(currBoard->gameBoard[i].fixed == FALSE)
                        {                           
                            // No more alternatives
                            if((valid = makeGuess(currBoard, i)) == FALSE)
                            {
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

                                if((i <= index && currBoard->gameBoard[i].value == currBoard->size) || solFound == TRUE)
                                {
                                    if(solFound == FALSE && flagStop == 1)
                                    {
                                        #pragma omp critical
                                        {
                                            finished++;
                                            printf("Acabou o trabalho %d\n", omp_get_thread_num());
                                            printf("finished: %d\n", finished);
                                            printf("numThreads: %d\n", numThreads);
                                        }
                                    }
                                    freeBoard(currBoard);
                                    free(currBoard);
                                    result = FALSE;
                                    break;
                                }
                            }
                        }
                    }

                    if(i == currBoard->size*currBoard->size && valid == TRUE)
                    {
                        result = TRUE;
                    }
                    
                    solFound = (result == TRUE)? TRUE: solFound;

                    if(result == TRUE){
                        #pragma omp critical
                        {
                            printf("EHEHEHindex:%d-%d [%d]\n",index ,threshold , omp_get_thread_num());
                            printBoard(currBoard);
                        }
                        freeBoard(currBoard);
                        free(currBoard);
                        break;
                    }
                    
                }
                else
                {
                    // Looks for the next empty cell 
                    while(currBoard->gameBoard[index].fixed == TRUE && index < currBoard->size*currBoard->size)
                    {
                        index++;
                        threshold++;
                        if(index == currBoard->size * currBoard->size)
                        {
                            break;
                        }

                    }
                    // Check if currBoard is the solution was found 
                    if(index == currBoard->size * currBoard->size)
                    {
                        printf("AWINDACNK EDHQJSA\n");
                        solFound = TRUE;
                       // printBoard(currBoard);
                        continue;
                    }

                    //printf("AAAAAAAAAAAAAA[%d]\n", index);
                    
                    //printBoard(currBoard);
                    fillQueue(privQ, currBoard, index + currBoard->squareSize, index);

                    freeBoard(currBoard);
                    free(currBoard);
                    #pragma omp critical (updateQueue)
                    {
                        merge(mainQ, privQ);
                    }
                    
                }  
            }
        }
    }

    if(solFound == FALSE)
    {
        printf("There's no solution for the given sudoku!\n");
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



