#include "stack.h"

Stack *iniStack()
{
    return NULL;
}

Stack *newElem(Board b, int index)
{
    int i = 0;

    Stack *new = (Stack*)malloc(sizeof(Stack));
    if(new == NULL)
    {
        printf("Error on malloc\n");
        return NULL;
    }

    new->boardCopy.gameBoard = (Cell*)malloc(b.size * b.size * sizeof(Cell));
    if(new->boardCopy.gameBoard == NULL)
    {
        printf("Error on malloc\n");
        return NULL;
    }

    new->boardCopy.size = b.size;
    new->boardCopy.squareSize = b.squareSize;

    for(i = 0; i < b.size * b.size; i++)
    {
        new->boardCopy.gameBoard[i].value = b.gameBoard[i].value;
        new->boardCopy.gameBoard[i].fixed = b.gameBoard[i].fixed;
        new->boardCopy.gameBoard[i].minPoss = b.gameBoard[i].minPoss;
        new->boardCopy.gameBoard[i].countPossibilities = b.gameBoard[i].countPossibilities;
    }

    new->index = index;
    new->next = NULL;

    return new;
}

Stack *pushElem(Stack **head, Board b, int index)
{
    Stack *new = newElem(b, index);
    if(new == NULL)
    {
        return NULL;
    }
    new->next = *head;
    *head = new;

    return *head;
    //printBoard(&(*head)->boardCopy);
}

Stack *popElem(Stack *head, Board *b, int *index)
{
    Stack *aux = head;
    int i = 0;

    *index = aux->index;

    for(i = 0; i < b->size * b->size; i++)
    {
        b->gameBoard[i].value = aux->boardCopy.gameBoard[i].value;
        b->gameBoard[i].fixed = aux->boardCopy.gameBoard[i].fixed;
        b->gameBoard[i].minPoss = aux->boardCopy.gameBoard[i].minPoss;
        b->gameBoard[i].countPossibilities = aux->boardCopy.gameBoard[i].countPossibilities;
    }

    head = head->next;
    free(aux->boardCopy.gameBoard);
    free(aux);

    return head;
    //printBoard(&(popBoard->boardCopy));
}

Stack *getElem(Stack **head)
{
    Stack *res = NULL;

    if(head != NULL)
    {
        res = *head;
        *head = res->next;
    }

    return res;
}

int checkEmpty(Stack *head)
{
    return head == NULL ? TRUE : FALSE;
}

void freeStack(Stack *head)
{
    Stack *aux = head;
    Stack *tmp = NULL;

    while(aux != NULL)
    {
        tmp = aux->next;
        free(aux->boardCopy.gameBoard);
        free(aux);
        aux = tmp;
    }
}

