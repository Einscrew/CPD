#include <mpi.h>
#include <unistd.h>
#include "sudoku-mpi.h"

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

void bruteForce(Board *b){

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
					return;
				}
			}
		}
	}

	/* If the solution is find, print it */
	printBoard(b);
}

/****************************************************************************
*    Allocs a board, fills that board and find a solution if there's one    *
*                                        			   						*
* Returns: 0 at exit     		   											*        								    
****************************************************************************/

int main(int argc, char *argv[]) {

    Board *board = (Board*)malloc(sizeof(Board));;
    char * r;
    char * first = malloc(sizeof(char)+sizeof(int));
    int s = 0 ;
    /* Checks if teh user gives the input file */
    MPI_Status status;
    int id, p;
                
    // MPI_INIT & MPI_COMM
    MPI_Init (&argc, &argv);
    
    MPI_Comm_rank (MPI_COMM_WORLD, &id);
    MPI_Comm_size (MPI_COMM_WORLD, &p);

    if(!id){
        if(argv[1] != NULL)
        {

            if((fillGameBoard(board, argv[1])) == 0)
            {
                r = NULL;
                s = compressBoard(board, 1, -1, &r);
                memcpy(first, &board->squareSize, sizeof(char));
                memcpy(&first[sizeof(char)], &s, sizeof(int));
                
                
                /*
                printf(">%d-%d|\n>",first[0], s);
                for (long int i = 0; i < s; ++i){
                    printf("%d", r[i]);
                    if((i+1)%5==0){
                        printf("|");
                    }
                }
                printf("\n");
                decompressBoard(board, r, s);
                printBoard(board);
                printf("\n...................................................\n");
                
                bruteforce();
                compressBoard(board, 1, 1, &final);
                */

                //VVVVVVV
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
    }
       
    //verificar isto
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Bcast(first, sizeof(char)+sizeof(int), MPI_BYTE, 0, MPI_COMM_WORLD);

   if(id){

        memcpy(&board->squareSize, first, sizeof(char));
        board->size = board->squareSize*board->squareSize;

        memcpy(&s, &first[sizeof(char)], sizeof(int));

        r = malloc(s);
    }
    
    //printf("s %d\n", s);

    MPI_Bcast(r, s, MPI_BYTE, 0, MPI_COMM_WORLD);

    if(id==1){
        /*
        printf(">%d|",r[0]);
        for (long int i = 1; i < s; ++i){
            printf("%d", r[i]);
            if((i)%5==0){
                printf("|");
            }
        }
        printf("\n");
        fflush(stdout);
        */
        decompressBoard(board, r, s);
        printf("%d\n", board->squareSize);
        printBoard(board);
        freeBoard(board);
        free(board);
    }

    free(r);

    //MPI_Recv

    MPI_Finalize();
    
    return 0;
}


