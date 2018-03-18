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
int bruteForceStrategy(Board *board)
{
    int i = 0, try = 0;
    MinCell minCell;
    Board copyOfBoard;

    if(minimumPossibilities(board, &minCell) == FALSE)
    {
        return FALSE;
    }

    for(i = 0; i < board->size * board->size; i++)
    {
        printf("ENTROU\n");
        /* Only cares about values that can be a solution */
        if(board->gameBoard[minCell.row][minCell.col].possibleValues[i] == FALSE)
        {
            continue;
        }
        else
        {
            /* Copies the board that will receive the try number */
            copyBoard(board, &copyOfBoard);
            /* The vector of possibilities has 9 positions, so our guess will be the number after the position in this vector */
            try = i + 1;

            /* Assigns the try number to the copied board and updates the possible values of the other cells in the same row,
            column or box */
            copyOfBoard.gameBoard[minCell.row][minCell.col].value = try;
            updateBoardValues(board, minCell.row, minCell.col, try);

            if(solveSudoku(&copyOfBoard) == TRUE)
            {
                /* Then we found the solution and copies it to the final board */
                copyBoard(&copyOfBoard, board);
                return TRUE;
            }
        }
    }

    return FALSE;
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

    if(bruteForceStrategy(board) == TRUE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }

}