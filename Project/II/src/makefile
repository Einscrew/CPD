CC=mpicc
FLAGS=-g -Wall -std=c99

NAME=sudoku-mpi

$(NAME): $(NAME).o  board.o
	$(CC) -o $@ $^ $(FLAGS)

$(NAME).o: $(NAME).c board.h
	$(CC) -c $^ $(FLAGS)

board.o: board.c board.h
	$(CC) -c $^ $(FLAGS)

clean:
	rm -f *.*o *.h.gch $(NAME)

