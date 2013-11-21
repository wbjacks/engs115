// Includes
#include <stdlib.h>
#include <stdio.h>
#include "../util/macros.h"
#include "../util/board.h"

// Statics
static void solve(void *board, int n, int row, int col);


int main(int argc, char *argv[]) {
    // N-Queens problem
    CHECK_ARGC(2);

    int n = atoi(argv[1]);
    void *board = initialize(n);

    solve(board, n, 0, 0);
    cleanUp(board);

}

static void solve(void *board, int n, int row, int col) {
    void *new_board;
    // Base case 1: solved
    if (numSetQueens(board) == n) {
        printBoard(board);
        cleanUp(board);

    }
    // Recursive case
    else if (onBoard(board, row, col)) {
        if (isLegalMove(board, row, col)) {
            // Launch new recursion with this move placed
            new_board = boardCpy(board);
            setQueen(new_board, row, col);
            solve(new_board, n, row+1, 0);

        }
        // Recurse
        solve(board, n, row, col+1);

    }
}
