CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -pthread

tickets: tickets.c
	$(CC) $(CFLAGS) tickets.c -o tickets

run: tickets
	./tickets

clean:
	rm -f tickets

.PHONY: run clean
