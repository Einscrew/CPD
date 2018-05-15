#include <mpi.h>
#include <omp.h>
#include <unistd.h>
#include "sudoku-mpi.h"


//#define OUT 1
#define WORK 10
#define IDLE -10
#define NO_WORK 5
#define  GET_WORK 555
#define ASK_WORK 444



Board * originalBoard = NULL;
Board * finalBoard = NULL;

int work4neighbor = FALSE;
int EXIT = FALSE, TERM = FALSE;
int NoSolution = TRUE, sendTermination = FALSE;

int msg;

int startIndex = 0;

int left, right;

int iPrint = FALSE;

MPI_Request msgReq, boardReq;

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


void sendSolution(int id, int p){
    int finish_id = id - p;
    MPI_Status s;

    MPI_Send(&finish_id, 1, MPI_INT, left, ASK_WORK, MPI_COMM_WORLD ); // Receiver has to receive??

    MPI_Wait(&msgReq, &s);

    do{
        if(msg <= finish_id || msg == TERM){
        
            if(msg == finish_id){
                msg = TERM;
                iPrint= TRUE;
            }else if(msg == TERM){
                if(!iPrint)
                    MPI_Send(&msg, 1, MPI_INT, left, ASK_WORK, MPI_COMM_WORLD ); // Receiver has to receive??
                else
                    printBoard(finalBoard);
                EXIT = TRUE;
                return;
            }
            MPI_Send(&msg, 1, MPI_INT, left, ASK_WORK, MPI_COMM_WORLD ); // Receiver has to receive??
        }
        MPI_Recv(&msg, 1, MPI_INT, right, ASK_WORK, MPI_COMM_WORLD, &s);
    }while(1);
}

int giveWork(Board *b, int i, int id){
    int sendFlag = FALSE, receiveFlag = FALSE, size;
    char * compressed  = NULL;
    MPI_Status s;

    if(work4neighbor){

        size = compressBoard(b, 0, i, &compressed);
        MPI_Isend(compressed, size, MPI_BYTE, right, GET_WORK, MPI_COMM_WORLD, &boardReq);     
        
        while(!sendFlag){
            //test if board request was completed
            MPI_Test(&boardReq, &sendFlag, &s);
            if( sendFlag ){
                /*printf("[%d] compressed sent %d @ index: %d\n", id, size, i);
                #ifdef OUT
                #endif
                #ifdef OUT
                printf(">[%d]", id );
                #endif
                for (long int j = 0; j < size; ++j){
                    #ifdef OUT
                    printf("%d", compressed[j]);
                    #endif
                    if((j+1)%5==0){
                        #ifdef OUT
                        printf("|");
                        #endif
                    }
                }
                #ifdef OUT
                printf("\n");
                #endif
                fflush(stdout);*/

                free(compressed);
                work4neighbor = FALSE;
                return WORK;
            }

            //verify if solution found
            MPI_Test(&msgReq, &receiveFlag, &s);
            if( receiveFlag ){
                receiveFlag = FALSE;

                if(msg < 0){
                    MPI_Cancel(&boardReq);
                    free(compressed);
                    return IDLE;
                }
                MPI_Irecv(&msg, 1, MPI_INT, right, ASK_WORK, MPI_COMM_WORLD, &msgReq);
            }
        }
    }else{

        MPI_Test(&msgReq, &receiveFlag, &s);
        if(receiveFlag){
            receiveFlag = FALSE;
            

            if(msg < 0){
                return IDLE;
            }
            else if(msg == right){ // have to give work
                #ifdef OUT
                //printf("right [%d] needs work from %d\n", right, id);
                #endif
                MPI_Irecv(&msg, 1, MPI_INT, right, ASK_WORK, MPI_COMM_WORLD, &msgReq);

                size = compressBoard(b, 0, i, &compressed);
                MPI_Isend(compressed, size, MPI_BYTE, right, GET_WORK, MPI_COMM_WORLD, &boardReq);     
                
                while(!sendFlag){
                    //test if board request was completed
                    MPI_Test(&boardReq, &sendFlag, &s);
                    if( sendFlag ){
                       /* printf("[%d] compressed sent %d @ index: %d\n", id, size, i);
                        #ifdef OUT
                        printf(">[%d]", id );
                        #endif
                        for (long int j = 0; j < size; ++j){
                            #ifdef OUT
                            printf("%d", compressed[j]);
                            #endif
                            if((j+1)%5==0){
                                #ifdef OUT
                                printf("|");
                                #endif
                            }
                        }
                        #ifdef OUT
                        printf("\n");
                        #endif
                        fflush(stdout);*/

                        free(compressed);
                        work4neighbor = FALSE;
                        return WORK;
                    }

                    //verify if solution found
                    MPI_Test(&msgReq, &receiveFlag, &s);
                    if( receiveFlag ){
                        receiveFlag = FALSE;
                        if(msg < 0){
                            MPI_Cancel(&boardReq);
                            free(compressed);
                            return IDLE;
                        }
                        
                        MPI_Irecv(&msg, 1, MPI_INT, right, ASK_WORK, MPI_COMM_WORLD, &msgReq);
                    }
                }

            }
        
            MPI_Irecv(&msg, 1, MPI_INT, right, ASK_WORK, MPI_COMM_WORLD, &msgReq);
        }

    }

    return NO_WORK;

}

