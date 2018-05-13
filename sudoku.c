#include <stdio.h>
#include "sudoku.h"
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>

const int DIM = 9;

static const int EMPTY = 0;
static const char BLANK = '-';
static const int MIN = 1;
static const int MAX = 9;

static const int SUCCESS = 0;
static const int ERROR = -1;
static const int ERASE_EMPTY_CELL = -2;
static const int ERASE_FILLED_CELL = -3;
static const int ERROR_NEXT_CELL = -4;

struct sudoku {
  int puzzle[DIM * DIM];
  int sol[DIM * DIM];
};

//Helpers:

static bool cell_empty(const int board[], int row, int col);
void reset_sol(struct sudoku *s);
int erase_cell(struct sudoku *s, int row, int col);
  
int find(int row, int col) {
  assert(0 <= row && row < DIM);
  assert(0 <= col && col < DIM);
  
  return DIM * row + col;
}

void adjustment(int row, int col, int x[DIM / 3], int y[DIM / 3]) {
  assert(0 <= row && row < DIM);
  assert(0 <= col && col < DIM);
  assert(x);
  assert(y);
  
  for (int i = 0; i < 3; ++i) {
    x[i] = row + i - (row % 3);
    y[i] = col + i - (col % 3);
  }
}
  
bool check(int x[DIM]) {
  assert(x);
  for (int i = 0; i < DIM; ++i) {
    assert(0 <= x[i] && x[i] <= DIM);
  }
  
  bool arr[DIM + 1];
  
  for (int i = 0; i <= DIM ; ++i) {
    arr[i] = false;
  }
  
  for (int i = 0; i < DIM; ++i) {
    if (x[i] == 0) continue;
    if (arr[x[i]] == true) return false;
    arr[x[i]] = true;
  }
  return true;
}
  
bool within(int x, int arr[], int len) {
  assert(arr);
  assert(len > 0);
  
  for (int i = 0; i < len; ++i) {
    if (arr[i] == x) return true;
  }
  
  return false;
}

bool valid(int x, int arr[DIM]) {
  assert(arr);
  assert(1 <= x && x <= DIM);
  
  return !within(x, arr, DIM);
}

bool row_valid(int row, int const puz[DIM * DIM]) {
  assert(puz);
  assert(0 <= row && row < DIM);
  
  int temp[DIM];
  
  for (int i = 0; i < DIM; ++i) {
    if (puz[find(row, i)] == EMPTY) return false;
    temp[i] = puz[find(row, i)];
  }
  return check(temp);
}

bool col_valid(int col, int const puz[DIM * DIM]) {
  assert(puz);
  assert(0 <= col && col < DIM);
  
  int temp[DIM];
  
  for (int i = 0; i < DIM; ++i) {
    if(puz[find(i, col)] == EMPTY) return false;
    temp[i] = puz[find(i, col)];
  }
  return check(temp);
}

bool sqr_valid(int row, int col, int const puz[DIM * DIM]) {
  assert(puz);
  assert(row == 0 || row == 3 || row == 6);
  assert(col == 0 || col == 3 || col == 6);
  
  int temp[DIM];
  int acc = 0;
  
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      temp[acc] = puz[find(row + i, col + j)];
      acc++;
    }
  }
  return check(temp);
}

void find_empty(const struct sudoku *s, int *row, int *col) {
  assert(s);
  assert(row);
  assert(col);

  for (int i = 0; i < DIM; ++i) {
    for (int j = 0; j < DIM; ++j) {
      if (!cell_empty(s->sol, i, j)) continue;
      *row = i;
      *col = j;
      return;
    }
  }
}



    
// You need to implement the following four functions.

int fill_cell(struct sudoku *s, int row, int col, int num) {
  assert(s);
  assert(0 <= row && row < DIM);
  assert(0 <= col && col < DIM);
  assert(1 <= num && num <= DIM);
  
  int mychoices[DIM];
  int mynum = 0;
  
  choices_cell(s, row, col, mychoices, &mynum);

  if (mynum == 0) return ERROR;
  
  if (within(num, mychoices, mynum)) {
    (s->sol)[find(row, col)] = num;
    return SUCCESS;
  } else {
    return ERROR;
  }
}

