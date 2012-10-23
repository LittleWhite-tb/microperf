CC=gcc

all:	glibc_malloc.so

glibc_malloc.so:	glibc_malloc.c
	$(CC) glibc_malloc.c -o glibc_malloc.so -O3 -shared -fPIC -Wall -Wextra 

clean:
	rm -rf *.o *.so *~
