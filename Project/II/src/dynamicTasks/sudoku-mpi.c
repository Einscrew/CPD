#include <mpi.h>
#include <omp.h>
#include <unistd.h>
#include "sudoku-mpi.h"

//#define OUT 1
#define FINISH 111
#define PRINT 222
#define GET_WORK 333
#define ASK_WORK 444

char found = FALSE;
Board * finalBoard = NULL;
char print = FALSE;
char stopwork = FALSE;
char haveBoard = FALSE;
int startindex = 0;

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
            /*
            if((valid = makeGuess(  , i)) == TRUE){
                Test(wr, &flag, &s);
                if(flag != 0){
                    //SEND WORK DIRECTLY TO...
                    //MAKE MPI_Irec() to keep listening 4 other requests
                    //flag = 0

                    (valid = makeGuess(b, i)) == FALSE; 
                }
            }*/
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
            printf("[%d] terminated bruteForce\n", id);
            fflush(stdout);
            stopwork = TRUE;
            return FALSE;
        }


	}
	/* If the solution is find, print it */


    // send Solution Alert
    //int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
    printf("[%d]FINISH\n", id);
    fflush(stdout);

    finalBoard = b;
    found = TRUE;
    return TRUE;
}

void termination(int id, int p,  MPI_Request *rr, char *fid){
    
    MPI_Request rs;
    MPI_Status s;
    char copyid;
    char pid = -1, finish = 0;
    int from = (id==0)?(p-1):(id-1), flagr = 0, flags = 0;


    //APAGAR
    int recv = 0, send = 0;
    
    
    copyid = id;
    

    if(found==TRUE) MPI_Isend(&copyid, 1, MPI_CHAR, (id+1)%p, FINISH, MPI_COMM_WORLD, &rs);

    //printf("[%d]copyid %d\n", id, copyid);

    while(recv == 0){
        //trabalhjo.amawokdawd
        //awidampwd
        MPI_Test(rr ,&flagr ,&s);
        recv = (flagr==0)?recv:1;
    }
    //printf("[%d] recebeu fid: %d\n", id, fid );
    if(found == TRUE){
        while(send == 0){
            //trabalhjo.amawokdawd
            //awidampwd
            MPI_Test(&rs ,&flags ,&s);
            send = (flags==0)?send:1;
        }
    }

    if(*fid >= id || found == FALSE){
        //if(*fid == id) printf("1st msg by [%d]\n", id );
        if(*fid > pid)
            pid = *fid;
    
        //printf("[%d] vai tentar enviar fid: %d\n", id, *fid );
        MPI_Send(fid, 1, MPI_CHAR, (id+1)%p, FINISH, MPI_COMM_WORLD);
        //printf("[%d] enviou  fid: %d\n", id, *fid );
    }
    
    do{
        MPI_Recv(fid, 1,MPI_CHAR, from, FINISH, MPI_COMM_WORLD, &s);
        //printf("[%d] recebeu fid: %d ---\n", id, *fid );
        if(*fid == pid || *fid == id){
            if(*fid == pid) finish = 1;
            if(pid != id){
                MPI_Send(fid, 1, MPI_CHAR, (id+1)%p, FINISH, MPI_COMM_WORLD);
                //printf("[%d] enviou  fid: %d ---\n", id, *fid );
            }
            pid = *fid;
        }else if(*fid > pid){
            //printf("[%d]dawdaw fid:%d pid:%d\n",  id, *fid, pid);
            pid = *fid;
            
            if(*fid >= id){
                MPI_Send(fid, 1, MPI_CHAR, (id+1)%p, FINISH, MPI_COMM_WORLD);
                //printf("[%d] enviou  fid: %d ---\n", id, *fid );
            }
        }
    }while(finish == 0);

    if(*fid == id){
        //printf("\n[%d]IHNAWDIAWND\n\n", id);
        printBoard(finalBoard);
    }

}

int check(Board *b, int id, int p){

    char fid = FALSE;
    int size;
    MPI_Status s;
    
    printf("[%d] @ check()\n",id);

    MPI_Request r;
    MPI_Irecv(&fid, 1, MPI_CHAR, (id==0)?p-1:id-1, FINISH, MPI_COMM_WORLD, &r );
    
    while(!stopwork || !found){
        if(haveBoard){
            printf("[%d] have board\n", id);
            bruteForce(b, startindex, id, p, &fid, &r);
            if(found == FALSE && stopwork == FALSE)
                haveBoard = FALSE;
            
        }else{
            printf("[%d] don't have board\n", id);
            
            //send request to neighbors
            //MPI_SEND____
            while(){
                //CHECK for new board on probe
                MPI_Iprobe(MPI_ANY_SOURCE, GET_WORK, MPI_COMM_WORLD, &flag, &s);
                if(flag != 0){
                    MPI_Get_count(&s, MPI_BYTE, &size);
                    r = malloc(size); //<......................size in BYTES???????
                    MPI_Recv(r, size, MPI_BYTE, s.MPI_SOURCE );
                }

                //Test any other neighbor with no work
            }   

        }
        
        if(stopwork == TRUE)
            printf("[%d]Stopped Work\n", id);
        
        if(found == TRUE)
            printf("[%d]Discovered a solution\n", id);
        if(found == FALSE && stopwork == FALSE){
            printf("[%d]No more Work\n", id);
            fflush(stdout);
        }
        
    }
    
    
    termination(id, p, &r, &fid);

    return 0;

}

/****************************************************************************
*    Allocs a board, fills that board and find a solution if there's one    *
*                                        			   						*
* Returns: 0 at exit     		   											*        								    
****************************************************************************/

int main(int argc, char *argv[]) {

    Board *board = (Board*)malloc(sizeof(Board));
    char * r;
    char * first = malloc(sizeof(char)+sizeof(int));
    int s = 0 ;
    /* Checks if teh user gives the input file */
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
                s = compressBoard(board, 1, board->size*board->size-1 , &r);
                memcpy(first, &board->squareSize, sizeof(char));
                memcpy(&first[sizeof(char)], &s, sizeof(int));
                haveBoard = TRUE;
                /*halfbruteForce(board);
                free(r);

                s = compressBoard(board, 0, board->size*board->size/2, &r );

                decompressBoard(board, r, s);
                printBoard(board);
                free(r);
                freeBoard(board);
                free(board);*/
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
    MPI_Bcast(first, sizeof(char)+sizeof(int), MPI_BYTE, 0, MPI_COMM_WORLD);

   if(id){

        memcpy(&board->squareSize, first, sizeof(char));
        board->size = board->squareSize*board->squareSize;
        board->gameBoard = NULL;

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
        
        //printf("%d\n", board->squareSize);
        //printf("Does process %d work? %c\n",id, (check(board))?'Y':'N' );
        /*if(id==1){
            printf("TIME:::%lf\n", t);
            printBoard(board);
        }*/
    }
        


    free(r); //<--------------------------------------------------------------------------
    
    check(board, id, p);

    freeBoard(board);
    free(board);

    

    //MPI_Recv

    MPI_Finalize();
    
    return 0;
}


