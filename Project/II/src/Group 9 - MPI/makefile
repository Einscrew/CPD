CC=mpicc
FLAGS=-g -Wall -std=c99

SERIAL=sudoku-serial
MPI=sudoku-mpi

all: $(MPI) $(SERIAL)

$(SERIAL): $(SERIAL).o board.o
	$(CC) -o $(CC) -o $@ $^ $(FLAGS)

$(SERIAL).o: $(SERIAL).c board.h
	$(CC) -c $^ $(FLAGS)

$(MPI): $(MPI).o  board.o
	$(CC) -o $@ $^ $(FLAGS)

$(MPI).o: $(MPI).c board.h
	$(CC) -c $^ $(FLAGS)

board.o: board.c board.h
	$(CC) -c $^ $(FLAGS)

clean:
	rm -f *.*o *.h.gch $(MPI) $(SERIAL)

