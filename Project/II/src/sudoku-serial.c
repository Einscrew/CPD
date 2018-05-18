#include <mpi.h>
#include <unistd.h>
#include "sudoku-mpi.h"

int NoSolution = TRUE;
/*******************************************************
*    Makes a guess in a game board at a given index    *
*                                        			   *
* Returns: TRUE if it finds a valid guess     		   *
*          FALSE otherwise               			   *
*******************************************************/

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

/****************************************************************************************
*    Brute force algorithm that receives a board and finds a solution if there's one    *            										   	    *
*****************************************************************************************/

//void bruteForce(Board *b){
int bruteForce(Board *b, int start, int id, int p, int askWork){

	int i = 0, valid = TRUE;

	for(i = 0; i < b->size*b->size; i++)
	{
		if(valid == FALSE)
		{
			i--;
			valid = TRUE;
		}
        
        /* Looks for an unfixed cell */
		if(b->gameBoard[i].fixed == FALSE)
		{
            /* Checks if there is a valid guess for that index */
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

					i--;

				}while (i >= 0 && (b->gameBoard[i].fixed == TRUE || b->gameBoard[i].value == b->size));

				/* If the index is < 0, then there's no solution for the gicen sudoku */
				if(i < 0)
                {
					printf("No solution\n");
					return 1;
				}
			}
		}
	}

	/* If the solution is find, print it */
	printBoard(b);
    return 0;
}

void checkPossibilities(Board * b, int cindex, int index, int * possible, int id){
    int value = 0;
    if(cindex > b->size*b->size-1){
        NoSolution = FALSE;

        if(id == 0){
            printBoard(b);
            fflush(stdout);
        }
        return;
    }
    if(b->gameBoard[cindex].fixed){
        checkPossibilities(b, cindex+1, index+1, possible, id);
    }
    else{
        for (value = 1; value <= b->size; value++)
        {
        
            if(checkValidityMasks(b, cindex, value) == TRUE)
            {
                if(cindex == index){
                    *possible +=1;
                }
                else{
                    b->gameBoard[cindex].value = value;
                    updateMasks(b, cindex);
                    checkPossibilities(b, cindex+1, index, possible, id);
                    removeMasks(b, cindex);
                    b->gameBoard[cindex].value = 0;
                }

            }
        }
        
    }
}

void checkPossibilitiesL(Board * b, int cindex, int index, int * possibleFinal, int * possiblePrev, int id, int p){
    int value = 0;

    if(cindex > b->size*b->size-1){
        NoSolution = FALSE;

        if(id == 0){
             
            printBoard(b);
            fflush(stdout);
        }
        return;
    }
    if(b->gameBoard[cindex].fixed){
        checkPossibilitiesL(b, cindex+1, index+1, possibleFinal, possiblePrev, id, p);
    }
    else{
        for (value = 1; value <= b->size; value++)
        {
            if(cindex == index-1 && *possiblePrev+*possibleFinal >= p){
                return;
            }
            if(checkValidityMasks(b, cindex, value) == TRUE)
            {
                if(cindex == index-1){
                   *possiblePrev -=1;
                }
                if(cindex == index){
                    *possibleFinal +=1;
                }
                else{//(cindex != index){
                    b->gameBoard[cindex].value = value;
                    updateMasks(b, cindex);
                    checkPossibilitiesL(b, cindex+1, index, possibleFinal, possiblePrev, id, p);
                    removeMasks(b, cindex);
                    b->gameBoard[cindex].value = 0;
                }

            }
        }
        
    }
}


void giveWork(Board * b, int cindex, int index, int * possibleFinal, int * possiblePrev, int id, int p, int *possible){
    int value = 0;

    if(cindex > b->size*b->size-1){
        NoSolution = FALSE;

        if(id == 0){
             
            printBoard(b);
            fflush(stdout);
        }
        return;
    }
    if(b->gameBoard[cindex].fixed){
        giveWork(b, cindex+1, index+1, possibleFinal, possiblePrev, id, p, possible);
    }
    else{
        for (value = b->size; value >= 1; value--)
        {
            if(checkValidityMasks(b, cindex, value) == TRUE)
            {  
                if(cindex == index && *possibleFinal > 0){
                    if(*possible%p == id){
                        b->gameBoard[cindex].value = value;
                        updateMasks(b, cindex);
                        printf("[%d]------------------------\n", id);
                        printBoard(b);
                        *possibleFinal-=1;
                        removeMasks(b, cindex);
                        b->gameBoard[cindex].value = 0;
                    }
                    *possible +=1;
                }else if(cindex == index-1 && *possiblePrev > 0){
                    if(*possible%p == id){
                        b->gameBoard[cindex].value = value;
                        updateMasks(b, cindex);
                        printf("[%d]------------------------\n", id);
                        printBoard(b);
                        *possiblePrev-=1;
                        removeMasks(b, cindex);
                        b->gameBoard[cindex].value = 0;
                    }
                    *possible +=1;
                }
                else{//(cindex != index){
                    b->gameBoard[cindex].value = value;
                    updateMasks(b, cindex);
                    giveWork(b, cindex+1, index, possibleFinal, possiblePrev, id, p, possible);
                    removeMasks(b, cindex);
                    b->gameBoard[cindex].value = 0;
                }

                

            }
        }
        
    }
}


/****************************************************************************
*    Allocs a board, fills that board and find a solution if there's one    *
*                                        			   						*
* Returns: 0 at exit     		   											*        								    
****************************************************************************/
/*
mpicc -c sudoku-serial.c board.h -g -Wall -std=c99
mpicc -o sudoku-serial sudoku-serial.o board.o -g -Wall -std=c99
*/
int main(int argc, char *argv[]) {

    Board *board = (Board*)malloc(sizeof(Board));
    char * r;
    char * final;
    double t = 0;
    /* Checks if teh user gives the input file */
    int i, id=3, p=8, possibleFinal = 0, possiblePrev = 0, index = 0, possible = 0;


    if((fillGameBoard(board, argv[1])) == 0)
    {
        checkPossibilities(board, 0, 0, &possibleFinal, id);
        index++;
        printf("[%d]-trying: %d - Final: %d   Prev %d   P: %d\n", id, index, possiblePrev, possibleFinal,  possibleFinal+possiblePrev);
        while(p > possibleFinal+possiblePrev){

            possiblePrev = possibleFinal;
            possibleFinal = 0;
            checkPossibilitiesL(board, 0, index, &possibleFinal, &possiblePrev, id, p);
           
            if(NoSolution == FALSE){
                return 0;
            }
            //#ifdef OUT 
            printf("[%d]-trying: %d - Final: %d   Prev %d   P: %d\n", id, index, possibleFinal ,possiblePrev,  possibleFinal+possiblePrev);
            fflush(stdout);
            //#endif
            index++;
        }

        index--;

        giveWork(board, 0, index, &possibleFinal, &possiblePrev, id, p, &possible);
        
        freeBoard(board);
    }
    else{
        printf("No file was specified!\n");
    }
    free(board);


    return 0;
}


