#include "board.h"

int allocBoard(Board *b)
{

    b->gameBoard = (Cell*)malloc(b->size*b->size*sizeof(Cell));
    b->rowMask = (int *)malloc(b->size*b->size * sizeof(int));
    b->colMask = (int *)malloc(b->size*b->size * sizeof(int));
    b->boxMask = (int *)malloc(b->size*b->size * sizeof(int));

    for (int i = 0; i < b->size; ++i)
    {
        b->rowMask[i] = 0;
        b->colMask[i] = 0;
        b->boxMask[i] = 0;
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
        
        new->rowMask[row(i, original->size)] = original->rowMask[row(i, original->size)];
        new->colMask[col(i, original->size)] = original->colMask[col(i, original->size)];
        new->boxMask[box(i, original->squareSize)] = original->boxMask[box(i, original->squareSize)];
    }
    return new;
}

void removeMasks(Board * b, int i)  {
    int value = b->gameBoard[i].value;
    if(value == 0){
        return;
    }

    int row = row(i,b->size), col = col(i,b->size), box = box(i,b->squareSize);
    
    b->rowMask[row] = removemask(b->rowMask[row], value);
    b->colMask[col] = removemask(b->colMask[col], value);
    b->boxMask[box] = removemask(b->boxMask[box], value);
}

void updateMasks(Board * b, int i){
    int row = row(i,b->size), col = col(i,b->size), box = box(i,b->squareSize);
    int value = b->gameBoard[i].value;
     if(value == 0){
        return;
    }
    
    b->rowMask[row] = addmask(b->rowMask[row], value);
    b->colMask[col] = addmask(b->colMask[col], value);
    b->boxMask[box] = addmask(b->boxMask[box], value);
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
int checkValidity(Board *b, int index, int value){
    //int v = b->gameBoard[index].value;
    /*for (int i = 0; i < l*l; ++i) {
        printf("[%d]Row: ",i);
        printMask(b->rowMask[i], l*l);
        printf(" Col: ");
        printMask(b->colMask[i], l*l);
        printf("Box: ");
        printMask(b->boxMask[i], l*l);
        printf("\n");
    }*/
   /* printf("Trying %d\n", value);
    printMask(b->rowMask[row(index,l*l)], l*l);
    printf(" %d\n", valid(value,b->rowMask[row(index+1,l*l)]));

    printMask(b->colMask[col(index,l*l)], l*l);
    printf(" %d\n",valid(value,b->colMask[col(index+1,l*l)]));

    printMask(b->boxMask[box(index,l)], l*l);
    printf(" %d\n",valid(value,b->boxMask[box(index,l)]));

*/
    int l = b->squareSize, totalMask, ret;
    int r = row(index,(l*l)), c = col(index,(l*l)), bo = box(index,l);

    int mr  = b->rowMask[r];
    int mc = b->colMask[c];
    int mb = b->boxMask[bo];
    totalMask = ((mr | mc) | mb);
    // printMask(totalMask, 9);
    ret = valid(value, totalMask);
    return ret;
}
/*int checkValidity(Board *b, int x)
{
    //[ 0  1  2  3|
    //| 4  5  6  7|
    //| 8  9 10 11|
    //|12 13 14 15]

    int i = 0, l = row(x,b->size), c = col(x,b->size), result = TRUE;
    int bline = off(l, b->squareSize), bcol = off(c, b->squareSize);

    int currb = 0, currl = no(0, l, b->size), currc = no(c, 0, b->size);

    int * existInLine = (int*)malloc(b->size * sizeof(int));
    int * existInCol = (int*)malloc(b->size * sizeof(int));
    int * existInBox = (int*)malloc(b->size * sizeof(int));

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

        if(b->gameBoard[currl].value  != 0){
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
}*/

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
    int i = 0, j = 0, index = 0, box = 0;
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
                    index = no(j,i,board->size);
                    box = box(index, board->squareSize);

                    if(atoi(aux) == 0)
                    {
                        board->gameBoard[index].fixed = FALSE;

                    }else
                    {
                        board->gameBoard[index].fixed = TRUE;
                        board->rowMask[i] = addmask(board->rowMask[i], atoi(aux));
                        board->colMask[j] = addmask(board->colMask[j], atoi(aux));
                        board->boxMask[box] = addmask(board->boxMask[box], atoi(aux));
                    }

                    board->gameBoard[index].value = atoi(aux);
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

    fclose(fptr);
    free(line);
    return 0;
}



/* Prints the game board */
void printBoard(Board *b)
{
    int i = 0;
    //int k = 0;

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
