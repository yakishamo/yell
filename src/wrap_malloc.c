#include <stdio.h>
#include <stdlib.h>

#include "wrap_malloc.h"

void *xmalloc(size_t size) {
  void *p = malloc(size);
  if(!p) {
    perror("malloc");
    exit(1);
  }
  return p;
}

void *xrealloc(void* ptr, size_t size) {
  void *p = realloc(ptr, size);
  if(!p) {
    perror("realloc");
    exit(2);
  }
  return p;
}
