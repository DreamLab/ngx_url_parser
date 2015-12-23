CC=gcc
CXX=g++

all: example static-lib 

obj/ngx_url_parser.o: ngx_url_parser.c
	mkdir -p obj
	$(CC) -fPIC -I. -o $@ -c $^ 

static-lib: obj/ngx_url_parser.o
	mkdir -p lib 
	ar -rcs lib/libngx_url_parser.a $^
	ranlib lib/libngx_url_parser.a

clean:
	rm -f obj/*.o
