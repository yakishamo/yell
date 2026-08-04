#ifndef READLINE_H
#define READLINE_H

char *read_line();
void print_prompt();

void add_history(char *str);
void close_history();

#endif /* READLINE_H */
