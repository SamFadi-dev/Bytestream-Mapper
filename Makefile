# Makefile

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -O3

SRC = magic.c main.c
OBJ = $(SRC:.c=.o)
EXEC = test_magic

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) $(EXEC)
