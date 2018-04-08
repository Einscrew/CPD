/**************************************************************************
 *                           sudoku-omp.c                                 *
 **************************************************************************                                                                   
 * Sudoku solver program that uses OpenMP to parallelize the algorithm    *
 * designed in sudoku-serial.c                                            *
 *************************************************************************/

#include <omp.h>
#include "sudoku-omp.h"

int activeThreads = 0;
int END = 0;
int flagStart = 0;
int flagINI = 0;
int numIniActiveThreads = 0;

/**************************************************************************
*           makeGuess - Makes a guess in a game board at a given index    *
*                                        			                      *
* Receives: Board *b - a game board                                       *
*           int i    - position of the board for the guess to be made     *  
* Returns: TRUE if it finds a valid guess     		                      *
*          FALSE otherwise               			                      *
**************************************************************************/

int makeGuess(Board *b, int i)
{   
    /* Remove masks before assigning a new guess */
    removeMasks(b, i);
    int value = b->gameBoard[i].value + 1;

    while(value <= b->size)
    {
        /* Checks if value is a valid number for cell with index i */
        if(checkValidityMasks(b, i, value) == TRUE)
        {
            b->gameBoard[i].value = value;
            updateMasks(b, i); /* Update masks according to the guess done */
            return TRUE;
        }
        
        /* Test the next value */
        value++;
    }
    return FALSE;
}

/**************************************************************************
*           workNeeded - Checks if all threads are working                *
*                                        			                      *
* Receives: int numThreads - number of threads executing the program      *  
* Returns: TRUE if the number of active threads is less or equal to       *
*               the number of threads executing the program               *
*          FALSE otherwise               			                      *
**************************************************************************/


int workNeeded(int numThreads)
{
    int res = FALSE;

    #pragma omp critical (checkThreads)
    {
        if(activeThreads <= numThreads)
        {
            /* Increment the number of threads that are working */
            activeThreads++;
            res = TRUE;
        }
    }

    return res;
}

/******************************************************************************************************************
*  taskBruteForce - Brute force algorithm with backtracking that generates work for threads that are not working  *
*           and finds a solution if there's one, otherwise prints No Solution                                     *
*                                                                                                                 *
* Receives: Board *b       - a game board                                                                         *
*           int start      - position of the board from which the algorithm starts                                *
*           int numThreads - number of threads that are executing the program                                     *
*           int threshold  - number of fixed cells count from the start index for each game board                 *
*                                                                                                                 *
******************************************************************************************************************/

void taskBruteForce(Board *b, int start, int numThreads, int threshold)
{
    int i = 0, valid = TRUE;

    /* Each thread will start at the same time where 
    all the possible values for the first unfixed cell were tested and added to the pool of tasks*/
    while(flagStart != b->size)
    {}
    
    /* Flag that is used to know when the threads start working */
    if(flagINI == 0)
    {
        #pragma omp critical (updateFlagINI)
        {
            flagINI = 1;
            /* Keep track of how many threads are working */
            if(numThreads <= numIniActiveThreads)
            {
                activeThreads = numThreads;
            }
            else
            {
                activeThreads = numIniActiveThreads;
            }
        }
    }

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

        /* Looks for an unfixed cell */
        if(b->gameBoard[i].fixed == FALSE)
        {
            /* Check if there's work for all threads after square size unfixed cells filled and if the number of threads is != from 1 */
            if(numThreads != 1 && (((i - start) % b->squareSize) == threshold) && workNeeded(numThreads))
            {   
                /* Create work to one of other threads if possible */
                if((valid = makeGuess(b, i)) == TRUE)
                {
                    Board *new = copyBoard(b);

                    /* Create a task with that new board */
                    #pragma omp task
                    {
                        taskBruteForce(new, i + 1, numThreads, 0);
                        freeBoard(new);
                        free(new);
                    }
                }
            } 
            
            /* Checks if there is a valid guess for that index, to the thread that is currently working */
            if((valid = makeGuess(b, i)) == FALSE)
            {
                /* If there's no possible value for that unfixed index, it has to backtrack to the last unfixed cell */
                do
                {
                    /* Put back the 0 value into the unfixed cell and removes masks */
                    if(b->gameBoard[i].fixed == FALSE)
                    {
                        removeMasks(b, i);
                        b->gameBoard[i].value = 0;
                    }
                    else
                    {
                        /* Decrements the variable that keeps track of the fixed cells on that board */
                        threshold--;
                    }

                    i--;

                }while (i >= start && (b->gameBoard[i].fixed == TRUE || b->gameBoard[i].value == b->size));

                /* If the index is < 0, then decrements the number of active threads */
                if(i < start)
                {
                    if(numThreads != 1)
                    {
                        #pragma omp critical (checkThreads)
                        {
                            activeThreads--;
                        }
                    }
                
                    return;
                }
            }
        }
        else
        {
            /* Increments the variable that keeps track of the fixed cells on that board */
            threshold++;
        }
    }

    /* In that case, a thread found a solution and has to update the END flag to abort other threads */
    #pragma omp critical (print)
    {
        if(END == FALSE)
        {
            END = TRUE;
            /* Prints the solution */
            printBoard(b);
        }
    }

    return;
}

