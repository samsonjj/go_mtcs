#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#define BOARD_SIZE 9 

typedef enum {
    EMPTY, BLACK, WHITE
} Color;

typedef struct {
    int row, col;
} Coord;

typedef struct {
    Color board[BOARD_SIZE][BOARD_SIZE];
    Color turn;
} Go;

Color* Go_getp(Go *go, int row, int col);

typedef int Index;

Coord Index_to_coord(Index index) {
    return (Coord) {
        index / BOARD_SIZE,
        index % BOARD_SIZE
    };
}

Index Coord_to_index(Coord coord) {
    return coord.row * BOARD_SIZE + coord.col;
}

Color* Go_coordp(Go *go, Coord coord) {
    return &(go->board[coord.row][coord.col]);
}

Color* Go_getp(Go *go, int row, int col) {
    return &(go->board[row][col]);
}

// Call this when a piece is placed onto a coord. Thus, the liberty that was
// previously at that location is being removed.
int Go_get_liberties(Go *go, Coord coord) {
    Color c = *Go_coordp(go, coord);
    if (c == EMPTY) return -1;

    bool visited_lib[BOARD_SIZE][BOARD_SIZE] = {false};
    bool visited_stone[BOARD_SIZE][BOARD_SIZE] = {false};

    const int dr[4] = {0, 0, 1, -1};
    const int dc[4] = {1, -1, 0, 0};

    Coord stack[BOARD_SIZE * BOARD_SIZE] = {0};
    stack[0] = coord;
    int top = 0;

    int liberties = 0;

    while (top >= 0) {
        coord = stack[top];
        top--;
        printf("Move at (%d,%d)\n", coord.row, coord.col);
        for (int di = 0; di < 4; di++) {
            int nr = coord.row + dr[di];
            int nc = coord.col + dc[di];

            if (nr < 0 || nr >= BOARD_SIZE ||
                nc < 0 || nc >= BOARD_SIZE) {
                continue;
            }

            Color cc = *Go_getp(go, nr, nc);
            if (cc == EMPTY && (!visited_lib[nr][nc])) {
                visited_lib[nr][nc] = true;
                printf("Liberty at (%d,%d)\n", nr, nc);
                liberties++;
            } else if (cc == c && !visited_stone[nr][nc]) {
                visited_stone[nr][nc] = true;
                top++;
                stack[top] = (Coord){nr, nc};
            }
        }
    }

    return liberties;
}

Go Go_new() {
    return (Go){
        .board = {{EMPTY}},
        .turn = BLACK,
    };
}



void Go_make_move(Go *go, Coord move) {
    *Go_getp(go, move.row, move.col) = go->turn;
    if (go->turn == WHITE) {
        go->turn = BLACK;
    } else {
        go->turn = WHITE;
    }
}

char* get_terminal_char(Color color, int row, int col) {
    switch (color) {
        case BLACK:
            return "◯";
        case WHITE:
            return "⬤";
        case EMPTY:
            if (row == 0) {
                if (col == 0) {
                    return "┌";
                }
                if (col == BOARD_SIZE-1) {
                    return "┐";
                }
                return "┬";
            }
            if (row == BOARD_SIZE-1) {
                if (col == 0) {
                    return "└";
                }
                if (col == BOARD_SIZE-1) {
                    return "┘";
                }
                return "┴";
            }
            if (col == 0) {
                return "├";
            }
            if (col == BOARD_SIZE-1) {
                return "┤";
            }
            return "┼";
    }

    assert(false); // unreachable
}

void Go_display(Go *go) {
    printf("  ");
    for (int j = 0; j < BOARD_SIZE; j++) {
        printf("%c ", 'A' + j);
    }
    printf("\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("%c ", 'A' + i);
        for (int j = 0; j < BOARD_SIZE; j++) {
            Color c = *Go_getp(go, i, j);
            char* s = get_terminal_char(c, i, j);
            if (j != BOARD_SIZE-1) {
                printf("%s─", s);
            } else {
                printf("%s", s);
            }
        }
        printf(" %c", 'A' + i);
        printf("\n");
    }
    printf("  ");
    for (int j = 0; j < BOARD_SIZE; j++) {
        printf("%c ", 'A' + j);
    }
    printf("\n");
}

Coord input_move() {
    char a, b;

    printf("Enter Coord: ");
    scanf(" %c%c", &a, &b);
    return (Coord) {
        .row = toupper(a) - 'A',
        .col = toupper(b) - 'A',
    };
}

int main() {
    Go go = Go_new();

    Go_make_move(&go, (Coord){0, 0});
    Go_make_move(&go, (Coord){1, 0});
    Go_make_move(&go, (Coord){1, 1});

    while (1) {
        Go_display(&go);
        Coord move = input_move();
        Go_make_move(&go, move);
        printf("%d\n", Go_get_liberties(&go, move));
    }

    return 0;
}
