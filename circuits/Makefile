CC = g++
CFLAGS = -Wall -Wextra -std=c++17 $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs)

SRC = affichage_circuit.cpp
OBJ = $(SRC:.cpp=.o)
EXEC = sdl_app

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)
