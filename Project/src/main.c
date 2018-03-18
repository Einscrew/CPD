#include "game.h"

int main(int argc, const char* argv[]){

    Board *board = (Board*)malloc(sizeof(Board));
  
    if(argv[1] != NULL)
    {
        if((fillGameBoard(board, argv[1])) == 0)
        {
            deleteStrategy(board);
            if(checkAllBoard(board) == TRUE)
                printf("Valid Sudoku!\n\n");
            printBoard(board->gameBoard, (board->size * board->size));
            freeBoard(board); 
        }
    }
    else
    {
        printf("No file was specified!\n");
    }

    free(board);

    return 0;
}
