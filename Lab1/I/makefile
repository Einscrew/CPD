EXTRA=-fopenmp
FLAGS=-g -Wall

omp-1: omp-1.o
	gcc -o $@ $^ $(EXTRA)
 #gcc -c omp-1.o omp-1.c -fopenmp

omp-1.o: omp-1.c
	gcc -c $^ $(FLAGS) $(EXTRA)
 #gcc -c omp-1.c -fopenmp

clean:
	rm -f *.*o omp-1