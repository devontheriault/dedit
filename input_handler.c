#include <unistd.h>

#include "input_handler.h"

#define CTRL_Q 17
#define BKSPC 8
#define DEL 127

int handle_input(Buffer *buf, char c)
{

  switch(c) {
    case '\x1b': //Handle Arrows
      char seq[2];
      if (read(STDIN_FILENO, &seq[0], 1) != 1) break;
      if (read(STDIN_FILENO, &seq[1], 1) != 1) break;
      if(seq[0] == '['){
        switch(seq[1]){
          case 'A': //Up Arrow
            buffer_move_cursor_up(buf); 
            break;
          case 'B': //Down Arrow
            buffer_move_cursor_down(buf); 
            break;
          case 'C': //Right Arrow
            buffer_move_cursor_right(buf); 
            break;
          case 'D': //Left Arrow
            buffer_move_cursor_left(buf); 
            break;
        }
      }
      break;
    case CTRL_Q: //Quit
      return 0;
    case '\n':
    case '\r':
      buffer_new_line(buf, NULL, 0);
      break;
    case DEL:
    case BKSPC:
      buffer_delete_char(buf);
      break;
    default: // Handle a normal keypress
      buffer_insert_char(buf, c);
      break;
  }

  return 1;
}
