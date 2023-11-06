#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life

// Any live cell with fewer than two live neighbors dies, as if by underpopulation.
// Any live cell with two or three live neighbors lives on to the next generation.
// Any live cell with more than three live neighbors dies, as if by overpopulation.
// Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.

// condensed rules
// Any live cell with two or three live neighbors survives.
// Any dead cell with three live neighbors becomes a live cell.
// All other live cells die in the next generation. Similarly, all other dead cells stay dead.


enum { 
    N =8,
    ALIVE = 1,
    DEAD = 0
};

typedef char board_t[N][N]; // Square board excatly 8x8 with toroidal topology

#define life_cell(b, x, y) b[x][y]
#define set_cell(b, x, y, v) b[x][y] = v

board_t BLINKER;
board_t TOAD;
board_t BEACON;
board_t RANDOM;

static board_t current_board;
static board_t next_board;

typedef struct coord_s {
    int x;
    int y;
} coord_t;

// populate a board with random 0/1
static void random_board() {
    for (int row = 0; row < N; ++row) {
        for (int col = 0; col < N; ++col) {
            set_cell(current_board, row, col, rand() % 2);
        }
    }
}

// populate a blinker pattern
static void blinker() {
    memset(current_board, 0, sizeof(current_board));
    set_cell(current_board, 3, 4, 1);
    set_cell(current_board, 4, 4, 1);
    set_cell(current_board, 5, 4, 1);
}

// populate a blinker pattern
static void toad() {
    memset(current_board, 0, sizeof(current_board));
    set_cell(current_board, 3, 3, 1);
    set_cell(current_board, 3, 4, 1);
    set_cell(current_board, 3, 5, 1);
    set_cell(current_board, 4, 2, 1);
    set_cell(current_board, 4, 3, 1);
    set_cell(current_board, 4, 4, 1);
}

// populate a beacon pattern
static void beacon() {
    memset(current_board, 0, sizeof(current_board));
    set_cell(current_board, 1, 5, 1);
    set_cell(current_board, 1, 6, 1);
    set_cell(current_board, 2, 5, 1);
    set_cell(current_board, 2, 6, 1);
    set_cell(current_board, 3, 3, 1);
    set_cell(current_board, 3, 4, 1);
    set_cell(current_board, 4, 3, 1);
    set_cell(current_board, 4, 4, 1);
}

static void test_board() {
    memset(current_board, 0, sizeof(current_board));
    set_cell(current_board, 0, 0, 1);
    set_cell(current_board, N-1, 0, 1);
    set_cell(current_board, N-1, 1, 1);
}


static void print_board(board_t board) {
    for (int row = 0; row < N; ++row) {
        for (int col = 0; col < N; ++col) {
           printf("%c", life_cell(board, row, col) == ALIVE ? 'X' : '.');
        }
        printf("\n");
    }
}


// find the 8 neighbors of a cell and return the number of live neighbors
// use torroidal topology (e.g. row above 0 is at row N-1,  row below N-1 is at row 0)
static int live_neighbors(board_t board, int row, int columnn, int total_rows, int total_colunmns) {
    typedef struct offset_s {
        int r;
        int c;
    } offset_t;
    static const offset_t offsets[8] = { {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

    int num_alive = 0;
    for (int i = 0; i < 8; i++) {
        int r = ((row + offsets[i].r + total_rows)) % total_rows;
        int c = (columnn + offsets[i].c + total_colunmns) % total_colunmns; 
        if (life_cell(board, r, c) == ALIVE) {
            ++num_alive;
        }
    }
    return num_alive;
};

static void generation(board_t from_board, board_t to_board) {
    for (int row = 0; row < N; ++row) {
        for (int col = 0; col < N; ++col) {
            int numNeighbors = live_neighbors(from_board, row, col, N, N);
            // Any live cell with fewer than two live neighbours dies, as if by underpopulation.
            if (numNeighbors < 2) {
                set_cell(to_board, row, col, DEAD);
            } else if (numNeighbors == 2) {
                set_cell(to_board, row, col, life_cell(from_board, row, col));
            } else if (numNeighbors == 3) {
                set_cell(to_board, row, col, ALIVE);
            } else if (numNeighbors > 3) {
                set_cell(to_board, row, col, DEAD);
            }
        }
    }
}

/* 
* Initialize the current board with the given pattern
*/
static void init(const char* pattern) {
    if (!strcasecmp(pattern, "blinker")) {
        blinker();
    } else if (!strcasecmp(pattern, "toad")) {
        toad();
    } else if (!strcasecmp(pattern, "beacon")) {
        beacon();
    } else if (!strcasecmp(pattern, "random")) {
        random_board();  // generate a random board
#ifdef TESTING
    } else if (!strcasecmp(pattern, "test")) {
        test_board();  // for testing
#endif
    } else {
        printf("Unknown pattern: %s\n", pattern);
        exit(1);
    }

}

static void run() {
    // pointers to boards so we can swap without copying
    board_t *b1 = &current_board;
    board_t *b2 = &next_board;

    print_board(*b1);
    printf("\n");
    for (int i = 0; i < 3; ++i) {
        generation(*b1, *b2);
        print_board(*b2);
        printf("\n");
        // swap boards
        board_t *tmp = b1;
        b1 = b2;
        b2 = tmp;
    }
}


int main(int argc, const char* argv[]) {
#ifndef TESTING
    if (argc < 2) {
        printf("Usage: %s <pattern>\n", argv[0]);
        exit(1);
    }
    // initialize current board
    init(argv[1]);
#else
    init("test");  // for now, just manually testing
#endif
    // exercise the generation for some cycles
    run();
    return 0;
}
