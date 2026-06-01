#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#define BOARD_SIZE 9 

typedef enum {
    EMPTY, BLACK, WHITE
} Color;

char* Color_as_str(Color color) {
    switch (color) {
        case BLACK:
            return "*";
        case WHITE:
            return "0";
        case EMPTY:
            return "┼";
    }

    assert(false); // unreachable
}

typedef struct {
    Color board[BOARD_SIZE][BOARD_SIZE];
    Color turn;
} Go;

Go Go_new() {
    return (Go){
        .board = {{EMPTY}},
        .turn = BLACK,
    };
}

Color* Go_getp(Go *go, int row, int col) {
    return &(go->board[row][col]);
}

void Go_display(Go *go) {
    for (int i = 0; i < BOARD_SIZE; i++) {

        
        for (int j = 0; j < BOARD_SIZE; j++) {
            Color c = *Go_getp(go, i, j);
            char* s = Color_as_str(c);
            printf("%s-", s);
        }
        printf("\n");

    }
}

int main() {
    Go go = Go_new();

    *Go_getp(&go, 0, 0) = BLACK;
    *Go_getp(&go, 1, 0) = WHITE;
    *Go_getp(&go, 1, 1) = BLACK;

    Go_display(&go);
    return 0;
}
