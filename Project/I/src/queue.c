#include <stdlib.h>
#include "queue.h"

Queue *create() {
	Queue *q = (Queue *) malloc(sizeof(Queue));
	if(q == NULL){
		printf("No Queue created\n");
		return NULL;
	}

	q->size = 0;
	q->head = NULL;
	q->tail = NULL;
	return q;
}

int empty(Queue *q) {
	return (q->size == 0)? 1 : 0;
}

int push(Board *b, Queue *q, int index){

	Elem * e = (Elem*)malloc(sizeof(Elem));
	if(q == NULL){
		printf("No element created\n");
		return 0;
	}

	e->next = NULL;
	e->index = index;

	e->board = copyBoard(b);
	if(q->head == NULL)
	{	
		q->head = e;
		q->tail = e;
	}
	else
	{
		q->tail->next = e;
		q->tail = e;
	}

	q->size++;

	return 1;
}


Board *pop(Queue *q, int *index){

	if(empty(q)){
		return NULL;
	}
	Elem *e = q->head;
	Board *r = e->board;

	*index = e->index;
	q->head = q->head->next;
	q->size--;

	if(q->head == NULL)
	{
		q->tail = q->head;
	}

	free(e);
	return r;
}

int merge(Queue *original, Queue *priv){

	if(priv->head == NULL)
	{
		return 0;
	}

	if(original->tail == NULL)
	{
		original->head = priv->head;
		original->tail = priv->tail;
	}
	else
	{
		original->tail->next = priv->head;
		original->tail = priv->tail;
	}

	original->size += priv->size;

	priv->head = NULL;
	priv->tail = NULL;
	priv->size = 0;

	return 1;
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

void destroy(Queue *q){

	destroyElem(q->head);

	free(q);
}


void printQueue(Queue *q){
	int i = 0;
	Elem * e = q->head;
	printf("HEAD\n");
	if(e == NULL)
		printf("EMPTY Queue\n");
	while(e != NULL){
		printf("[%i]---------------V-------------\n", ++i);
		printBoard(e->board);
		e = e->next;
	}

}