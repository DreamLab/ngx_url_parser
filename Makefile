CC=gcc
CPP=g++

all: example ngx_url_parser.a test

ngx_url_parser.o: ngx_url_parser.c
	$(CC) -I. -o $@ -c $^

ngx_url_parser.a: ngx_url_parser.o
	ar rcs libngx_url_parser.a $^


eample.o: example.c
	$(CC) -o $@ -c $^

example: ngx_url_parser.o example.o
	$(CC) $^ -o $@

ngx_url_parser_test.o: tests/ngx_url_parser_test.c
	$(CC) $^ -o $@

test: ngx_url_parser.c tests/ngx_url_parser_test.c tests/main.cpp
	$(CPP)  -lgtest -lpthread -L. -lngx_url_parser $^ -o run 

clean:
	rm *.o
