CC = gcc
CFLAGS = -Wall -Wextra -Wshadow -Wconversion -Wuninitialized -O2
TARGET = hy345sh.out

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

#source files names seperated by space
SRCS = $(wildcard $(SRC_DIR)/*.c)
#object files names seperated by space
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(TARGET)

#link and create executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

#create object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: clean all
