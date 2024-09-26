files = snake.c
libs = -lncurses

snake: snake.c 
	gcc -o snake $(files) $(libs)

debug: snake.c 
	gcc -o snake $(files) $(libs) -g
