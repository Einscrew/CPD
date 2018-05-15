CC=mpicc
FLAGS=-g -Wall
FILE=2x2.txt
P=2

NAME=sudoku-mpi

$(NAME): $(NAME).o  board.o
	$(CC) -o $@ $^ $(FLAGS)

$(NAME).o: $(NAME).c board.h
	$(CC) -c $^ $(FLAGS)

board.o: board.c
	$(CC) -c $^ $(FLAGS)

clean:
	rm -f *.*o *.h.gch $(NAME)

run:
	mpirun -np $(P) ./$(NAME) ../../tests/$(FILE)
