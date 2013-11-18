#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./macros.h"

// Prototypes

// Creates and returns a board
void *initialize(int n);

// Safely cleans up a board
void cleanUp(void *board);

// Copies the given board into a new board struct
void *boardCpy(void *board);

// Prints out a board
void printBoard(void *board);

// Sets a queen at specified location
void setQueen(void *board, int row,  int column);

// Checks if the specified location is a legal "move" for n-queens
int isLegalMove(void *board, int row, int column);

// Removes a queen from the specified location
void removeQueen(void *board, int row, int column);

// Checks if specified location has a queen
int hasQueen(void *board, int row, int column);

// Check for the total number of queens on the board
int numSetQueens(void *board);

// Check if given row / col is on the board
int onBoard(void *board, int row, int column);
