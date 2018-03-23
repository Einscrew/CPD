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

          printBoard(board);
          printf("Solution?%d\n", bruteforce(board));
          printBoard(board);
          //Already frees board
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
