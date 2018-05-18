#include <mpi.h>
#include <omp.h>
#include <unistd.h>
#include "sudoku-mpi.h"

//#define TIME 1

#define WORK 10
#define IDLE -10
#define NO_WORK 5
#define GET_WORK 555
#define ASK_WORK 444

/* Threshold according to the game board size */
int limit = 0;

/* Number of current empty cells */
int extern emptyCells;

/* Number of original empty cells */
int extern originalEmptyCells;

/* Original game borad */
Board * originalBoard = NULL;
Board * finalBoard = NULL;

/* Flag to know if the right processor needs work */  
int work4neighbor = FALSE;

/* Flag to know if the processor can exit the program */ 
int EXIT = FALSE;

/* Variable that holds the special number */ 
int TERM = FALSE;

/* Flag to know if there's a solution */ 
int NoSolution = TRUE;

/* Flag to know if the processor sent */
int sendTermination = FALSE;

/* Message sent between neighbors */
int msg;

/* Start index */
int startIndex = 0;

/* Left and right neighbors */
int left, right;

/* Flag to know if the processor will print the solution */ 
int iPrint = FALSE;

/* Requests */
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

/**************************************************************************************************************
*    This code is executed by the processors that discover a solution and it's to warn the other processors   *
*    that a solution was found and all processors can exit the program                                        *
*                                                                                                             *
*  Parameters: id - processor id                                                                              *
*              p - number of processors                                                                       *
*                                                                                                             *
**************************************************************************************************************/

void sendSolution(int id, int p){
    /* Number sent when a processor discovers a solution */
    int finish_id = id - p;
    MPI_Status s;

    /* Sends a message with the finish_id */
    MPI_Send(&finish_id, 1, MPI_INT, left, ASK_WORK, MPI_COMM_WORLD ); // Receiver has to receive??
    /* Waits until the message is sent */
    MPI_Wait(&msgReq, &s);

    /* Wait until receives its own finish_id message or until receive the special number used to terminate the program */
    do{
        /* Only the processor with the lowest finish_id will print the solution
        in case of two or more processors find a solution at the same time */
        if(msg <= finish_id || msg == TERM){
        
            if(msg == finish_id){
                /* Change the msg to the termination number */
                msg = TERM;
                /* Updates the flag that indicates that a processor will print a solution */
                iPrint= TRUE;
            }else if(msg == TERM){
                /* If there's other processor that discovered a solution with a finish_id greater, this processor only have to
                forward the termination number and can exit */
                if(!iPrint)
                    MPI_Send(&msg, 1, MPI_INT, left, ASK_WORK, MPI_COMM_WORLD ); // Receiver has to receive??
                else
                    printBoard(finalBoard); /* Otherwise, the processor will print the solution and then can exit the program too */
                EXIT = TRUE;
                return;
            }
            /* Forwards the message received */
            MPI_Send(&msg, 1, MPI_INT, left, ASK_WORK, MPI_COMM_WORLD ); // Receiver has to receive??
        }

        /* Listens the rigth neighbor */
        MPI_Recv(&msg, 1, MPI_INT, right, ASK_WORK, MPI_COMM_WORLD, &s);

    }while(1);
}

/*****************************************************************************************************************
*    This function checks if the rigth neighbor needs work, meaning that it's idle and waiting for a game board. *
*    If the flag work4neighbor is TRUE, means that the neighbor needs work.                                      *
*    However, the processor will only distribute a peace of its current board if its worth it. This decision its *
*   done taking in account the index on which the processor its currently working.                               *
*     If the flag work4neighbor is FALSE, then the processor will listen its neighbor in order to check if needs *
*   work.                                                                                                        *
*                                                                                                                *
*  Parameters: b - current game board                                                                            *
*              i - current index                                                                                 *
*              id - processor id                                                                                 *
*                                                                                                                *
*  Returns:    WORK - if the processor gave work to its rigth neighbor                                           *
*              IDLE - if someone found a solution                                                                *
*              NO_WORK - if the processor didn't give any board                                                  *
*                                                                                                                *
*****************************************************************************************************************/

