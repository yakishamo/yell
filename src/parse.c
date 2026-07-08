#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "parse.h"

#define TOK_DELIM " \t\n"
#define TOK_SIZE 32
#define ARGV_SIZE 16

Command *parse_command(char *str) {
  if(!str) {
    return NULL;
  }

  Command *com = malloc(sizeof(Command));
  if(!com) {
    perror("malloc");
    return NULL;
  }

  com->argv = malloc(sizeof(char*) * ARGV_SIZE);
  if(!com->argv) {
    perror("malloc");
    return NULL;
  }
  com->argc = 0;
  com->argv_capacity = ARGV_SIZE;
  com->infile = NULL;
  com->outfile = NULL;
  com->append = 0;
  
  char *tok_str = strtok(str, TOK_DELIM);
  if(tok_str == NULL) {
    return com;
  }
  com->argv[0] = malloc(sizeof(char) * (strlen(tok_str) + 1));
  if(!com->argv[0]) {
    perror("malloc");
    free(com);
    return NULL;
  }
  strcpy(com->argv[0], tok_str);
  com->argc++;

  while(1) {
    for(int i = com->argc; i < com->argv_capacity; i++) {
      tok_str = strtok(NULL, TOK_DELIM);
      if(!tok_str) {
        return com;
      }
      com->argv[i] = malloc(sizeof(char) * (strlen(tok_str) + 1));
      strcpy(com->argv[i], tok_str);
      com->argc++;
    }
    com->argv_capacity += TOK_SIZE;
    void *tmp = realloc(com->argv, sizeof(char*) * com->argv_capacity);
    if(!tmp) {
      free_command(com);
      return NULL;
    }
    com->argv = tmp;
  }
}

void free_command(Command *com) {
  if(!com) {
    return;
  }
  for(int i = 0; i < com->argc; i++) {
    free(com->argv[i]);
  }
  free(com->argv);
  free(com);
}
