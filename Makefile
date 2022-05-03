.PHONY: clean

CC = gcc
FLAGS = -lncurses


wordle: src/wordle.c
	${CC} -o $@ $^ ${FLAGS}

%.o: %.cpp
	${CC} -c $< -o $@


clean:
	rm wordle
