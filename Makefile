main: main.c
	gcc main.c -o sokoban -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 
clean:
	rm -rf sokoban
run:
	gcc main.c -o sokoban -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 
	./sokoban
