#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include "ll_buffer.h"
#include "renderer.h"
#include "input_handler.h"

// TODO: Handle deleting a line when there is data on the current line
// Handle moving around the screen with arrow keys
// FIX THE UP AND DOWN ARROWS - CURRENTLY SHIFTS ROWS DOWN??
//
// NOTE: Maybe I'll just start with a linked list to keep things simple.
// I'm going to be building a line by line editor, then I just have to deal
// with in line edits and change the pointers. This should be fine for now
// This is most likely how I'm to do any text handling behind the scenes 
// and without it I shouldn't start grappling with any text display logic

void clrscr() 
{
  // \x1b -- Escape code
  // [2J - clears the screen
  // [1;1H sets cursor to row 1 col 1 
  fputs("\x1b[1;1H\x1b[2J", stdout);
  fflush(stdout);
}

void altscr()
{
  // Sets terminal to alternate terminal
  fputs("\x1b[?1049h", stdout);
  fflush(stdout);
}

void hmscr()
{
  // Sets terminal to home terminal
  fputs("\x1b[?1049l", stdout);
  fflush(stdout);
}

void set_ncan_term()
{
  struct termios t;
  tcgetattr(STDIN_FILENO, &t);

  t.c_lflag &= ~ICANON;
  t.c_lflag &= ~ECHO;

  t.c_cc[VMIN] = 1;
  t.c_cc[VTIME] = 0;

  // Disable CTRL-S/CTRL-Q control flow
  t.c_iflag &= ~(IXON | ICRNL);

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &t);
}

void rest_term(struct termios *t)
{
  tcsetattr(STDIN_FILENO, TCSANOW, t);
}

int main()
{
  altscr();
  clrscr();

  struct termios default_term;
  tcgetattr(STDIN_FILENO, &default_term);

  set_ncan_term();

  Buffer *buf = buffer_create();
  if(!buf){
    printf("Failed to create buffer");
  }

  int running = 1;
  char c;
  while(read(STDIN_FILENO, &c, 1) == 1 && running){
    running = handle_input(buf, c);
    if(!running) break;

    render(buf);
  }

  rest_term(&default_term);
  hmscr();

  printf("%d: %s\n", buf->cursor_row, buf->cursor_line->data);

  return 0;
}
