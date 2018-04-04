#include <omp.h>
#include <unistd.h>
#include "strategies.h"


int END = FALSE;
//void search(Board * b, int index){

/*    int v = b->gameBoard[index].value;
    #pragma omp critical
    {
        printf("[%d]I will sleep %d s [value:%d]\n", omp_get_thread_num(), index, v);
    }
    sleep(index+2);
    #pragma omp critical
    {
        printf("[%d] [value:%d]v:%d s \n", omp_get_thread_num(), b->gameBoard[index].value, v);
    }
*/
    //printf("[%d]@%d\n", omp_get_thread_num(), index );
   /* if(END == TRUE){
        return;
    }

    do{
        index++;
    }while(b->gameBoard[index].fixed == TRUE && index < b->size*b->size);   
    //printf("[%d]got b[%d]=%d\n", omp_get_thread_num(), index, b->gameBoard[index-1].value);
    if(index >= b->size*b->size){
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
            {
                //Board * n = copyBoard(b);
                search(b, index);
                //free(n);
            }

            #pragma omp taskwait
            removeMasks(b,index);
        }

        if(END == TRUE){
            return;
        }
        
    }
    return;
}
*/
void createTask(Board *b, int level, int bottom)
{
    int i = 0;

    // bottom = (bottom > 4)? 4: bottom;

    do{
        level++;
        bottom++;
    }while(level < b->size*b->size && b->gameBoard[level].fixed == TRUE);

    if(level == b->size*b->size)
    {
        END=TRUE;
        return;
    }

    
    for(i = 1; i <= b->size; i++)
    {
        if(checkValidity(b, level, i) == TRUE)
        {
            b->gameBoard[level].value = i;
            updateMasks(b, level);
            if(level < bottom )
            {
                createTask( b, bottom, level+1);
            }
            else
            {
                // task()
                #pragma omp task
                search(b, level);
            }
            removeMasks(b, level);
            //b->gameBoard[level].value = old;
        }
    }
    b->gameBoard[level].value = 0;
                    
}



void search(Board *b, int index)
{
     if(END == TRUE){
        return;
    }

    do{
        index++;
    }while(b->gameBoard[index].fixed == TRUE && index < b->size*b->size);   
    //printf("[%d]got b[%d]=%d\n", omp_get_thread_num(), index, b->gameBoard[index-1].value);
    if(index >= b->size*b->size){
        printBoard(b);
        END = TRUE;
        return;
    }

    createTask(b, index, b->squareSize);
}

int solver(Board *b)
{
    
    int size = b->size;

    omp_set_num_threads(4);

    int index = 0;
    
    while(b->gameBoard[index].fixed == TRUE && index < b->size*b->size){
        index++;
    }
    
    
    #pragma omp parallel shared(END,b) firstprivate(index, size)
    {

        #pragma omp for schedule(dynamic)
        for(char value = 1; value <= size ; value++){

            if(checkValidity(b, index, value) == TRUE){
                printf("[%d]Created b[%d]=%d\n", omp_get_thread_num(), index, value);
                
                //task()
                #pragma omp task untied
                {
                    Board * newB = copyBoard(b);
                    
                    newB->gameBoard[index].value = value;

                    updateMasks(newB,index);
                    
                    search(newB, index);
                    
                    freeBoard(newB);
                    
                }
                
            }

            // CHECK VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
        }
    
    }   

    return END;

}


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



