#include "force.hpp"

// Force Motrice
ForceMotrice::ForceMotrice(double intensite) : intensite(intensite) {}

std::pair<double, double> ForceMotrice::calculer_force(const Voiture& voiture) const {
    double rad = voiture.getAngle() * M_PI / 180.0;
    return { intensite * cos(rad), intensite * sin(rad) };
}

// Force Frottement
ForceFrottement::ForceFrottement(double coeff, double gravite)
    : coefficient(coeff), g(gravite) {}
    

    std::pair<double, double> ForceFrottement::calculer_force(const Voiture& voiture) const {
        double vitesse = voiture.getVitesse();
        if (vitesse == 0) return {0, 0};
    
        double frottement = coefficient * voiture.getVitesse() * voiture.getVitesse() * voiture.getMasse() * g;
    
        double fx = -frottement * (voiture.getVx() / vitesse);
        double fy = -frottement * (voiture.getVy() / vitesse);
    
        return {fx, fy};
    }

    
    


