#include "game.h"

/* Allocates memory to the game board */
int allocBoard(Board *board)
{
    int j = 0;

    board->gameBoard = (Cell**)malloc(board->size*board->size*sizeof(Cell*));

    if(board->gameBoard == NULL)
    {
        printf("Error on malloc!\n");
        return -1;
    }
    for(j = 0; j < board->size * board->size; j++)
    {
        board->gameBoard[j] = (Cell*)malloc(board->size*board->size*sizeof(Cell));
        board->gameBoard[j]->possibleValues = NULL;
        if(board->gameBoard[j] == NULL)
        {
            printf("Error on malloc!\n");
            return -1;
        }
    }
    return 0;
}

/* Creates a vector of possibilities for each cell that has a value != 0 */
void createVectorPossibilities(Cell *aux, int size)
{
    int i = 0;
    aux->possibleValues = (int*)malloc(size*sizeof(int));

    if(aux->possibleValues == NULL)
    {
        printf("Error on malloc!\n");
        exit(-1);
    }
    /* Initializes the vector of possibilities of each empty cell with all possible values */
    for(i = 0; i < size; i++)
    {
        aux->possibleValues[i] = TRUE;
    }
}


/* Reads the file and fills the board game */
int fillGameBoard(Board *board, char const* file)
{
    int i = 0, j = 0;
    char *line = NULL;
    char *aux = NULL;
    FILE *fptr = fopen(file, "r");

    if(fptr == NULL)
    {
        printf("Error reading load file!\n");
        return -1;
    }
    else
    {
        /* Gets the board size */
        if(fscanf(fptr, "%d\n", &(board->size)) != 1){
            printf("Board size not specified in the file!\n");
            fclose(fptr);
            return -1;
        }
        else if(board->size <= MAX_SIZE && board->size >= MIN_SIZE)
        {
            if(allocBoard(board) != 0)
            {
                fclose(fptr);
                return -1;
            }
        }
        else
        {
            printf("Invalid size of board game, please check if the file is correct!\n");
            fclose(fptr);
            return -1;
        }

        line = (char*)malloc(MAX * sizeof(char));

        if(line == NULL)
        {
            printf("Error on malloc!\n");
            fclose(fptr);
            return -1;
        }

        /* Get every line from the file and assign it to a vector of integers, then fill the board */
        while(fgets(line, MAX, fptr) != NULL)
        {
            aux = strtok(line, " ");

            /* Reads until the \n */
            while(aux && j < (board->size * board->size))
            {
                if(atoi(aux) >= 0 && atoi(aux) <= (board->size * board->size))
                {
                    if(atoi(aux) == 0)
                    {
                        createVectorPossibilities(&(board->gameBoard[i][j]), (board->size*board->size));
                        board->gameBoard[i][j].countPossibilities = (board->size*board->size);
                    }
                    else
                    {
                        board->gameBoard[i][j].possibleValues = NULL;
                        board->gameBoard[i][j].countPossibilities = 0;
                    }
                    
                    board->gameBoard[i][j].value = atoi(aux);
                    j++;
                }
                else
                {
                    printf("There is an invalid number on the board game, check the file!\n");
                    fclose(fptr);
                    return -1;
                }

                aux = strtok(NULL, " ");
            }
            j=0;
            i++;
        }
    }

    /* Updates the possibilities for each incomplete cell */
    for(i = 0; i < board->size*board->size; i++)
    {
        for(j = 0; j < board->size*board->size; j++)
        {
            updateBoardValues(board, i, j, board->gameBoard[i][j].value);
        }  
    } 
    
    fclose(fptr);
    free(line);
    return 0;
}

/* Prints the game board */
void printBoard(Cell **board, int size)
{
    int i = 0, j = 0;

    for(i = 0; i < size; i++)
    {
        for(j = 0; j < size; j++)
        {
            printf("%d ", board[i][j].value);
        }
        printf("\n");
    }
}

