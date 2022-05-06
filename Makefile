.PHONY: clean debug

CC = gcc
FLAGS = -lncurses
DEBUG = -D DEBUG


wordle: src/wordle.c
	${CC} -o $@ $^ ${FLAGS}

debug: src/wordle.c
	${CC} -o $@ $^ ${FLAGS} ${DEBUG}

%.o: %.cpp
	${CC} -c $< -o $@


clean:
	rm wordle debug
