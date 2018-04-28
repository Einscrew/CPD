CC=gcc
FLAGS=-g -Wall
EXTRA=-fopenmp

NAME=sudoku-serial

$(NAME): $(NAME).o  board.o
	$(CC) -o $@ $^ $(FLAGS) $(EXTRA)

$(NAME).o: $(NAME).c board.h
	$(CC) -c $^ $(FLAGS) $(EXTRA)

board.o: board.c
	$(CC) -c $^ $(FLAGS) $(EXTRA)

clean:
	rm -f *.*o *.h.gch $(NAME)