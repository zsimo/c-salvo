#include <stdio.h>
#include <unistd.h>

#define GRID_COLS 25
#define GRID_ROWS 25
#define GRID_CELLS (GRID_COLS * GRID_ROWS)
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
    printf("\x1b[3J\x1b[H\x1b[2J"); // Clear screen
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

/** Returns the number of living cells neighbors of x,y */
int count_living_neighbors (char *grid, int x, int y) {
    int alive = 0;
    for (int y0 = -1; y0 <= 1; y0++) {
        for (int x0 = -1; x0 <= 1; x0++) {
            if (x0 == 0 && y0 == 0) {
                continue;
            }
            if (get_cell(grid, x+x0, y+y0) == ALIVE) {
                alive ++;
            }
        }
    }
    return alive;
}
/** Compute the new state of Game of Life according to its rules. */
void compute_new_state (char *old, char *new) {
    for (int y = 0; y < GRID_ROWS; y++) {
        for (int x = 0; x < GRID_COLS; x++) {
            int n_alive = count_living_neighbors(old, x, y);
            int new_state = DEAD;
//            printf("%i: %i,%i\n", n_alive, x, y);
            if (get_cell(old, x, y) == ALIVE) {
                if (n_alive == 2 || n_alive == 3) {
                    new_state = ALIVE;
                }
            } else {
               if (n_alive == 3) {
                    new_state = ALIVE;
                }
            }
            set_cell(new, x, y, new_state);
        }
    }
}
int main (void) {
    char old_grid[GRID_CELLS];
    char new_grid[GRID_CELLS];
    set_grid(old_grid, DEAD);
//    set_cell(old_grid, 10, 10, ALIVE);
//    set_cell(old_grid, 10, 11, ALIVE);
//    set_cell(old_grid, 10, 12, ALIVE);

    set_cell(old_grid, 10, 10, ALIVE);
    set_cell(old_grid, 9, 10, ALIVE);
    set_cell(old_grid, 11, 10, ALIVE);
    set_cell(old_grid, 11, 9, ALIVE);
    set_cell(old_grid, 10, 8, ALIVE);
    while (1) {
        compute_new_state(old_grid, new_grid);
        print_grid(new_grid);
        usleep(100000);
        compute_new_state(new_grid, old_grid);
        print_grid(old_grid);
        usleep(100000);
    }

    return 0;
}

