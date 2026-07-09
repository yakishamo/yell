#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

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

static int validate_redirection(Pipeline *pl) {
  for(int i = 0; i < pl->size; i++) {
    Command *com = pl->coms[i];

    if(com->infile != NULL && i != 0) {
      fprintf(stderr,"yell: input redirection is allowed only for first command\n");
      return -1;
    }

    if(com->outfile != NULL && i != pl->size - 1) {
      fprintf(stderr, "yell: output redirection is allowed only for last command\n");
      return -1;
    }
  }
  return 0;
}

void setup_redirection(Command *com) {
  if(com ->infile != NULL) {
    int fd = open(com->infile, O_RDONLY);
    if(fd == -1) {
      perror(com->infile);
      exit(1);
    }

    if(dup2(fd, STDIN_FILENO) == -1) {
      perror("dup2");
      close(fd);
      exit(1);
    }

    close (fd);
  }
  if(com->outfile != NULL) {
    int flags = O_WRONLY | O_CREAT;

    if(com->append) {
      flags |= O_APPEND;
    } else {
      flags |= O_TRUNC;
    }

    int fd = open(com->outfile, flags, 0644);
    if(fd == -1) {
      perror(com->outfile);
      exit(1);
    }

    if(dup2(fd, STDOUT_FILENO) == -1) {
      perror("dup2");
      close(fd);
      exit(1);
    }

    close(fd);
  }
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

int execute_pipeline(Pipeline *pl) {
  if(validate_redirection(pl) != 0) {
    return 1;
  }
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
