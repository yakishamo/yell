#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#include "execute.h"
#include "parse.h"

static void reset_child_signal(void) {
  struct sigaction sa;

  sa.sa_handler = SIG_DFL;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }
}

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
  char **argv = com->argv;
  char argc = com->argc;

  if(argc == 0) {
    return 0;
  }

  if(strcmp(argv[0], "cd") == 0){ 
    return cd(argv[1]);
  }

  if(strcmp(argv[0], "exit") == 0) {
    exit(0);
  }

  pid_t pid = fork();
  if(pid == -1) {
    perror("fork");
  } else if(pid == 0) {
    reset_child_signal();
    execvp(argv[0], argv);
    perror(argv[0]);
    exit(1);
  } else if(pid > 0) {
    int status;
    waitpid(pid, &status, 0);
  }
  return 0;
}
