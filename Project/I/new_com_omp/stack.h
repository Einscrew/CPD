#ifndef STACK
#define STACK

#include "board.h"

typedef struct Stack{

    Board boardCopy;
    int index;
    struct Stack *next;

}Stack;

Stack *iniStack();
Stack *newElem(Board b, int index);
Stack *pushElem(Stack **head, Board b, int index);
Stack *popElem(Stack *head, Board *b, int *index);
Stack *getElem(Stack **head);
int checkEmpty(Stack *head);
void freeStack(Stack *head);
//Board *copyBoard(Board *b, Stack *copy);

#endif