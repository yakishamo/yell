#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "parse.h"

#define TOK_DELIM " \t\n"
#define TOK_SIZE 16

void init_token(Token *tok, char *str) {
  int len = strlen(str);
  tok->size = len;
  tok->value = malloc(sizeof(char) * (len + 1));
  strcpy(tok->value, str);
}

Command *parse_command(char *str) {
  if(!str) {
    return NULL;
  }

  Command *com = malloc(sizeof(Command));
  if(!com) {
    perror("malloc");
    return NULL;
  }

  com->tokens = malloc(sizeof(Token) * TOK_SIZE);
  if(!com->tokens) {
    perror("malloc");
    return NULL;
  }
  com->size = 0;
  com->capacity = TOK_SIZE;
  com->cursor = 0;
  
  char *tok_str = strtok(str, TOK_DELIM);
  if(tok_str == NULL) {
    return com;
  }
  init_token(&com->tokens[0], tok_str);
  com->size++;

  while(1) {
    for(int i = com->size; i < com->capacity; i++) {
      tok_str = strtok(NULL, TOK_DELIM);
      if(!tok_str) {
        return com;
      }
      init_token(&com->tokens[i], tok_str);
      com->size++;
    }
    com->capacity += TOK_SIZE;
    void *tmp = realloc(com->tokens, sizeof(Token) * com->capacity);
    if(!tmp) {
      free_command(com);
      return NULL;
    }
    com->tokens = tmp;
  }
}

void free_command(Command *com) {
  if(!com) {
    return;
  }
  for(int i = 0; i < com->size; i++) {
    free(com->tokens[i].value);
  }
  free(com->tokens);
  free(com);
}

char *next_token(Command *com) {
  if(com->cursor >= com->size) {
    return NULL;
  }
  char *ret = com->tokens[com->cursor].value;
  com->cursor++;
  return ret;
}
