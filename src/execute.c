#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "execute.h"
#include "parse.h"

int cd(char *path) {
  if(!path) {
    path = getenv("HOME");
    if(!path) {
      fprintf(stderr, "cd: $HOME not set");
      return 0;
    }
  }
  if (chdir(path) != 0) {
    perror("cd");
  }
  return 0;
}

int execute_command(Command *com) {
  char **argv = malloc(sizeof(char*) * com->size);

  int i = 0;
  while(1) {
    argv[i] = next_token(com);
    if(!argv[i]) {
      break;
    }
    i++;
  }

  if(strcmp(argv[0], "cd") == 0){ 
    return cd(argv[1]);
  }

  pid_t pid = fork();
  if(pid == 0) {
    execvp(argv[0], argv);
    perror(argv[0]);
    exit(1);
  } else if(pid > 0) {
    wait(NULL);
  }
  return 0;
}
