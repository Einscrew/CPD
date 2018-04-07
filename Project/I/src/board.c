#include "board.h"

int allocBoard(Board *b)
{

    b->gameBoard = (Cell*)malloc(b->size*b->size*sizeof(Cell));

    /*
    b->rowMask = (long int *)malloc(b->size*b->size * sizeof(long int));
    b->colMask = (long int *)malloc(b->size*b->size * sizeof(long int));
    b->boxMask = (long int *)malloc(b->size*b->size * sizeof(long int));
    */

    b->rowMask = (long int **)malloc(sizeof(long int *) * b->size);
    b->colMask = (long int **)malloc(sizeof(long int *) * b->size);
    b->boxMask = (long int **)malloc(sizeof(long int *) * b->size);

    for (int i = 0; i < b->size; ++i)
    {
        /*
        b->rowMask[i] = 0;
        b->colMask[i] = 0;
        b->boxMask[i] = 0;
        */
        b->rowMask[i] = (long int *)malloc(sizeof(long int ) * 3);
        b->rowMask[i][0]=0;
        b->rowMask[i][1]=0;
        b->rowMask[i][2]=0;

        b->colMask[i] = (long int *)malloc(sizeof(long int ) * 3);
        b->colMask[i][0]=0;
        b->colMask[i][1]=0;
        b->colMask[i][2]=0;

        b->boxMask[i] = (long int *)malloc(sizeof(long int ) * 3);
        b->boxMask[i][0]=0;
        b->boxMask[i][1]=0;
        b->boxMask[i][2]=0;
    }

    if(b->gameBoard == NULL)
    {
        printf("Error creating board!\n");
        return -1;
    }
    return 0;
}

Board * copyBoard(Board * original){

    Board * new = (Board *)malloc(sizeof(Board));

    new->size = original->size;
    new->squareSize = original->squareSize;

    allocBoard(new);
    
    
    for(int i = 0 ; i < original->size*original->size ; i++){
        new->gameBoard[i].value = original->gameBoard[i].value;
        new->gameBoard[i].fixed = original->gameBoard[i].fixed;
        
        /*
        new->rowMask[row(i, original->size)] = original->rowMask[row(i, original->size)];
        new->colMask[col(i, original->size)] = original->colMask[col(i, original->size)];
        new->boxMask[box(i, original->squareSize)] = original->boxMask[box(i, original->squareSize)];
        */
        new->rowMask[row(i, original->size)][0] = original->rowMask[row(i, original->size)][0];
        new->rowMask[row(i, original->size)][1] = original->rowMask[row(i, original->size)][1];
        new->rowMask[row(i, original->size)][2] = original->rowMask[row(i, original->size)][2];

        new->colMask[col(i, original->size)][0] = original->colMask[col(i, original->size)][0];
        new->colMask[col(i, original->size)][1] = original->colMask[col(i, original->size)][1];
        new->colMask[col(i, original->size)][2] = original->colMask[col(i, original->size)][2];

        new->boxMask[box(i, original->squareSize)][0] = original->boxMask[box(i, original->squareSize)][0];
        new->boxMask[box(i, original->squareSize)][1] = original->boxMask[box(i, original->squareSize)][1];
        new->boxMask[box(i, original->squareSize)][2] = original->boxMask[box(i, original->squareSize)][2];
    }
    return new;
}

void removeMasks(Board * b, int i)  {
    int value = b->gameBoard[i].value;
    if(value == 0){
        return;
    }
    int m_index = b->gameBoard[i].value/32;
    value -= (m_index*32);

    int row = row(i,b->size), col = col(i,b->size), box = box(i,b->squareSize);
    
    /*
    b->rowMask[row] = removemask((b->rowMask[row]), value);
    b->colMask[col] = removemask((b->colMask[col]), value);
    b->boxMask[box] = removemask((b->boxMask[box]), value);
    */

    b->rowMask[row][m_index] = removemask((b->rowMask[row][m_index]), value);
    b->colMask[col][m_index] = removemask((b->colMask[col][m_index]), value);
    b->boxMask[box][m_index] = removemask((b->boxMask[box][m_index]), value);
}

