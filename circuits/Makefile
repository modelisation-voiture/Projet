CC = g++
CFLAGS = -Wall -Wextra -std=c++17 -I/opt/local/include/SDL2 -I/opt/local/include
LDFLAGS = -L/opt/local/lib -lSDL2 -lSDL2_image
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