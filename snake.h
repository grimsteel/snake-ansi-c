#pragma once

#include <stdint.h>
#include <threads.h>
#include "posqueue.h"

#define INITIAL_DIR DIR_UP
#define INITIAL_LENGTH 3

typedef struct {
  Pos head;
  Pos tail;
  PosQueue bends;
  uint8_t length;
  mtx_t mutex;
  Direction newDir;
  int endPipe;
} Snake;

void initSnake(Snake* snake, uint8_t x, uint8_t y);
int snakeLoop(void* threadData);