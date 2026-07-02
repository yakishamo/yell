#include <stdio.h>

int main() {
  char input[100];
  while(1) {
    printf("$ ");
    if (fgets(input, sizeof(input), stdin) == NULL) {
      printf("\n");
      break;
    }
  }
}
