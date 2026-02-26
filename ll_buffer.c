#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ll_buffer.h"

Buffer *buffer_create()
{
  Buffer *buf = malloc(sizeof(Buffer));

  Line *line = malloc(sizeof(Line));
  line->data = malloc(16);
  line->len = 0;
  line->capacity = 16;
  line->prev = NULL;
  line->next = NULL;

  buf->head = line;
  buf->tail = line;
  buf->line_count = 1;
  buf->cursor_line = line;
  buf->cursor_row = 0;
  buf->cursor_col = 0;

  return buf;
}

void buffer_increase_capacity(Buffer *buf)
{
  Line *line = buf->cursor_line;
  line->capacity *= 2;
  line->data = realloc(line->data, line->capacity);
}

void buffer_concat_lines(Buffer *buf, Line *line1, Line *line2)
{
  buf->cursor_col = line1->len;
  buf->cursor_line = line1;
  for(int i = 0; i < line2->len; i++){
    buffer_insert_char(buf, line2->data[i]);
  }
}

void free_line(Line *line)
{
  free(line->data);
  free(line);
}

void buffer_delete_line(Buffer *buf)
{
  
  Line *current_line = buf->cursor_line;
  if(current_line == buf->head)return;

  Line *next = current_line->next;
  Line *prev = current_line->prev;

  if(current_line->len){
    buffer_concat_lines(buf, prev, current_line);
  }

  if(prev) prev->next = next;
  if(next) next->prev = prev;
  if(buf->tail == current_line) buf->tail = prev;
  
  buf->cursor_line = prev;
  buf->cursor_row--;
  buf->cursor_col = prev->len;
  buf->line_count--;
    
  free_line(current_line);
}

void buffer_insert_char(Buffer *buf, char c)
{
  Line *current_line = buf->cursor_line;
  if(current_line->len > current_line->capacity){
    buffer_increase_capacity(buf);
  }

  if(buf->cursor_col < current_line->len){
    memmove(&current_line->data[buf->cursor_col + 1],
            &current_line->data[buf->cursor_col],
            current_line->len - buf->cursor_col);
  }

  current_line->data[buf->cursor_col] = c;
  buf->cursor_col++;
  current_line->len++;
}

void buffer_delete_char(Buffer *buf)
{
  Line *current_line = buf->cursor_line;
  if(current_line->len == 0 || buf->cursor_col == 0){
    buffer_delete_line(buf);

    return;
  } else{
    memmove(&current_line->data[buf->cursor_col - 1],
            &current_line->data[buf->cursor_col],
            current_line->len - buf->cursor_col);
    buf->cursor_col--;
    current_line->len--;
    //buf->line_count--;
    current_line->data[current_line->len] = '\0';
  }
}

void buffer_new_line(Buffer *buf)
{
  Line *current_line = buf->cursor_line;

  Line *line = malloc(sizeof(Line));
  line->data = malloc(16);
  line->len = 0;
  line->capacity = 16;
  line->prev = current_line;

  if(current_line == buf->tail){
    buf->tail = line;
    line->next = NULL;
  }else{
    line->next = current_line->next;
    line->next->prev = line;
  }

  current_line->next = line;

  buf->line_count++;
  buf->cursor_line = line;
  buf->cursor_row++;
  buf->cursor_col = 0;
}

void buffer_move_cursor_up(Buffer *buf)
{
  if(buf->cursor_line == buf->head) return;
  
  buf->cursor_line = buf->cursor_line->prev;
  
  if(buf->cursor_line->len < buf->cursor_col){
    buf->cursor_col = buf->cursor_line->len;
  }

  buf->cursor_row--;
}

void buffer_move_cursor_down(Buffer *buf)
{
  if(buf->cursor_line == buf->tail)return;

  buf->cursor_line = buf->cursor_line->next;

  if(buf->cursor_line->len < buf->cursor_col){
     buf->cursor_col = buf->cursor_line->len;
  }

  buf->cursor_row++;
}

void buffer_move_cursor_right(Buffer *buf)
{
  if(buf->cursor_line == buf->tail && 
    buf->cursor_col == buf->cursor_line->len)return;
  
  if(buf->cursor_col == buf->cursor_line->len){
    buf->cursor_col = 0;
    buf->cursor_line = buf->cursor_line->next;
    buf->cursor_row++;
  }else{
    buf->cursor_col++;
  }
}

void buffer_move_cursor_left(Buffer *buf)
{
  if(buf->cursor_line == buf->head && buf->cursor_col == 0)return;
  if(buf->cursor_col == 0){
    buf->cursor_line = buf->cursor_line->prev;
    buf->cursor_col = buf->cursor_line->len;
    buf->cursor_row--;
  }else{
    buf->cursor_col--;
  }

}
