modele: Force.o VoitureRC.o ForceMotrice.o main.o
	g++ -Wall -Wextra -std=c++17 -o modele Force.o VoitureRC.o ForceMotrice.o main.o

Force.o: Force.cpp Force.hpp
	g++ -Wall -Wextra -std=c++17 -c Force.cpp

VoitureRC.o: VoitureRC.cpp VoitureRC.hpp
	g++ -Wall -Wextra -std=c++17 -c VoitureRC.cpp

ForceMotrice.o: ForceMotrice.cpp ForceMotrice.hpp
	g++ -Wall -Wextra -std=c++17 -c ForceMotrice.cpp

main.o: ./main.cpp
	g++ -Wall -Wextra -std=c++17 -c ./main.cpp

clean:
	rm -f Force.o VoitureRC.o ForceMotrice.o main.o modele
