CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

TARGET = rk4_solver
SRC = SolveurODE.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean