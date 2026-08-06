#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "line.h"

static void assert_line_equals(
  line_buffer lb,
  const char *expected,
  int expected_size,
  int expected_cursor
) {
  assert(lb_get_line_size(lb) == expected_size);
  assert(lb_get_cursor(lb) == expected_cursor);

  /*
   * 編集中のlineは '\0' 終端されていないため、
   * strcmp()ではなくmemcmp()を使う。
   */
  assert(memcmp(
    lb_get_line(lb),
    expected,
    (size_t)expected_size
  ) == 0);
}

static void test_init(void) {
  line_buffer lb;

  lb_init(&lb);

  assert(lb_get_line_size(lb) == 0);
  assert(lb_get_cursor(lb) == 0);

  lb_free(&lb);
}

static void test_append_char(void) {
  line_buffer lb;

  lb_init(&lb);

  assert(lb_add_char(lb, 'a') == 1);
  assert(lb_add_char(lb, 'b') == 1);
  assert(lb_add_char(lb, 'c') == 1);

  assert_line_equals(lb, "abc", 3, 3);

  lb_free(&lb);
}

static void test_insert_char(void) {
  line_buffer lb;

  lb_init(&lb);

  lb_add_char(lb, 'a');
  lb_add_char(lb, 'b');
  lb_add_char(lb, 'd');

  assert(lb_move_cursor(lb, -1) == -1);
  assert(lb_add_char(lb, 'c') == 1);

  assert_line_equals(lb, "abcd", 4, 3);

  lb_free(&lb);
}

static void test_delete_at_end(void) {
  line_buffer lb;

  lb_init(&lb);

  lb_add_char(lb, 'a');
  lb_add_char(lb, 'b');
  lb_add_char(lb, 'c');

  assert(lb_del_char(lb) == 1);

  assert_line_equals(lb, "ab", 2, 2);

  lb_free(&lb);
}

static void test_delete_in_middle(void) {
  line_buffer lb;

  lb_init(&lb);

  lb_add_char(lb, 'a');
  lb_add_char(lb, 'b');
  lb_add_char(lb, 'X');
  lb_add_char(lb, 'c');
  lb_add_char(lb, 'd');

  /*
   * abXcd|
   *      ↓ 2文字左へ
   * abX|cd
   *
   * BackspaceでXを削除する。
   */
  assert(lb_move_cursor(lb, -2) == -2);
  assert(lb_del_char(lb) == 1);

  assert_line_equals(lb, "abcd", 4, 2);

  lb_free(&lb);
}

static void test_delete_at_beginning(void) {
  line_buffer lb;

  lb_init(&lb);

  lb_add_char(lb, 'a');
  lb_add_char(lb, 'b');

  assert(lb_move_cursor(lb, -2) == -2);

  assert(lb_del_char(lb) == 0);
  assert_line_equals(lb, "ab", 2, 0);

  lb_free(&lb);
}

static void test_cursor_bounds(void) {
  line_buffer lb;

  lb_init(&lb);

  lb_add_char(lb, 'a');
  lb_add_char(lb, 'b');
  lb_add_char(lb, 'c');

  assert(lb_move_cursor(lb, -100) == -3);
  assert(lb_get_cursor(lb) == 0);

  assert(lb_move_cursor(lb, 100) == 3);
  assert(lb_get_cursor(lb) == 3);

  lb_free(&lb);
}

static void test_reallocation(void) {
  line_buffer lb;

  lb_init(&lb);

  /*
   * 初期容量128を超えて追加し、
   * realloc後も内容が壊れないことを確認する。
   */
  for(int i = 0; i < 300; i++) {
    assert(lb_add_char(lb, (unsigned char)('a' + i % 26)) == 1);
  }

  assert(lb_get_line_size(lb) == 300);
  assert(lb_get_cursor(lb) == 300);

  for(int i = 0; i < 300; i++) {
    assert(
      lb_get_line(lb)[i] ==
      (char)('a' + i % 26)
    );
  }

  lb_free(&lb);
}

int main(void) {
  test_init();
  test_append_char();
  test_insert_char();
  test_delete_at_end();
  test_delete_in_middle();
  test_delete_at_beginning();
  test_cursor_bounds();
  test_reallocation();

  puts("all line_buffer tests passed");
  return 0;
}
