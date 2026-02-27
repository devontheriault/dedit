#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

#include "window.h"

//NOTE: Remove printf when there's a proper error system
void get_terminal_size(Window *win)
{
  struct winsize w;
  if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0){
    win->height = w.ws_row - 1;
    win->width = w.ws_col;
  }else{
    printf("ioctl: get win size error");
  }
}
