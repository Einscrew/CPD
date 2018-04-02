CC=gcc
EXTRA=-fopenmp
FLAGS=-g -Wall

omp-2: omp-2.o
	$(CC) -o $@ $^ $(EXTRA)

omp-2.o: omp-2.c
	$(CC) -c $^ $(FLAGS) $(EXTRA)

clean:
	rm -f *.*o omp-2 omp-2c omp-2d
