

all: runner

runner: alfa.o beta.o gamma.o
	gcc $^ -o $@

%.o : %.c
	@echo $<
	@gcc -c $<

