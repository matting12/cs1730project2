main: main.o
	g++ -Wall -g -lncurses -o main main.o
main.o: main.cpp
	g++ -Wall -std=c++14 -g -O0 -pedantic-errors -lncurses -c main.cpp
clean:
	rm -f main.o
	rm -f main.cpp