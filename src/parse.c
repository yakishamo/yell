#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "parse.h"
#include "wrap_malloc.h"

#define TOK_DELIM " \t\n"
#define TOK_SIZE 32
#define ARGV_SIZE 16
#define PIPE_SIZE 8

static Command *init_command() {
  Command *com = xmalloc(sizeof(Command));
  com->argv = xmalloc(sizeof(char*) * ARGV_SIZE);
  com->argc = 0;
  com->argv_capacity = ARGV_SIZE;
  com->infile = NULL;
  com->outfile = NULL;
  com->append = 0;
  
  return com;
}

static void extend_argv(Command *com) {
  com->argv_capacity += ARGV_SIZE;
  com->argv = xrealloc(com->argv, sizeof(char*) * com->argv_capacity);
}

static void add_tok(Command *com, char *str) {
  if(str == NULL) {
    com->argv[com->argc] = NULL;
    com->argc++;
  } 
  if(com->argc == com->argv_capacity) {
    extend_argv(com);
  }
  int size = strlen(str) + 1;
  com->argv[com->argc] = xmalloc(sizeof(char) * size);
  strcpy(com->argv[com->argc], str);
  com->argc++;
}

static Pipeline *init_pipe() {
  Pipeline *pl = xmalloc(sizeof(Pipeline));
  pl->coms = xmalloc(sizeof(Command*) * PIPE_SIZE);
  pl->size = 0;
  pl->capacity = PIPE_SIZE;
  return pl;
}

static void extend_pipe(Pipeline *pl) {
  pl->capacity += PIPE_SIZE;
  pl->coms = xrealloc(pl->coms, sizeof(Command*) * pl->capacity);
}

static void add_com(Pipeline *pl, Command *com) {
  if(pl->size == pl->capacity) {
    extend_pipe(pl);
  }
  pl->coms[pl->size] = com;
  pl->size++;
} 

Pipeline *parse(char *str) {
  if(!str) {
    return NULL;
  }

  Pipeline *pl = init_pipe();

  char *tok = NULL;
  Command *com = init_command();
  while((tok = strtok(str, TOK_DELIM)) != NULL) {
    str = NULL;
    if(strcmp(tok, "|") == 0) {
      add_tok(com, NULL);
      add_com(pl, com);
      com = init_command();
      continue;
    } 
    add_tok(com, tok);
  }
  add_tok(com, NULL);
  add_com(pl, com);

  return pl;

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

void free_pipe(Pipeline *pl) {
  if(!pl) {
    return;
  }
  for(int i = 0; i < pl->size; i++) {
    free_command(pl->coms[i]);
  }
  free(pl->coms);
  free(pl);
}
