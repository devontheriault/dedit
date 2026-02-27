#ifndef WINDOW_H
#define WINDOW_H

typedef struct Window {
  int height;
  int width;

  int hoffset;
  int woffset;
} Window;

void get_terminal_size(Window *win);
 
#endif // WINDOW_H
