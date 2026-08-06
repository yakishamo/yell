TARGET   = yell

CC       = gcc
CFLAGS   = -Wall -Wextra -Wno-unused-result -O2 -g -Iinclude

SRC_DIR  = src
OBJ_DIR  = obj

SRCS     = $(wildcard *.c) $(wildcard $(SRC_DIR)/*.c)
OBJS     = $(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(SRCS)))

TEST_TARGET = test_line
TEST_SRC    = tests/test_line.c
TEST_OBJS   = obj/test_line.o obj/line.o obj/wrap_malloc.o

vpath %.c $(SRC_DIR)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/test_line.o: $(TEST_SRC) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning up..."
	rm -rf $(OBJ_DIR) $(TARGET)

run: all
	./$(TARGET)

.PHONY: all clean run
