#ifndef PARSE_H
#define PARSE_H

typedef struct Command {
  char **argv;
  int argc;
  int argv_capacity;
  char *infile;
  char *outfile;
  int append;
} Command;

Command *parse_command(char *str);
void free_command(Command *com);

#endif /* PARSE_H */
