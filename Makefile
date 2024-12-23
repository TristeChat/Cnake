cc = gcc
files = snake.c
file_opt = snake_opt.c
libs = -lncurses

snake: $(files) 
	@echo "Currently incomplete"
	$(cc) -o snake $(files) $(libs)

opt: $(file_opt) 
	@echo "Currently very unstable and incomplete"
	$(cc) -o snake $(file_opt) $(libs)

debug: $(files)
	$(cc) -o snake $(files) $(libs) -g

optbug: $(file_opt)
	$(cc) -o snake $(file_opt) $(libs) -g

clean: snake 
	rm snake
