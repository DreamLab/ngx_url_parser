CC=gcc
CXX=g++

all: example static-lib test

obj/ngx_url_parser.o: ngx_url_parser.c
	mkdir -p obj
	$(CC) -I. -o $@ -c $^

static-lib: obj/ngx_url_parser.o
	mkdir -p lib 
	ar rcs lib/libngx_url_parser.a $^

obj/example.o: example.c
	$(CC) -o $@ -c $^

example: obj/ngx_url_parser.o obj/example.o
	$(CC) $^ -o $@

test: ngx_url_parser.c tests/ngx_url_parser_test.cpp tests/main.cpp
	$(CXX)  -lgtest -lpthread $^ -o unit_tests

clean:
	rm -f obj/*.o
