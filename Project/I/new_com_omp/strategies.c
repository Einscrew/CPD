#include "strategies.h"

Stack *bruteforce(Board *b, Stack *head){

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
                    head = pushElem(&head, *b, i);

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
                        head = pushElem(&head, *b, i);
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
    Stack *current = NULL;
    int i = 0, solutionFound = FALSE, inc = 0;

    head = bruteforce(b, head);

    omp_set_num_threads(4);

    #pragma omp parallel shared(solutionFound) private(i, current, inc)
    {
        #pragma omp critical
        current = getElem(&head);

        while(solutionFound == FALSE && current != NULL)
        {
            i = current->index;

            /* Look for the next unfixed value */
            i = incPosition(&(current->boardCopy), i);
            current->boardCopy.gameBoard[i].value = current->boardCopy.gameBoard[i].minPoss;

            inc = 1;

            while(i < b->size * b->size && solutionFound == FALSE && inc > 0)
            {
                if(current->boardCopy.gameBoard[i].value < b->size && current->boardCopy.gameBoard[i].fixed == FALSE)
                {
                    current->boardCopy.gameBoard[i].value++;

                    if(checkValidity(&(current->boardCopy), i) == TRUE)
                    {
                        i = incPosition(&(current->boardCopy), i);
                        current->boardCopy.gameBoard[i].value = current->boardCopy.gameBoard[i].minPoss;
                        inc++;
                    }
                }
                else
                {
                    /* Turns back to the initial value */
                    current->boardCopy.gameBoard[i].value = 0;

                    i = decPosition(&(current->boardCopy), i);
                    inc--;
                }
            }

            if(i == b->size*b->size)
            {
                solutionFound = TRUE;
                solution = copyBoard(&(current->boardCopy), solution);
                continue;
            }

            free(current->boardCopy.gameBoard);
            free(current);

            #pragma omp critical
            current = getElem(&head);
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
