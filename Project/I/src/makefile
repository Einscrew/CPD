CC=gcc
FLAGS=-g -Wall
EXTRA=-fopenmp

OMP=sudoku-omp
SERIAL=sudoku-serial

all: $(SERIAL) $(OMP)
	
$(SERIAL): $(SERIAL).o  board.o
	$(CC) -o $@ $^ $(FLAGS) $(EXTRA)

$(SERIAL).o: $(SERIAL).c board.h
	$(CC) -c $^ $(FLAGS) $(EXTRA)

$(OMP): $(OMP).o  board.o
	$(CC) -o $@ $^ $(FLAGS) $(EXTRA)

$(OMP).o: $(OMP).c board.h
	$(CC) -c $^ $(FLAGS) $(EXTRA)

board.o: board.c
	$(CC) -c $^ $(FLAGS) $(EXTRA)

clean:
	rm -f *.*o *.h.gch $(OMP) $(SERIAL)