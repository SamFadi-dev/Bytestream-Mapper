CC = gcc
INCLUDES = -Isrc
CFLAGS = -Wall -Wextra -O2 $(INCLUDES)
SRC = src
OBJS = $(SRC)/magic.o


# Compilation principale
all: test perf

# Test de validité
test: main_test.o $(OBJS)
	$(CC) $(CFLAGS) -o test main_test.o $(OBJS)

# Test de performance
perf: main_perf.o $(OBJS)
	$(CC) $(CFLAGS) -o perf main_perf.o $(OBJS)

# Compilation des fichiers objets
$(SRC)/magic.o: $(SRC)/magic.c $(SRC)/magic.h
	$(CC) $(CFLAGS) -c $(SRC)/magic.c -o $(SRC)/magic.o

main_test.o: main_test.c $(SRC)/magic.h
	$(CC) $(CFLAGS) -c main_test.c

main_perf.o: main_perf.c $(SRC)/magic.h
	$(CC) $(CFLAGS) -c main_perf.c

# Exécution des deux
run: test perf
	@echo ==== Running test program ====
	./test
	@echo ==== Running performance test ====
	./perf

# Nettoyage
clean:
	rm -f *.o $(SRC)/*.o test perf
