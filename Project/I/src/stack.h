#ifndef STACK
#define STACK

#include "board.h"


typedef struct Elem{
	Board * board;
	struct Elem * next;
}Elem;

typedef struct Stack{
	Elem * top;
	int capacity;
	int size;
}Stack;


Stack * create(int capacity);

int full(Stack *s);
int empty(Stack *s);

int push(Board * b, Stack * s, int i);
Board * pop(Stack *s);

void destroyElem(Elem * e);
void destroy(Stack *s);
void printStack(Stack *s);
#endif
