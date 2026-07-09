#ifndef PARSE_H
#define PARSE_H

typedef struct {
  char **argv;
  int argc;
  int argv_capacity;
  char *infile;
  char *outfile;
  int append;
} Command;

typedef struct {
  Command **coms;
  int size;
  int capacity;
} Pipeline;

Pipeline *parse(char *str);
void free_pipe(Pipeline *pipe);

#endif /* PARSE_H */
