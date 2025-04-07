CXX = g++
CXXFLAGS = -std=c++17 -I ../../SFML-2.6.2/include

# Détection du système
UNAME_S := $(shell uname -s)

# Configuration des options de liaison selon l'OS
ifeq ($(UNAME_S), Darwin)  # macOS
    LDFLAGS = -F ../../SFML-2.6.2/Frameworks -Wl,-rpath,../../SFML-2.6.2/Frameworks \
              -framework sfml-graphics -framework sfml-window -framework sfml-system
else  # Linux (Ubuntu, etc.)
    LDFLAGS = -L ../../SFML-2.6.2/lib -lsfml-graphics -lsfml-window -lsfml-system
endif

SRC = main.cpp voiture.cpp map.cpp 
OBJ = $(SRC:.cpp=.o)
TARGET = rc_simulation

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $(TARGET) $(OBJ) $(LDFLAGS)

%.o: %.cpp
	$(CXX) -v $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
