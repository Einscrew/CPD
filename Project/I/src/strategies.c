#include "strategies.h"


int backtrack(Board * b){

  int i, j;
  for( i=0; i < b->size; i++){
    for( j=0; j < b->size; j++){
      /*
      if ij valid cell
      | increment &&
      | remove itself this.possibilities &&
      | continue??
      |
      else
      | continue

      if not valid
      | backtrack(&i, &j)
      */
    }
  }

  return 1;
}
