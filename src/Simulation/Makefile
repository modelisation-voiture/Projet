all: main

main: main.o Voiture.o Force.o Map.o
	g++ main.o Voiture.o Force.o Map.o -o main -std=c++17

main.o: main.cpp Voiture.hpp Force.hpp Map.hpp
	g++ -c main.cpp -std=c++17

Voiture.o: Voiture.cpp Voiture.hpp
	g++ -c Voiture.cpp -std=c++17

Force.o: Force.cpp Force.hpp Voiture.hpp
	g++ -c Force.cpp -std=c++17

Map.o: Map.cpp Map.hpp
	g++ -c Map.cpp -std=c++17

clean:
	rm -f main *.o trajectoire.txt
