// ModeleRC.cpp
#include "ModeleRC.hpp"
#include <cmath>

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
    
    using namespace boost::numeric::odeint;
    
    std::vector<Etat> etats;
    etats.reserve(static_cast<size_t>((t_fin - t_debut) / dt) + 1);
    
    // Correction ici: utilisation d'une capture par copie de etats pour éviter le problème de const
    auto observateur = [etats = &etats](const Etat& x, double) mutable {
        etats->push_back(x);
    };
    
    // Création d'une copie locale de l'état initial
    Etat etat = etat_initial;
    
    // Intégration avec Runge-Kutta 4 adaptatif de Boost
    typedef runge_kutta_dopri5<Etat> stepper_type;
    
    // Intégration avec pas adaptatif et contrôle d'erreur
    integrate_adaptive(
        make_controlled<stepper_type>(1.0e-6, 1.0e-6),  // tolérance absolue et relative
        *this,
        etat,  // Utilisation de la copie locale
        t_debut,
        t_fin,
        dt,
        observateur
    );
    
    return etats;
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