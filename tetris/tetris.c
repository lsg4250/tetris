// 기존 코드

// 기존 코드

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <conio.h>

enum Cell {
    EMPTY = 0,
    WALL = 1,
    BLOCK = 2

};

#define INNER_W 12
#define INNER_H 20

#define BOARD_W (INNER_W+2)
#define BOARD_H (INNER_H+1)

int board[BOARD_H][BOARD_W];
int score = 0;

static const char* CELL_CHARS[] = {
   "  ",  // EMPTY
   "[]",  // WALL
   "[]"   // BLOCK
};

static void init_board() {
    for (int y = 0; y < BOARD_H; y++) {
        for (int x = 0; x < BOARD_W; x++) {
            board[y][x] = EMPTY;
        }
    }

    for (int y = 0; y < INNER_H; ++y) {
        board[y][0] = WALL;
        board[y][BOARD_W - 1] = WALL;
    }
    for (int x = 0; x < BOARD_W; ++x) {
        board[BOARD_H - 1][x] = WALL;
    }
}

static void draw_board(void) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hOut, (COORD) { 0, 0 });

    // printf("   ");
    for (int x = 0; x < BOARD_W; ++x) {
        // printf("%2d", x % 10);
    }
    printf("\n");

    for (int y = 0; y < BOARD_H; ++y) {
        // printf("%2d ", y);
        for (int x = 0; x < BOARD_W; ++x) {
            int v = board[y][x];
            if (v < 0 || v > 2) v = 0;
            printf("%s", CELL_CHARS[v]);
        }
        printf("\n");
    }
    printf("\n현재 점수: %d점\n", score); //점수
    fflush(stdout);
}

//다른 함수는 이 아래에 작성해주세요..

static void block(int type, int x, int y, int c) {
    if (type == 0) { //세로4개블록
        board[y][x] = c;
        board[y + 1][x] = c;
        board[y + 2][x] = c;
        board[y + 3][x] = c;
    }
    else if (type == 1) { //가로4개
        board[y][x] = c;
        board[y][x + 1] = c;
        board[y][x + 2] = c;
        board[y][x + 3] = c;
    }
    else if (type == 2) { //2x2
        board[y][x] = c;
        board[y][x + 1] = c;
        board[y + 1][x] = c;
        board[y + 1][x + 1] = c;
    }
}

static void line_remove() { // 한줄 차면 지우기
    // 수정 : 기존 코드 읽어보니까 full이면 지우는데, 벽까지 지워지는 오류가 있어서 수정했습니다.
    for (int y = 0; y < INNER_H; y++) {
        int full = 1;
        for (int x = 1; x <= BOARD_W - 2; x++) {
            if (board[y][x] == EMPTY) { full = 0; break; }
        }
        if (full) {
            for (int k = y; k > 0; k--) {
                for (int x = 1; x <= BOARD_W - 2; x++) {
                    board[k][x] = board[k - 1][x];
                }
            }
            for (int x = 1; x <= BOARD_W - 2; x++) board[0][x] = EMPTY;
            score += 10;
        }
    }
}

int is_empty(int y, int x) {
    return (x >= 0 && x < BOARD_W && y >= 0 && y < BOARD_H && board[y][x] == EMPTY);
}

int move_down(int type, int x, int y) { // 밑으로 내려갈 수 있는지 확인
    if (type == 0) { // 세로 4칸
        if (y + 4 >= BOARD_H) return 0;
        return is_empty(y + 4, x);
    }
    if (type == 1) { // 가로 4칸
        if (y + 1 >= BOARD_H) return 0;
        for (int i = 0; i < 4; i++) if (!is_empty(y + 1, x + i)) return 0;
        return 1;
    }
    if (type == 2) { // 2x2 블록
        if (y + 2 >= BOARD_H) return 0;
        return is_empty(y + 2, x) && is_empty(y + 2, x + 1);
    }
    return 0;
}

