CC=gcc
FLAGS=-g -Wall
EXTRA=-fopenmp

I: sudoku-omp.o  board.o
	$(CC) -o $@ $^ $(FLAGS) $(EXTRA)

sudoku-omp.o: sudoku-omp.c board.h
	$(CC) -c $^ $(FLAGS) $(EXTRA)

board.o: board.c
	$(CC) -c $^ $(FLAGS) $(EXTRA)

clean:
	rm -f *.*o *.h.gch I