int giveWork(Board *b, int i, int id){

    /* Flags to know when a board was sent and if any message was received */
    int sendFlag = FALSE, receiveFlag = FALSE, size;

    char * compressed  = NULL;
    MPI_Status s;

    /* Flag used to know if the threshold was reached which means that the processor
    won't send the current game board */
    int didSend = FALSE;

    /* Checks if the limit was rechead */
    if(limit <= emptyCells){
        didSend = TRUE;
    }

    /* If the right neighbor needs work, the current board will be compressed and sent */
    if(work4neighbor){

        /* If it's worth it to send the board, sends it */
        if(didSend){
            size = compressBoard(b, 0, i, &compressed);
            MPI_Isend(compressed, size, MPI_BYTE, right, GET_WORK, MPI_COMM_WORLD, &boardReq);    
        } 

        /* While the board is being sent test if someone found a solution */ 
        while(!sendFlag){

            if(didSend){
                /* Test if the message was sent */ 
                MPI_Test(&boardReq, &sendFlag, &s);
                if(sendFlag){
                    free(compressed);
                    work4neighbor = FALSE;
                    return WORK;
                }
            }else{               
                sendFlag = TRUE;
            }

            /* Verifies if someone found a solution */
            MPI_Test(&msgReq, &receiveFlag, &s);
            if(receiveFlag){
                receiveFlag = FALSE;

                /* If a message has a negative value, means that a solution was found */
                if(msg < 0){
                    MPI_Cancel(&boardReq);
                    free(compressed);
                    return IDLE;
                }

                /* Listens the right neighbor again */
                MPI_Irecv(&msg, 1, MPI_INT, right, ASK_WORK, MPI_COMM_WORLD, &msgReq);
            }
        }
    }else{

        /* Tests if a message was received */
        MPI_Test(&msgReq, &receiveFlag, &s);
        if(receiveFlag){
            receiveFlag = FALSE;
            
            /* If a message has a negative value, means that a solution was found */
            if(msg < 0){
                return IDLE;
            }
            else if(msg == right){

                /* If the message received has the id of the rigth neighbor, means that it needs work */
                work4neighbor = TRUE;

                /* If its worth to send a board */
                if(didSend){ // have to give work
                    #ifdef OUT
                    //printf("right [%d] needs work from %d\n", right, id);
                    #endif

                    /* Listens its right neighbor again */
                    MPI_Irecv(&msg, 1, MPI_INT, right, ASK_WORK, MPI_COMM_WORLD, &msgReq);

                    /* Updates the flag */
                    work4neighbor = TRUE;

                    /* Compresses the board and sends it */
                    size = compressBoard(b, 0, i, &compressed);
                    MPI_Isend(compressed, size, MPI_BYTE, right, GET_WORK, MPI_COMM_WORLD, &boardReq);     

                    /* Checks if someone found a solution while sending the board */ 
                    while(!sendFlag){
                        MPI_Test(&boardReq, &sendFlag, &s);
                        if( sendFlag ){
                            free(compressed);
                            work4neighbor = FALSE;
                            return WORK;
                        }
                        
                        /* Verifies if someone found a solution */
                        MPI_Test(&msgReq, &receiveFlag, &s);
                        if(receiveFlag){
                            receiveFlag = FALSE;

                            /* If a message has a negative value, means that a solution was found */
                            if(msg < 0){
                                MPI_Cancel(&boardReq);
                                free(compressed);
                                return IDLE;
                            }

                            /* Listens the right neighbor again */
                            MPI_Irecv(&msg, 1, MPI_INT, right, ASK_WORK, MPI_COMM_WORLD, &msgReq);
                        }
                    }
                }
            }
        
            /* Listens the right neighbor again */
            MPI_Irecv(&msg, 1, MPI_INT, right, ASK_WORK, MPI_COMM_WORLD, &msgReq);
        }
    }

    return NO_WORK;
}

/*****************************************************************************************************************
*    This is the function that is called when a processor ends its work and continues to listen if someone found *
*   a solution or if there's no solution.                                                                        *
*                                                                                                                *
*  Parameters: id - processor id                                                                                 *
*              p - number of processors                                                                          *
*                                                                                                                *
*****************************************************************************************************************/

