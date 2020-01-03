PROG = lab3

all: $(PROG)

$(PROG): main.o io.o
	gcc main.o io.o -o $(PROG) -lpthread

main.o: main.c io.h
	gcc -c main.c

io.o: io.c io.h
	gcc -c io.c