void listenNeighbors(int id, int p){
// MPI_Waitany()??????????????????????????????????????????????????????????????????????????????
    int recv = FALSE;
    MPI_Status s;
    
    do{
        
        MPI_Test(&msgReq, &recv, &s);
        
        if(recv){

            if(msg == right || msg == TERM || msg < 0 || (msg >= 0 && work4neighbor == TRUE) || msg == id){

                if(msg < 0){ // Solution found by a working node
                    EXIT = TRUE;
                    NoSolution = FALSE;
                    #ifdef OUT
                    printf("[%d] solution found ->[%d]\n", id, msg);fflush(stdout);
                    #endif
                    //Pass message wait 4 TERM          
                }else if(msg == id){ //No solution
                    sendTermination = TRUE;
                    #ifdef OUT
                    printf("[%d] no solution ->[%d]\n", id, msg);fflush(stdout);
                    #endif

                    //Send TERM wait TERM
                    msg = TERM;
                    EXIT = TRUE;
                }else if(msg == right){ // nothing I can do??? Neighbor need work, I don't have
                    work4neighbor = TRUE;
                    #ifdef OUT
                    printf("[%d] neighbor needs work ->[%d] \n", id, msg);
                    #endif

                    //Pass message wait 4 another
                    //wait = FALSE;                                         ???????????????????
                }else if(msg == TERM){ // exit instruction
                    #ifdef OUT
                    printf("[%d] terminating ->[%d] EXIT = %c\n", id, msg, EXIT ? 'y' : 'n');fflush(stdout);
                    #endif
                    EXIT = TRUE;

                    if(sendTermination == FALSE){
                        // Send TERM
                        MPI_Send(&msg, 1, MPI_INT, left, ASK_WORK, MPI_COMM_WORLD);
                    }

                    if(NoSolution && id == 0){
                        
                        printf("No solution\n");
                        
                        fflush(stdout);
                    } 

                    return;//??
                }else{
                    #ifdef OUT
                    printf("[%d] no match ->[%d]\n", id, msg);fflush(stdout);
                    #endif
                }

                //if(!((msg == TERM) && (NoSolution == TRUE)))
                MPI_Send(&msg, 1, MPI_INT, left, ASK_WORK, MPI_COMM_WORLD); 
            }else{
                #ifdef OUT
                printf("[%d] ignoring ->[%d]\n", id, msg);
                #endif
            }

            MPI_Irecv(&msg, 1, MPI_INT, right, ASK_WORK, MPI_COMM_WORLD, &msgReq);
            recv = FALSE;
        }

    }while(EXIT);
}

