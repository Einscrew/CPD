#include <omp.h>
#include "sudoku-omp.h"

int activeThreads = 0;
int END = 0;
int flagStart = 0;
int flagINI = 0;
int numIniActiveThreads = 0;

/************************************
*    Make guess to certain index    *
*                                   *
* Returns: TRUE  on valid guess     *
*          FALSE otherwise          *
************************************/
int makeGuess(Board *b, int i){

    removeMasks(b, i);
    int value = b->gameBoard[i].value + 1;

    while(value <= b->size)
    {
        if(checkValidityMasks(b, i, value) == TRUE){
            b->gameBoard[i].value = value;
            updateMasks(b, i);
            return TRUE;
        }
        
        value++;
    }
    return FALSE;
}

/************************************
*           PARALLEL TASK           *
************************************/

void taskBruteForce(Board *b, int start, int numThreads, int threshold)
{
    int i = 0, valid = TRUE;

    while(flagStart != b->size)
    {}
    
    if(flagINI == 0)
    {
        flagINI = 1;

        if(numThreads <= numIniActiveThreads)
        {
            activeThreads = numThreads;
        }
        else
        {
            activeThreads = numIniActiveThreads;

        }
    }

    //printf(">>>>[%d|%d] v:%d@[%d]\n", omp_get_thread_num(),activeThreads, b->gameBoard[start].value, start);

    for(i = start; i < b->size*b->size; i++)
    {
        if(END == TRUE)
        {
            return;
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
            //work will only be generated at indexes multiple of sudoku Square Size
            if(numThreads != 1 && (((i - start) % b->squareSize) == threshold) && (activeThreads < numThreads))
            {   
                // Create work to one of other threads if possible
                if((valid = makeGuess(b, i)) == TRUE)
                {
                    Board *new = copyBoard(b);

                    // Let all threads know that work will already be created
                    #pragma omp atomic
                        activeThreads++;

                    #pragma omp task
                    {
                        taskBruteForce(new, i + 1, numThreads, 0);
                        freeBoard(new);
                        free(new);
                    }
                }
            } 
            // Keep current thread working
            if((valid = makeGuess(b, i)) == FALSE)
            {
                // Backtrack on no other possible way to procede
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
                        //erasing number of fixed positions saw during the backtrack
                        threshold--;
                    }

                    i--;

                }while (i >= start && (b->gameBoard[i].fixed == TRUE || b->gameBoard[i].value == b->size));

                if(i < start)
                { 
                    if(numThreads != 1)
                    {
                        #pragma omp atomic
                        activeThreads--;
                    }
                    
                    //printf("<<<<[%d|%d]\n", omp_get_thread_num(),activeThreads);
                    return;
                }
            }

        }
        else
        {
            //incrementing number of fixed positions saw
            threshold++;
        }
    }

    #pragma omp critical (print)
    {
        if(END == FALSE)
        {
            END = TRUE;
            //printf("Thread: %d\n", omp_get_thread_num());   
            printBoard(b);
        }
    }
    
    return;
}


/************************************
*      PARALLEL IMPLEMENTATION      *
************************************/

int solver(Board *b)
{    
    int start_index = 0;

    while(b->gameBoard[start_index].fixed == TRUE)
    {
        start_index++;
    }

    #pragma omp parallel shared(activeThreads, b) firstprivate(start_index)
    {
        /*Board *new = copyBoard(b);
        int threshold = 0;
        int numThreads = omp_get_num_threads();

        numIniActiveThreads = 1;
        flagStart == b->size;

        #pragma omp single
            taskBruteForce(new, start_index, numThreads, threshold);*/

        int threshold = 0;
        int numThreads = omp_get_num_threads();
        
        #pragma omp for schedule(dynamic)
        for (int i = 1; i <= b->size; i++)
        {
             if(checkValidityMasks(b, start_index, i) == TRUE)
             {
                Board *new = copyBoard(b);

                #pragma omp atomic
                numIniActiveThreads++;

                new->gameBoard[start_index].value = i;
                //printf("[%d]->%d@%d\n", omp_get_thread_num(), new->gameBoard[start_index].value, start_index);
                updateMasks(new, start_index);

                #pragma omp task
                {
                    taskBruteForce(new, start_index + 1, numThreads, threshold);
                    freeBoard(new);
                    free(new);
                }
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

/************************************
*               MAIN                *
************************************/
int main(int argc, char const *argv[]) {

    Board *board = NULL;

    if(argv[1] != NULL)
    {
            board = (Board*)malloc(sizeof(Board));
            if((fillGameBoard(board, argv[1])) == 0)
            {
                solver(board);              
            //  checkValidity(board, 0, 2);
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