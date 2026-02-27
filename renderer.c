#include <stdio.h>

#include "renderer.h"

//TODO: Right now I'm rendering everything no matter what which causes
// some issues when you start exceeding view width/height.
// I should only render what can fit into the display window.

void render(Buffer *buf, Window *win)
{
  get_terminal_size(win);

  // [?25l -- This hides the cursor     
  // [H -- This moves the cursor to pos 1 1
  fputs("\x1b[?25l\x1b[H", stdout);
  //TODO: Stop clearing full screen and only clear what's needed
  fputs("\x1b[2J", stdout);
  
  Line *current = buf->head;
  while(current != NULL){
    printf("%.*s\n", (int)current->len, current->data);
    current = current->next;
  }
  
  // [?25h -- This shows the cursor
  printf("\x1b[%d;%dH\x1b[?25h", buf->cursor_row + 1, buf->cursor_col + 1);
  fflush(stdout);
}