int GetOrCheckWork(Board * board, int id){
    int ret = FALSE, size;
    char * compressed = NULL;
    MPI_Status s;

    MPI_Iprobe(left, GET_WORK, MPI_COMM_WORLD, &ret, &s);
    if(ret){
        
        MPI_Get_count(&s, MPI_BYTE, &size);
        #ifdef OUT
        printf("[%d] will receive %d\n", id, size );
        #endif
        fflush(stdout);

        compressed = malloc(size);

        MPI_Recv(compressed, size, MPI_BYTE, left, GET_WORK, MPI_COMM_WORLD, &s);
        
        #ifdef OUT
        long int i = 0;
        printf("<[%d]", id);
        for (i = 0; i < size; ++i){
            printf("%d", compressed[i]);
            if((i+1)%5==0){
                printf("|");
            }
        }
        
        printf("\n");
        
        fflush(stdout);
        #endif

        makeCopyBoard(board, originalBoard);
        
        /*printf("----originalBoard------\n");
        #ifdef OUT
        #endif
        printBoardT(board, id);
        fflush(stdout);*/
        
        startIndex = decompressBoard(board, compressed, size, FALSE);
        free(compressed);
        
        /*printf("----decompressBoard------\n");
        #ifdef OUT
        #endif
        printBoardT(board, id);
        fflush(stdout);*/
        
        
        return TRUE;
    }
    #ifdef OUT
   // printf("[%d] end checkWork()\n", id);
    #endif
    fflush(stdout);

    return FALSE;
}

/****************************************************************************************
*    Brute force algorithm that receives a board and finds a solution if there's one    *            										   	    *
*****************************************************************************************/
int bruteForce(Board *b, int start, int id, int p, int askWork){
    int i = start, valid = TRUE;
    int idcpy = id, area = b->size*b->size;

    for(i = start; i < area ; i++)
    {
        /*if(!id){
            #ifdef OUT
            printf("[%d] ----working------\n", id);
            #endif
            //printBoardT(b, id);
            printBM(b);
            fflush(stdout);
        }else{
            #ifdef OUT
            printf(".");
            #endif
            fflush(stdout);
        }*/
        
        

        if(valid == FALSE)
        {
            i--;
            valid = TRUE;
        }
        
        /* Looks for an unfixed cell */
        if(b->gameBoard[i].fixed == FALSE)
        {
            #ifdef OUT
            //printf("ENTROU i = %d\n", i);
            #endif
            /* Checks if there is a valid guess for that index */
            if((valid = makeGuess( b , i)) == TRUE){

                switch(giveWork(b, i, id)){
                    case IDLE: //alguem encontrou a solucao
                       /* printf("[%d] end giveWork IDLE\n", id);
                       #ifdef OUT
                       #endif
                        fflush(stdout);*/
                        EXIT = TRUE; // wait 4 TERM
                        NoSolution = FALSE;
                        MPI_Send(&msg, 1, MPI_INT, left, ASK_WORK, MPI_COMM_WORLD); 
                        MPI_Irecv(&msg, 1, MPI_INT, right, ASK_WORK, MPI_COMM_WORLD, &msgReq);
                        listenNeighbors(id, p); // always ends the process (EXIT = TRUE)
                        return FALSE;
                        break;
                    
                    case WORK:
                        /*printf("[%d] end giveWork WORK\n", id);
                        #ifdef OUT
                        #endif
                        fflush(stdout);*/
                        
                        valid = makeGuess(b, i); 
                        if(valid == TRUE){
                            #ifdef OUT
                            //printf("EHEHEHEHEHE[%d]@%d\n", id, i);
                            #endif
                        }   
                        break;

                }
                
            /*
            */
            }
            if(valid == FALSE)
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
                    
                    if(askWork){
                        idcpy = id;
                        MPI_Send(&idcpy, 1, MPI_INT, left, ASK_WORK, MPI_COMM_WORLD ); // Receiver has to receive??
                        #ifdef OUT
                        printf("[%d] ASKED work %d\n", id, left);
                        #endif
                        fflush(stdout);
                    }
                    return FALSE;
                }
            }
        }

    }
    
    finalBoard = b;
    //send Solution Alert
    sendSolution(id, p);
    #ifdef OUT
    printf("[%d]FINISH\n", id);
    #endif
    fflush(stdout);
    NoSolution = FALSE;

    return TRUE;
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

