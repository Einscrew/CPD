#include <stdio.h>
#include <stdlib.h>
#include "strategies.h"


int main(int argc, char const *argv[]) {

    Board *board = NULL;

    if(argv[1] != NULL)
    {
            board = (Board*)malloc(sizeof(Board));
            if((fillGameBoard(board, argv[1])) == 0)
            {
            	//printf("Valid: %d\n",checkValidity(board, 0, 1));
                //printf("Solution?%d\n", bruteforce(board, 5));
                //printf("Solution?%d\n", checkValidity(board, 0, 1));
                solver(board);
               // printBoard(board);
 
                    //testStack(board);
               // parallelSolver(board);
                 //Already frees board ?????
                freeBoard(board);
            }
            free(board);
    }
    else
    {
            printf("No file was specified!\n");
    }
    printf("THE END\n" );
    return 0;
}
