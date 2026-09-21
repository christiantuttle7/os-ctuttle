CC = cc
CFLAGS = -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Werror -O0 -g

all: getuid

getuid: getuid.c
	$(CC) $(CFLAGS) -o $@ $<

run: getuid
	./getuid

disasm: getuid
	objdump -d --disassemble=raw_getuid getuid

clean:
	rm -f getuid

.PHONY: all run disasm clean