void listenNeighbors(int id, int p){

    int recv = FALSE;
    MPI_Status s;
    

    do{
        /* Test if the right neighbor sent a message */
        MPI_Test(&msgReq, &recv, &s);
        
        if(recv){

            if((msg == right && work4neighbor==FALSE) || msg == TERM || msg < 0 || (msg >= 0 && work4neighbor == TRUE) || (msg == id && work4neighbor == TRUE)){

                /* Checks if the message received its negative, meaning that someone found a solution */ 
                if(msg < 0){ // Solution found by a working node

                    /* Updates the flag to exit the program */
                    EXIT = TRUE;

                    /* Updates the flag that means that there's a solution */
                    NoSolution = FALSE;
                              
                }else if(msg == id){ /* Checks if the message received has its own id, meaning that there's no solution */ 

                    /* Updates the flag that means that the processor can send the termination number */
                    sendTermination = TRUE;

                    /* Change the message to the special number that indicates that the processors can exit the program */
                    msg = TERM;

                    /* Updates the flag to exit the program */
                    EXIT = TRUE;
                }else if(msg == right){ /* Checks if the message received has the right neighbor id, meaning that the neighbor needs work */

                    /* Updates the flag related to need of work of its right neighbor */
                    work4neighbor = TRUE;
                
                }else if(msg == TERM){ /* Checks if the message received has the special number, meaning that the processor can exit the program */
                   
                    /* Updates the flag to exit the program */
                    EXIT = TRUE;

                    /* Only the last processors to discover that there's no solution for the given sudoku will send the special number */
                    if(sendTermination == FALSE){
                        /* Sends the message with the special number to terminate the program */
                        MPI_Send(&msg, 1, MPI_INT, left, ASK_WORK, MPI_COMM_WORLD);
                    }

                    /* If there's no solution, the processor that will print the result will be the processor with id = 0 */
                    if(NoSolution && id == 0){
                        
                        printf("No solution\n");
                        fflush(stdout);
                    } 

                    return;
                }

                /* Forwards a message to its left neighbor */
                MPI_Send(&msg, 1, MPI_INT, left, ASK_WORK, MPI_COMM_WORLD); 
            }

            /* Listens its right neighbor again */
            MPI_Irecv(&msg, 1, MPI_INT, right, ASK_WORK, MPI_COMM_WORLD, &msgReq);
            recv = FALSE;
        }
    
    }while(EXIT);
}

/*****************************************************************************************************************
*    This is the function where a processor checks if its left neighbor sent a board, when it's is idle and      *
*   waiting for another board.                                                                                   *
*                                                                                                                *
*  Parameters: board - board on which the decompress function will be done                                       *
*              id - processor id                                                                                 *
*                                                                                                                *
*  Returns: TRUE if the processor received a new board                                                           *
*           FALSE otherwise                                                                                      *
*                                                                                                                *
*****************************************************************************************************************/

int GetOrCheckWork(Board * board, int id){

    int ret = FALSE, size = 0;
    char *compressed = NULL;
    MPI_Status s;

    /* Checks if the prcoessor received a game board from its left neighbor */
    MPI_Iprobe(left, GET_WORK, MPI_COMM_WORLD, &ret, &s);

    if(ret){
        
        /* If a board was recieved count the size of the array sent */
        MPI_Get_count(&s, MPI_BYTE, &size);
    
        /* Allocates memory for the array sent */
        compressed = malloc(size);

        /* Receives the array containing the non-fixed positions that are already filled of the current game board of its left neighbor */
        MPI_Recv(compressed, size, MPI_BYTE, left, GET_WORK, MPI_COMM_WORLD, &s);
        
        /* Cleans the board, resetting it with the original board */
        makeCopyBoard(board, originalBoard);
        
        /* Gets the start index for the current game board */
        startIndex = decompressBoard(board, compressed, size, FALSE);

        /* Fress memory */
        free(compressed);

        return TRUE;
    }
    return FALSE;
}

/**************************************************************************************************************
*    Brute force algorithm that receives a board and finds a solution if there's one                          *  
*                                                                                                             *
*   Parameters: b - current game board                                                                        *
*               start - start index                                                                           *
*               id - processor id                                                                             *
*               p - number of processors                                                                      * 
*               askWork - flag to know if the processor already explored all of its own initial possibilities *
*                         							                                                          *			   	         
*                                                                                                             * 
*   Returns:    TRUE if a solution was found                                                                  *
*               FALSE otherwise                                                                               *
*                                                                                                             *
**************************************************************************************************************/

