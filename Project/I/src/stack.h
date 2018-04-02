#ifndef STACK
#define STACK

#include "board.h"


typedef struct Elem{
	Board * board;
	struct Elem * next;
}Elem;

typedef struct Stack{
	Elem * top;
	int size;
}Stack;


Stack * create();

int empty(Stack *s);

int push(Board * b, Stack * s);
Board * pop(Stack *s);

void destroyElem(Elem * e);
void destroy(Stack *s);
void printStack(Stack *s);
#endif