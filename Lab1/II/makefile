EXTRA=-fopenmp
FLAGS=-g -Wall

omp-2: omp-2.o
	gcc -o $@ $^ $(EXTRA)
 #gcc -c omp-2.o omp-2.c -fopenmp

omp-2.o: omp-2.c
	gcc -c $^ $(FLAGS) $(EXTRA)
 #gcc -c omp-2.c -fopenmp

omp-2c: omp-2c.o
	gcc -o $@ $^ $(EXTRA)

omp-2c.o: omp-2c.c
	gcc -c $^ $(FLAGS) $(EXTRA)

clean:
	rm -f *.*o omp-2
