#include <curses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define FRAMERATE 10

typedef struct {
    int x;
    int y;
} vec2d;

typedef struct tail_piece {
    vec2d position;
    struct tail_piece *next;
} tail_piece;

vec2d genRandomPosition() {
    vec2d pos = {arc4random_uniform(getmaxx(stdscr)),
                 arc4random_uniform(getmaxy(stdscr))};
    return pos;
}

bool arePositionsEqual(vec2d pos1, vec2d pos2) {
    return pos1.x == pos2.x && pos1.y == pos2.y;
}

int main() {
    // Window setup
    initscr();
    cbreak();
    noecho();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    int gameRunning = 1;

    vec2d headPos = {getmaxx(stdscr) / 2, getmaxy(stdscr) / 2};
    enum { UP, DOWN, LEFT, RIGHT } snakeDir = RIGHT;
    int score = 0;
    vec2d fruitPos = genRandomPosition();

    // Initialize tail as a linked list
    tail_piece *lastTailPiece = NULL;
    for (int i = 0; i < 3; i++) {
        tail_piece *prevTailPiece = lastTailPiece;
        lastTailPiece = malloc(sizeof(tail_piece));
        lastTailPiece->position = headPos;
        lastTailPiece->position.x -= i + 1;
        lastTailPiece->next = prevTailPiece;
    }

    while (gameRunning) {
        // Regulate framerate
        usleep(1000000 / FRAMERATE);

        switch (getch()) {
        case 'q':
            gameRunning = 0;
            break;
        case 'w':
            if (snakeDir != DOWN)
                snakeDir = UP;
            break;
        case 's':
            if (snakeDir != UP)
                snakeDir = DOWN;
            break;
        case 'a':
            if (snakeDir != RIGHT)
                snakeDir = LEFT;
            break;
        case 'd':
            if (snakeDir != LEFT)
                snakeDir = RIGHT;
            break;
        }

        tail_piece *tailPiece = lastTailPiece;
        while (tailPiece != NULL) {
            // Check tail-head collisions
            if (arePositionsEqual(tailPiece->position, headPos)) {
                gameRunning = 0;
                nodelay(stdscr, FALSE);
            }

            // Tail following head
            if (tailPiece->next == NULL) {
                tailPiece->position = headPos;
            } else {
                tailPiece->position = tailPiece->next->position;
            }
            tailPiece = tailPiece->next;
        }

        // Move head
        switch (snakeDir) {
        case UP:
            headPos.y--;
            break;
        case DOWN:
            headPos.y++;
            break;
        case LEFT:
            headPos.x--;
            break;
        case RIGHT:
            headPos.x++;
            break;
        }

        // Screen edges
        if (headPos.x < 0 || headPos.x > getmaxx(stdscr) - 1 || headPos.y < 0 ||
            headPos.y > getmaxy(stdscr) - 1) {
            gameRunning = 0;
            nodelay(stdscr, FALSE);
        }

        // Snek eat froot
        if (arePositionsEqual(fruitPos, headPos)) {
            fruitPos = genRandomPosition();
            score++;

            // Extend tail
            tail_piece *prevTailPiece = lastTailPiece;
            lastTailPiece = malloc(sizeof(tail_piece));
            lastTailPiece->position = headPos;
            lastTailPiece->position = prevTailPiece->position;
            lastTailPiece->next = prevTailPiece;
        }

        // Begin drawing
        clear();

        // Fruit
        move(fruitPos.y, fruitPos.x);
        addch('F');

        // Tail
        tail_piece *currentTailPiece = lastTailPiece;
        while (currentTailPiece != NULL) {
            vec2d pos = currentTailPiece->position;
            move(pos.y, pos.x);
            addch('s');
            currentTailPiece = currentTailPiece->next;
        }

        // Snake head
        move(headPos.y, headPos.x);
        addch('S');

        // Score
        char scoreStr[getmaxx(stdscr)];
        sprintf(scoreStr, "Score: %d", score);
        move(0, 0);
        addstr(scoreStr);

        move(getmaxy(stdscr) - 1, getmaxx(stdscr) - 1);
        refresh();
        // End drawing
    }

    // Free tail linked list
    tail_piece *tailPiece = lastTailPiece;
    while (tailPiece != NULL) {
        tail_piece *next = tailPiece->next;
        free(tailPiece);
        tailPiece = next;
    }

    // Show game over
    move(getmaxy(stdscr) / 2, getmaxx(stdscr) / 2 - 4);
    addstr("Game Over");
    move(0, 0);
    refresh();
    getch();

    endwin();
    return 0;
}
