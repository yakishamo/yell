#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#define READLINE_BUFSIZE 128
#define DIRNAME_SIZE 128

// handle line buffer and output
int process_input(char c, char *line, int *i) {
  switch(c) {
    // EOF
    case 0x04:
      return 1;

    // backspace
    case 0x7f:
      if(*i == 0) break;
      write(STDOUT_FILENO, "\b \b", 3);
      (*i)--;
      line[*i] = '\0';
      break;

    default:
      line[*i] = c;
      (*i)++;
      write(STDOUT_FILENO, &c, 1);
      break;
  } 
  fflush(stdout);
  return 0;
} 

char *read_line() {
  char *line = NULL;
  size_t line_size = 0;
  if(isatty(STDIN_FILENO)) {
    struct termios original;
    struct termios modified;

    tcgetattr(STDIN_FILENO, &original);
    modified = original;

    modified.c_lflag &= ~ICANON;
    modified.c_lflag &= ~ECHO;
    modified.c_cc[VMIN] = 1;
    modified.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &modified);

    char c;
    int i = 0;
    line = malloc(sizeof(char) * READLINE_BUFSIZE);
    do {
      if(read(STDIN_FILENO, &c, 1) != 1) {
        break;
      }
      if(process_input(c, line, &i) == 1) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
        free(line);
        return NULL;
      }
    } while(c != '\n');
    line[i] = '\0';

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
    return line;

  } else {
    if(getline(&line, &line_size, stdin) == -1) {
      return NULL;
    }
    return line;
  } 
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
  fflush(stdout);
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
