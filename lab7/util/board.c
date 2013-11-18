// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Defines
#define QUEEN 1
#define NO_QUEEN 0

// Structs
struct __board {
    int **board;
    int queen_count;
    int n;

};
typedef struct __board Board_t;

void *initialize(int n) {
    int i;
    void *b;

    b = malloc(sizeof(Board_t));
    b->queen_count = 0;
    b->n = n;

    for (i = 0; i < n; i++)
        b->board[i] = malloc(n * sizeof(int));
    return b;

}

void cleanUp(void *board) {
    int i;
    Board_t *b = (Board_t *)board;

    for (i = 0; i < b->n; i++)
        free(b->board[i]);
    free(b);

}

void *boardCpy(void *board) {
    int i;
    Board_t *b = (Board_t *)board;
    Board_t *new_b = (Board_t *)initialize(b->n);

    new_b->n = b->n;
    new_b->queen_count = b->queen_count;

    for (i = 0; i < b->n; i++)
        memcpy(b->board[i], new_b->board[i], n*sizeof(int));

    return (void *)new_b;

}

void printBoard(void *board) {
    int i, j;
    Board_t *b = (Board_t *)board;

    for (i = 0; i < b->n; i++) {
        for (j = 0; j < b->n; j++) {
            printf("|%c", (hasQueen(b, i, j) ? 'Q' : ' '));
        printf("|\n");
    }
}

void setQueen(void *board, int row, int column) {
    Board_t *b = (Board_t *)board;
    b->board[row][column] = QUEEN;
    b->queen_count++;

}

// Again with the linear searching 'cuz I am lazy
int isLegalMove(void *board, int row, int column) {
    int i, j;
    Board_t *b = (Board_t *)board;

    // Check row
    for (i = 0; i < b->n; i++)
        if (hasQueen(board, row, i) && i != column)
            return FALSE;

    // Check col
    for (i = 0; i < b->n; i++)
        if (hasQueen(board, i, column) && i != row)
            return FALSE;

    // Check diagonally up
    for (i = row+1, j = column+1; i < b->n && j < b->n; i++, j++)
        if (hasQueen(board, i, j))
            return FALSE;

    // Check diagonally down
    for (i = row-1, j = column-1; i >= 0 && j >= 0; i--, j--)
        if (hasQueen(board, i, j))
            return FALSE;

    // Pass!
    return TRUE;

}

void removeQueen(void *board, int row, int column) {
    Board_t *b = (Board_t *)board;
    b->board[row][column] = NO_QUEEN;
    b->queen_count--;

}

int hasQueen(void *board, int row, int column) {
    return ((Board_t *)board)->board[row][column] == QUEEN;

}

int numSetQueens(void *board) {
    return ((Board_t *)board)->queen_count;

}
