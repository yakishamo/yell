#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#include "execute.h"
#include "parse.h"
#include "wrap_malloc.h"

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

void setup_redirection(Command *com) {
} 

int execute_pipeline(Pipeline *pl) {
  if(pl->size == 0) {
    return 0;
  } 
  if(pl->size == 1) {
    return execute_command(pl->coms[0]);
  }
  int ncoms = pl->size;
  int (*pipes)[2] = NULL;
  pipes = xmalloc(sizeof(int[2]) * (ncoms - 1));
  for(int i = 0; i < ncoms - 1; i++) {
    if(pipe(pipes[i]) == -1) {
      perror("pipe");
      return 1;
    }
  }

  pid_t *pids = xmalloc(sizeof(pid_t) * ncoms);
  for(int i = 0; i < ncoms; i++) {
    pid_t pid = fork();
    if(pid == -1) {
      perror("fork");
      return 1;
    }

    if(pid == 0) {
      // child
      reset_child_signal();
      if(i > 0) {
        dup2(pipes[i - 1][0], STDIN_FILENO);
      } 
      if(i < ncoms - 1) {
        dup2(pipes[i][1], STDOUT_FILENO);
      }
      for(int j = 0; j < ncoms-1; j++) {
        close(pipes[j][0]);
        close(pipes[j][1]);
      }
      setup_redirection(pl->coms[i]);

      execvp(pl->coms[i]->argv[0], pl->coms[i]->argv);
      perror(pl->coms[i]->argv[0]);
      exit(127);
    }
    pids[i] = pid;
  }

  //parent
  for(int i = 0; i < ncoms-1; i++) {
    close(pipes[i][0]);
    close(pipes[i][1]);
  }

  int status = 0;
  for(int i = 0; i < ncoms; i++) {
    waitpid(pids[i], &status, 0);
  }

  return status;
}