int move_side(int type, int x, int y, int z) {
    if (type == 0) { // 세로4
        if (x + z < 1 || x + z > BOARD_W - 2) return 0; // 내부 한계
        for (int i = 0; i < 4; i++) if (!is_empty(y + i, x + z)) return 0;
        return 1;
    }
    if (type == 1) { // 가로4
        if (z == -1) {
            if (x - 1 < 1) return 0;
            return is_empty(y, x - 1);
        }
        else { // z==1
            if (x + 4 > BOARD_W - 2) return 0;
            return is_empty(y, x + 4);
        }
    }
    if (type == 2) { // 2x2
        if (z == -1) {
            if (x - 1 < 1) return 0;
            return is_empty(y, x - 1) && is_empty(y + 1, x - 1);
        }
        else { // z==1
            if (x + 2 > BOARD_W - 2) return 0;
            return is_empty(y, x + 2) && is_empty(y + 1, x + 2);
        }
    }
    return 0;
}

int start_block(int type) {
    if (type == 0) return BOARD_W / 2;
    if (type == 1) return (BOARD_W - 1 - 4) / 2;
    return (BOARD_W - 1 - 2) / 2;
}

int can_place(int type, int x, int y) {
    if (type == 0) {              // 세로4
        if (x < 1 || x > BOARD_W - 2) return 0;
        if (y + 3 >= BOARD_H - 1) return 0;
        return is_empty(y, x) && is_empty(y + 1, x) && is_empty(y + 2, x) && is_empty(y + 3, x);
    }
    else if (type == 1) {       // 가로4
        if (x < 1 || x + 3 > BOARD_W - 2) return 0;
        if (y >= BOARD_H - 1) return 0;
        return is_empty(y, x) && is_empty(y, x + 1) && is_empty(y, x + 2) && is_empty(y, x + 3);
    }
    else if (type == 2) {       // 2x2
        if (x < 1 || x + 1 > BOARD_W - 2) return 0;
        if (y + 1 >= BOARD_H - 1) return 0;
        return is_empty(y, x) && is_empty(y, x + 1) && is_empty(y + 1, x) && is_empty(y + 1, x + 1);
    }
    return 0;
}

int block_move() {
    int type = rand() % 3;
    int x = (type == 1) ? (BOARD_W - 1 - 4) / 2 : (BOARD_W / 2);
    int y = 0;

    if (!can_place(type, x, y)) return 0;

    if (!move_down(type, x, y)) return 0;

    const DWORD gravity_ms = 300;
    ULONGLONG last = GetTickCount64();

    while (1) {
        block(type, x, y, BLOCK);
        draw_board();

        if (_kbhit()) {
            int c = _getch();
            int k;
            if (c == 0 || c == 224) k = _getch();
            else k = c;

            switch (k) {

            case 75: case 'a': case 'A':
                if (move_side(type, x, y, -1)) { block(type, x, y, EMPTY); x--; }
                break;

            case 77: case 'd': case 'D':
                if (move_side(type, x, y, 1)) { block(type, x, y, EMPTY); x++; }
                break;

            case 80: case 's': case 'S':
                if (move_down(type, x, y)) {      // 이동 가능할 때만
                    block(type, x, y, EMPTY);     // 블록을 지우고
                    y++;                           // y 증가
                }
                break;

            case 72: case 'w': case 'W':
            {
                if (type == 0 || type == 1) {
                    int newtype = (type == 0) ? 1 : 0;
                    block(type, x, y, EMPTY);
                    if (can_place(newtype, x, y)) {
                        type = newtype;
                    }
                }
                break;
            }

            case ' ':
                block(type, x, y, EMPTY);
                while (move_down(type, x, y)) { y++; }
                block(type, x, y, BLOCK);
                draw_board();
                line_remove();
                return 1;

            default:
                break;
            }
        }

        ULONGLONG now = GetTickCount64();
        if (now - last >= gravity_ms) { 
            last = now;
            if (move_down(type, x, y)) {
                block(type, x, y, EMPTY);
                y++;
            }
            else {
                line_remove();
                return 1;
            }
        }

        Sleep(8);
    }
}

int main(void) {
    srand((unsigned)time(NULL));

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0; GetConsoleMode(hOut, &mode);
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, mode);

    CONSOLE_CURSOR_INFO ci = { 1, FALSE };
    SetConsoleCursorInfo(hOut, &ci);

    init_board();
    draw_board();

    while (1) {
        if (!block_move()) {
            printf("\nGame Over!\n");
            printf("\n최종 점수: %d점\n", score);
            break;
        }
    }
    return 0;
}