/****************************************************************************************************************
*  solver - Looks for the first unfixed cell, assigns the first possible values to copies of the original board *
*           and create tasks to find a solution for the sudoku calling the taskBruteForce() function            *
*                                                                                                               *
* Receives: Board *b - a game board                                                                             *
****************************************************************************************************************/

void solver(Board *b)
{    
    int start_index = 0;

    /* Looks for the first unfixed cell */
    while(b->gameBoard[start_index].fixed == TRUE && start_index < b->size * b->size)
    {
        start_index++;
    }

    /* If the start_index isn't the last cell then the board given isn't completly solved */
    if(start_index < b->size * b->size)
    {
        /* Initiates the parallel code */
        #pragma omp parallel shared( b) firstprivate(start_index)
        {
            int threshold = 0;
            int numThreads = omp_get_num_threads();
            
            /* Assigns the possible values of the first unfixed cell and creates a task for each board with each possibility */
            #pragma omp for schedule(dynamic)
            for (int i = 1; i <= b->size; i++)
            {
                 if(checkValidityMasks(b, start_index, i) == TRUE)
                 {
                    Board *new = copyBoard(b);

                    /* Increments the number of threads that will be working from the beggining */
                    #pragma omp atomic
                    numIniActiveThreads++;

                    new->gameBoard[start_index].value = i;
                    updateMasks(new, start_index);

                    /* Create tasks */
                    #pragma omp task
                    {
                        taskBruteForce(new, start_index + 1, numThreads, threshold);
                        freeBoard(new);
                        free(new);
                    }
                }

                /* Flag used to know when all the possibilities were tested and when threads will pick up a board
                and start performing the taskBruteForce() function */ 
                #pragma omp atomic
                flagStart++;
            }       
        }
    }
    else 
    {
        /* If the board read has no unfixed cells, it will be checked if the game board is valid and in that case is printed,
        otherwise prints Invalid Sudoku */
        if(checkAllBoard(b) == TRUE)
        {
            printBoard(b);
        }
        else
        {
            printf("Invalid Sudoku\n");
        }
        
        return;
    }

    if(END == FALSE)
    {
        printf("No solution\n");
    }

    return;
}

/*******************************************************************************
*  main - Allocs a board, fills that board and find a solution if there's one  *
*                                                                              *
* Receives: name of the file that contains the sudoku map                      *
* Returns: 0 at exit                                                           *                                            
*******************************************************************************/

int main(int argc, char const *argv[]) {

    Board *board = NULL;

    /* Checks if the user gives the input file */
    if(argv[1] != NULL)
    {
        board = (Board*)malloc(sizeof(Board));
        if(board == NULL){
            return 1;
        }
        if((fillGameBoard(board, argv[1])) == 0)
        {
            /* Finds a solution if there's one */
            solver(board);

            /* Frees memory allocated */      
            freeBoard(board);
        }

        /* Frees memory allocated */      
        free(board);
    }
    else
    {
        /* If theres's no input file given by the user */
        printf("No file was specified!\n");
    }
   
    return 0;
}