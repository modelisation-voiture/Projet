#ifndef FORCE_HPP
#define FORCE_HPP

#include "voiture.hpp"
#include <utility>
#include <cmath>
#include <algorithm> // Pour std::min et std::max génériques



// Classe abstraite
class Force {
public:
    virtual std::pair<double, double> calculer_force(const Voiture& voiture) const = 0;
    virtual ~Force() = default;
};

// Force Motrice
class ForceMotrice : public Force {
private:
    double intensite; // Newtons
public:
    ForceMotrice(double intensite);
    std::pair<double, double> calculer_force(const Voiture& voiture) const override;
};

// Force Frottement
class ForceFrottement : public Force {
private:
    double coefficient; // coefficient de frottement
    double g;           // gravité (9.81 m/s²)
public:
    ForceFrottement(double coeff, double gravite = 9.81);
    std::pair<double, double> calculer_force(const Voiture& voiture) const override;
};
    
// Force de Virage (À améliorer encore puisque ça n'est fonctionnelle que pour un modele "velo")
class ForceVirage : public Force {
    private:
        double angle_braquage_deg;
    
    public:
        ForceVirage(double angle_deg) : angle_braquage_deg(angle_deg) {}
    
        std::pair<double, double> calculer_force(const Voiture& voiture) const override {
            double vx = voiture.getVitesseX();
            double vy = voiture.getVitesseY();
            double vpsi = voiture.getVitesseLacet();
            double angle_rad = voiture.getAngle() * M_PI / 180.0;
            double masse = voiture.getMasse();
            double empattement = voiture.getEmpattement();
    
            if (std::abs(vx) < 1e-3) return {0.0, 0.0};
    
            // Conversion braquage en radians
            double braquage_rad = angle_braquage_deg * M_PI / 180.0;
    
            // Calcul de l'angle de dérive simplifié
            double beta = (vy + empattement * vpsi) / vx - braquage_rad;
    
            // Paramètres du pneu
            double rigidite = 1000.0;  // N/rad
            double Fmax = 0.8 * masse * 9.81; // Adhérence max
    
            // Réduction d’adhérence si frein actif
            if (voiture.isFreinActif()) {
                Fmax *= 0.6; // Perte de grip sous freinage
            }
    
            // Force latérale avec saturation
            double Fy = rigidite * beta;
            Fy = std::max(-Fmax, std::min(Fy, Fmax));

    
            // Projection dans le repère global
            double fx = -Fy * std::sin(angle_rad);
            double fy = Fy * std::cos(angle_rad);
    
            return {fx, fy};
        }
    };
    
// Force de Freinage (activée par l'utilisateur)
class ForceFreinage : public Force {
    private:
        double intensite_max; // Force maximale de freinage (N)
        const double epsilon = 1e-3; // Pour éviter division par 0
        public:
    ForceFreinage(double intensite) : intensite_max(intensite) {}

    std::pair<double, double> calculer_force(const Voiture& voiture) const override {
        if (!voiture.isFreinActif()) return {0.0, 0.0};

        double vitesse = voiture.getVitesse();
        if (vitesse < epsilon) return {0.0, 0.0};

        double angle = voiture.getAngle() * M_PI / 180.0;

        // Force de freinage opposée à la direction de la vitesse
        double fx = -intensite_max * std::cos(angle);
        double fy = -intensite_max * std::sin(angle);

        return {fx, fy};
    }
    void avancer(double dt);

};

#endif
