all: main.o file.o
	g++ main.o file.o -o kp.out

main.o: main.cpp file.h
	g++ -c main.cpp

file.o: file.cpp file.h
	g++ -c file.cpp