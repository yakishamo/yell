#include <stdio.h>
#include <stdlib.h>

#include "readline.h"

int main() {
  char *input;
  while(1) {
    printf("$ ");
    if ((input = readline()) == NULL) {
      printf("\n");
      break;
    }
    printf("%s", input);
    free(input);
  }
}
