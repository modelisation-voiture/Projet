#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include <cmath>

using namespace std;

namespace Simulation {
    namespace Solver {

template<typename State, typename Time>
class OdeSolver {
public:
    OdeSolver(double abs_tolerance, double rel_tolerance)
        : m_abs_tolerance(abs_tolerance), m_rel_tolerance(rel_tolerance) {}

    // Implémentation de la méthode ode45
    void solve(const function<void(const State&, State&, Time)>& system_function, State& initial_state, Time t_start, Time t_end, Time dt) {
        Time t = t_start;
        State y = initial_state;
        
        while (t < t_end) {
            // Effectuer une étape d'intégration avec la méthode RK4
            State y1 = y;
            rk4_step(system_function, y, t, dt);

            // Calculer l'erreur et ajuster le pas de temps
            Time error_estimate = compute_error(y1, y);

            // Si l'erreur est trop grande, on réduit le pas de temps
            if (error_estimate > m_abs_tolerance) {
                dt *= 0.5;
            } else {
                // Si l'erreur est acceptable, on peut accepter la solution
                t += dt;
                y = y1;

                // Augmenter le pas de temps pour la prochaine étape
                if (error_estimate < m_abs_tolerance / 10) {
                    dt *= 2.0;
                }
            }

            // Afficher le résultat de cette étape (si nécessaire)
            cout << "t = " << t << ", y = ";
            for (auto& val : y) cout << val << " ";
            cout << endl;
        }
    }

private:
    double m_abs_tolerance;
    double m_rel_tolerance;

    // Méthode RK4 pour un seul pas de temps
    void rk4_step(const function<void(const State&, State&, Time)>& system_function, State& y, Time t, Time dt) {
        State k1 = y;
        system_function(y, k1, t);
        for (auto& val : k1) val *= dt;

        State k2 = y;
        for (auto& val : k2) val += 0.5 * k1[val] * dt;
        system_function(k2, k2, t + 0.5 * dt);
        for (auto& val : k2) val *= dt;

        State k3 = y;
        for (auto& val : k3) val += 0.5 * k2[val] * dt;
        system_function(k3, k3, t + 0.5 * dt);
        for (auto& val : k3) val *= dt;

        State k4 = y;
        for (auto& val : k4) val += k3[val] * dt;
        system_function(k4, k4, t + dt);
        for (auto& val : k4) val *= dt;

        // Mise à jour de y avec la combinaison des valeurs k1, k2, k3, k4
        for (size_t i = 0; i < y.size(); ++i) {
            y[i] += (k1[i] + 2*k2[i] + 2*k3[i] + k4[i]) / 6;
        }
    }

    // Méthode pour calculer l'erreur (approximative)
    Time compute_error(const State& y1, const State& y) {
        Time max_error = 0;
        for (size_t i = 0; i < y.size(); ++i) {
            max_error = std::max(max_error, std::abs(y1[i] - y[i]));
        }
        return max_error;
    }
};

    } // namespace Modeles
} // namespace Simulation

/*int main() {
    // Exemple d'un système simple de 2 équations différentielles : dx/dt = y, dy/dt = -x
    auto system_function = [](const vector<double>& y, vector<double>& dydt, double t) {
        dydt[0] = y[1];    // dx/dt = y
        dydt[1] = -y[0];   // dy/dt = -x
    };

    vector<double> initial_state = {1.0, 0.0}; // Condition initiale (x(0) = 1, y(0) = 0)
    double t_start = 0.0, t_end = 10.0, dt = 0.1;
    
    // Créer un solveur avec une tolérance absolue et relative
    OdeSolver<vector<double>, double> solver(1e-6, 1e-6);
    
    // Résoudre le système
    solver.solve(system_function, initial_state, t_start, t_end, dt);

    return 0;
}*/
