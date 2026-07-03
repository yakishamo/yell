#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "io.h"
#include "parse.h"
#include "execute.h"

#define CURDIR_SIZE 20

int main() {
  char *input;
  char cur_dir[CURDIR_SIZE];
  while(1) {
    if(!getcwd(cur_dir, CURDIR_SIZE)) {
      printf("(getcwd failed) $ ");
    } else {
      printf("%s $ ", cur_dir);
    }
    if ((input = readline()) == NULL) {
      printf("interrupt\n");
      break;
    }

    Command *com = parse_command(input);
    execute_command(com);

    free_command(com);
    free(input);
  }
}
