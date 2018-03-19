CC=gcc
EXTRA=-fopenmp
FLAGS=-g -Wall

sudoku: main.o game.o strategies.o aux1.o
	$(CC) -o $@ $^ $(EXTRA)

main.o: main.c game.h aux1.h strategies.h
	$(CC) -c $^ $(FLAGS) $(EXTRA)

game.o: game.c game.h aux1.h strategies.h
	$(CC) -c $^ $(FLAGS) $(EXTRA)

strategies.o: strategies.c strategies.h aux1.h
	$(CC) -c $^ $(FLAGS) $(EXTRA)

aux1.o: aux1.c aux1.h
	$(CC) -c $^ $(FLAGS) $(EXTRA)

clean:
	rm -f *.*o sudoku
