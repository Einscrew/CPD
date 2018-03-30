#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "strategies.h"

int main(int argc, char const *argv[]) {

    Board *board = NULL;
    Board *solution = (Board*)malloc(sizeof(Board));

    if(argv[1] != NULL)
    {
        board = (Board*)malloc(sizeof(Board));
        Stack *head = iniStack(&head);

        if((fillGameBoard(board, argv[1])) == TRUE)
        {
            solution->size = board->size;
            solution->squareSize = board->squareSize;
            solution = allocBoard(solution);
            /*Stack *new = newElem(*board, 0);
            pushElem(&boardStack, new);
            Board copy = popElem(&boardStack);
            printBoard(&copy);
            if(checkEmpty(boardStack))
                printf("Stack empty\n");*/
            //printf("test[3]: value = %d fixed = %d minPoss = %d count = %d\n", res->copy.gameBoard[3].value, res->copy.gameBoard[3].fixed, res->copy.gameBoard[3].minPoss, res->copy.gameBoard[3].countPossibilities);*/
            //printBoard(&(res->copy));
            //printBoard(board);
            //printBoard(board);
            solution = solveSudoku(head, board, solution);
            printBoard(&solution);
            /*head = bruteforce(board, head);
            if(head != NULL)
            {
                printBoard(&(head->boardCopy));
            }*/
            //printf("Solution?%d\n", bruteforce(board, test));
            //printf("Solution?%d\n", checkValidity(board, 15));

            /* Frees the stack if it's not empty */
            freeStack(head);
            /*if(head != NULL)
            {
                free(head->boardCopy.gameBoard);
                free(head);
            }*/


        }
        else
        {
            printf("There's no possible solution for the given sudoku!\n");
        }

        /* Frees the game board */
        if(board->gameBoard != NULL)
        {
            freeBoard(board);
        }

        if(solution->gameBoard != NULL)
        {
            freeBoard(solution);
        }

        free(solution);
        free(board);

    }
    else
    {
        printf("No file was specified!\n");
    }

    printf("THE END\n" );
    return 0;
}
