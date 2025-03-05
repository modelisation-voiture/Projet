#ifndef VOITURERC_HPP
#define VOITURERC_HPP

#include <vector>
#include "Force.hpp"

class VoitureRC {
private:
    double m, Iz, Vx, Vy, psi;
    std::vector<Force*> Forces; // Liste des Forces appliquées

public:
    VoitureRC(double masse, double inertie);
    void ajouterForce(Force* Force);
    void mettreAJourEtat();
    void appliquerForces();
    
    // Accesseurs
    double getVx() const;
    double getVy() const;
};

#endif // VOITURERC_HPP
