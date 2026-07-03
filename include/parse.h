#ifndef PARSE_H
#define PARSE_H

typedef struct Token {
  char *value;
  int size;
} Token;

typedef struct Command {
  Token *tokens;
  int size;
  int capacity;
  int cursor;
} Command;

Command *parse_command(char *str);
void free_command(Command *com);
char *next_token(Command *com);

#endif /* PARSE_H */
