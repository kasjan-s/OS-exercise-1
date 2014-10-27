all: process

process: process.o err.o
	cc -Wall -o process process.o err.o

process.o: process.c err.h
	cc -Wall -c process.c

err.o: err.c err.h
	cc -Wall -c err.c

clean:
	rm -f *.o process
