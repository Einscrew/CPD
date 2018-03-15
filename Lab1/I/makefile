CC=gcc
EXTRA=-fopenmp
FLAGS=-g -Wall

omp-1: omp-1.o
	$(CC) -o $@ $^ $(EXTRA)

omp-1.o: omp-1.c
	$(CC) -c $^ $(FLAGS) $(EXTRA)

clean:
	rm -f *.*o omp-1
