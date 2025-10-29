#include <stdio.h>
#include <string.h>

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

int main(void) {
	init_board();
	draw_board();

	// 다른 함수 호출 코드는 이 아래에 적어주세요

	return 0;
}
