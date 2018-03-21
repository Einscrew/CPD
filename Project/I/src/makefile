CC=gcc
EXTRA=-fopenmp
FLAGS=-g -Wall

I: main.o strategies.o board.o utils.o
	$(CC) -o $@ $^ $(EXTRA)

main.o: main.c board.h strategies.h utils.h
	$(CC) -c $^ $(FLAGS) $(EXTRA)

board.o: board.c utils.h 
	$(CC) -c $^ $(FLAGS) $(EXTRA)

strategies.o: strategies.c board.h
	$(CC) -c $^ $(FLAGS) $(EXTRA)

utils.o: utils.c
	$(CC) -c $^ $(FLAGS) $(EXTRA)

clean:
	rm -f *.*o *.h.gch I
