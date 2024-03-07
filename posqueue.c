#include "posqueue.h"
#include <stdlib.h>
#include <string.h>

// Init a pos queue with a capacity of 8
void initPosQueue(PosQueue* posQueue) {
  posQueue->capacity = 8;
  posQueue->count = 0;
  posQueue->values = calloc(posQueue->capacity, sizeof(Pos));
}

// Grow by 2x
void growPosQueue(PosQueue* posQueue) {
  uint8_t newSize = posQueue->capacity * 2;
  Pos* newValues = calloc(newSize, sizeof(Pos));
  // Copy old to new
  memcpy(newValues, posQueue->values, posQueue->count * sizeof(Pos));
  // Update struct
  free(posQueue->values);
  posQueue->values = newValues;
  posQueue->capacity = newSize;
}

void pushPosQueue(PosQueue* posQueue, Pos* pos) {
  if (posQueue->count + 1 > posQueue->capacity) growPosQueue(posQueue);
  posQueue->values[posQueue->count++] = *pos;
}

Pos* peekPosQueue(PosQueue* posQueue) {
  if (posQueue->count == 0) return NULL;
  else return &posQueue->values[0];
}

void popPosQueue(PosQueue* posQueue) {
  // just return the blank element at the beginning 
  if (posQueue->count == 0) return;
  // Shift over
  memmove(posQueue->values, posQueue->values + 1, sizeof(Pos) * --posQueue->count);
  // Clear the leftover last el
  posQueue->values[posQueue->count] = (Pos) { 0 };
}

void freePosQueue(PosQueue* posQueue) {
  free(posQueue->values);
  posQueue->values = NULL;
  posQueue->count = 0;
  posQueue->capacity = 0;
}