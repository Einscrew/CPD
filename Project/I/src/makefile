CC=gcc
FLAGS=-g -Wall
EXTRA=-fopenmp

sudoku-serial: sudoku-serial.o  board.o utils.o
	$(CC) -o $@ $^ $(FLAGS) $(EXTRA)

sudoku-serial.o: sudoku-serial.c board.h
	$(CC) -c $^ $(FLAGS) $(EXTRA)

board.o: board.c utils.h
	$(CC) -c $^ $(FLAGS) $(EXTRA)

utils.o: utils.c
	$(CC) -c $^ $(FLAGS) $(EXTRA)

clean:
	rm -f *.*o *.h.gch I
