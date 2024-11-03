cc = gcc
files = snake.c
libs = -lncurses

snake: snake.c 
	$(cc) -o snake $(files) $(libs)

debug: snake.c 
	$(cc) -o snake $(files) $(libs) -g
