#ifndef WINDOW_H
#define WINDOW_H

typedef struct Window {
  int height;
  int width;
} Window;

void get_terminal_size(Window *win);
 
#endif // WINDOW_H