int bruteForce(Board *b, int start, int id, int p, int askWork){

    int i = start, valid = TRUE;
    int idcpy = id, area = b->size*b->size;

    for(i = start; i < area ; i++)
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
            if((valid = makeGuess( b , i)) == TRUE){

                /* Checks if the right neighbor needs a game board or if someone found a solution */ 
                switch(giveWork(b, i, id)){
                    /* If the function returns IDLE means that someone found a solution */
                    case IDLE: 

                        /* Updates the flag to exit the program */
                        EXIT = TRUE;

                        /* Updates the flag since there's a solution */
                        NoSolution = FALSE;

                        /* Forwards the message to the left neighbor */ 
                        MPI_Send(&msg, 1, MPI_INT, left, ASK_WORK, MPI_COMM_WORLD);

                        /* Listens the right neighbor to receive the special number to terminate */ 
                        MPI_Irecv(&msg, 1, MPI_INT, right, ASK_WORK, MPI_COMM_WORLD, &msgReq);
                        
                        /* Wait unitl receive the  special number to terminate */
                        listenNeighbors(id, p);
                        return FALSE;
                        break;
                    
                    /* If the function returns WORK means that the right neighbor has received the current game board and the processor
                    needs to start working on another guess */
                    case WORK:
                        
                        /* Makes the next guess for the current game board */   
                        valid = makeGuess(b, i);  
                        break;
                }
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
                        emptyCells++;
                    }

                    i--;

                }while (i >= start && (b->gameBoard[i].fixed == TRUE || b->gameBoard[i].value == b->size));

                /* If the index is < 0, then there's no solution for the gicen sudoku */
                if(i < start)
                {
                    /* Only will ask for work if the processor explored all of its initial guesses */                    
                    if(askWork){
                        idcpy = id;
                        /* Asks its left neighbor for work */
                        MPI_Send(&idcpy, 1, MPI_INT, left, ASK_WORK, MPI_COMM_WORLD ); // Receiver has to receive??
                    }
                    return FALSE;
                }
            }else
                emptyCells--;
        }
    }
    
    /* Updates the final board */
    finalBoard = b;
    /* If the processor found a solution has to warn all the other processors that have to terminate */
    sendSolution(id, p);
    /* Updates the flag since there's a solution */
    NoSolution = FALSE;

    return TRUE;
}

/****************************************************************************
*    This function counts the number of for the first index                 *
*                                                                           *
*   Parameters: b - original game board                                     *
*               cindex - current index                                      *
*               index - index until the check function will be done         *
*               possible - number of possibilities                          * 
*               id - processor id                                           *
*                                                                           *                      
****************************************************************************/

void checkPossibilities(Board *b, int cindex, int index, int * possible, int id){
    int value = 0;
    /* If the last index was reached, means that a solution was found */
    if(cindex > b->size*b->size-1){

        /* Updates the flag since there's a solution */
        NoSolution = FALSE;

        /* Only one processor will print the solution */ 
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

        /* Test if the values are valid in the current index */
        for (value = 1; value <= b->size; value++)
        {
            if(checkValidityMasks(b, cindex, value) == TRUE)
            {
                /* Update the number of valid possibilities of the current index */
                if(cindex == index){
                    *possible +=1;
                }
                else{
                    /* Updates the value of the index */ 
                    b->gameBoard[cindex].value = value;
                    updateMasks(b, cindex);
                    /* Call the function to the next index */
                    checkPossibilities(b, cindex+1, index, possible, id);
                    removeMasks(b, cindex);

                    /* Resets the value of the index */
                    b->gameBoard[cindex].value = 0;
                }

            }
        }
    }
}

/**************************************************************************************************************
*    This function counts the number of possibilities in each index                                           *  
*                                                                                                             *
*   Parameters: b - original game board                                                                       *
*               cindex - current index                                                                        *                               
*               index - index until the check function will be done                                           *
*                possibleFinal - number of possibilities for the current index                                *
*               possiblePrev - number of possibilities for previous index                                     * 
*               id - processor id                                                                             *
*                p - number of processors                                                                     *
*                                                                                                             *                      
**************************************************************************************************************/

void checkPossibilitiesL(Board * b, int cindex, int index, int * possibleFinal, int * possiblePrev, int id, int p){
    int value = 0;

     /* If the last index was reached, means that a solution was found */
    if(cindex > b->size*b->size-1){

        /* Updates the flag since there's a solution */
        NoSolution = FALSE;

        /* Only one processor will print the solution */ 
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
        /* Test if the values are valid in the current index */
        for (value = 1; value <= b->size; value++)
        {
            if(cindex == index-1 && *possiblePrev+*possibleFinal >= p){
                return;
            }
            if(checkValidityMasks(b, cindex, value) == TRUE)
            {
                /* Update the number of valid possibilities of the current index */
                if(cindex == index-1){
                   *possiblePrev -=1;
                }
                if(cindex == index){
                    *possibleFinal +=1;
                }
                else{
                    /* Updates the value of the index */ 
                    b->gameBoard[cindex].value = value;
                    updateMasks(b, cindex);

                    /* Call the function to the next index */
                    checkPossibilitiesL(b, cindex+1, index, possibleFinal, possiblePrev, id, p);
                    removeMasks(b, cindex);

                    /* Resets the value of the index */
                    b->gameBoard[cindex].value = 0;
                }

            }
        }
        
    }
}

