CC = gcc
CFLAGS = -D_DEFAULT_SOURCE -std=c11 -Wall -Wextra

all:
	$(CC) $(CFLAGS) CMA.c test.c -o test

clean:
	rm -f test *.o