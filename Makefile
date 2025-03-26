# Makefile

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -O3
SRC_DIR = src
EXEC = magic_test

SRC = main.c $(SRC_DIR)/magic.c

.PHONY: all clean

all: $(EXEC)

$(EXEC):
	$(CC) $(CFLAGS) $(SRC) -I$(SRC_DIR) -o $@

clean:
	rm -f $(EXEC)
