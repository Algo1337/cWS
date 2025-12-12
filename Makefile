.PHONY: all

all: compile

compile:
	gcc main.c src/*.c

debug:
	gcc main.c src/*.c -g -g3 -ggdb