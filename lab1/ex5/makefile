CC = gcc -Wall

main: bar.o foo.o
	$(CC) bar.o foo.o -o main

bar.o: bar.c bar.h
	$(CC) -c bar.c -o bar.o

foo.o: foo.c foo.h bar.h
	$(CC) -c foo.c -o foo.o

clean:
	rm -f foo.o bar.o main


