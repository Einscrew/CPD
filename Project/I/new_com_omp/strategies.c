#include "strategies.h"

Stack *fillStack(Board *b, Stack *head){

    int i = 0, num = 0, increasePos = FALSE;
    int index = 0;

    Board *currBoard = (Board*)malloc(sizeof(Board));
    currBoard->size = b->size;
    currBoard->squareSize = b->squareSize;
    currBoard = allocBoard(currBoard);

    for(i = 0; i < b->size*b->size; i++){

        if(increasePos == TRUE)
        {
            i--;
            increasePos = FALSE;
        }

        /* Only cares about the non fixed values */
        if(b->gameBoard[i].fixed == FALSE){

            if(b->gameBoard[i].value == 0)
            {
                b->gameBoard[i].value = b->gameBoard[i].minPoss;
            }

            do
            {
                /* Starts by trying the first possible value for the cell */
                b->gameBoard[i].value++;

                if(checkValidity(b, i))
                {
                    head = pushElem(head, *b, i);
                    //printBoard(&(head->boardCopy));

                    if(head == NULL)
                    {
                        return FALSE;
                    }
                    else
                    {
                        num++;
                    }
                }

            }while(b->gameBoard[i].value < b->size);

            if(b->gameBoard[i].value == b->size)
            {
                if(checkEmpty(head) == FALSE && num < b->size)
                {
                    /* Pops the head and assigns its board and index to currBoard and index, respectively */
                    head = popElem(head, currBoard, &index);
                }
                else
                {
                    break;
                }

                b = currBoard;
                i = index;

                num--;

                increasePos = TRUE;

                /* Look for the next unfixed value */
                do
                {
                    i++;
                    if(i == b->size*b->size - 1) /* Means that a solution was found */
                    {
                        head = pushElem(head, *b, i);
                        break;
                    }

                }while((b->gameBoard[i].fixed == TRUE && i < b->size*b->size) || (b->gameBoard[i].fixed == FALSE && b->gameBoard[i].value == b->size));
            }
        }
    }

    freeBoard(currBoard);
    free(currBoard);

    return head;
}

Board *solveSudoku(Stack *head, Board *b, Board *solution)
{
    //Stack *current = NULL;
    int i = 0, solutionFound = FALSE, inc = 0, index = 0;
    Board *currBoard = NULL;

    head = fillStack(b, head);

    omp_set_num_threads(8);

    #pragma omp parallel shared(solutionFound, solution, head, b) private(i, inc, index) firstprivate(currBoard)
    { 
        while(solutionFound == FALSE && head != NULL)
        {
        	#pragma omp critical (pop)
        	{
        		if(head != NULL)
				{	  	
		        	currBoard = (Board*)malloc(sizeof(Board));
				    currBoard->size = b->size;
				    currBoard->squareSize = b->squareSize;
				    currBoard = allocBoard(currBoard);
			    	head = popElem(head, currBoard, &index);
			    }
			    else
			    {
			    	currBoard = NULL;
			    }
	        	//current = getElem(&head);
        	}

        	if(currBoard != NULL)
        	{
	            /* Look for the next unfixed value */
	            i = incPosition(currBoard, index);
	            currBoard->gameBoard[i].value = currBoard->gameBoard[i].minPoss;

	            inc = 1;

	            while(i < b->size * b->size && solutionFound == FALSE && inc > 0)
	            {
	            	//printf("i: %d\n", i);
	                if(currBoard->gameBoard[i].value < b->size && currBoard->gameBoard[i].fixed == FALSE)
	                {
	                    currBoard->gameBoard[i].value++;

	                    if(checkValidity(currBoard, i) == TRUE)
	                    {
	                        i = incPosition(currBoard, i);
	                        currBoard->gameBoard[i].value = currBoard->gameBoard[i].minPoss;
	                        inc++;
	                    }
	                }
	                else
	                {
	                    /* Turns back to the initial value */
	                    currBoard->gameBoard[i].value = 0;

	                    i = decPosition(currBoard, i);
	                    inc--;
	                }
	            }
	            
	            
	        	if(i == b->size*b->size)
	            {
	            	//printf("SOLUÇÃO FOUND by thread %d\n", omp_get_thread_num());
	                solutionFound = TRUE;
	                solution = copyBoard(currBoard, solution);
	                free(currBoard->gameBoard);		
	       			free(currBoard);
	                continue;
	            }

	            if(currBoard->gameBoard != NULL)
	            {           	
	            	free(currBoard->gameBoard);	
	           		free(currBoard);
	            }

        	}        	    	
        }
    }

    return solution;
}


int incPosition(Board *b, int index)
{
    do
    {
        index++;

    }while(b->gameBoard[index].fixed == TRUE && index < b->size*b->size);

    return index;
}

int decPosition(Board *b, int index)
{
    do
    {
        index--;

    }while(index > 0 && b->gameBoard[index].fixed == TRUE);

    return index;
}
