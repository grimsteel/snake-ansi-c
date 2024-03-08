#pragma once

#include <stdint.h>
#include <threads.h>
#include "posqueue.h"
#include "board.h"

#define INITIAL_DIR DIR_UP
#define INITIAL_LENGTH 3

typedef uint8_t Board[BOARD_SIZE * BOARD_SIZE / 8 + 1];

typedef struct Snake {
  Pos head;
  Pos tail;
  PosQueue bends;
  uint8_t length;
  mtx_t mutex;
  Direction newDir;
  int endPipe;
  Board boardStatus;
} Snake;

void initSnake(Snake* snake, uint8_t x, uint8_t y);
int snakeLoop(void* threadData);