#pragma once

#define BOARD_SIZE 30

typedef enum {
  KEY_LEFT,
  KEY_RIGHT,
  KEY_UP,
  KEY_DOWN,
  KEY_UNKNOWN,
  KEY_EXIT,
  KEY_END,
  KEY_Y,
  KEY_N
} KeyType;

int initializeInput(int* readPipe);
KeyType getChar(int cancelPipe, int endPipe);

void clearScreen();
void drawBoard();
void deinitBoard();