void initialWork(Board * b, int cindex, int index, int * possible, int id, int p){
    int value = 1;

    if(cindex > b->size*b->size-1)
        return;
    if(b->gameBoard[cindex].fixed){
        initialWork(b, cindex+1, index+1, possible, id, p);
    }
    else{
        for (value = 1; value <= b->size; value++)
        {
            if (NoSolution == FALSE){
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
                        if(bruteForce(b, cindex+1, id, p, FALSE) == FALSE){
                            removeMasks(b, cindex);
                            b->gameBoard[cindex].value = 0;
                        }
                        if(NoSolution == FALSE)
                            return;
                    }
                    *possible +=1;
                }
                else{
                    b->gameBoard[cindex].value = value;
                    updateMasks(b, cindex);
                    initialWork(b, cindex+1, index, possible, id, p);
                    if(NoSolution == TRUE){
                        removeMasks(b, cindex);
                        b->gameBoard[cindex].value = 0;
                    }
                }
            }
        }
        
    }
}


int check(int id, int p){

    int idcpy = id, index = 0;
    int possible = 0;
    int maxP = (p>200)?200:p;   

    left = (id==0)?p-1:id-1;
    right = (id+1)%p;

    Board * board = copyBoard(originalBoard);

    TERM = p+1;

    while( maxP > possible){
        possible = 0;
        checkPossibilities(board, 0, index, &possible, id);
        if(NoSolution == FALSE){
            return 0;
        }
        #ifdef OUT 
        #ifdef OUT
        printf("[%d]-trying: %d - p: %d\n",id, index, possible);
        #endif
        #endif
        index++;
    }
    index--;

    //t += MPI_Wtime();

    MPI_Irecv(&msg, 1, MPI_INT, right, ASK_WORK, MPI_COMM_WORLD, &msgReq);
    
    if(id < 200){ // Test your own possibilities
        possible = 0;
        #ifdef OUT
        printf("[%d]- Hello\n",id);
        #endif
        initialWork(board, 0, index, &possible, id, p);
    }

    if(NoSolution == TRUE){
        MPI_Isend(&idcpy, 1, MPI_INT, left, ASK_WORK, MPI_COMM_WORLD, &boardReq); 
    }
      
    while(!EXIT){
        #ifdef OUT
        //printf("[%d]-OUT OF WORK\n", id);
        #endif
        if( NoSolution==TRUE && GetOrCheckWork(board, id) ){ // updates haveWork
            /*printf("[%d] will work\n", id);
            #ifdef OUT
            #endif
            printBoardT(board, id);
            fflush(stdout);*/
            bruteForce( board,  startIndex,  id,  p, TRUE);
        }else{
            listenNeighbors(id, p);
        }
    }
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
            #ifdef OUT
            printf("No file was specified!\n");
            #endif

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
    
    #ifdef OUT
    //printf("s %d\n", s);
    #endif
    free(first);

    MPI_Bcast(r, s, MPI_BYTE, 0, MPI_COMM_WORLD);

    if(id){

        /*
        #ifdef OUT
        printf(">%d|",r[0]);
        #endif
        for (long int i = 1; i < s; ++i){
            #ifdef OUT
            printf("%d", r[i]);
            #endif
            if((i)%5==0){
                #ifdef OUT
                printf("|");
                #endif
            }
        }
        #ifdef OUT
        printf("\n");
        #endif
        fflush(stdout);
        
        */
        
        decompressBoard(board, r, s, TRUE);
        
        #ifdef OUT
        //printf("%d\n", board->squareSize);
        #endif
        #ifdef OUT
        //printf("Does process %d work? %c\n",id, (check(board))?'Y':'N' );
        #endif
        /*if(id==1){
            #ifdef OUT
            printf("TIME:::%lf\n", t);
            #endif
            printBoard(board);
        }*/
    }
        


    free(r); //<--------------------------------------------------------------------------
    
    originalBoard = board;
    check(id, p);

    freeBoard(board);
    free(board);

    

    //MPI_Recv

    #ifdef OUT
    //printf("FINALIZE ID = %d\n", id);
    #endif
    MPI_Finalize();
    #ifdef OUT
    printf("AFTER FINALIZE ID = %d\n", id);
    #endif

    return 0;
}



