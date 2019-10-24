all: liblthread.a main setup

lthread.o:lthread.c
		gcc -Wall -c lthread.c -o lthread.o -I../include

structs.o:structs.c structs.o
		gcc -Wall -c structs.c -o structs.o -I../include

liblthread.a:lthread.o structs.o
		ar rcs liblthread.a lthread.o structs.o

main.o:main.c
		gcc -w -c main.c -o main.o -I../include

main:main.o setup
		gcc -o ../bin/main main.o -L. -llthread

run:
		../bin/main

clean:
		rm -rf  ../bin/main
		rm -f *.o 
		rm -f *.so
		rm -f *.a

.PHONY:clean setup
