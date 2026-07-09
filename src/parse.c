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
  com->argv[0] = NULL;
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
  if(com->argc + 1 == com->argv_capacity) {
    extend_argv(com);
  }
  int size = strlen(str) + 1;
  com->argv[com->argc] = xmalloc(sizeof(char) * size);
  strcpy(com->argv[com->argc], str);
  com->argc++;
  com->argv[com->argc] = NULL;
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

void free_command(Command *com) {
  if(!com) {
    return;
  }
  for(int i = 0; i < com->argc; i++) {
    free(com->argv[i]);
  }
  free(com->infile);
  free(com->outfile);
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
      if(com->argc == 0) {
        fprintf(stderr, "yell: syntax error near unexpected token `|'\n");
        free_command(com);
        free_pipe(pl);
        return NULL;
      }
      add_com(pl, com);
      com = init_command();
      continue;
    } else if (strcmp(tok, ">") == 0) {
      if(!(tok = strtok(NULL, TOK_DELIM))) {
        fprintf(stderr, "yell: syntax error: expected filename after `>'\n");
        free_command(com);
        free_pipe(pl);
        return NULL;
      }
      com->outfile = xstrdup(tok);
      com->append = 0;
    } else if(strcmp(tok, ">>") == 0) {
      if(!(tok = strtok(NULL, TOK_DELIM))) {
        fprintf(stderr, "yell: syntax error: expected filename after `>>'\n");
        free_command(com);
        free_pipe(pl);
        return NULL;
      }
      com->outfile = xstrdup(tok);
      com->append = 1;
    } else if(strcmp(tok, "<") == 0) {
      if(!(tok = strtok(NULL, TOK_DELIM))) {
        fprintf(stderr, "yell: syntax error: expected filename after `<'\n");
        free_command(com);
        free_pipe(pl);
        return NULL;
      }
      com->infile = xstrdup(tok);
    } else {
      add_tok(com, tok);
    }
  }
  if(com->argc == 0) {
    free_command(com);
    if(pl->size == 0) {
      free_pipe(pl);
      return NULL;
    }
    fprintf(stderr, "yell: syntax error near unexpected token `|'\n");
    free_pipe(pl);
    return NULL;
  }
  add_com(pl, com);

  return pl;

}
