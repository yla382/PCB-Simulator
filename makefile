run: list.o a3.o
	gcc -o run list.o a3.o

list.o: list.c list.h
	gcc -c list.c

a3.o: a3.c list.h
	gcc -c a3.c

clean:
	rm  run a3.o list.o
