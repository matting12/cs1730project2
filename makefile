main: main.o
	g++ -Wall -g -lform -lpanel -lncurses -o main main.o
main.o: main.cpp methods.cpp
	g++ -Wall -std=c++14 -g -O0 -pedantic-errors -lform -lpanel -lncurses -c main.cpp
clean:
	rm -f main.o
	rm -f main
