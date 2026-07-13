CC ?= gcc
CFLAGS ?= -std=c17 -O2 -Wall -Wextra -Wpedantic
SRC = src/main.c src/index.c

indexcraft: $(SRC)
	$(CC) $(CFLAGS) -o $@ $(SRC) -lm

clean:
	rm -f indexcraft indexcraft.exe

.PHONY: clean

