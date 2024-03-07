#include "snake.h"
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <threads.h>

void initSnake(Snake* snake, uint8_t x, uint8_t y) {
  snake->head.x = x;
  snake->head.y = y;
  snake->head.dir = INITIAL_DIR;

  snake->length = INITIAL_LENGTH;

  snake->tail = snake->head;
  snake->tail.y += snake->length - 1;

  // Now draw it
  printf("\e[%d;%dH\e[32m", snake->head.y + 1, snake->head.x + 1);
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

void advanceSnake(Snake* snake) {
  printf("\e[%d;%dH ", snake->tail.y + 1, snake->tail.x + 1);
  advancePos(&snake->head);
  advancePos(&snake->tail);
  
  // If the tail now lies on a bend, make sure it points in the direction of the bend
  Pos* lastBend = peekPosQueue(&snake->bends);
  if (lastBend != NULL && snake->tail.x == lastBend->x && snake->tail.y == lastBend->y) {
    popPosQueue(&snake->bends);
    snake->tail.dir = lastBend->dir;
  }
  
  printf("\e[%d;%dH█", snake->head.y + 1, snake->head.x + 1);
  fflush(stdout);
}

int snakeLoop(void* threadData) {
  Snake* snake = (Snake*) threadData;

  while (true) {
    usleep(500000); // 0.5s
    mtx_lock(&snake->mutex);
    advanceSnake(snake);
    mtx_unlock(&snake->mutex);
  }

  return 0;
}