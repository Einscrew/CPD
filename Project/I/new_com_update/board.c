

#include "board.h"

#define no(x,y,l) (y*l+x)
#define line(n,l) (n/l)
#define col(n,l) (n%l)
#define off(i,sl) ((i / sl) * sl)

int allocBoard(Board *b)
{

    b->gameBoard = (Cell*)malloc(b->size*b->size*sizeof(Cell));

    if(b->gameBoard == NULL)
    {
        printf("Error on malloc!\n");
        return -1;
    }
    return 0;
}


/* Checks each row, column or box */
int checkValidity(Board *b, int x)
{
    //[ 0  1  2  3|
    //| 4  5  6  7|
    //| 8  9 10 11|
    //|12 13 14 15]

    int i = 0, l = line(x,b->size), c = col(x,b->size), result = TRUE;
    int bline = off(l, b->squareSize), bcol = off(c, b->squareSize);

    int currb = 0, currl = no(0, l, b->size), currc = no(c, 0, b->size);

    int *existInLine = (int*)malloc(b->size * sizeof(int));
    int *existInCol = (int*)malloc(b->size * sizeof(int));
    int *existInBox = (int*)malloc(b->size * sizeof(int));

    for(i = 0; i < b->size; i++){
        existInLine[i] = FALSE;
        existInCol[i] = FALSE;
        existInBox[i] = FALSE;

    }

    for(i = 0; i < b->size; i++){
        //line
        //printf("%d\n", b->gameBoard[no(i, l, b->size)].value);

        //column
        //printf("%d\n", b->gameBoard[no(c, i, b->size)].value);

        //box
        //printf("%d %d\n", bcol, bline+i);//b->gameBoard[no(bcol, bline+i, b->size)].value);

        currc = no(c, i, b->size);
        currl = no(i, l, b->size);
        currb = no(bcol+i, bline, b->size);

        //printf("currl: %d currc: %d currbox:%d \n", currl, currc, currb);

        if(b->gameBoard[currl].value != 0){
            if(existInLine[b->gameBoard[currl].value-1] == TRUE){
                result = FALSE;
                break;
            }
            existInLine[b->gameBoard[currl].value-1] = TRUE;
        }

        if(b->gameBoard[currc].value != 0){

            if(existInCol[b->gameBoard[currc].value-1] == TRUE){
                result = FALSE;
                break;
            }
            existInCol[b->gameBoard[currc].value-1] = TRUE;
        }

        if(b->gameBoard[currb].value != 0){
            if(existInBox[b->gameBoard[currb].value -1] == TRUE){
                result = FALSE;
                break;
            }
            existInBox[b->gameBoard[currb].value -1] = TRUE;
        }

        if(((i+1)%b->squareSize) == 0){
            bcol -= b->squareSize;
            bline++;
        }

    }

    free(existInLine);
    free(existInCol);
    free(existInBox);

    return result;
}

/* Checks if there aren't any duplicate values in each row, column or box */
int checkAllBoard(Board *b)
{
    return TRUE;
}


void freeBoard(Board *b)
{
    free(b->gameBoard);
}

/* Creates a vector of possibilities for each cell that has a value != 0 */
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
                        board->gameBoard[no(j,i,board->size)].fixed = FALSE;
                        board->gameBoard[no(j,i,board->size)].countPossibilities = board->size;
                        board->gameBoard[no(j,i,board->size)].minPoss = -1;

                    }else
                    {
                        board->gameBoard[no(j,i,board->size)].fixed = TRUE;
                        board->gameBoard[no(j,i,board->size)].countPossibilities = 0;
                        board->gameBoard[no(j,i,board->size)].minPoss = -1;
                    }

                    board->gameBoard[no(j,i,board->size)].value = atoi(aux);

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

    if(updatePossibilities(board) == FALSE)
    {
        return FALSE;
    }

    fclose(fptr);
    free(line);
    return TRUE;
}

int updatePossibilities(Board *board)
{
    int i = 0, j = 0, val = 0;

    /* Updates the possibilities of each cell according to the values that are read from the file only */
    for(i = 0; i < board->size * board->size; i++)
    {
        if(board->gameBoard[i].fixed == FALSE)
        {
            /* Try all possibilities and check if there's conflicts */
            for (j = 1; j <= board->size; j++) {

                board->gameBoard[i].value = j;
                val = checkValidity(board, i);
                if (val == FALSE){

                    board->gameBoard[i].countPossibilities--;
                }
                else if(val == TRUE && board->gameBoard[i].minPoss == -1)
                {
                    board->gameBoard[i].minPoss = j - 1; /* That's the first possible value for this cell */
                }
                /* If a cell doesn't have possibilities then the sudoku hasn't solution */
                if(board->gameBoard[i].countPossibilities == 0)
                {
                    printf("Invalid Sudoku!\n");
                    return FALSE;
                }

                /* If there's only one possibility assigns it */
                if(j == board->size && board->gameBoard[i].countPossibilities == 1)
                {
                    board->gameBoard[i].value = board->gameBoard[i].minPoss + 1;
                    board->gameBoard[i].fixed = TRUE;
                }
                else
                {
                    /* Put the initial state again */
                    board->gameBoard[i].value = 0;
                }

                //printf("count i = %d [%d]: %d\n", i, j,board->gameBoard[i].countPossibilities);
            }
        }
        //printf("\n");
    }

    return TRUE;
}

/* Prints the game board */
void printBoard(Board *b)
{
    int i = 0, k = 0;

    /*printf("| " );
    for(k = 0; k <= b->size+ +1   ; k++){
        printf("- ");
    }
    printf("|\n| " );*/
    for(i = 0; i < b->size*b->size; i++)
    {
        printf("%d ", b->gameBoard[i].value);

        /*if ((i+1)%b->squareSize == 0) {
            printf("| " );
        }*/
        if((i+1)%(b->size) == 0){

            printf("\n");
        }
        /*
        if((i+1)%(b->size * b->squareSize) == 0) {
            for(k = 0; k <= b->size+ +1   ; k++){
                printf("- ");
            }
            printf("|\n| " );
        }*/

    }
    printf("\n" );
}
