#include "strategies.h"


void fillQueue(Queue *q, Board *b, int threshold, int level)
{
    int i = 0;

    push(b, q, -1);

    /*
    if(threshold == b->size*b->size)
    {
        return;
    }

    if(b->gameBoard[level].fixed == FALSE)
    {
        for(i = 1; i <= b->size; i++)
        {
            b->gameBoard[level].value = i;
            if(checkValidity(b, level) == TRUE)
            {
                if(level < threshold)
                {
                    fillQueue(q, b, threshold, level+1);
                }
                else
                {
                    //ver copy
                    if(push(b, q, level) == 0)
                    {
                        printf("Failed to push\n");
                    }
                }
            }
        }

        b->gameBoard[level].value = 0;
    }
    else
    {
        fillQueue(q, b, threshold+1, level+1);   
    } */                 
}

void testQueue(Board *b)
{
    Queue *one = create();
    Queue *two = create();

    int i = 0;

    push(b, one, i);
    push(b, two, i);
    push(b, one, i);
    push(b, two, i);
    push(b, one, i);
    push(b, two, i);
    
    printQueue(one);
    printQueue(two);

    printf("merge: %d\n", merge(one, two));
    printQueue(one);
    printQueue(two);

    
    printf("POP:\n");
    pop(one, &i);
    pop(one, &i);
    pop(one, &i);
    pop(one, &i);
    pop(one, &i);
    Board * bi = pop(one, &i);
    printBoard(bi);
    printQueue(one);

}

int solver(Board *b)
{
    Queue *mainQ = create();
    int i = 0, solFound = FALSE;
    
    fillQueue(mainQ, b, 1, 0);
    printQueue(mainQ);
    printf("Size: %d\n", mainQ->size);
    omp_set_num_threads(4);

    #pragma omp parallel shared(mainQ, solFound) private(i)
    {
        #pragma omp master
            printf("THREADS: %d\n", omp_get_num_threads());

        Board *currBoard = NULL;
        Queue *privQ = create();
        int index = 0;  

        while(solFound == FALSE)
        {
            #pragma omp critical (updateStack)
            {
                currBoard = pop(mainQ, &index);
                
                //printBoard(currBoard);
            }


            if(currBoard != NULL){


                /* Looks for the next empty cell */
                do
                {
                    index++;
                    if(index == currBoard->size * currBoard->size){
                        break;
                    }

                }while(currBoard->gameBoard[index].fixed == TRUE && index < currBoard->size*currBoard->size);
                /* Check if currBoard is the solution was found */
                if(index == currBoard->size * currBoard->size)
                {
                    solFound = TRUE;
                    printBoard(currBoard);
                    continue;
                }

                //printf("AAAAAAAAAAAAAA[%d]\n", index);
                
                //printBoard(currBoard);
                for(i = 1; i <= currBoard->size; i++)
                {
                    currBoard->gameBoard[index].value = i;
                    if(checkValidity(currBoard, index) == TRUE)
                    {
                        /* Push possible boards into the queue */
                       // printf("VVVVVVVVVVVVVV[%d]\n", index);
                       // printBoard(currBoard);
                        push(currBoard, privQ, index);

                    }
                }

                free(currBoard);

                #pragma omp critical (updateStack)
                {
                    merge(mainQ, privQ);
                }
            }
        }
    }

    return 0;

}






int bruteforce(Board *b){

    int i, valid = TRUE;

    for(i = 0; i < b->size*b->size; i++){

        if(valid == FALSE)
        {
            i--;
            valid = TRUE;
        }
        // For non fixed values:
        if(b->gameBoard[i].fixed == FALSE){

            do
            {
                b->gameBoard[i].value++;

                valid = checkValidity(b, i);
                if(valid == TRUE){
                    break;
                }

            }while(b->gameBoard[i].value < b->size);

            if(valid == FALSE){

                //backtrack
                do
                {
                    if(b->gameBoard[i].fixed == FALSE)
                    {
                        b->gameBoard[i].value = 0;
                    }

                    i--;

                }while ((b->gameBoard[i].fixed == TRUE && i > 0) || b->gameBoard[i].value == b->size);

                if(i == 0 && b->gameBoard[i].value == b->size){
                    return FALSE;
                }

            }
        }
    }
    return TRUE;
}

