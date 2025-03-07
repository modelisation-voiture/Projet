#include <vector>
#include <cmath>

// Paramètres du véhicule RC
struct RCParams {
    double mass;        // Masse (kg)
    double Iz;          // Moment d'inertie (kg·m²)
    double a;           // Empattement avant (m)
    double b;           // Empattement arrière (m)
    double d;           // Demi-voie (m)
    double Cf;          // Rigidité latérale avant (N/rad)
    double Cr;          // Rigidité latérale arrière (N/rad)
};

// État du véhicule
struct RCState {
    double Vx;   // Vitesse longitudinale (m/s)
    double Vy;   // Vitesse latérale (m/s)
    double psi;  // Angle de lacet (rad)
    double Vpsi; // Vitesse angulaire (rad/s)
};

// Calcul des forces longitudinales et latérales pour une roue
void computeWheelForces(double Fx, double Fy, double delta, double &fx, double &fy) {
    fx = Fx * std::cos(delta) - Fy * std::sin(delta);
    fy = Fx * std::sin(delta) + Fy * std::cos(delta);
}

// Calcul des dérivées d'état
std::vector<double> computeDerivatives(const RCParams &params, const RCState &state, double delta_f) {
    // Forces longitudinales et latérales (simplifiées ici)
    double Fx1 = 0, Fx2 = 0, Fx3 = 0, Fx4 = 0;
    double Fy1 = 0, Fy2 = 0, Fy3 = 0, Fy4 = 0;

    // Forces projetées sur les axes du véhicule
    double fx1, fy1, fx2, fy2, fx3, fy3, fx4, fy4;
    computeWheelForces(Fx1, Fy1, delta_f, fx1, fy1);
    computeWheelForces(Fx2, Fy2, delta_f, fx2, fy2);
    computeWheelForces(Fx3, Fy3, 0, fx3, fy3);
    computeWheelForces(Fx4, Fy4, 0, fx4, fy4);

    // Équations différentielles
    double dVx = (fy1 + fy2 + fy3 + fy4) / params.mass - state.Vy * state.Vpsi;
    double dVy = (fx1 + fx2 + fx3 + fx4) / params.mass + state.Vx * state.Vpsi;
    double dPsi = state.Vpsi;
    double dVpsi = (params.a * (fy1 + fy2) - params.b * (fy3 + fy4)) / params.Iz;

    return {dVx, dVy, dPsi, dVpsi};
}
