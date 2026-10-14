CC = gcc
CFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 
main: main.c
	$(CC) main.c data/levels.c -o sokoban $(CFLAGS)
clean:
	rm -rf sokoban
run:
	$(CC) main.c data/levels.c -o sokoban $(CFLAGS)
	./sokoban
