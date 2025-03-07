CXX = g++
CXXFLAGS = -std=c++17 -I SFML-2.5.1/include/SFML/
LDFLAGS = -L SFML-2.5.1/ -lsfml-graphics -lsfml-window -lsfml-system 
SRC = src/main.cpp src/CarModel.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = rc_simulation

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
