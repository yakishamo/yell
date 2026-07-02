TARGET   = yell

CC       = gcc
CFLAGS   = -Wall -Wextra -O2 -g -Iinclude

SRC_DIR  = src
OBJ_DIR  = obj

SRCS     = $(wildcard *.c) $(wildcard $(SRC_DIR)/*.c)
OBJS     = $(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(SRCS)))

vpath %.c $(SRC_DIR)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	@echo "Cleaning up..."
	rm -rf $(OBJ_DIR) $(TARGET)

run: all
	./$(TARGET)

.PHONY: all clean run
