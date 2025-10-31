#include <stdio.h>
#include <string.h>
#include <conio.h>

enum Cell {
	EMPTY = 0,
	WALL = 1
};

#define INNER_W 12 // 내부 크기
#define INNER_H 20 // 내부 크기

#define BOARD_W (INNER_W+2) // 전체 보드의 가로 크기(벽 포함)
#define BOARD_H (INNER_H+1) // 전체 보드의 세로 크기(벽 포함)

static int board[BOARD_H][BOARD_W]; // 테트리스 판이 생길 구역

static const char *CELL_CHARS[] = {
	"  ", // 비어있는 칸
	"[]"  // 막혀있는 칸
};

static void init_board() {
	for (int y = 0; y < BOARD_H; y++) {
		for (int x = 0; x < BOARD_W; x++) {
			board[y][x] = EMPTY; // 빈 공간
		}
	}

	for (int y = 0; y < INNER_H; ++y) {
		board[y][0] = WALL; // 왼쪽 벽
		board[y][BOARD_W - 1] = WALL; // 오른쪽 벽
	}

	for (int x = 0; x < BOARD_W; ++x) {
		board[BOARD_H - 1][x] = WALL; // 바닥
	}
}

static void draw_board(void) {
	/*printf("   ");
	for (int x = 0; x < BOARD_W; ++x) {
		printf("%2d", x % 10);
	}
	printf("\n");*/ // 테트리스 보드 가로줄 인덱스

	for (int y = 0; y < BOARD_H; ++y) {
		// printf("%2d ", y); // 테트리스 보드 세로줄 인덱스
		for (int x = 0; x < BOARD_W; ++x) {
			printf("%s", CELL_CHARS[ board[y][x] ]);
		}
		printf("\n");
	}
	fflush(stdout);
}

//다른 함수는 이 아래에 작성해주세요..

void block (int type, int x, int y, char c) {
    if (type == 0) { //세로4개블록
        board[y][x] = c;
        board[y+1][x] = c;
        board[y+2][x] = c;
        board[y+3][x] = c;
    } else if (type == 1) { //가로4개
        board[y][x] = c;
        board[y][x+1] = c;
        board[y][x+2] = c;
        board[y][x+3] = c;
    } else if (type == 2) { //2x2
        board[y][x] = c;
        board[y][x+1] = c;
        board[y+1][x] = c;
        board[y+1][x+1] = c;
    }
}

void line_remove() { // 한줄 차면 지우기
    for (int i = 0; i < BOARD_H; i++) {
        int full = 1;
        for (int j = 0; j < BOARD_W; j++) {
            if (board[i][j] == EMPTY) {
                full = 0;
                break;
            }
        }

        if (full) {
            for (int k = i; k > 0; k--) {
                for (int j = 0; j < BOARD_W; j++) {
                    board[k][j] = board[k - 1][j];
                }
            }
            for (int j = 0; j < BOARD_W; j++)
                board[0][j] = EMPTY;
        }
    }
}

int move_down(int type, int x, int y) { // 밑으로 내려갈 수 있는지 확인
    if (type == 0) { // 세로 4칸
        if (y + 4 >= BOARD_H) return 0;
        return board[y + 4][x] == EMPTY;
    }
    if (type == 1) { // 가로 4칸
        if (y + 1 >= BOARD_H) return 0;
        for (int i = 0; i < 4; i++)
            if (board[y + 1][x + i] != EMPTY)
                return 0;
        return 1;
    }
    if (type == 2) { // 2x2 블록
        if (y + 2 >= BOARD_H) return 0;
        return (board[y + 2][x] == EMPTY && board[y + 2][x + 1] == EMPTY);
    }
    return 0;
}

int move_side(int type, int x, int y, int z) { // 좌우 이동 가능 확인
    if (type == 0) { // 세로 4칸
        if (x + z < 0 || x + z >= BOARD_W) return 0;
        for (int i = 0; i < 4; i++)
            if (board[y + i][x + z] != EMPTY)
                return 0;
        return 1;
    }
    if (type == 1) { // 가로 4칸
        if (z == -1) { // 왼쪽
            if (x - 1 < 0) return 0;
            return board[y][x - 1] == EMPTY;
        }
        if (z == 1) { // 오른쪽
            if (x + 4 >= BOARD_W) return 0;
            return board[y][x + 4] == EMPTY;
        }
    }
    if (type == 2) { // 2x2
        if (z == -1) {
            if (x - 1 < 0) return 0;
            return (board[y][x - 1] == EMPTY && board[y + 1][x - 1] == EMPTY);
        }
        if (z == 1) {
            if (x + 2 >= BOARD_W) return 0;
            return (board[y][x + 2] == EMPTY && board[y + 1][x + 2] == EMPTY);
        }
    }
    return 0;
}

int block_drop() { // 블록 떨어뜨리기
    int type = rand() % 3;
    int x = BOARD_W / 2;
    int y = 0;

    while (1) {
        block(type, x, y, '■');
        draw_board();

        if (_kbhit()) {
            char input = _getch(); //이거 그냥 인터넷에 실시간입력코드 검색해서 쓴건데 혹시 오류나면? 그냥 바꿀게요..

            if (input == 'a' && move_side(type, x, y, -1)) { // 왼쪽
                block(type, x, y, EMPTY);
                x--;
            } 
            else if (input == 'd' && move_side(type, x, y, 1)) { // 오른쪽
                block(type, x, y, EMPTY);
                x++;
            } 
            else if (input == 's') { // 빠르게 내리기
                while (move_down(type, x, y)) {
                    block(type, x, y, EMPTY);
                    y++;
                }
            }
        }

        if (move_down(type, x, y)) {
            block(type, x, y, EMPTY);
            y++;
        } else {
            line_remove();
            return 1;
        }
    }
}

int main(void) {
	init_board();
	draw_board();

	// 다른 함수 호출 코드는 이 아래에 적어주세요

	return 0;
}
