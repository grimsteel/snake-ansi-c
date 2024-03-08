#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <threads.h>
#include "board.h"
#include "snake.h"
#include "posqueue.h"

int main() {
  int cancellationPipe;
  int endPipeFd[2];

  if (initializeInput(&cancellationPipe) != 0 || cancellationPipe == 0 || pipe(endPipeFd)) {
    printf("Could not create pipe or initialize input\n");
  }

  printf("Welcome to Snake!\nWould you like to play? (y/n)\n");
  KeyType key = getChar(cancellationPipe, endPipeFd[0]);
  if (key == KEY_N) goto end;

  drawBoard();

  Snake snake = {
    .boardStatus = {0}
  };
  initSnake(&snake, BOARD_SIZE / 2, BOARD_SIZE / 2);
  initPosQueue(&snake.bends);
  mtx_init(&snake.mutex, mtx_plain);
  snake.endPipe = endPipeFd[1];

  thrd_t snakeThread;
  thrd_create(&snakeThread, snakeLoop, (void*) &snake);

  while (true) {
    key = getChar(cancellationPipe, endPipeFd[0]);

    mtx_lock(&snake.mutex);
    Direction newDir = snake.head.dir;
    if (key == KEY_EXIT || key == KEY_END) break;
    else if (key == KEY_UP && newDir != DIR_DOWN) newDir = DIR_UP;
    else if (key == KEY_DOWN && newDir != DIR_UP) newDir = DIR_DOWN;
    else if (key == KEY_RIGHT && newDir != DIR_LEFT) newDir = DIR_RIGHT;
    else if (key == KEY_LEFT && newDir != DIR_RIGHT) newDir = DIR_LEFT;

    snake.newDir = newDir;
    mtx_unlock(&snake.mutex);
  }
  freePosQueue(&snake.bends);

  printf("\e[%d;1H", BOARD_SIZE + 3);
  if (key == KEY_END) printf("You lost!\n");
  printf("Thank you for playing Snake\n");
  end:
  close(cancellationPipe);
  close(endPipeFd[0]);
  close(endPipeFd[1]);
  deinitBoard();
  return 0;
}