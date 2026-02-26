#ifndef LL_BUFFER_H
#define LL_BUFFER_H

#include <stddef.h>

typedef struct Line {
  char *data;
  size_t len;
  size_t capacity;

  struct Line *prev;
  struct Line *next;
} Line;

typedef struct Buffer {
  Line *head;
  Line *tail;

  size_t line_count;

  Line *cursor_line;
  size_t cursor_row;
  size_t cursor_col;
} Buffer;

Buffer *buffer_create(void);
void buffer_insert_char(Buffer *buf, char c);
void buffer_delete_char(Buffer *buf);
void buffer_new_line(Buffer *buf, const char *text, int len);
void buffer_move_cursor_up(Buffer *buf);
void buffer_move_cursor_down(Buffer *buf);
void buffer_move_cursor_right(Buffer *buf);
void buffer_move_cursor_left(Buffer *buf);
void buffer_to_file(Buffer *buf, const char *filename);
void buffer_load_from_file(Buffer *buf, const char *filename);
#endif // !fndef LL_BUFFER_H
