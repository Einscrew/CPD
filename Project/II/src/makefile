CC=mpicc
FLAGS=-g -Wall
EXTRA=-fopenmp
FILE=2x2.txt
P=2

NAME=sudoku-mpi

$(NAME): $(NAME).o  board.o
	$(CC) -o $@ $^ $(FLAGS) $(EXTRA)

$(NAME).o: $(NAME).c board.h
	$(CC) -c $^ $(FLAGS) $(EXTRA)

board.o: board.c
	$(CC) -c $^ $(FLAGS) $(EXTRA)

clean:
	rm -f *.*o *.h.gch $(NAME)

run:
	mpirun -np $(P) ./$(NAME) ../tests/$(FILE)