run: my_set.o
	gcc -g -ansi -Wall -pedantic my_set.o -o run

my_set.o: my_set.c my_set.h
	gcc -ansi -Wall -pedantic -c my_set.c -o my_set.o
	
clean:
	rm *.o
