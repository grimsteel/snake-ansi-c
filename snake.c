#include "snake.h"
#include "board.h"
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <threads.h>
#include <wchar.h>

void initSnake(Snake* snake, uint8_t x, uint8_t y) {
  snake->head.x = x;
  snake->head.y = y;
  snake->head.dir = INITIAL_DIR;
  snake->newDir = INITIAL_DIR;

  snake->length = INITIAL_LENGTH;

  snake->tail = snake->head;
  snake->tail.y += snake->length - 1;

  // Now draw it
  printf("\e[%d;%dH\e[32m", snake->head.y + 2, snake->head.x + 2);
  for (int i = snake->head.y; i <= snake->tail.y; i++) {
    printf("█\e[D\e[B");
  }
  fflush(stdout);
}

// Advance a pos in its correct direction
void advancePos(Pos* pos) {
  switch (pos->dir) {
    case DIR_UP:
      pos->y--;
      break;
    case DIR_DOWN:
      pos->y++;
      break;
    case DIR_LEFT:
      pos->x--;
      break;
    case DIR_RIGHT:
      pos->x++;
      break;
  }
}

void printPos(Pos* pos, char* c) {
  printf("\e[%d;%dH%s", pos->y + 2, pos->x + 2, c);
}

void advanceSnake(Snake* snake) {
  printPos(&snake->tail, " ");
  printPos(&snake->head, "█");

  fflush(stdout);

  // make sure the head points in the direction of the newest bend
  if (snake->newDir != snake->head.dir) {
    snake->head.dir = snake->newDir;
    pushPosQueue(&snake->bends, &snake->head);
  }

  advancePos(&snake->head);
  advancePos(&snake->tail);

  // If the tail now lies on a bend, make sure it points in the direction of the bend
  Pos* oldestBend = peekPosQueue(&snake->bends);
  if (oldestBend != NULL && snake->tail.x == oldestBend->x && snake->tail.y == oldestBend->y) {
    snake->tail.dir = oldestBend->dir;
    popPosQueue(&snake->bends);
  }

  if (snake->head.x >= BOARD_SIZE || snake->head.y >= BOARD_SIZE) {
    close(snake->endPipe);
  } else {
    printPos(&snake->head, "▓");
  }
  fflush(stdout);
}

int snakeLoop(void* threadData) {
  Snake* snake = (Snake*) threadData;

  while (true) {
    usleep(100000); // 0.5s
    mtx_lock(&snake->mutex);
    advanceSnake(snake);
    mtx_unlock(&snake->mutex);
  }

  return 0;
}