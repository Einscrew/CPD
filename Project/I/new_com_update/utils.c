#include "utils.h"

void * mallocV(int size){
    void * r = malloc(size);
    if ( r==NULL ){
        printf("Error: No memory\n" );
        exit(EXIT_FAILURE);
    }
    return r;
}

