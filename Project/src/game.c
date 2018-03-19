#include "game.h"

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

