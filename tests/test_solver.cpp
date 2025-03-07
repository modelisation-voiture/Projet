// tests/test_solver.cpp
// Programme simple pour tester le solver et le modèle de voiture RC

#include "Simulation/Modeles/ModeleRC.hpp"
#include "Simulation/Solver/Solver.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>

int main() {
    using namespace Simulation::Modeles;
    using namespace Simulation::Solver;
    
    std::cout << "=== Test du solver pour la simulation de voiture RC ===" << std::endl;
    
    // Création du modèle avec des paramètres réalistes pour une voiture RC
    ModeleRC::Parametres params{
        .masse = 1.5,         // 1.5 kg
        .inertie = 0.025,     // moment d'inertie en kg.m²
        .longueur = 0.3,      // 30 cm
        .mu = 0.1,            // coefficient de frottement
        .mu_lateral = 0.8     // coefficient de frottement latéral
    };
    
    ModeleRC modele(params);
    
    // État initial: [x, y, theta, v, omega]
    ModeleRC::Etat etat_initial = {0.0, 0.0, 0.0, 0.0, 0.0};
    
    // Tester plusieurs scénarios de contrôle
    std::vector<ModeleRC::Controle> scenarios = {
        {0.8, 0.0},    // Accélération en ligne droite
        {0.8, 0.2},    // Virage léger à droite
        {0.8, -0.2},   // Virage léger à gauche
        {0.8, 0.5},    // Virage serré à droite
        {0.0, 0.0}     // Arrêt (décélération due aux frottements)
    };
    
    // Paramètres de simulation
    double t_debut = 0.0;
    double t_fin = 3.0;
    double dt = 0.01;
    
    // Tester différentes méthodes d'intégration
    std::vector<std::pair<OdeSolver<ModeleRC::Etat>::Method, std::string>> methodes = {
        {OdeSolver<ModeleRC::Etat>::Method::RK4, "Runge-Kutta 4"},
        {OdeSolver<ModeleRC::Etat>::Method::DormandPrince, "Dormand-Prince 5(4)"},
        {OdeSolver<ModeleRC::Etat>::Method::BulirschStoer, "Bulirsch-Stoer"}
    };
    
    for (const auto& [methode, nom] : methodes) {
        std::cout << "\nTest avec la méthode: " << nom << std::endl;
        
        OdeSolver<ModeleRC::Etat> solver(methode);
        solver.set_tolerances(1.0e-8, 1.0e-8);
        
        for (size_t i = 0; i < scenarios.size(); ++i) {
            ModeleRC::Controle ctrl = scenarios[i];
            std::cout << "  Scénario " << i+1 << ": accélération=" << ctrl.acceleration 
                      << ", direction=" << (ctrl.direction * 180 / 3.14159) << "°" << std::endl;
            
            modele.setControle(ctrl);
            
            auto start = std::chrono::high_resolution_clock::now();
            
            auto resultats = solver.solve(
                [&modele](const ModeleRC::Etat& x, ModeleRC::Etat& dxdt, double t) {
                    modele(x, dxdt, t);
                },
                etat_initial,
                t_debut,
                t_fin,
                dt
            );
            
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> duration = end - start;
            
            std::cout << "    " << resultats.size() << " points calculés en " 
                      << duration.count() << " ms" << std::endl;
            
            // Écrire les résultats dans un fichier CSV
            std::string filename = "resultats_" + std::to_string(i+1) + "_" + 
                                   std::to_string(static_cast<int>(methode)) + ".csv";
            std::ofstream fichier(filename);
            fichier << "t,x,y,theta,v,omega\n";
            
            for (size_t j = 0; j < resultats.size(); ++j) {
                const auto& etat = resultats[j];
                double t = t_debut + j * dt;
                
                fichier << std::fixed << std::setprecision(6)
                        << t << ","
                        << etat[0] << ","
                        << etat[1] << ","
                        << etat[2] << ","
                        << etat[3] << ","
                        << etat[4] << "\n";
            }
            
            // Afficher quelques statistiques
            if (!resultats.empty()) {
                const auto& etat_final = resultats.back();
                std::cout << "    Position finale: (" << etat_final[0] << ", " << etat_final[1] << ")" << std::endl;
                std::cout << "    Vitesse finale: " << etat_final[3] << " m/s" << std::endl;
            }
        }
    }
    
    std::cout << "\nTous les tests sont terminés. Vérifiez les fichiers CSV pour les résultats détaillés." << std::endl;
    
    return 0;
}