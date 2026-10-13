main: main.c
	gcc main.c data/levels.c -o sokoban -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 
clean:
	rm -rf sokoban
run:
	gcc main.c data/levels.c -o sokoban -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 
	./sokoban
