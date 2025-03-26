#include "force.hpp"

// Force Motrice
ForceMotrice::ForceMotrice(double intensite_max, double vitesse_saturation)
    : intensite_max(intensite_max), vitesse_saturation(vitesse_saturation) {}

    std::pair<double, double> ForceMotrice::calculer_force(const Voiture& voiture) const {
        double vitesse = voiture.getVitesse();  // vitesse totale
        double ratio = 1.0 - (vitesse / vitesse_saturation);
    
        // Clamp entre 0 et 1
        if (ratio < 0.0) ratio = 0.0;
        if (ratio > 1.0) ratio = 1.0;
    
        double force = intensite_max * ratio;
    
        double rad = voiture.getAngle() * M_PI / 180.0;
        return { force * std::cos(rad), force * std::sin(rad) };
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

    
    


