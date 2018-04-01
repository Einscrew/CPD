#include <stdio.h>
#include <stdlib.h>
#include "strategies.h"

int main(int argc, char const *argv[]) {

    Board *board = NULL;
    Board *solution = NULL;

    if(argv[1] != NULL)
    {
        board = (Board*)malloc(sizeof(Board));
        Stack *head = iniStack();

        if((fillGameBoard(board, argv[1])) == TRUE)
        {
        	solution = (Board*)malloc(sizeof(Board));
            solution->size = board->size;
            solution->squareSize = board->squareSize;
            solution = allocBoard(solution);
            solution = solveSudoku(head, board, solution);
            printBoard(solution);

            /* Frees the stack if it's not empty */
            freeStack(head);

            if(solution->gameBoard != NULL)
	        {
	            freeBoard(solution);
	        }

            free(solution);

            /* Frees the game board */
	        if(board->gameBoard != NULL)
	        {
	            freeBoard(board);
	        }
        }
        else
        {
            printf("There's no possible solution for the given sudoku!\n");
        }        

        free(board);
    }
    else
    {
        printf("No file was specified!\n");
    }

    return 0;
}
