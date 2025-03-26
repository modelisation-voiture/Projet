#include "map.hpp"
#include <fstream>

void Map::ajouterPosition(double x, double y) {
    positions.emplace_back(x, y);
}

void Map::sauvegarderTrajectoire(const std::string& fichier) const {
    std::ofstream ofs(fichier);
    for (const auto& pos : positions)
        ofs << pos.first << "," << pos.second << "\n";
}
