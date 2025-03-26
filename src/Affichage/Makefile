# #Make avec les forces de Nadir
# CXX = g++
# CXXFLAGS = -std=c++17 -I ../../SFML-2.5.1/include
# LDFLAGS = -L ../../SFML-2.5.1 -lsfml-graphics -lsfml-window -lsfml-system

# SRC = main.cpp  voiture.cpp map.cpp force.cpp
# OBJ = $(SRC:.cpp=.o)
# TARGET = rc_simulation

# all: $(TARGET)

# $(TARGET): $(OBJ)
# 	$(CXX) -o $@ $^ $(LDFLAGS)

# %.o: %.cpp
# 	$(CXX) -v $(CXXFLAGS) -c $< -o $@

# clean:
# 	rm -f $(OBJ) $(TARGET)
#Make avec les forces de Nadir
CXX = g++
CXXFLAGS = -std=c++17 -I ../../SFML-2.6.2/include
# On lie avec les frameworks SFML
# Vérifie que tes frameworks sont bien installés dans /Library/Frameworks, sinon ajuste le chemin
LDFLAGS = -F ../../SFML-2.6.2/Frameworks -Wl,-rpath,../../SFML-2.6.2/Frameworks -framework sfml-graphics -framework sfml-window -framework sfml-system

SRC = main.cpp voiture.cpp map.cpp force.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = rc_simulation

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $(TARGET) $(OBJ) $(LDFLAGS)

%.o: %.cpp
	$(CXX) -v $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
