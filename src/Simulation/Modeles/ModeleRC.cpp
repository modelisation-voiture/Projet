// ModeleRC.cpp
#include "ModeleRC.hpp"
#include "../Solver/Solver.hpp" 
#include <cmath>
#include <iostream>

namespace Simulation {
namespace Modeles {

ModeleRC::ModeleRC(const Parametres& params) 
    : m_params(params), m_controle{0.0, 0.0} {}

void ModeleRC::operator()(const Etat& x, Etat& dxdt, double t) const {
    // x[0]: position x
    // x[1]: position y
    // x[2]: orientation theta
    // x[3]: vitesse linéaire v
    // x[4]: vitesse angulaire omega
    
    const double& theta = x[2];
    const double& v = x[3];
    
    // Calcul des forces
    double F_propulsion = calculerForcePropulsion(v);
    double F_frottement = calculerForceFrottement(v);
    double F_laterale = calculerForceLaterale(v, m_controle.direction);
    
    // Équations différentielles du mouvement
    dxdt[0] = v * std::cos(theta);                   // dx/dt = v * cos(theta)
    dxdt[1] = v * std::sin(theta);                   // dy/dt = v * sin(theta)
    dxdt[2] = x[4];                                  // dtheta/dt = omega
    dxdt[3] = (F_propulsion - F_frottement) / m_params.masse;  // dv/dt = F_net / m
    
    // La vitesse angulaire dépend de la force latérale et de la direction
    // Modèle simplifié: omega = f(v, angle_direction)
    double beta = std::atan(m_params.longueur * std::tan(m_controle.direction) / 2.0);
    dxdt[4] = (v / m_params.longueur) * std::sin(beta) + 
              (F_laterale * m_params.longueur / 2.0) / m_params.inertie;
}

void ModeleRC::setControle(const Controle& ctrl) {
    m_controle = ctrl;
}

std::vector<ModeleRC::Etat> ModeleRC::simuler(
    const Etat& etat_initial, 
    double t_debut, 
    double t_fin, 
    double dt) {
    
    std::cout << "Simulation du modèle RC de t=" << t_debut << " à t=" << t_fin << std::endl;
    std::cout << "Contrôles: accélération=" << m_controle.acceleration 
              << ", direction=" << (m_controle.direction * 180.0 / M_PI) << "°" << std::endl;
    
    // Utiliser notre propre solveur au lieu de Boost
    Simulation::Solver::OdeSolver<Etat, double> solver(1.0e-6, 1.0e-6);
    
    // Créer une capture de this pour passer la fonction du système
    auto system_function = [this](const Etat& x, Etat& dxdt, double t) {
        this->operator()(x, dxdt, t);
    };
    
    // Appeler directement notre solveur avec une copie de l'état initial
    Etat etat_copie = etat_initial;
    std::vector<Etat> resultats = solver.solve(system_function, etat_copie, t_debut, t_fin, dt);
    
    // Afficher quelques statistiques
    if (!resultats.empty()) {
        const auto& dernier_etat = resultats.back();
        std::cout << "Résultats de la simulation:" << std::endl;
        std::cout << "  Nombre de points: " << resultats.size() << std::endl;
        std::cout << "  Position finale: (" << dernier_etat[0] << ", " << dernier_etat[1] << ")" << std::endl;
        std::cout << "  Orientation finale: " << (dernier_etat[2] * 180.0 / M_PI) << "°" << std::endl;
        std::cout << "  Vitesse finale: " << dernier_etat[3] << " m/s" << std::endl;
        std::cout << "  Vitesse angulaire finale: " << dernier_etat[4] << " rad/s" << std::endl;
    } else {
        std::cerr << "Erreur: Aucun résultat obtenu de la simulation!" << std::endl;
    }
    
    return resultats;
}

double ModeleRC::calculerForcePropulsion(double vitesse) const {
    // Modèle de propulsion avec limitation de puissance à haute vitesse
    // La force diminue à mesure que la vitesse augmente (courbe caractéristique du moteur)
    constexpr double vitesse_max = 15.0;  // m/s, vitesse maximale théorique
    double facteur_reduction = std::max(0.0, 1.0 - std::pow(vitesse / vitesse_max, 2));
    return m_params.masse * 9.81 * m_controle.acceleration * facteur_reduction;
}

double ModeleRC::calculerForceFrottement(double vitesse) const {
    // Frottement de roulement + résistance de l'air (proportionnelle à v²)
    const double signe = (vitesse > 0) ? 1.0 : ((vitesse < 0) ? -1.0 : 0.0);
    return m_params.mu * m_params.masse * 9.81 * signe + 
           0.05 * vitesse * vitesse * signe;  // Coefficient aérodynamique 0.05
}

double ModeleRC::calculerForceLaterale(double vitesse, double angle) const {
    // Force latérale due à l'angle des roues et à la vitesse
    // Dépend de l'angle de dérive (angle entre direction du mouvement et orientation des roues)
    double angle_derive = std::atan(std::tan(angle) / 2.0);  // Simplification
    return m_params.mu_lateral * m_params.masse * 9.81 * std::sin(angle_derive) * 
           (1.0 - std::exp(-3.0 * vitesse));  // Effet plus prononcé à haute vitesse
}

} // namespace Modeles
} // namespace Simulation