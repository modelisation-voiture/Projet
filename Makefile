# Makefile pour le projet de simulation de voiture RC

# Compilateur et options
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic -O2
LDFLAGS = -lboost_system -lboost_filesystem

# Répertoires
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Création des répertoires si nécessaires
$(shell mkdir -p $(BUILD_DIR) $(BIN_DIR))

# Détection automatique des fichiers source
SRCS = $(wildcard $(SRC_DIR)/*.cpp) \
       $(wildcard $(SRC_DIR)/Simulation/Modeles/*.cpp) \
       $(wildcard $(SRC_DIR)/Simulation/Solver/*.cpp) \
       $(wildcard $(SRC_DIR)/Controle/*.cpp) \
       $(wildcard $(SRC_DIR)/Affichage/*.cpp) \
       $(wildcard $(SRC_DIR)/Donnees/*.cpp)

# Conversion des fichiers .cpp en .o dans le répertoire build
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Création des sous-répertoires dans build
$(shell mkdir -p $(dir $(OBJS)))

# Cible principale
all: $(BIN_DIR)/simulator

# Règle de compilation du binaire
$(BIN_DIR)/simulator: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Règle de compilation des objets
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) -c -o $@ $<

# Cible pour tester le solver uniquement (exemple simplifié)
test_solver: $(BUILD_DIR)/test_solver.o $(BUILD_DIR)/Simulation/Modeles/ModeleRC.o
	$(CXX) $(CXXFLAGS) -o $(BIN_DIR)/test_solver $^ $(LDFLAGS)

$(BUILD_DIR)/test_solver.o: tests/test_solver.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) -c -o $@ $<

# Cible pour nettoyer le projet
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Phony targets
.PHONY: all clean test_solver
