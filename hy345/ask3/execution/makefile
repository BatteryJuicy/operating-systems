CC = gcc
TARGET = test

all: $(TARGET)

$(TARGET): test.c
	$(CC) $< -o $@

clean:
	rm -f $(TARGET)

.PHONY: all clean
