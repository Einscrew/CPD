#include "strategies.h"

/* Update vector of possibilities when a number is inserted */
void updateBoardValues(Board *board, int row, int col, int value)
{
    int indexValue = value - 1; /* used to update the possibilites of other cells */
    int i = 0;
    int boxStartRow = (row / board->size) * board->size;
    int boxStartCol = (col / board->size) * board->size;

    if(value == 0)
    {
        return;
    }

    if(board->gameBoard[row][col].possibleValues != NULL)
    {
        board->gameBoard[row][col].countPossibilities--;
        board->gameBoard[row][col].possibleValues[indexValue] = FALSE;
    }

    for(i = 0; i < (board->size*board->size); i++)
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
        if(board->gameBoard[boxStartRow + i / board->size][boxStartCol + i % board->size].value == 0)
        {
            if(board->gameBoard[boxStartRow + i / board->size][boxStartCol + i % board->size].possibleValues[indexValue] == TRUE)
            {
                board->gameBoard[boxStartRow + i / board->size][boxStartCol + i % board->size].possibleValues[indexValue] = FALSE;
                board->gameBoard[boxStartRow + i / board->size][boxStartCol + i % board->size].countPossibilities --;
            }
        }
    }
}

/* Checks if a value it's the only possible solution */
void deleteStrategy(Board *board)
{
    int changed = 0, row = 0, col = 0, i = 0;

    while(changed == 0)
    {
        changed = -1;
        for(row = 0; row < board->size * board->size; row++)
        {
            for(col = 0; col < board->size * board->size; col++)
            {
                if(board->gameBoard[row][col].value != 0 || board->gameBoard[row][col].countPossibilities != 1)
                {
                    continue;
                }

                for(i = 0; i < board->size * board->size; i++)
                {
                    /* Place in the game boar the only possible solution */
                    if(board->gameBoard[row][col].possibleValues[i] == TRUE)
                    {
                        board->gameBoard[row][col].value = i + 1; /* the possible value is the index +1 */
                        break;
                    }
                }
                updateBoardValues(board, row, col, board->gameBoard[row][col].value); /* updates the other cells */
                changed = 0;
            }
        }

        if(changed == -1)
        {
            break;
        }
    }
}

/* Guesses a value based on the cell with the less possible values and tries to solve the sudoku with that guess.
If the guess is wrong undo to the previous board and continue seraching. If no solution was found then there isn't
any solution */
/*int bruteForceStrategy(Board *board)
{
    int i = 0, try = 0;
    CellCoord minCell;

    if(minimumPossibilities(board, &minCell) == FALSE)
    {
        return FALSE;
    }

    for(i = 0; i < board->size * board->size; i++)
    {
        
        if(board->gameBoard[minCell.row][minCell.col].possibleValues[i] == FALSE)
        {
            continue;
        }
        else
        {
           
            copyBoard(board, copy, FALSE);

            try = i + 1;

           

            copy->gameBoard[minCell.row][minCell.col].value = try;
            updateBoardValues(copy, minCell.row, minCell.col, try);
           

            if(solveSudoku(board) == TRUE)
            {
           
                copyBoard(copy, board, FALSE);
                return TRUE;

            }
        }
    }

    return FALSE;
}*/

int solve(Board * b){
  int i, j, k, prev;
  Coord cell;

    for (i = 0; i < b->size; i++) {
        for (j = 0; j < b->size; j++) {
            if (b->gameBoard[i][j].value == 0) {
                for (k = 0; k < b->size; k++) {
                    /* When the value is zero and has no more possibilities left */
                    if(b->gameBoard[i][j].countPossibilities == 0)
                    {
                        return FALSE;
                    }

                    if (b->gameBoard[i][j].possibleValues[k] == TRUE){
                        prev = b->gameBoard[i][j].value;
                        b->gameBoard[i][j].value = k+1;
                        /* Means that it's the first time that we going to try a number and 
                        there's only one possibilitie for this cell */
                        if(prev == 0 && b->gameBoard[i][j].countPossibilities == 1)
                        {
                            updateBoardValues(b, i, j, b->gameBoard[i][j].value);
                        }
                    }

                    /* Checks if the guess is a possible choice in each row, column and box */
                    if(checkAllBoard(b, &cell) == FALSE){
                        /* If it's impossible, remove the value from the possibilities and decreases the number of possibilities */
                        b->gameBoard[i][j].possibleValues[k] = FALSE;
                        b->gameBoard[i][j].countPossibilities--;

                        if(b->gameBoard[i][j].countPossibilities == 0)
                        {
                            updateBoardValues(b, i, j, b->gameBoard[i][j].value);
                        }
                        //retornar ao valor anterior ?? PODE VOLTAR A ZERO SIMPLESMENTE ??
                        b->gameBoard[i][j].value = prev;
                        //b->gameBoard[i][j].value = 0;

                        /* Going back to the last element set in game board */
                        if(k == b->size)
                        {
                            while(b->gameBoard[i][j].value != 0){
                            
                                if(j == 0)
                                {
                                    i--;   
                                }
                                else if(i == 0){
                                    break;
                                }
                                else{
                                    i--;
                                }
                            }
                        }

                    }
                    else{
                        break;
                    }

                }
            }
        }
    }
}



/* Function that solves a sudoku starting with the humanistic approach (delete approach, etc) and then if it's not solved tries the
brute force approach */
int solveSudoku(Board *board)
{
    deleteStrategy(board);

    if(checkBoardComplete(board) == TRUE)
    {
        return TRUE;
    }

    /*if(solve(board) == TRUE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }*/

}
