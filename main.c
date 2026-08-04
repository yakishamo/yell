#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "io.h"
#include "parse.h"
#include "execute.h"

#define CURDIR_SIZE 20

static void setup_shell_signal(void) {
  struct sigaction sa;
  sa.sa_handler = SIG_IGN;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }
}

int main() {
  char *input;

  setup_shell_signal();

  while(1) {
    print_prompt();
    if ((input = read_line()) == NULL) {
      printf("interrupt\n");
      break;
    }

    add_history(input);

    Pipeline *pl = parse(input);
    if(pl) {
      execute_pipeline(pl);
      free_pipe(pl);
    }

    free(input);
  }

  close_history();
}
