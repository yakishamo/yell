#include <stdio.h>
#include <stdlib.h>

#include "readline.h"
#include "parse.h"

int main() {
  char *input;
  while(1) {
    printf("$ ");
    if ((input = readline()) == NULL) {
      printf("\n");
      break;
    }
    Command *com = parse_command(input);
    free_command(com);
    free(input);
  }
}
