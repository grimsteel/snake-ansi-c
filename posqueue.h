#pragma once
#include <stdint.h>

typedef enum {
  DIR_UP,
  DIR_LEFT,
  DIR_RIGHT,
  DIR_DOWN
} Direction;

typedef struct {
  uint8_t x;
  uint8_t y;
  Direction dir;
} Pos;

typedef struct {
  Pos* values;
  uint8_t capacity;
  uint8_t count;
} PosQueue;

void initPosQueue(PosQueue* posQueue);
void growPosQueue(PosQueue* posQueue);
void pushPosQueue(PosQueue* posQueue, Pos* pos);
Pos* peekPosQueue(PosQueue* posQueue);
void popPosQueue(PosQueue* posQueue);
void freePosQueue(PosQueue* posQueue);