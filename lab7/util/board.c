// Includes
#include "./board.h"

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
    Board_t *b;

    b = (Board_t *)malloc(sizeof(Board_t));
    b->queen_count = 0;
    b->n = n;

    // Build board spots
    b->board = malloc(n * sizeof(int *));
    for (i = 0; i < n; i++) {
        b->board[i] = malloc(n * sizeof(int));
        memset(b->board[i], 0, n*sizeof(int));

    }
    return (void *)b;

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
        new_b->board[i] = memcpy(new_b->board[i], b->board[i], b->n*sizeof(int));

    return (void *)new_b;

}

void printBoard(void *board) {
    int i, j;
    Board_t *b = (Board_t *)board;

    for (i = 0; i < b->n; i++) {
        for (j = 0; j < b->n; j++)
            printf("|%c", (hasQueen(b, i, j) ? 'Q' : ' '));
        printf("|\n");

    }
    printf("\n\n");
}

void setQueen(void *board, int row, int column) {
    Board_t *b = (Board_t *)board;
    if (!hasQueen(board, row, column)) {
        b->board[row][column] = QUEEN;
        b->queen_count++;

    }
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

    // Check diagonally R up
    for (i = row+1, j = column+1; i < b->n && j < b->n; i++, j++)
        if (hasQueen(board, i, j))
            return FALSE;

    // Check diagonally R down
    for (i = row-1, j = column-1; i >= 0 && j >= 0; i--, j--)
        if (hasQueen(board, i, j))
            return FALSE;

    // Check diagonally L up
    for (i = row+1, j = column-1; i < b->n && j >= 0; i++, j--)
        if (hasQueen(board, i, j))
            return FALSE;

    // Check diagonally L down
    for (i = row-1, j = column+1; i >= 0 && j < b->n; i--, j++)
        if (hasQueen(board, i, j))
            return FALSE;

    // Pass!
    return TRUE;

}

void removeQueen(void *board, int row, int column) {
    Board_t *b = (Board_t *)board;
    if (hasQueen(board, row, column)) {
        b->board[row][column] = NO_QUEEN;
        b->queen_count--;

    }
}

int hasQueen(void *board, int row, int column) {
    return (((Board_t *)board)->board[row][column] == QUEEN);

}

int numSetQueens(void *board) {
    //return ((Board_t *)board)->queen_count;
    int i, j;
    int count = 0;
    Board_t *b = (Board_t *)board;

    for (i = 0; i < b->n; i++) {
        for (j = 0; j < b->n; j++) {
            if (b->board[i][j] == QUEEN)
                count++;

        }
    }
    return count;
}

int onBoard(void *board, int row, int column) {
    return (row < ((Board_t *)board)->n && column < ((Board_t *)board)->n);

}

void *packBoard(void *board, size_t *size) {
    int i;
    char *pt;
    void *ret;
    Board_t *b = (Board_t *)board;

    ret = malloc(2*sizeof(int) + b->n * b->n * sizeof(int));
    pt = (char *)ret;
    memcpy(pt, &(b->n), sizeof(int));
    pt+= sizeof(int);

    memcpy(pt, &(b->queen_count), sizeof(int));
    pt += sizeof(int);

    for (i = 0; i < b->n; i++) {
        memcpy(pt, b->board[i], b->n*sizeof(int));
        pt += b->n*sizeof(int);

    }
    *size = pt - (char *)ret;
    return ret;

}

void *unpackBoard(void *buf, size_t *size) {
    int i, n;
    char *pt;
    Board_t *ret;

    pt = (char *)buf;

    // Get n
    memcpy(&n, pt, sizeof(int));
    pt += sizeof(int);

    // Build board
    ret = initialize(n);

    // Get queen
    memcpy(&(ret->queen_count), pt, sizeof(int));
    pt += sizeof(int);

    // Get board
    for (i = 0; i < n; i++) {
        memcpy(ret->board[i], pt, n*sizeof(int));
        pt += sizeof(n*sizeof(int));

    }
    *size = pt - (char *)buf;
    return ret;

}
