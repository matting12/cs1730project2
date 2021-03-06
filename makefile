1730ed: main.o
	g++ -Wall -g -lform -lpanel -lncurses -o 1730ed main.o
main.o: main.cpp methods.cpp
	g++ -Wall -std=c++14 -g -O0 -pedantic-errors -lform -lpanel -lncurses -c main.cpp
methods.o: methods.cpp
	 g++ -Wall -std=c++14 -g -O0 -pedantic-errors -lform -lpanel -lncurses -c methods.cpp
clean:
	rm -f main.o
	rm -f 1730ed
	rm -f methods.o
