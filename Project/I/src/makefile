CC=gcc
FLAGS=-g -Wall
EXTRA=-fopenmp

I: main.o strategies.o queue.o board.o utils.o
	$(CC) -o $@ $^ $(FLAGS) $(EXTRA)

main.o: main.c 
	$(CC) -c $^ $(FLAGS) $(EXTRA)

strategies.o: strategies.c 
	$(CC) -c $^ $(FLAGS) $(EXTRA)

queue.o: queue.c 
	$(CC) -c $^ $(FLAGS) $(EXTRA)

#stack.o: stack.c board.h
#	$(CC) -c $^ $(FLAGS) $(EXTRA)	

board.o: board.c 
	$(CC) -c $^ $(FLAGS) $(EXTRA)

utils.o: utils.c
	$(CC) -c $^ $(FLAGS) $(EXTRA)

clean:
	rm -f *.*o *.h.gch I
