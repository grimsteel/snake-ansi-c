#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <threads.h>
#include "board.h"
#include "snake.h"
#include "posqueue.h"

int main() {
  int cancellationPipe;
  if (initializeInput(&cancellationPipe) != 0 || cancellationPipe == 0) {
    printf("Could not create pipe or initialize input\n");
  }

  printf("Welcome to Snake!\nWould you like to play? (y/n)\n");
  KeyType key = getChar(cancellationPipe);
  if (key == KEY_N) goto end;

  drawBoard();

  Snake snake;
  initSnake(&snake, BOARD_SIZE / 2, BOARD_SIZE / 2);
  initPosQueue(&snake.bends);
  mtx_init(&snake.mutex, mtx_plain);

  thrd_t snakeThread;
  thrd_create(&snakeThread, snakeLoop, (void*) &snake);

  while (true) {
    key = getChar(cancellationPipe);

    mtx_lock(&snake.mutex);
    Direction newDir = snake.head.dir;
    if (key == KEY_EXIT) break;
    else if (key == KEY_UP) newDir = DIR_UP;
    else if (key == KEY_DOWN) newDir = DIR_DOWN;
    else if (key == KEY_RIGHT) newDir = DIR_RIGHT;
    else if (key == KEY_LEFT) newDir = DIR_LEFT;

    if (newDir != snake.head.dir) {
      snake.head.dir = newDir;
      pushPosQueue(&snake.bends, &snake.head);
    }
    mtx_unlock(&snake.mutex);
  }
  freePosQueue(&snake.bends);

  end:
  close(cancellationPipe);
  deinitBoard();
  return 0;
}