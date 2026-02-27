#include <stdio.h>

#include "renderer.h"

//TODO: Right now I'm rendering everything no matter what which causes
// some issues when you start exceeding view width/height.
// I should only render what can fit into the display window.

//NOTE: If I wanted this to be completely correct I would need to change
// the window size whenever a signal is sent... not just when a user types

void render(Buffer *buf, Window *win)
{
  get_terminal_size(win);

  // [?25l -- This hides the cursor     
  // [H -- This moves the cursor to pos 1 1
  fputs("\x1b[?25l\x1b[H", stdout);
  //TODO: Stop clearing full screen and only clear what's needed
  fputs("\x1b[2J", stdout);
  
  Line *current = buf->head;
  int i = 1;
  while(current != NULL){
    if(i < win->height){
      printf("%.*s\n", win->width, current->data);
      //printf("%.*s\n", (int)current->len, current->data);
    }else{
      break;
    }

    current = current->next;
    i++;
  }
  
  // [?25h -- This shows the cursor
  printf("\x1b[%d;%dH\x1b[?25h", buf->cursor_row + 1, buf->cursor_col + 1);
  fflush(stdout);
}
