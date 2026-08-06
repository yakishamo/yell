#ifndef LINE_H
#define LINE_H

typedef struct line_buffer *line_buffer;

void lb_init(line_buffer *lb);
int lb_add_char(line_buffer lb, unsigned char c);
int lb_del_char(line_buffer lb);
int lb_move_cursor(line_buffer lb, int offset);
char *lb_get_line(line_buffer lb);
char *lb_release(line_buffer *lb);
void lb_free(line_buffer *lb);

#endif /* LINE_H */
