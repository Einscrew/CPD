#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "strategies.h"


int main(int argc, char const *argv[]) {

  Board *board = NULL;

  if(argv[1] != NULL)
  {
      board = (Board*)malloc(sizeof(Board));
      if((fillGameBoard(board, argv[1])) == 0)
      {   
          solve(board);
          printf("SAIU DO SOLVE!\n");
          /*deleteStrategy(board);*/
          if(checkAllBoard(board) == TRUE)
              printf("Valid Sudoku!\n\n");
          printBoard(board->gameBoard, (board->size));

          /* Frees game board */
          freeBoard(board);
      }
      else
      {
        free(board);
      }
  }
  else
  {
      printf("No file was specified!\n");
  }
  printf("THE END\n" );
  return 0;
}
