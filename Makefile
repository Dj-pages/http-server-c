CC = gcc
CFLAGS = -Wall -Wextra -g

server: src/server.c
	$(CC) $(CFLAGS) -o server src/server.c

clean:
	rm -f server

.PHONY: clean
