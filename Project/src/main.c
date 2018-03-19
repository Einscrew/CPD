#include "game.h"

int main(int argc, const char* argv[]){

    Board *board = (Board*)malloc(sizeof(Board));
    Board *copyOfBoard = NULL;
    copyBoard(board, copyOfBoard, TRUE);
  
    if(argv[1] != NULL)
    {
        if((fillGameBoard(board, argv[1])) == 0)
        {
            /* To guarantee that the board given hasn't already wrong values */
            if(checkAllBoard(board) == TRUE)
            {
                 //deleteStrategy(board);
                if(solveSudoku(board) == TRUE)
                    printf("SUDOKU SOLVED!\n");
                else
                    printf("There's no soltuion for the sudoku proposed!\n");
                if(checkBoardComplete(board) == TRUE)
                    printf("Valid Sudoku!\n\n");
                else
                    printf("Something is wrong!\n");
            
                printBoard(board->gameBoard, (board->size * board->size));
                freeBoard(copyOfBoard);
                freeBoard(board); 
            }
            else
            {
                printf("Invalid sudoku board, please check the file!\n");
            }
        }
    }
    else
    {
        printf("No file was specified!\n");
    }

    free(copyOfBoard);
    free(board);

    return 0;
}