/**************************************************************************************************************
*    This function distributes the initial work for the processors according to its id                        *  
*                                                                                                             *
*   Parameters: b - original game board                                                                       *
*               cindex - current index                                                                        *
*               index - index until the check function will be done                                           *
*               possible - number of possibilities                                                            * 
*               id - processor id                                                                             *
*                p - number of processors                                                                     *
*                                                                                                             *                      
**************************************************************************************************************/

void initialWork(Board * b, int cindex, int index, int * possibleFinal, int * possiblePrev, int id, int p, int *possible){
    int value = 0;

    if(b->gameBoard[cindex].fixed){
        initialWork(b, cindex+1, index+1, possibleFinal, possiblePrev, id, p, possible);
    }
    else{
        for(value = b->size; value >= 1; value--)
        {
            /* If there's a solution, exit the program */
            if (NoSolution == FALSE){
                return;
            }
            if(checkValidityMasks(b, cindex, value) == TRUE)
            {  
                /* If the index on which there are possibilities for all the processors start working, then each possinility is
                assigned according to the id of the processor */
                if(cindex == index && *possibleFinal > 0){

                    if(*possible%p == id){
                        b->gameBoard[cindex].value = value;
                        updateMasks(b, cindex);
                        
                        /* Execute the brute force algorithm to find a solution */
                        if(bruteForce(b, cindex+1, id, p, FALSE) == FALSE){
                            removeMasks(b, cindex);
                            b->gameBoard[cindex].value = 0;
                        }
                        /* Descrement the number of possibilities at the current index */
                        *possibleFinal-=1;

                        /* If a solution was found during the bruteforce function, can exit the program */
                        if(NoSolution == FALSE)
                            return;
                    }

                    /* Increment the number of possibilities */
                    *possible +=1;
                }else if(cindex == index-1 && *possiblePrev > 0){
                    if(*possible%p == id){
                        b->gameBoard[cindex].value = value;
                        updateMasks(b, cindex);
                        
                        /* Execute the brute force algorithm to find a solution */
                        if(bruteForce(b, cindex+1, id, p, FALSE) == FALSE){
                            removeMasks(b, cindex);
                            b->gameBoard[cindex].value = 0;
                        }

                        /* Descrement the number of possibilities at the current index */
                        *possiblePrev-=1;

                        /* If a solution was found during the bruteforce function, can exit the program */
                        if(NoSolution == FALSE)
                            return;
                    }

                    /* Increment the number of possibilities */
                    *possible +=1;
                }
                else{
                    b->gameBoard[cindex].value = value;
                    updateMasks(b, cindex);
                    initialWork(b, cindex+1, index, possibleFinal, possiblePrev, id, p, possible);

                    /* If no solution was found with that possibility resets the value of the start index */
                    if(NoSolution == TRUE){
                        removeMasks(b, cindex);
                        b->gameBoard[cindex].value = 0;
                    }
                }
            }
        }
    }
}

/********************************************************************************************************************
*     This function checks the state of each processor. First calculates the index from which all the processors    *
*    have al least one game board to start working. After that, each processor will work on its game board.         *
*     If the processor didn't find any solution on its own possibilities will send a message to its left neighbor   *
*    asking for work.                                                                                               *
*     When a processor receives a game board will call the bruteforce function and will try to solve the given      *
*    game board. Every time a processor finsihes a game board will listen its neighbors calling the listenNeighbors *
*    and the GetOrCheckWork functions.                                                                              *
*                                                                                                                   *  
*                                                                                                                   *
*   Parameters:  id - processor id                                                                                  *
*                p - number of processors                                                                           *
*                                                                                                                   *
*   Returns: 0 at exit                                                                                              *
*                                                                                                                   *                      
********************************************************************************************************************/

