CC=gcc

all:	dedicated_arrays.so

dedicated_arrays.so:	dedicated_arrays.c sa_malloc.o
	$(CC) dedicated_arrays.c sa_malloc.o -o dedicated_arrays.so -O3 -shared -fPIC -Wall -Wextra 

sa_malloc.o:	sa_malloc.c sa_malloc.h
	$(CC) sa_malloc.c -o sa_malloc.o -O3 -shared -fPIC -c -Wall -Wextra

clean:
	rm -rf *.o *.so *~
