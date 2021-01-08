# Makefile for examples week08

all:	pipetest dotprod s_sh

CC=gcc
CFLAGS=-lpthread 

pipetest:	pipetest.c
	${CC} ${CFLAGS} pipetest.c -o pipetest

dotprod:	dotprod.c
	${CC} ${CFLAGS} dotprod.c -o dotprod 

s_sh:	s_sh.c
	${CC} s_sh.c -o s_sh 

clean:
	rm -f *.o pipetest dotprod s_sh
