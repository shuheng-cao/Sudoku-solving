#include <stdio.h>
#include "sudoku.h"
#include <stdbool.h>
#include <assert.h>

int main(void) {
    printf("Welcome to ASCII sudoku!\n");
    
    struct sudoku *s = read_sudoku();
    if (solve(s)) {
        print_sol(s);
    } else {
        printf("Could not solve the puzzle using search and backtracking :(\n");
    }
    sudoku_destroy(s);
    return 0;
}
