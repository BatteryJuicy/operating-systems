CC = gcc

all: test1 test2 test3 test4 test5

test1: test1.c
	$(CC) $< -o test1
test2: test1.c
	$(CC) $< -o test2
test3: test1.c
	$(CC) $< -o test3
test4: test1.c
	$(CC) $< -o test4
test5: test5.c
	$(CC) $< -o test5

clean:
	rm -f $(TARGET)

.PHONY: all clean
