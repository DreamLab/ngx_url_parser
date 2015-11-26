CC=gcc

all: test

ngx_url_parser.o: ngx_url_parser.c
	$(CC) -I. -o $@ -c $^

test.o: test.c
	$(CC) -o $@ -c $^

test: ngx_url_parser.o test.o
	$(CC) $^ -o $@

clean:
	rm ngx_url_parser.o
	rm test.o
