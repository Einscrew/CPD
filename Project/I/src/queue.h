#ifndef QUEUE
#define QUEUE

#include "board.h"

typedef struct Elem{
	Board * board;
	int index;
	struct Elem * next;
}Elem;

typedef struct Queue{
	Elem * head;
	Elem *tail;
	int size;
}Queue;


Queue * create();

int full(Queue *q);
int empty(Queue *q);

int push(Board * b, Queue *q, int index);
Board *pop(Queue *q, int *index);
int merge(Queue *original, Queue *priv);

void destroyElem(Elem * e);
void destroy(Queue *q);
void printQueue(Queue *q);
#endif