# Makefile for examples week08

all:	pgflt_test cache_check rlimit

CC=gcc
CFLAGS=-lpthread 

rlimit:	rlimit.c
	${CC} ${CFLAGS} rlimit.c -o rlimit

pgflt_test:	pgflt_test.c
	${CC} ${CFLAGS} pgflt_test.c -o pgflt_test 

cache_check:	cache_check.c
	${CC} ${CFLAGS} cache_check.c -o cache_check 

clean:
	rm -f *.o rlimit pgflt_test cache_check
