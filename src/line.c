#include <string.h>
#include <stdlib.h>

#include "line.h"
#include "wrap_malloc.h"
#include "macros.h"

#define LINE_SIZE 128

struct line_buffer {
  char *line;
  int capacity;
  int line_size;
  int cursor;
};

void lb_init(line_buffer *lb) {
  *lb = xmalloc(sizeof(struct line_buffer));
  (*lb)->line = xmalloc(sizeof(char) * LINE_SIZE);
  (*lb)->capacity = LINE_SIZE;
  (*lb)->line_size = 0;
  (*lb)->cursor = 0;
}

void lb_realloc(line_buffer lb) {
  lb->capacity += LINE_SIZE;
  lb->line = xrealloc(lb->line, sizeof(char) * lb->capacity);
}

// returns num of char added
int lb_add_char(line_buffer lb, unsigned char c) {
  if(lb->capacity == lb->line_size) {
    lb_realloc(lb);
  }
  memmove(
      lb->line + lb->cursor + 1,
      lb->line + lb->cursor,
      (size_t)(lb->line_size - lb->cursor)
    );
  lb->line[lb->cursor] = (char)c;
  lb->cursor++;
  lb->line_size++;
  return 1;
}

// returns num of char deleted
int lb_del_char(line_buffer lb) {
  if(lb->cursor == 0) return 0;
  memmove(
      lb->line + lb->cursor - 1,
      lb->line + lb->cursor,
      (size_t)(lb->line_size - lb->cursor)
    );
  lb->cursor--;
  lb->line_size--;
  return 1;
}

int lb_move_cursor(line_buffer lb, int offset) {
  if(offset == 0) {
    return 0;
  } else if (offset > 0) {
    int move = MIN(lb->line_size - lb->cursor, offset);
    lb->cursor += move;
    return move;
  } else if (offset < 0) {
    int move = MAX(-lb->cursor, offset);
    lb->cursor += move;
    return move;
  }
  return 0; // never reach
}

char *lb_get_line(line_buffer lb) {
  return lb->line;
}

int lb_get_cursor(line_buffer lb) {
  return lb->cursor;
}

int lb_get_line_size(line_buffer lb) {
  return lb->line_size;
}

char *lb_release(line_buffer *lb) {
  char *line = (*lb)->line;
  free(*lb);
  *lb = NULL;
  return line;
}

void lb_free(line_buffer *lb) {
  if(lb == NULL || *lb == NULL) {
    return;
  }
  free((*lb)->line);
  free(*lb);
  *lb = NULL;
} 
