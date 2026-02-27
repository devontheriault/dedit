#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include "ll_buffer.h"
#include "renderer.h"
#include "input_handler.h"

//TODO: It's time to move onto proper window rendering
// Choose the file to open
// Add CTRL-S to save the file

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

int main(int argc, char *argv[])
{
  altscr();
  clrscr();

  struct termios default_term;
  tcgetattr(STDIN_FILENO, &default_term);

  set_ncan_term();

  Buffer *buf = buffer_create();
  if(!buf){
    printf("Failed to create buffer");
    return 1;
  }
  buf->filename = NULL;

  if(argc >= 2){
    char *filename = argv[1];
    buf->filename = filename;
    buffer_load_from_file(buf, filename);
  }

  render(buf);

  int running = 1;
  char c;
  while(read(STDIN_FILENO, &c, 1) == 1 && running){
    running = handle_input(buf, c);
    if(!running) break;

    render(buf);
  }

  rest_term(&default_term);
  hmscr();

  printf("%d: %s | %s\n", buf->cursor_row, buf->head->data, buf->tail->data);

  return 0;
}
