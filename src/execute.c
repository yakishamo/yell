#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "execute.h"
#include "parse.h"

int execute_command(Command *com) {
  char **argv = malloc(sizeof(char*) * com->size);

  for(int i = 0; i < com->size; i++) {
    argv[i] = next_token(com);
    if(!argv[i]) {
      break;
    }
  }

  pid_t pid = fork();
  if(pid == 0) {
    execvp(argv[0], argv);
    perror("yell");
    exit(1);
  } else if(pid > 0) {
    wait(NULL);
  }
  return 0;
}
