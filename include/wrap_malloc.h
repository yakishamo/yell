#ifndef WRAP_MALLOC_H
#define WRAP_MALLOC_H

void *xmalloc(size_t size);
void *xrealloc(void *ptr, size_t size);
char *xstrdup(char *str);

#endif /* WRAP_MALLOC_H */
