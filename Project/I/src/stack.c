#include <stdlib.h>
#include "stack.h"

Stack * create(int capacity) {
	Stack *s = (Stack *) malloc(sizeof(Stack));
	if(s == NULL){
		printf("No stack created\n");
		return NULL;
	}

	s->capacity = capacity;
	s->size = 0;
	s->top = NULL;
	return s;
}

int full(Stack *s) {
	return (s->size >= s->capacity)? 1 : 0;
}

int empty(Stack *s) {
	return (s->size == 0)? 1 : 0;
}

int push(Board * b, Stack * s, int index){

	if (full(s))
	{
		printf("Stack full\n");
		return 0;
	}

	Elem * e = (Elem*)malloc(sizeof(Elem));
	if(s == NULL){
		printf("No element created\n");
		return 0;
	}


	e->board = copyBoard(b);
	(index >= 0)? ( e->board->gameBoard[index].fixed == FALSE )? e->board->gameBoard[index].value++ : index : index;
	//copy boards!!!!!!;
	e->next = s->top;
	s->top = e;
	s->size++;

	return 1;
}


Board * pop(Stack *s){
	if(empty(s)){
		return NULL;
	}
	Elem * e = s->top;
	Board * r = e->board;

	s->top = s->top->next;
	s->size--;

	free(e);
	return r;
}

void destroyElem(Elem * e){
	if (e == NULL){
		return;
	}
	
	freeBoard(e->board);
	free(e->board);

	destroyElem(e->next);
	free(e);

	return;
}

void destroy(Stack * s){

	destroyElem(s->top);

	free(s);
}


void printStack(Stack *s){
	int i = 0;
	Elem * e = s->top;
	if(e == NULL)
		printf("EMPTY STACK\n");
	while(e != NULL){
		printf("[%i]---------------V-------------\n", ++i);
		printBoard(e->board);
		e = e->next;
	}

}