int check(int id, int p){

    int idcpy = id, index = 0;
    int possible = 0;
    int possibleFinal = 0, possiblePrev = 0;

    /* Define left and right neighbors */
    left = (id==0)?p-1:id-1;
    right = (id+1)%p;

    /* Makes a copy of the original board */
    Board * board = copyBoard(originalBoard);

    /* Defines the threshold from which a processor won't give a peace of its current board
    according to the board size */
    switch(board->size){
        case 4:
            limit = originalEmptyCells+1;
            break;
        case 9:
            limit = 9;
            break;
        case 16:
            limit = 7;
            break;
        case 25:
            limit = 6;
            break;
        case 36:
            limit = 5;
            break;
        default:
            limit = 3;
    }

    /* States the termination special number */
    TERM = p+1;

    checkPossibilities(board, 0, 0, &possibleFinal, id);
    index++;

    while(p > possibleFinal+possiblePrev){

        possiblePrev = possibleFinal;
        possibleFinal = 0;
        /* Call the chechPossibilities function until there's sufficient possibilitues for all the processors */
        checkPossibilitiesL(board, 0, index, &possibleFinal, &possiblePrev, id, p);

        /* If a solution is found, can exit the program */ 
        if(NoSolution == FALSE){
            return 0;
        }
        index++;
    }

    /* Index from which all processor will start working on bruteforce function */
    index--;

    /* Listens its right number */
    MPI_Irecv(&msg, 1, MPI_INT, right, ASK_WORK, MPI_COMM_WORLD, &msgReq);
    
    /* Tests all the initial possibilites */
    initialWork(board, 0, index, &possibleFinal, &possiblePrev, id, p, &possible);

    /* If no solution was found in the initialWork function, send a message to its left neighbor asking for work */ 
    if(NoSolution == TRUE){ 
        MPI_Isend(&idcpy, 1, MPI_INT, left, ASK_WORK, MPI_COMM_WORLD, &boardReq); 
    }

    /* While no one finds a solution or until there's no more boards to explore */
    while(!EXIT){

        /* If no one finds a solution yet, checks if the right neighbor sent a game board */
        if( NoSolution==TRUE && GetOrCheckWork(board, id) ){ // updates haveWork
            bruteForce( board,  startIndex,  id,  p, TRUE);
        }else{

            /* Listens its neighbors if no one finds a solution or if the left neighbor didn't send a game board yet  */
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
    char * r = NULL;
    char * first = malloc(sizeof(char)+sizeof(int));

    int s = 0;
    /* Checks if teh user gives the input file */
    int id = 0, p = 0;

    MPI_Init (&argc, &argv);
    
    MPI_Comm_rank (MPI_COMM_WORLD, &id);
    MPI_Comm_size (MPI_COMM_WORLD, &p);

    if(!id){
        if(argv[1] != NULL)
        {
            /* Only the processor 0 will read the input file */
            if((fillGameBoard(board, argv[1])) == 0)
            {
                /* Size of the compressed board */
                s = compressBoard(board, 1, board->size*board->size-1 , &r);
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

    /* Broadcasts the size of the compressed array with the game board */ 
    MPI_Bcast(first, sizeof(char)+sizeof(int), MPI_BYTE, 0, MPI_COMM_WORLD);

    if(id){

        memcpy(&board->squareSize, first, sizeof(char));
        board->size = board->squareSize*board->squareSize;
        board->gameBoard = NULL;

        memcpy(&s, &first[sizeof(char)], sizeof(int));
        /* Allocates the memory to receive the compressed */ 
        r = malloc(s);
    }
    
    free(first);

    /* Broadcasts the compressed game board */
    MPI_Bcast(r, s, MPI_BYTE, 0, MPI_COMM_WORLD);

    if(id){
        
        /* Decompress the initial game board */ 
        decompressBoard(board, r, s, TRUE);
        /* Updates the number of empty cells of the initial game board */
        originalEmptyCells = emptyCells;
    }
    free(r);

    /* Keeps a pointer to the original board */
    originalBoard = board;

    #ifdef TIME
    double t = -MPI_Wtime();
    #endif
    /* This function is where the processors listen its neighbors and work on its game boards and on the game boards received by its right neighbor */
    check(id, p);

    #ifdef TIME
    t += MPI_Wtime();
    #endif

    #ifdef TIME
    if(!id)printf("TIME: %lfs\n", t);
    #endif
    /* Frees memory */ 
    freeBoard(board);
    free(board);

    MPI_Finalize();

    return 0;
}



