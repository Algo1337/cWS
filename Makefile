.PHONY: all

all: compile

compile:
	gcc main.c src/*.c

lib:
	gcc -c src/*.c
	ar rcs cws.a *.o; rm *.o
	mv cws.a /usr/local/lib/libcws.a

mv_headers:
	mkdir /usr/include/cws
	cp src/init.h /usr/include/cws.h

debug:
	gcc main.c src/*.c -g -g3 -ggdb -fsanitize=address