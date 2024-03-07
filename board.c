#include "board.h"

#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

static void sig_handler(int num, siginfo_t* info, void* ucontext) {
  static int pipefd = 0;
  if (info->si_signo == SIGUSR1) pipefd = info->si_value.sival_int;
  else if (info->si_signo == SIGINT && pipefd != 0) {
    // Close pipefd to notify the main thread
    close(pipefd);
  }
}

int initializeInput(int* readPipe) {
  // SETUP SIGNAL
  struct sigaction termination_action;
  termination_action.sa_sigaction = sig_handler;
  termination_action.sa_flags = SA_SIGINFO; // use sigaction instead of handler
  sigaction(SIGINT, &termination_action, NULL);
  sigaction(SIGUSR1, &termination_action, NULL);

  // SETUP PIPE
  int pipefd[2];
  if (pipe(pipefd)) {
    *readPipe = 0;
    return -1;
  }

  // SEND PIPE TO SIGNAL
  sigval_t value;
  value.sival_int = pipefd[1]; // write end
  sigqueue(getpid(), SIGUSR1, value);

  *readPipe = pipefd[0];

  // DISABLE CANONICAL MODE
  struct termios terminal;
  tcgetattr(STDIN_FILENO, &terminal);
  terminal.c_lflag &= ~(ICANON | ECHO); // disable ICANON and ECHO
  tcsetattr(STDIN_FILENO, TCSANOW, &terminal);

  // HIDE CURSOR
  printf("\e[?25l");

  return 0;
}

KeyType getChar(int cancelPipe) {
  static fd_set read_fds;
  static char inputChar;

  FD_ZERO(&read_fds); // clear out the fd set
  FD_SET(STDIN_FILENO, &read_fds); // add stdin
  FD_SET(cancelPipe, &read_fds); // add the cancellation pipe

  select(cancelPipe + 1, &read_fds, NULL, NULL, NULL);

  // If the cancel pipe is set, exit
  if (FD_ISSET(cancelPipe, &read_fds)) return KEY_EXIT;

  if (FD_ISSET(STDIN_FILENO, &read_fds) && read(STDIN_FILENO, &inputChar, sizeof(inputChar))) {
    if (inputChar == 'q') return KEY_EXIT;
    else if (inputChar == 'y') return KEY_Y;
    else if (inputChar == 'n') return KEY_N;
    else if (inputChar == '\e') {
      // Escape sequence
      if (getchar() == '[') {
        // CSI
        switch (getchar()) {
          case 'A': return KEY_UP;
          case 'B': return KEY_DOWN;
          case 'C': return KEY_RIGHT;
          case 'D': return KEY_LEFT;
          default: break;
        }
      }
    }
  }

  return KEY_UNKNOWN;
}

void clearScreen() {
  printf("\e[1;1H\e[2J");
}

void drawBoard() {
  if (BOARD_SIZE < 0) return;

  clearScreen();

  printf("┏");
  for (int i = 0; i < BOARD_SIZE; i++) {
    printf("━");
  }
  printf("┓\n");
  for (int i = 0; i < BOARD_SIZE; i++) {
    printf("┃\e[%dC┃\n", BOARD_SIZE);
  }
  printf("┗");
  for (int i = 0; i < BOARD_SIZE; i++) {
    printf("━");
  }
  printf("┛\n\n");
}

void deinitBoard() {
  struct termios terminal;
  tcgetattr(STDIN_FILENO, &terminal);
  terminal.c_lflag |= (ICANON | ECHO); // enable canonical and echo modes
  tcsetattr(STDIN_FILENO, TCSANOW, &terminal);
  // Show curor
  printf("\e[?25h");
}