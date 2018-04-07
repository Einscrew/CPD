CC=gcc
FLAGS=-g -Wall
EXTRA=-fopenmp

NAME=sudoku-serial

$(NAME): sudoku-serial.o  board.o
	$(CC) -o $@ $^ $(FLAGS) $(EXTRA)

sudoku-serial.o: sudoku-serial.c board.h
	$(CC) -c $^ $(FLAGS) $(EXTRA)

board.o: board.c
	$(CC) -c $^ $(FLAGS) $(EXTRA)

clean:
	rm -f *.*o *.h.gch $(NAME)
