#include <omp.h>
#include <unistd.h>
#include "strategies.h"


int END = FALSE;

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

    bottom = (bottom > 4)? 4: bottom;
    
    if(bottom == b->size*b->size)
    {
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
                if(level < bottom )
                {
                    fillQueue(q, b, bottom, level+1);
                }
                else
                {
                    //ver copy
                    if(push(b, q, level) == 0)
                    {
                        printf("Failed to push\n");
                    }
                }
                removeMasks(b, level);
                //b->gameBoard[level].value = old;
            }
        }
        b->gameBoard[level].value = 0;
    }
    else
    {
        fillQueue(q, b, bottom+1, level+1);   
    }                 
}

void search(Board * b, int index){
    
    
 /*   if(){
        int v = b->gameBoard[0].value;
        #pragma omp critical
        //printf("[%d]will sleep:%d[%d]\n",omp_get_thread_num(), s , b->gameBoard[0].value);
        sleep(index +4 );
        #pragma omp critical
        {
            printf("[%d]-%d\n",omp_get_thread_num(),v);
            //printBoard(b);
        }
    }*/
    
    if(END == TRUE){
        return;
    }


    do{
        index++;
    }while(b->gameBoard[index].fixed == TRUE && index < b->size*b->size);   
    //printf("[%d]got b[%d]=%d\n", omp_get_thread_num(), index, b->gameBoard[index-1].value);
    if(index == b->size*b->size){
        printBoard(b);
        END = TRUE;
        return;
    }
    
    for(char value = 1; value <= b->size; value++){
        if(checkValidity(b, index, value)){
           // printf("[%d]Created b[%d]=%d\n", omp_get_thread_num(), index, value);
            b->gameBoard[index].value = value;
            updateMasks(b,index);
            //task()
            
            #pragma omp task
                search(b, index);

            
        }

        if(END == TRUE){
            return;
        }
        #pragma omp taskwait
        removeMasks(b,index);
        
    }
    return;
    
}


int solver(Board *b)
{
    
    int size = b->size;

    omp_set_num_threads(4);

    int index = 0;
    
    while(b->gameBoard[index].fixed == TRUE && index < b->size*b->size){
        index++;
    }
    Board * newB = NULL;
    
    #pragma omp parallel shared(END, b) firstprivate(index, size, newB)
    {

        newB = copyBoard(b);

        #pragma omp for
        for(char value = 1; value <= size; value++){

            if(checkValidity(newB, index, value) == TRUE){
                //printf("[%d]Created b[%d]=%d\n", omp_get_thread_num(), index, value);
                newB->gameBoard[index].value = value;
                updateMasks(newB,index);
                
                //task()
                #pragma omp task
                    search(newB, index);        
            
                
            }

            // CHECK VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
            #pragma omp taskwait
            removeMasks(newB,index);
        }
    
    }   

    return END;

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



