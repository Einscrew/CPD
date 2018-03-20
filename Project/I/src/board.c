#include "board.h"

int allocBoard(Board *b)
{
    int j = 0;


    b->gameBoard = (Cell**)malloc(b->size*sizeof(Cell*));

    if(b->gameBoard == NULL)
    {
        printf("Error on malloc!\n");
        return -1;
    }
    for(j = 0; j < b->size ; j++)
    {
        b->gameBoard[j] = (Cell*)malloc(b->size*sizeof(Cell));
        if(b->gameBoard[j] == NULL)
        {
            printf("Error on malloc!\n");
            free(b->gameBoard);
            return -1;
        }
        b->gameBoard[j]->possibleValues = NULL;
    }
    return 0;
}


/* Checks each row, column or box */
int checkValidity(Cell *aux, int size)
{
    //int exist[size] = {FALSE};
    int *exist = (int*)malloc(sizeof(int)*size);
    int i = 0;

    //REVIEW
    for(i = 0; i < size; i++)
    {
        exist[i] = FALSE;
    }

    for(i = 0; i < size; i++)
    {
        if(aux[i].value != 0)
        {
          //If any elements was on a previous cell
          if(exist[aux[i].value - 1] == TRUE)
          {
            free(exist);
            return FALSE;
          }
          exist[aux[i].value - 1] = TRUE;
        }

    }
    free(exist);
    return TRUE;
}

/* Checks if there aren't any duplicate values in each row, column or box */
int checkAllBoard(Board *b)
{
    int i = 0, k = 0, w = 0, z = 0;
    int aux1 = 0, aux2 = 0;

    /* Used to compare */
    Cell rows[b->size];
    Cell cols[b->size];
    Cell boxes[b->size];

    for(i = 0; i < b->size ; i++)
    {
        if(aux1 == b->size)
        {
            aux1 = 0; /* Resets the value when reaches the last cell in a row */
            aux2 = aux2 + b->size; /* Increments k so that the box starts at the left corner up cell */
        }

        w = 0;
        for(k = aux2; k < b->squareSize + aux2; k++)
        {
            for(z = aux1; z < b->squareSize + aux1; z++)
            {
                boxes[w] = b->gameBoard[k][z];
                w++;
            }
        }
        aux1 = aux1 + b->squareSize; /* Increments z so that the box end at the right corner up cell */

        /* Check duplicates inside a box */
        if(checkValidity(boxes, b->size) == FALSE)
        {
            return FALSE;
        }

        /* Check duplicates inside a column */
        for(w = 0; w < b->size ; w++)
        {
            cols[w] = b->gameBoard[w][i];
        }

        if(checkValidity(cols, b->size*b->size) == FALSE)
        {
            return FALSE;
        }

        /* Check duplicates inside a row */
        for(w = 0; w < b->size; w++)
        {
            rows[w] = b->gameBoard[i][w];
            //printf("rows[%d]: %d\n", j, b->gameBoard[i][j].value);
        }

        if(checkValidity(rows, b->size) == FALSE)
        {
            return FALSE;
        }
    }
    return TRUE;
}


void freeBoard(Board *b)
{
    int i = 0, j = 0;

    for(i = 0; i < b->size; i++)
    {
        for(j = 0; j < b->size; j++)
        {
            free(b->gameBoard[i][j].possibleValues);

        }
        free(b->gameBoard[i]);
    }
    free(b->gameBoard);
    free(b);
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

    //REVIEW podemos avaliar logo?
    /* Initializes the vector of possibilities of each empty cell with all possible values */
    for(i = 0; i < size; i++)
    {
        aux->possibleValues[i] = TRUE;
    }
}

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
        if(fscanf(fptr, "%d\n", &(board->squareSize)) != 1){
            printf("Board size not specified in the file!\n");
            fclose(fptr);
            return -1;
        }
        else if(board->squareSize <= MAX_SQUARE_SIZE && board->squareSize >= MIN_SQUARE_SIZE)
        {
            board->size = board->squareSize*board->squareSize;
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
            while(aux && j < (board->size))
            {
                if(atoi(aux) >= 0 && atoi(aux) <= (board->size))
                {
                    if(atoi(aux) == 0)
                    {
                        createVectorPossibilities(&(board->gameBoard[i][j]), board->size);
                        board->gameBoard[i][j].countPossibilities = (board->size);
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
    for(i = 0; i < board->size; i++)
    {
        for(j = 0; j < board->size; j++)
        {
            updateBoardValues(board, i, j, board->gameBoard[i][j].value);
        }
    }

    fclose(fptr);
    free(line);
    return 0;
}



//REVIEW why if i,j&value NEEDED? CELL could contain all?
/* Update vector of possibilities when a number is inserted */
void updateBoardValues(Board *board, int row, int col, int value)
{
    int indexValue = value - 1; /* used to update the possibilites of other cells */
    int i = 0;
    int boxStartRow = (row / board->squareSize) * board->squareSize;
    int boxStartCol = (col / board->squareSize) * board->squareSize;

    if(value == 0)
    {
        return;
    }

    for(i = 0; i < (board->size); i++)
    {
        /* Update cells in the same row, if the cell is empty */
        if(board->gameBoard[row][i].value == 0)
        {
            if(board->gameBoard[row][i].possibleValues[indexValue] == TRUE)
            {
                board->gameBoard[row][i].possibleValues[indexValue] = FALSE;
                board->gameBoard[row][i].countPossibilities --;
            }
        }

        /* Update cells in the same column, if the cell is empty */
        if(board->gameBoard[i][col].value == 0)
        {
            if(board->gameBoard[i][col].possibleValues[indexValue] == TRUE)
            {
                board->gameBoard[i][col].possibleValues[indexValue] = FALSE;
                board->gameBoard[i][col].countPossibilities --;
            }
        }

        /* Update cells in the same box, if the cell is empty */
        if(board->gameBoard[boxStartRow + i / board->squareSize][boxStartCol + i % board->squareSize].value == 0)
        {
            if(board->gameBoard[boxStartRow + i / board->squareSize][boxStartCol + i % board->squareSize].possibleValues[indexValue] == TRUE)
            {
                board->gameBoard[boxStartRow + i / board->squareSize][boxStartCol + i % board->squareSize].possibleValues[indexValue] = FALSE;
                board->gameBoard[boxStartRow + i / board->squareSize][boxStartCol + i % board->squareSize].countPossibilities --;
            }
        }
    }
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
