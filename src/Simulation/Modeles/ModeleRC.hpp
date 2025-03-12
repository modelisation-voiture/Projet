#pragma once

#include <array>
#include <boost/numeric/odeint.hpp>
#include <functional>
#include <vector>

namespace Simulation {
namespace Modeles {

class ModeleRC {
public:
    // État du système: [x, y, theta, v, omega]
    // x, y: position
    // theta: orientation
    // v: vitesse linéaire
    // omega: vitesse angulaire
    using Etat = std::array<double, 5>;
    
    // Paramètres de la voiture
    struct Parametres {
        double masse;         // masse en kg
        double inertie;       // moment d'inertie en kg.m²
        double longueur;      // longueur de la voiture en m
        double mu;            // coefficient de frottement
        double mu_lateral;    // coefficient de frottement latéral
    };
    
    // Entrées de contrôle
    struct Controle {
        double acceleration;  // accélération (force motrice normalisée)
        double direction;     // angle des roues avant (en radians)
    };
    
    ModeleRC(const Parametres& params);
    
    // Définit la fonction d'état pour odeint
    void operator()(const Etat& x, Etat& dxdt, double t) const;
    
    // Définit les contrôles actuels
    void setControle(const Controle& ctrl);
    
    // Simulation sur un intervalle de temps avec enregistrement des états
    std::vector<Etat> simuler(const Etat& etat_initial, double t_debut, double t_fin, double dt);

private:
    Parametres m_params;
    Controle m_controle;
    
    // Calcul des forces
    double calculerForcePropulsion(double vitesse) const;
    double calculerForceFrottement(double vitesse) const;
    double calculerForceLaterale(double vitesse, double angle) const;
};

} // namespace Modeles
} // namespace Simulation