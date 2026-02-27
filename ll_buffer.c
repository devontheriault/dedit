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
  size_t prev_len = prev->len;

  if(current_line->len){
    buffer_concat_lines(buf, prev, current_line);
  }

  if(prev) prev->next = next;
  if(next) next->prev = prev;
  if(buf->tail == current_line) buf->tail = prev;
  
  buf->cursor_line = prev;
  buf->cursor_row--;
  buf->cursor_col = prev_len;
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

void buffer_new_line(Buffer *buf, const char *text, int len)
{
  Line *current_line = buf->cursor_line;
  const char *data = text;

  int chars_to_move = 0;
  if(!text){
    data = &current_line->data[buf->cursor_col];
    chars_to_move = current_line->len - buf->cursor_col;
  }else{
    chars_to_move = len;
  } 
  int capacity = chars_to_move > 16 ? chars_to_move : 16;
  
  Line *line = malloc(sizeof(Line));
  line->data = malloc(capacity);
  line->len = chars_to_move;
  line->capacity = capacity;
  line->prev = current_line;

  if(chars_to_move){
    memcpy(line->data, 
           data, 
           chars_to_move);
  }

  if(!text){
    current_line->len = buf->cursor_col;
  }

  if(current_line->len < current_line->capacity){
    current_line->data[current_line->len] = '\0';
  }

  line->next = current_line->next;
  line->prev = current_line;

  if(current_line == buf->tail){
    buf->tail = line;
  }else{
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

void buffer_to_file(Buffer *buf)
{
  FILE *fp = fopen(buf->filename, "w");
  if(!fp) return;

  Line *current = buf->head;
  while(current){
    fwrite(current->data, 1, current->len, fp);
    fputc('\n', fp);

    current = current->next;
  }

  fclose(fp);
}

void buffer_load_from_file(Buffer *buf, const char *filename)
{
  FILE *fp = fopen(filename, "r");
  if(!fp)return;

  int first_line = (buf->line_count == 1 && buf->head->len == 0);

  char temp_buffer[1024];
  while(fgets(temp_buffer, sizeof(temp_buffer), fp)){
    size_t len = strlen(temp_buffer);
    if(len > 0 && temp_buffer[len - 1] == '\n'){
      temp_buffer[len - 1] = '\0';
      len--;
    }

    if(first_line){
      if(len >= buf->head->capacity) {
        buf->head->capacity = len + 1;
        buf->head->data = realloc(buf->head->data, buf->head->capacity);
      }
      memcpy(buf->head->data, temp_buffer, len);
      buf->head->len = len;
      first_line = 0;
    }else{
      buffer_new_line(buf, temp_buffer, len);
    }
  }

  buf->cursor_line = buf->head;
  buf->cursor_row = 0;
  buf->cursor_col = 0;
}


