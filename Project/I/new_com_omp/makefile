CC=gcc
FLAGS=-g -Wall
EXTRA=-fopenmp

I: main.o strategies.o board.o stack.o
	$(CC) -o $@ $^ $(FLAGS) $(EXTRA)

main.o: main.c strategies.h
	$(CC) -c $^ $(FLAGS) $(EXTRA)

strategies.o: strategies.c board.h stack.h
	$(CC) -c $^ $(FLAGS) $(EXTRA)

board.o: board.c 
	$(CC) -c $^ $(FLAGS) $(EXTRA)

stack.o: stack.c board.h
	$(CC) -c $^ $(FLAGS) $(EXTRA)

clean:
	rm -f *.*o *.h.gch I
