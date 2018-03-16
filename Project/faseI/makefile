CC=gcc
FLAGS=-g -Wall
EXTRA=-fopenmp

I: main.o
	$(CC) -o $@ $^ $(FLAGS) $(EXTRA)

main.o: main.c
	$(CC) -c $@ $^ $(FLAGS) $(EXTRA)

clean:
	rm -f *.*o I