void choices_cell(const struct sudoku *s, int row, int col, 
                 int choices[], int *num_choices) {
  assert(s);
  assert(0 <= row && row < DIM);
  assert(0 <= col && col < DIM);
  assert(choices);
  assert(num_choices);
  
  int myrow[DIM], mycol[DIM], mysqr[DIM];
  int x[DIM / 3], y[DIM / 3];
  int acc = 0;
  *num_choices = 0;
  
  for (int i = 0; i < DIM; ++i) {
    myrow[i] = (s->sol)[find(row, i)];
    mycol[i] = (s->sol)[find(i, col)];
  }
  
  adjustment(row, col, x, y);
  
  for (int i = 0; i <= 2; ++i) {
    for (int j = 0; j <= 2; ++j) {
      mysqr[acc] = (s->sol)[find(x[i], y[j])];
      acc++;
    }
  }
  
  //the main part
  for (int i = 1; i <= DIM; ++i) {
    if (valid(i, myrow) && valid(i, mycol) && valid(i, mysqr)) {
      choices[*num_choices] = i;
      (*num_choices)++;
    }
  }
}

bool solved_puzzle(const struct sudoku *s) {
  assert(s);

  int acc = true;
  
  for (int i = 0; i < DIM; i++) {
    acc &= row_valid(i, (s->sol));
    acc &= col_valid(i, (s->sol));
  }
  
  for (int i = 0; i < DIM / 3; ++i) {
    for (int j = 0; j < DIM / 3; ++j) {
      acc &= sqr_valid(3 * i, 3 * j, (s->sol));
    }
  }
  return acc;
}

bool solve(struct sudoku *s) {
  assert(s);

  int row = -1, col = -1, my_num = 0;
  int my_choices[DIM];
  bool ans = false;

  if (solved_puzzle(s)) return true;
  
  find_empty(s, &row, &col);
  
  if (row == -1 || col == -1) return false;
  
  choices_cell(s, row, col, my_choices, &my_num);
  
  
  if (my_num == 0) return false;
  
  for (int i = 0; i < my_num; ++i) {
    fill_cell(s, row, col, my_choices[i]);
    ans = solve(s);
    if (ans) return true;
  }
  erase_cell(s, row, col);
  return ans;
}

// There is no need to modify the rest of the functions.

struct sudoku *read_sudoku(void) {
  struct sudoku *s = (struct sudoku *) malloc(sizeof(struct sudoku));
  char c = 0;
  for (int row = 0; row < DIM; ++row) {
    for (int col = 0; col < DIM; ++col) {
        scanf(" %c", &c);
      if (c == BLANK) {
        s->puzzle[row * DIM + col] = 0;
      } else {
        s->puzzle[row * DIM + col] = c - '0';
      }
    }
  }

  // copy puzzle to solution
  reset_sol(s);

  return s;
}

void sudoku_destroy(struct sudoku *s) {
  assert(s);
  free(s);
}

void print_sol(const struct sudoku *s) {
  assert(s);

  printf("\n");
  for (int row = 0; row < DIM; ++row) {
    for (int col = 0; col < DIM; ++col) {
      int num = s->sol[row * DIM + col];
      if (num == EMPTY) {
        printf("%c", BLANK);
      } else {
        printf("%d", num);
      }  
    }
    printf("\n");
  }
  printf("\n");
}

void reset_sol(struct sudoku *s) {
  assert(s);

  for (int row = 0; row < DIM; ++row) {
    for (int col = 0; col < DIM; ++col) {
      s->sol[row * DIM + col] = s->puzzle[row * DIM + col];
    }
  }  
}

// cell_empty(board, row, col) returns true
// if cell (row,col) is empty on board.
// requires: board is a valid sudoku puzzle.
static bool cell_empty(const int board[], int row, int col) {
  assert(board);
  assert(0 <= row && row <= DIM - 1);
  assert(0 <= col && col <= DIM - 1);

  return board[row * DIM + col] == EMPTY;
}

int erase_cell(struct sudoku *s, int row, int col) {
  assert(s);
  assert(0 <= row && row <= DIM - 1);
  assert(0 <= col && col <= DIM - 1);

  if (cell_empty(s->sol, row, col)) {
    return ERASE_EMPTY_CELL;
  }
  if (!cell_empty(s->puzzle, row, col)) {
    return ERASE_FILLED_CELL;
  }
  s->sol[row * DIM + col] = EMPTY;
  return SUCCESS;
}


int next_cell(const struct sudoku *s, int *row, int *col) {
  assert(s);
  assert(row);
  assert(col);

  int choices[DIM];
  int num_choices = 0;
  for (int i = 0; i < DIM; ++i) {
    for (int j = 0; j < DIM; ++j) {
      if (!cell_empty(s->sol, i, j)) continue;
      choices_cell(s, i, j, choices, &num_choices);
      if (num_choices == 1) {
        *row = i;
        *col = j;
        return SUCCESS;
      }
    }
  }
  return ERROR_NEXT_CELL;
}