void updateMasks(Board * b, int i){
    int row = row(i,b->size), col = col(i,b->size), box = box(i,b->squareSize);
    int value = b->gameBoard[i].value;
    if(value == 0){
        return;
    }
    int m_index = b->gameBoard[i].value/32;
    value -= (m_index*32);
    
    /*  
    b->rowMask[row] = addmask(b->rowMask[row], value);
    b->colMask[col] = addmask(b->colMask[col], value);
    b->boxMask[box] = addmask(b->boxMask[box], value);
    */
    
    b->rowMask[row][m_index] = addmask(b->rowMask[row][m_index], value);
    b->colMask[col][m_index] = addmask(b->colMask[col][m_index], value);
    b->boxMask[box][m_index] = addmask(b->boxMask[box][m_index], value);
}


/* Checks each row, column or box */
void printMask(int n, int c){
    int k = 0;
    for ( c-- ; c >= 0; c--)
    {
        k = n >> c;
        if (k & 1)
          printf("1");
        else
          printf("0");
    }
    printf("\t");
}


int checkValidityMasks(Board *b, int index, int value){

    int l = b->squareSize, totalMask, ret;
    int r = row(index,(l*l)), c = col(index,(l*l)), bo = box(index,l);
    
    int m_index = value/32;
    /*
    long int mr  = b->rowMask[r];   
    long int mc = b->colMask[c];
    long int mb = b->boxMask[bo];
    */
    long int mr  = b->rowMask[r][m_index];
    long int mc = b->colMask[c][m_index];
    long int mb = b->boxMask[bo][m_index];
    totalMask = ((mr | mc) | mb);

    //if value is valid, given the totalMask
    ret = valid(value, totalMask);
    return ret;
}

void freeBoard(Board *b)
{
    free(b->gameBoard);
    
    
    for (int i = 0; i < b->size; ++i)
    {        
        free(b->rowMask[i]);
        free(b->colMask[i]);
        free(b->boxMask[i]);
    }
    
    free(b->rowMask);
    free(b->colMask);
    free(b->boxMask);
    
}

/* Creates a vector of possibilities for each cell that has a value != 0 */
int fillGameBoard(Board *board, char const* file)
{
    int i = 0, j = 0, index = 0, box = 0, m_index = 0, value = 0;
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
        if(fscanf(fptr, "%d\n", (int*)&(board->squareSize)) != 1){
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
            freeBoard(board);
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
                    index = no(j,i,board->size);
                    box = box(index, board->squareSize);

                    if(atoi(aux) == 0)
                    {
                        board->gameBoard[index].fixed = FALSE;

                    }else
                    {
                        /*
                        board->gameBoard[index].fixed = TRUE;
                        board->rowMask[i] = addmask(board->rowMask[i], atoi(aux));
                        board->colMask[j] = addmask(board->colMask[j], atoi(aux));
                        board->boxMask[box] = addmask(board->boxMask[box], atoi(aux));
                        */
                        board->gameBoard[index].fixed = TRUE;
                        value = atoi(aux);
                        m_index = value/32;
                        value -= m_index*32;
                        board->rowMask[i][m_index] = addmask(board->rowMask[i][m_index], value);
                        board->colMask[j][m_index] = addmask(board->colMask[j][m_index], value);
                        board->boxMask[box][m_index] = addmask(board->boxMask[box][m_index], value);
                    }

                    board->gameBoard[index].value = atoi(aux);
                    j++;
                }
                else
                {
                    printf("There is an invalid number on the board game, check the file!\n");
                    free(line);
                    freeBoard(board);
                    fclose(fptr);
                    return -1;
                }

                aux = strtok(NULL, " ");
            }
            j=0;
            i++;
        }
    }

    fclose(fptr);
    free(line);
    return 0;
}


/* Prints the game board */
void printBoard(Board *b)
{
    int i = 0;
    
    for(i = 0; i < b->size*b->size; i++)
    {
        printf("%d ", b->gameBoard[i].value);

        if((i+1)%(b->size) == 0){

            printf("\n");
        }
    }
    printf("\n" );
}

void printFixed(Board *b)
{
    int i = 0;
    
    for(i = 0; i < b->size*b->size; i++)
    {
        printf("%d ", (b->gameBoard[i].fixed)?1:0);

        if((i+1)%(b->size) == 0){

            printf("\n");
        }
    }
    printf("\n" );
}
