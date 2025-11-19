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

// 추가 : 현재 좌표가 기본 판 안에서 비어있는지 판별하는 보조 함수인데, 이동, 회전, 충돌체크에서 이용하려고 만들었습니다.
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

// 추가 : 블록이 판 중앙에서 나타나도록 초기 X좌표를 계산하는 함수 추가했습니다.
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

int block_move() { // 수정 : 함수 이름이 block_drop인데, 코드 보면 블록 이동을 전체적으로 제어하길래 이름을 move로 바꿨습니다.
    int type = rand() % 3;
    int x = (type == 1) ? (BOARD_W - 1 - 4) / 2 : (BOARD_W / 2);
    int y = 0;

    // 블록을 놓을 수 없으면 실패했다고 반환하는 코드입니다.
    if (!can_place(type, x, y)) return 0;

    if (!move_down(type, x, y)) return 0;

    const DWORD gravity_ms = 300; // 추가 : 떨어지는 속도를 조절하는 코드입니다.
    ULONGLONG last = GetTickCount64();

    while (1) {
        block(type, x, y, BLOCK); // 수정 : 기존 특수문자의 경우 block 가 선언된 값에 맞지 않아서 깨지는 문제가 발생합니다. 텍스트가 깨지지 않도록 수정했습니다.
        draw_board();

        if (_kbhit()) {
            // 수정 : 기존 input 변수 이름을 방향키와 wasd 일반키로 모두 받기 위해 변수를 쪼개면서 이름을 짧게 바꿨습니다.
            int c = _getch(); // 처음 입력된 값을 담는 변수입니다.
            int k; // 방향키인지, 일반키인지 검증하기 위해 만든 변수입니다.
            if (c == 0 || c == 224) k = _getch(); //0 또는 224는 특수키(방향키, 엔터 등)의 시작 값으로, 이 값을 거르고 이후 값을 받아야 입력이 제대로 이루어집니다. (이건 저도 몰라서 검색해보고 알았습니다)
            else k = c; // 특수키 시작값이 안들어오면 일반키이므로 그냥 c 값을 바로 대입합니다.

            switch (k) {
                // 수정 : wasd와 방향키 모두 작동 가능하도록 합니다. 여러 개의 케이스를 받는 경우 if문보다 case문이 더 효율적이기 때문에 수정했습니다.
            case 75: case 'a': case 'A':
                if (move_side(type, x, y, -1)) { block(type, x, y, EMPTY); x--; }
                break; // break 안적으면 블록이 지멋대로 사방으로 움직입니다..;
            case 77: case 'd': case 'D':
                if (move_side(type, x, y, 1)) { block(type, x, y, EMPTY); x++; }
                break;
            case 80: case 's': case 'S':
                if (move_down(type, x, y)) { block(type, x, y, EMPTY); y++; }
                break;
            case 72: case 'w': case 'W': //회전 케이스입니다, 위 방향키, w키 누르면 작동하고, 시간이 남아서 만들어봤습니다.
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
            case ' ': //스페이스바 입니다. 누르면 블록이 바로 설치됩니다. 얘 또한 시간이 남아서 만들었습니다.
                block(type, x, y, EMPTY);
                while (move_down(type, x, y)) { y++; }
                block(type, x, y, BLOCK);
                draw_board();
                line_remove();
                return 1;
            default: // 사용자가 다른 키를 입력해서 오류를 내지 못하도록 지정된 케이스가 아니면 default로 막아줘야 안전합니다.
                break;
            }
        }

        // 수정 : 아래로 낙하하는 코드입니다. 기존 코드는 속도 조절을 할 수 없고, 컴퓨터 처리속도에 따라 너무 빠르게 낙하해 제대로 된 플레이가 어렵다는 단점이 있어 수정했습니다.
        ULONGLONG now = GetTickCount64();
        if (now - last >= gravity_ms) { // 현재 시간에서 이전 시간을 뺀 값이 gravity보다 작을 경우 현재 시간을 이전 시간으로 지정하고, 블록을 한 칸 내리는 코드입니다.
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

    //추가 : 화면을 0,0 위치에서 계속 그리는 코드입니다. 이건 저도 모르겠어서 GPT랑 다른 개발자들의 도움을 받았습니다.
    //콘솔 커서를 맨 처음 위치로 가져와서 화면 업데이트를 해주는 기능입니다. 테트리스에서 자주 쓰는 코드라던데 실제로 자주 쓰는지는 잘 모르겠습니다;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0; GetConsoleMode(hOut, &mode);
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, mode);

    CONSOLE_CURSOR_INFO ci = { 1, FALSE };
    SetConsoleCursorInfo(hOut, &ci);

    init_board();
    draw_board();

    // 다른 함수 호출 코드는 이 아래에 적어주세요

    while (1) {
        if (!block_move()) {
            printf("\nGame Over!\n");
            printf("\n최종 점수: %d점\n", score); //게임 오버 시 총점 출력
            break;
        }
    }
    return 0;
}

/*
// 그래픽 코드드

// SDL 모듈을 사용하기 위한 define 선언
#define _CRT_SECURE_NO_WARNINGS
#define SDL_MAIN_HANDLED  // SDL 헤더를 사용할 때 보통 필요한 코드라고 합니다.

#pragma execution_character_set("utf-8")   // MSVC에서 실행 문자셋을 UTF-8로 설정합니다. 이 설정이 없으면 한글이 깨져서 나오는 문제가 발생합니다.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include <SDL2/SDL.h>                      // SDL 기본헤더(윈도우/렌더러/이벤트 처리에 이용합니다.)
#include <SDL2/SDL_ttf.h>                  // SDL_ttf(텍스트 렌더링에 이용합니다.)

enum Cell {
    EMPTY = 0,
    WALL = 1,
    BLOCK = 2
};

#define INNER_W 12
#define INNER_H 20

#define BOARD_W (INNER_W + 2)
#define BOARD_H (INNER_H + 1)

int board[BOARD_H][BOARD_W];
int score = 0;

// SDL 그래픽 화면을 위한 기본 설정입니다.
const int TILE = 24; // 픽셀 한 칸 크기를 지정합니다.
const int OFFSET_X = 24; // 화면 가로 너비 여백입니다.
const int OFFSET_Y = 36; // 화면 세로 너비 여백입니다.

SDL_Window* gWindow = NULL; // SDL 창을 띄웁니다.
SDL_Renderer* gRenderer = NULL; // SDL 렌더링할 때 필요합니다.
int WINDOW_W = 0, WINDOW_H = 0; // 실제 창 크기입니다.(셀 크기 + 여백으로 결정됩니다.)

TTF_Font* gFont = NULL; // 폰트를 이용하기 위한 변수입니다.
SDL_Color gTextColor = { 230, 230, 230, 255 }; // 점수 텍스트 색상 지정입니다.


void init_board(void) {
    for (int y = 0; y < BOARD_H; ++y)
        for (int x = 0; x < BOARD_W; ++x)
            board[y][x] = EMPTY;

    for (int y = 0; y < INNER_H; ++y) {
        board[y][0] = WALL;
        board[y][BOARD_W - 1] = WALL;
    }
    for (int x = 0; x < BOARD_W; ++x)
        board[BOARD_H - 1][x] = WALL;
}

// SDL을 이용해 보드를 렌더링 합니다.
void draw_board(void) {
    // 기본 SDL 창을 띄운 뒤, 배경을 어두운 색으로 칠해 화면을 클리어합니다. (화면 초기화할 때도 이루어집니다)
    SDL_SetRenderDrawColor(gRenderer, 22, 22, 26, 255);
    SDL_RenderClear(gRenderer); // 이전 프레임을 지웁니다.

    // 테트리스 판에 나타나는 얇은 격자 선을 출력합니다.
    SDL_SetRenderDrawColor(gRenderer, 40, 40, 46, 255); // 선 색상입니다.
    for (int y = 0; y <= BOARD_H; ++y)
        SDL_RenderDrawLine(gRenderer,
            OFFSET_X, OFFSET_Y + y * TILE,
            OFFSET_X + BOARD_W * TILE, OFFSET_Y + y * TILE);
    for (int x = 0; x <= BOARD_W; ++x)
        SDL_RenderDrawLine(gRenderer,
            OFFSET_X + x * TILE, OFFSET_Y,
            OFFSET_X + x * TILE, OFFSET_Y + BOARD_H * TILE);

    // 기본 판 위에 블록과 벽을 그립니다.
    for (int y = 0; y < BOARD_H; ++y) {
        for (int x = 0; x < BOARD_W; ++x) {
            int v = board[y][x];
            if (v == EMPTY) continue;
            SDL_Rect r = {
                OFFSET_X + x * TILE + 1,
                OFFSET_Y + y * TILE + 1,
                TILE - 2, TILE - 2
            };
            if (v == WALL) {
                SDL_SetRenderDrawColor(gRenderer, 100, 100, 110, 255); // 벽 색상을 지정합니다.
                SDL_RenderFillRect(gRenderer, &r);
            }
            else if (v == BLOCK) {
                SDL_SetRenderDrawColor(gRenderer, 0, 170, 200, 255); // 블록 색상입니다.
                SDL_RenderFillRect(gRenderer, &r);
                SDL_SetRenderDrawColor(gRenderer, 0, 120, 150, 255); // 블록 외곽선 색상입니다.
                SDL_RenderDrawRect(gRenderer, &r);
            }
        }
    }

    // 점수 텍스트를 출력합니다.
    {
        char buf[64]; // 점수 문자열 버퍼입니다.
        snprintf(buf, sizeof(buf), "현재 점수: %d점", score); // 콘솔에 프린트되는 형태에서 TTF를 이용해 그래픽상에 출력하는 형태로 바꿨습니다.
        SDL_Surface* s = TTF_RenderUTF8_Blended(gFont, buf, gTextColor);
        if (s) {
            SDL_Texture* t = SDL_CreateTextureFromSurface(gRenderer, s);
            if (t) {
                SDL_Rect dst = { OFFSET_X, OFFSET_Y - 28, s->w, s->h }; // 점수 위치를 지정합니다.
                SDL_RenderCopy(gRenderer, t, NULL, &dst);
                SDL_DestroyTexture(t);
            }
            SDL_FreeSurface(s);
        }
    }

    SDL_RenderPresent(gRenderer);
}

void block(int type, int x, int y, int c) {
    if (type == 0) {
        board[y][x] = c;
        board[y + 1][x] = c;
        board[y + 2][x] = c;
        board[y + 3][x] = c;
    }
    else if (type == 1) {
        board[y][x] = c;
        board[y][x + 1] = c;
        board[y][x + 2] = c;
        board[y][x + 3] = c;
    }
    else if (type == 2) {
        board[y][x] = c;
        board[y][x + 1] = c;
        board[y + 1][x] = c;
        board[y + 1][x + 1] = c;
    }
}

void line_remove(void) {
    for (int y = 0; y < INNER_H; ++y) {
        int full = 1;
        for (int x = 1; x <= BOARD_W - 2; ++x) {
            if (board[y][x] == EMPTY) { full = 0; break; }
        }
        if (full) {
            for (int k = y; k > 0; --k)
                for (int x = 1; x <= BOARD_W - 2; ++x)
                    board[k][x] = board[k - 1][x];
            for (int x = 1; x <= BOARD_W - 2; ++x) board[0][x] = EMPTY;
            score += 10;
        }
    }
}

int is_empty(int y, int x) {
    return (x >= 0 && x < BOARD_W &&
        y >= 0 && y < BOARD_H &&
        board[y][x] == EMPTY);
}

int move_down(int type, int x, int y) {
    if (type == 0) {
        if (y + 4 >= BOARD_H) return 0;
        return is_empty(y + 4, x);
    }
    if (type == 1) {
        if (y + 1 >= BOARD_H) return 0;
        for (int i = 0; i < 4; ++i) if (!is_empty(y + 1, x + i)) return 0;
        return 1;
    }
    if (type == 2) {
        if (y + 2 >= BOARD_H) return 0;
        return is_empty(y + 2, x) && is_empty(y + 2, x + 1);
    }
    return 0;
}

int move_side(int type, int x, int y, int z) {
    if (type == 0) {
        if (x + z < 1 || x + z > BOARD_W - 2) return 0;
        for (int i = 0; i < 4; ++i) if (!is_empty(y + i, x + z)) return 0;
        return 1;
    }
    if (type == 1) {
        if (z == -1) {
            if (x - 1 < 1) return 0;
            return is_empty(y, x - 1);
        }
        else {
            if (x + 4 > BOARD_W - 2) return 0;
            return is_empty(y, x + 4);
        }
    }
    if (type == 2) {
        if (z == -1) {
            if (x - 1 < 1) return 0;
            return is_empty(y, x - 1) && is_empty(y + 1, x - 1);
        }
        else {
            if (x + 2 > BOARD_W - 2) return 0;
            return is_empty(y, x + 2) && is_empty(y + 1, x + 2);
        }
    }
    return 0;
}

int can_place(int type, int x, int y) {
    if (type == 0) {
        if (x < 1 || x > BOARD_W - 2) return 0;
        if (y + 3 >= BOARD_H - 1) return 0;
        return is_empty(y, x) && is_empty(y + 1, x) && is_empty(y + 2, x) && is_empty(y + 3, x);
    }
    else if (type == 1) {
        if (x < 1 || x + 3 > BOARD_W - 2) return 0;
        if (y >= BOARD_H - 1) return 0;
        return is_empty(y, x) && is_empty(y, x + 1) && is_empty(y, x + 2) && is_empty(y, x + 3);
    }
    else if (type == 2) {
        if (x < 1 || x + 1 > BOARD_W - 2) return 0;
        if (y + 1 >= BOARD_H - 1) return 0;
        return is_empty(y, x) && is_empty(y, x + 1) && is_empty(y + 1, x) && is_empty(y + 1, x + 1);
    }
    return 0;
}


int block_move(void) {
    int type = rand() % 3;
    int x = (type == 1) ? (BOARD_W - 1 - 4) / 2 : (BOARD_W / 2);
    int y = 0;

    if (!can_place(type, x, y)) return 0;
    if (!move_down(type, x, y)) return 0;

    const Uint32 gravity_ms = 300; // 중력값을 지정할 수 있는 변수입니다. 기존 CUI 형태에서는 GetTickCount64로 판단했지만 GUI 형태로 변경한 지금은 SDL_GetTicks64를 기준으로 삼아 초를 카운트합니다.
    Uint64 last = SDL_GetTicks64(); // 마지막 낙하 시각을 구합니다.
    SDL_Event e; // 입력 이벤트를 감지합니다.

    while (1) {
        block(type, x, y, BLOCK);
        draw_board();

        // 입력 처리 부분입니다.
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return 0;
            if (e.type == SDL_KEYDOWN) {
                SDL_Keycode key = e.key.keysym.sym; // 어떤 키인지 판별합니다.
                switch (key) {
                case SDLK_LEFT: case SDLK_a: // _kbhit, _getch로 받아오던 키를 SDL 키 입력으로 받아옵니다.
                    if (move_side(type, x, y, -1)) { block(type, x, y, EMPTY); --x; }
                    break;
                case SDLK_RIGHT: case SDLK_d:
                    if (move_side(type, x, y, 1)) { block(type, x, y, EMPTY); ++x; }
                    break;
                case SDLK_DOWN: case SDLK_s:
                    if (move_down(type, x, y)) { block(type, x, y, EMPTY); ++y; }
                    break;
                case SDLK_UP: case SDLK_w:
                    if (type == 0 || type == 1) {
                        int newtype = (type == 0) ? 1 : 0;
                        block(type, x, y, EMPTY);
                        if (can_place(newtype, x, y)) type = newtype;
                    }
                    break;
                case SDLK_SPACE:
                    block(type, x, y, EMPTY);
                    while (move_down(type, x, y)) ++y;
                    block(type, x, y, BLOCK);
                    draw_board();
                    line_remove();
                    return 1;
                default: break;
                }
            }
        }

        Uint64 now = SDL_GetTicks64(); // SDL에서 현재 시각을 받아옵니다.
        if (now - last >= gravity_ms) {
            last = now;
            if (move_down(type, x, y)) {
                block(type, x, y, EMPTY);
                ++y;
            }
            else {
                line_remove();
                return 1;
            }
        }

        SDL_Delay(8);
    }
}

// 추가된 함수입니다
int app_init(void) {
#ifdef _WIN32
    HWND ch = GetConsoleWindow(); // 현재 콘솔 창 핸들입니다.
    if (ch) { ShowWindow(ch, SW_HIDE); FreeConsole(); }
#endif
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) { // SDL 서브시스템을 초기화 합니다.
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL_Init 실패", SDL_GetError(), NULL);
        return 0;
    }
    if (TTF_Init() != 0) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "TTF_Init 실패", TTF_GetError(), NULL);
        SDL_Quit(); // SDL을 종료합니다.
        return 0;
    }

    WINDOW_W = OFFSET_X * 2 + BOARD_W * TILE;
    WINDOW_H = OFFSET_Y + (BOARD_H + 1) * TILE;

    gWindow = SDL_CreateWindow("Tetris (SDL2)",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_W, WINDOW_H, SDL_WINDOW_SHOWN);
    if (!gWindow) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "창 생성 실패", SDL_GetError(), NULL);
        TTF_Quit(); SDL_Quit();
        return 0;
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!gRenderer) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "렌더러 생성 실패", SDL_GetError(), gWindow);
        SDL_DestroyWindow(gWindow); TTF_Quit(); SDL_Quit();
        return 0;
    }

    gFont = TTF_OpenFont("assets/fonts/NotoSansKR-Regular.ttf", 20);
    if (!gFont) {
        gFont = TTF_OpenFont("C:\\Windows\\Fonts\\malgun.ttf", 20);
        if (!gFont) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "폰트 로드 실패",
                "assets/fonts/NotoSansKR-Regular.ttf 또는 C:\\Windows\\Fonts\\malgun.ttf 를 찾을 수 없습니다.",
                gWindow);
            SDL_DestroyRenderer(gRenderer);
            SDL_DestroyWindow(gWindow);
            TTF_Quit(); SDL_Quit();
            return 0;
        }
    }

    init_board();
    return 1;
    return 1;
}

void app_quit(void) {
    if (gFont) { TTF_CloseFont(gFont); gFont = NULL; }  // 폰트를 해제합니다.
    if (gRenderer) { SDL_DestroyRenderer(gRenderer); gRenderer = NULL; } // 렌더러를 해제합니다.
    if (gWindow) { SDL_DestroyWindow(gWindow); gWindow = NULL; } // 창을 해제합니다.
    TTF_Quit();                                         // TTF를 종료합니다.
    SDL_Quit();                                         // SDL를 종료합니다.
}

int main(void) {
    srand((unsigned)time(NULL));
    if (!app_init()) return 1;

    draw_board();
    while (1) {
        if (!block_move()) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Game Over", "게임이 종료되었습니다.", gWindow);
            break;
        }
    }

    app_quit();
    return 0;
}
*/