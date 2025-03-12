#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include <cmath>
#include <stdexcept>

namespace Simulation {
    namespace Solver {

template<typename State, typename Time>
class OdeSolver {
public:
    OdeSolver(double abs_tolerance, double rel_tolerance)
        : m_abs_tolerance(abs_tolerance), m_rel_tolerance(rel_tolerance) {}

    // Méthode principale pour résoudre un système d'équations différentielles
    std::vector<State> solve(
        const std::function<void(const State&, State&, Time)>& system_function, 
        State& initial_state, 
        Time t_start, 
        Time t_end, 
        Time dt) {
        
        Time t = t_start;
        State y = initial_state;
        
        // Vecteur pour stocker les résultats
        std::vector<State> results;
        
        // Ajouter l'état initial
        results.push_back(y);
        
        // Compteur de sécurité pour éviter les boucles infinies
        int max_iterations = 10000;
        int iteration = 0;
        
        // Paramètres pour l'adaptation du pas de temps
        Time min_dt = 1e-10;
        Time max_dt = (t_end - t_start) / 10.0;
        
        std::cout << "Début de la résolution: t=" << t_start << " à t=" << t_end << std::endl;
        
        while (t < t_end && iteration < max_iterations) {
            iteration++;
            
            // Limiter le pas de temps pour ne pas dépasser t_end
            if (t + dt > t_end) {
                dt = t_end - t;
            }
            
            // Effectuer une étape d'intégration avec la méthode RK4
            State y_new = y;  // Copie de l'état actuel
            rk4_step(system_function, y_new, t, dt);
            
            // Calculer l'erreur
            Time error_estimate = compute_error(y, y_new);
            
            // Si l'erreur est trop grande, on réduit le pas de temps
            if (error_estimate > m_abs_tolerance && dt > min_dt) {
                dt *= 0.5;
                if (dt < min_dt) dt = min_dt;
                
                // Log pour déboguer
                if (iteration % 100 == 0) {
                    std::cout << "  Réduction du pas de temps: dt=" << dt << ", erreur=" << error_estimate << std::endl;
                }
            } else {
                // Si l'erreur est acceptable, on peut accepter la solution
                t += dt;
                y = y_new;  // Mise à jour de l'état
                
                // Stocker le résultat
                results.push_back(y);
                
                // Log pour afficher la progression
                if (results.size() % 100 == 0) {
                    std::cout << "  Progression: t=" << t << ", points calculés=" << results.size() << std::endl;
                }
                
                // Augmenter le pas de temps pour la prochaine étape si l'erreur est petite
                if (error_estimate < m_abs_tolerance / 10.0 && dt < max_dt) {
                    dt *= 1.5;  // Augmentation plus conservatrice
                    if (dt > max_dt) dt = max_dt;
                }
            }
            
            // Vérifier si les valeurs deviennent instables
            if (!is_state_valid(y)) {
                std::cerr << "Erreur: La solution a divergé à t=" << t << std::endl;
                break;
            }
        }
        
        // Afficher un résumé
        std::cout << "Résolution terminée: " << results.size() << " points calculés" << std::endl;
        
        if (iteration >= max_iterations) {
            std::cerr << "Attention: Nombre maximum d'itérations atteint!" << std::endl;
        }
        
        return results;
    }

private:
    double m_abs_tolerance;
    double m_rel_tolerance;

    // Méthode RK4 pour un seul pas de temps
    void rk4_step(const std::function<void(const State&, State&, Time)>& system_function, 
                 State& y, Time t, Time dt) {
        // Allouer de l'espace pour les termes k1, k2, k3, k4
        State k1 = y;  // Utiliser l'opérateur de copie pour initialiser avec la bonne taille
        State k2 = y;
        State k3 = y; 
        State k4 = y;
        State temp = y;  // État temporaire
        
        // Premier terme: k1 = f(y, t)
        system_function(y, k1, t);
        
        // Deuxième terme: k2 = f(y + dt*k1/2, t + dt/2)
        for (size_t i = 0; i < y.size(); ++i) {
            temp[i] = y[i] + dt * k1[i] * 0.5;
        }
        system_function(temp, k2, t + dt * 0.5);
        
        // Troisième terme: k3 = f(y + dt*k2/2, t + dt/2)
        for (size_t i = 0; i < y.size(); ++i) {
            temp[i] = y[i] + dt * k2[i] * 0.5;
        }
        system_function(temp, k3, t + dt * 0.5);
        
        // Quatrième terme: k4 = f(y + dt*k3, t + dt)
        for (size_t i = 0; i < y.size(); ++i) {
            temp[i] = y[i] + dt * k3[i];
        }
        system_function(temp, k4, t + dt);
        
        // Combiner les termes pour mettre à jour y
        for (size_t i = 0; i < y.size(); ++i) {
            y[i] += dt * (k1[i] + 2*k2[i] + 2*k3[i] + k4[i]) / 6.0;
        }
    }

    // Méthode pour calculer l'erreur entre deux états
    Time compute_error(const State& y1, const State& y2) {
        Time max_error = 0;
        for (size_t i = 0; i < y1.size(); ++i) {
            // Calcul de l'erreur absolue et relative
            Time abs_error = std::abs(y2[i] - y1[i]);
            Time rel_error = (std::abs(y1[i]) > 1e-10) ? abs_error / std::abs(y1[i]) : abs_error;
            
            // Prendre le maximum des deux erreurs
            Time error = std::max(abs_error / m_abs_tolerance, rel_error / m_rel_tolerance);
            max_error = std::max(max_error, error);
        }
        return max_error;
    }
    
    // Vérification de la validité de l'état (éviter les NaN et les valeurs trop grandes)
    bool is_state_valid(const State& y) {
        for (size_t i = 0; i < y.size(); ++i) {
            if (std::isnan(y[i]) || std::isinf(y[i]) || std::abs(y[i]) > 1e10) {
                return false;
            }
        }
        return true;
    }
};

    } // namespace Solver
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
