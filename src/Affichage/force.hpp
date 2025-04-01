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
// Ce modèle suppose que le moteur peut fournir une force maximale (intensité_max),
// mais que cette force dépend de l'accélérateur (de 0 à 1), comme une pédale d'accélération.
//
// On n'impose plus une "vitesse maximale" arbitraire.
// La vitesse d'équilibre apparaîtra naturellement :
// lorsque la somme des frottements (air, sol, etc.) équilibre la poussée motrice.

class ForceMotrice : public Force {
    private:
        double intensite_max;          // Force maximale en N
        double vitesse_saturation;     // Seuil de saturation (vitesse max utile)
    
    public:
        ForceMotrice(double intensite_max, double vitesse_saturation = 20.0);
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
                Fmax *= 0.1; // Perte de grip sous freinage
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

// Force de résistance de l'air
class ForceAerodynamique : public Force {
    private:
        double coefficient_trainee; // Coefficient de traînée (Cx * A * rho / 2)
    
    public:
        ForceAerodynamique(double coeff) : coefficient_trainee(coeff) {}
    
        std::pair<double, double> calculer_force(const Voiture& voiture) const override {
            double vx = voiture.getVitesseX();
            double vy = voiture.getVitesseY();
    
            // Vecteur vitesse et norme
            double v_norme = std::sqrt(vx * vx + vy * vy);
            if (v_norme < 1e-3) return {0.0, 0.0}; // pas de vitesse, pas de résistance
    
            // Force opposée à la vitesse, proportionnelle à v²
            double fx = -coefficient_trainee * vx * v_norme;
            double fy = -coefficient_trainee * vy * v_norme;
    
            return {fx, fy};
        }
    };
    
    // Force de freinage à main (glissement simulé avec accumulation progressive)
class ForceFreinGlisse : public Force {
    private:
        double intensite_max;            // Valeur maximale de la force appliquée
        mutable double intensite_courante = 0.0; // Force actuelle (accumulation progressive)
        double coeff_montée = 100.0;   // Taux de montée vers intensité_max (par seconde)
        double coeff_descente = 20000.0; // Taux de descente rapide quand le frein est relâché
    
    public:
        ForceFreinGlisse(double intensite_max)
            : intensite_max(intensite_max) {}
    
        std::pair<double, double> calculer_force(const Voiture& voiture) const override {
            double v = voiture.getVitesse();
            if (v < 0.1) return {0.0, 0.0}; // Pas de glisse si voiture quasi immobile
    
            // Gradation de la force de frein à main :
            // - En réalité, on n'active pas le frein d’un seul coup, on "tire" dessus
            // - Cette accumulation progressive simule cette montée
            // - Permet de mieux faire apparaître les dérapages visuellement
    
            if (voiture.isFreinMainActif()) {
                intensite_courante += coeff_montée * 0.01; // avec dt = 0.01
                if (intensite_courante > intensite_max)
                    intensite_courante = intensite_max;
            } else {
                intensite_courante -= coeff_descente * 0.01;
                if (intensite_courante < 0)
                    intensite_courante = 0;
            }
    
            double vx = voiture.getVx();
            double vy = voiture.getVy();
            double v_module = std::sqrt(vx * vx + vy * vy);
            if (v_module == 0) return {0.0, 0.0};
    
            // On applique une légère rotation à la direction du frein pour simuler un déséquilibre
            // Cela permet à la voiture de déraper plus rapidement
            double angle_decalage = M_PI / 12.0;  // ≈15°
            double vx_rot = vx * std::cos(angle_decalage) - vy * std::sin(angle_decalage);
            double vy_rot = vx * std::sin(angle_decalage) + vy * std::cos(angle_decalage);
            double v_mod_rot = std::sqrt(vx_rot * vx_rot + vy_rot * vy_rot);
    
            return {  -intensite_courante * (vx_rot / v_mod_rot),intensite_courante * (vy_rot / v_mod_rot)};
        }
    };
    
    
        
#endif
