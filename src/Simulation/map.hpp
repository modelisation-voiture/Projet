#ifndef MAP_HPP
#define MAP_HPP

#include <vector>
#include <string>

class Map {
public:
    std::vector<std::pair<double, double> > positions;

    void ajouterPosition(double x, double y);
    void sauvegarderTrajectoire(const std::string& fichier) const;
};

#endif
