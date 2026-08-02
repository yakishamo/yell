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

// print prompt with $HOME
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

#define HISTORY_FILENAME ".yell_history"
// create full path to history and open.
// returns file pointer.
static FILE *open_history() {
  char *home_dir = getenv("HOME");
  int home_len = strlen(home_dir);
  int history_len = strlen(HISTORY_FILENAME);

  // $HOME + '/' + HISTORY_FILESIZE + '\0'
  char *file_name = malloc(sizeof(char)*(home_len+1+history_len+1));
  strcpy(file_name, home_dir);
  strcat(file_name, "/");
  strcat(file_name, HISTORY_FILENAME);

  FILE *fp = fopen(file_name, "a+");
  if(!fp) {
    perror("fopen");
  }
  free(file_name); 
  return fp;
}

FILE *history_fp = NULL;
void add_history(char *str) {
  if(str[0] == '\0') {
    return;
  }
  if(history_fp == NULL) {
    history_fp = open_history();
    if(!history_fp) {
      fprintf(stderr, "add_history failed.");
      return;
    }
  }

  fprintf(history_fp, "%s", str);
}

void close_history() {
  if(history_fp)
    fclose(history_fp);
}
