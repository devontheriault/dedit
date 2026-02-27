#include <stdio.h>

#include "renderer.h"

//TODO: I'm only rendering what can fit into the current window
// but you can't navigate any code that exceedes the width/height 
// of the window

//NOTE: If I wanted this to be completely correct I would need to change
// the window size whenever a signal is sent... not just when a user types

//NOTE: I should probably break up this function soon
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
  
  if(buf->cursor_row < win->hoffset){
    win->hoffset = buf->cursor_row;
  }else if(buf->cursor_row >= win->hoffset + win->height){
    win->hoffset = buf->cursor_row - win->height + 1;
  }

  int left_padding = 4;
  if (buf->cursor_col < win->woffset + left_padding) {
    win->woffset = buf->cursor_col - left_padding;
  } 
  else if (buf->cursor_col >= win->woffset + win->width) {
    win->woffset = buf->cursor_col - win->width + 1;
  }

  if(win->woffset < 0) win->woffset = 0;

  while(current != NULL){
    if(i < win->hoffset){
      current = current->next;
      i++;
      continue;
    }

    if(i > win->hoffset + win->height){
      break;
    }
    
    if(win->woffset > current->len){
      printf("\n");
    }else{
      printf("%.*s\n", win->width, &current->data[win->woffset]);
    }

    current = current->next;
    i++;
  }
  
  // [?25h -- This shows the cursor
  printf("\x1b[%d;%dH\x1b[?25h", 
         (buf->cursor_row - win->hoffset) + 1, 
         (buf->cursor_col - win->woffset) + 1);
  fflush(stdout);
}
