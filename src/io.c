#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define READLINE_BUFSIZE 128
#define DIRNAME_SIZE 128

char *readline() {
  char *line = NULL ;
  char buf[READLINE_BUFSIZE];
  int i = 1;

  while(fgets(buf, sizeof(buf), stdin) != NULL) {
    if(!line) {
      line = malloc(sizeof(char) * READLINE_BUFSIZE);
      if(!line) {
        perror("malloc");
        exit(1);
      } 
      line[0] = '\0';
    } else {
      line = realloc(line, sizeof(char) * READLINE_BUFSIZE * i);
      if(!line) {
        perror("realloc");
        exit(1);
      }
    }

    strcat(line, buf);

    if(strchr(buf, '\n') != NULL) {
      break;
    }

    i++;
  }
  return line;
}

void print_prompt() {
  char full_dir[DIRNAME_SIZE];
  char *home_dir = getenv("HOME");
  int home_len = strlen(home_dir);
  if(!getcwd(full_dir, DIRNAME_SIZE) || !home_dir) {
    printf("(getcwd failed)$ ");
    return;
  }
  if(strncmp(full_dir, home_dir, home_len) == 0) {
    printf("~%s $ ", full_dir + home_len);
  } else {
    printf("%s $ ", full_dir);
  }
  return;
}
