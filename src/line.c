#include <string.h>
#include <stdlib.h>

#include "line.h"
#include "wrap_malloc.h"

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
  lb->line[lb->cursor] = c;
  lb->cursor++;
  return 1;
}

// returns num of char deleted
int lb_del_char(line_buffer lb) {
  if(lb->cursor == 0) return 0;
  lb->cursor--;
  lb->line[lb->cursor] = '\0';
  return 1;
}

char *lb_get_line(line_buffer lb) {
  return lb->line;
}

void lb_free(line_buffer *lb) {
  free(*lb);
} 
