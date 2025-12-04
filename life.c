#include <stdio.h>

#define GRID_COLS 25
#define GRID_ROWS 25
#define ALIVE '*'
#define DEAD '.'


int cell_to_index (int x, int y) {

    if (x < 0) {
        x = (-x) % GRID_COLS;
        x = GRID_COLS - x;
    }
    if (y < 0) {
        y = (-y) % GRID_ROWS;
        y = GRID_ROWS - y;
    }
    if (x >= GRID_COLS) {
        x = x % GRID_COLS;
    }
    if (y >= GRID_ROWS) {
        y = y % GRID_ROWS;
    }

    return y * GRID_COLS + x;
}

void set_cell (char *grid, int x, int y, char state) {
    grid[cell_to_index(x, y)] = state;
}
/** The function returns the state of the grid at x, y. */
char get_cell (char *grid, int x, int y) {
    return grid[cell_to_index(x, y)];
}
/** Show the grid on the screen, clearing the terminal using the required VT100 escape sequence. */
void print_grid(char *grid) {
    for (int y = 0; y < GRID_ROWS; y++) {
        for (int x = 0; x < GRID_COLS; x++) {
            printf("%c", get_cell(grid, x, y));
        }
        printf("\n");
    }
}
/** Set all the grid cells to the specified state. */
void set_grid (char *grid, char state) {
    for (int y = 0; y < GRID_ROWS; y++) {
        for (int x = 0; x < GRID_COLS; x++) {
            set_cell(grid, x, y, state);
        }
    }
}
int main (void) {
    char grid[GRID_COLS * GRID_ROWS];
    set_grid(grid, ALIVE);
    print_grid(grid);
    return 0;
}

