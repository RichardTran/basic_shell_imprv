COMPILER = gcc
CCFLAGS = -ansi -pedantic -Wall

parser: parser.o builtin.o cmd.h builtin.h
	$(COMPILER) parser.o builtin.o -o main

parser.o: parser.c cmd.h builtin.h
	$(COMPILER) -c parser.c

merp: merp.o cmd.h
	$(COMPILER) merp.o -o merp

merp.o: merp.c cmd.h
	$(COMPILER) -c merp.c

builtin: builtin.o builtin.h
	$(COMPILER) builtin.o -o builtin

builtin.o: builtin.c builtin.h
	$(COMPILER) -c builtin.c

clean:
	rm -f merp.o
	rm -f builtin.o
	rm -f parser.o
	rm -f main
