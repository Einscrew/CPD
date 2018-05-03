#include <mpi.h>
#include <omp.h>
#include <unistd.h>
#include "sudoku-mpi.h"

#define FINISH 123
#define PRINT 231
char found = FALSE;
Board * finalBoard = NULL;
char print = FALSE;

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

int bruteForce(Board *b, int start, int id, int p, char * fid, MPI_Request *r){
            
    char trash = FALSE;
	int i = start, valid = TRUE, flag = FALSE;

    MPI_Status s;

    //if MPI_Test)

	for(i = start; i < b->size*b->size; i++)
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

				}while (i >= start && (b->gameBoard[i].fixed == TRUE || b->gameBoard[i].value == b->size));

				/* If the index is < 0, then there's no solution for the gicen sudoku */
				if(i < start)
                {
					//printf("No solution\n");
					return FALSE;
				}
			}
		}

        MPI_Test(r, &flag, &s);
        if (flag != 0){
            printf("OLLLLLLLLLLL\n");
            fflush(stdout);
            if(!id){
                if(!print){
                    print = TRUE;
                    trash = TRUE;
                }
                MPI_Send(&trash, 1, MPI_CHAR, *fid, PRINT, MPI_COMM_WORLD);
                trash = FALSE;

            }
            found=TRUE;
            MPI_Send(fid, 1, MPI_CHAR, (id+1)%p, FINISH, MPI_COMM_WORLD);
            return FALSE;
        }
	}
	/* If the solution is find, print it */


    // send Solution Alert
    //int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
    printf("[%d]FINISH\n", id);
    fflush(stdout);
            
    found = TRUE;
    *fid = id;
    MPI_Send(fid, 1, MPI_CHAR, (id+1)%p, FINISH, MPI_COMM_WORLD);
    printf("[%d]Alert\n", id);


    if(id){
        MPI_Recv(&trash, 1, MPI_CHAR, 0, PRINT, MPI_COMM_WORLD, &s);
        if(trash)printBoard(b);
    }
    else
        printBoard(b);

    printf("[%d]print\n", id);
 


    //RECEIVE PRINT ORDER

    return TRUE;
}

void checkPossibilities(Board * b, int cindex, int index, int * possible){
    if(cindex > b->size*b->size-1)
        return;
    if(b->gameBoard[cindex].fixed){
        checkPossibilities(b, cindex+1, index+1, possible);
    }
    else{
        for (int value = 1; value <= b->size; value++)
        {
        
            if(checkValidityMasks(b, cindex, value) == TRUE)
            {
                if(cindex == index){
                    *possible +=1;
                }
                else{
                    b->gameBoard[cindex].value = value;
                    updateMasks(b, cindex);
                    checkPossibilities(b, cindex+1, index, possible);
                    removeMasks(b, cindex);
                    b->gameBoard[cindex].value = 0;
                }

            }
        }
        
    }
}

void work(Board * b, int cindex, int index, int * possible, int id, int p, char *fid, MPI_Request *r){

    if(cindex > b->size*b->size-1)
        return;
    if(b->gameBoard[cindex].fixed){
        work(b, cindex+1, index+1, possible, id, p, fid, r);
    }
    else{
        for (int value = 1; value <= b->size; value++)
        {
            if (found == TRUE){
                return;
            }
            if(checkValidityMasks(b, cindex, value) == TRUE)
            {
            
                if(cindex == index){
                    if(*possible%p == id){
                        #ifdef OUT 
                        printf("[%d]-Will process %d @ index: %d\n", id, *possible, cindex);
                        #endif
                        b->gameBoard[cindex].value = value;
                     
                        updateMasks(b, cindex);     
                        bruteForce(b, cindex+1, id, p, fid, r);
                        removeMasks(b, cindex);
                        b->gameBoard[cindex].value = 0;
                    }
                    *possible +=1;
                }
                else{
                    b->gameBoard[cindex].value = value;
                    updateMasks(b, cindex);
                    work(b, cindex+1, index, possible, id, p, fid, r);
                    removeMasks(b, cindex);
                    b->gameBoard[cindex].value = 0;
                }
            }
        }
        
    }
}

int check(Board *b, int id, int p){

    int possible = 0;
    int index = 0, flag = 0;
    char fid = FALSE;

    char trash = FALSE;

    MPI_Request r;
    MPI_Status s;

    double t = -MPI_Wtime();
    //IF BOARD COMPLETE ...........................................
    
    while(p > possible){
        possible = 0;
        checkPossibilities(b, 0, index, &possible);
        #ifdef OUT 
        printf("[%d]-trying: %d - p: %d\n",id, index, possible);
        #endif
        index++;
        if(possible == 0){
            break;
        }
        
    }
    t += MPI_Wtime();
    index--;
    #ifdef OUT 
    printf("[%d]-Index: %d\n\tPossibilities: %d\tTime: %lf\n",id,  index, possible, t);
    #endif
    
    if(possible != 0){
        possible = 0;
        #ifdef OUT 
        printf("[%d]-Going to work: index: %d\tpossible: %d\tid: %d\tp: %d\n",id,  index, possible, id, p);
        #endif

        //int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request)
        MPI_Irecv(&fid, 1, MPI_CHAR, (id==0)?p-1:id-1, FINISH, MPI_COMM_WORLD, &r );
        work(b, 0, index, &possible, id, p, &fid, &r);

        
        printf("[%d]No more Work\n", id);
        fflush(stdout);
        

        while(!flag){
            MPI_Test(&r, &flag, &s);
            if (flag != 0){

                if(!id){
                    if(!print){
                        print = TRUE;
                        trash = TRUE;
                    }
                    MPI_Send(&trash, 1, MPI_CHAR, fid, PRINT, MPI_COMM_WORLD);
                    trash = FALSE;

                }
                found=TRUE;
                MPI_Send(&fid, 1, MPI_CHAR, (id+1)%p, FINISH, MPI_COMM_WORLD);
            }
        }
        printf("[%d]Leaving\n", id);
        fflush(stdout);

    }else{   
        #ifdef OUT 
        printf("[%d]-Didn't work\n", id );
        #endif
    }

    return possible;

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
    int id, p;
    double t = 0;

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

            }
        }
        else
        {
            /* If theres's no input file given by the user */
            printf("No file was specified!\n");

        }
    }
       
    //verificar isto

    
    MPI_Barrier(MPI_COMM_WORLD);
    t = -MPI_Wtime();
    MPI_Bcast(first, sizeof(char)+sizeof(int), MPI_BYTE, 0, MPI_COMM_WORLD);

   if(id){

        memcpy(&board->squareSize, first, sizeof(char));
        board->size = board->squareSize*board->squareSize;

        memcpy(&s, &first[sizeof(char)], sizeof(int));

        r = malloc(s);
    }
    
    //printf("s %d\n", s);
    free(first);

    MPI_Bcast(r, s, MPI_BYTE, 0, MPI_COMM_WORLD);

    if(id){
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
        t += MPI_Wtime();
        //printf("%d\n", board->squareSize);
        //printf("Does process %d work? %c\n",id, (check(board))?'Y':'N' );
       /* if(id==1){
            printf("TIME:::%lf\n", t);
            printBoard(board);
        }*/
    }

    free(r); //<--------------------------------------------------------------------------

    check(board, id, p);

    freeBoard(board);
    free(board);
    //c

    //MPI_Recv

    MPI_Finalize();
    
    return 0;
}


