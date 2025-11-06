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

   /*printf("   ");*/
   for (int x = 0; x < BOARD_W; ++x) {
      /*printf("%2d", x % 10);*/
   }
   printf("\n");

   for (int y = 0; y < BOARD_H; ++y) {
      /*printf("%2d ", y);*/
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