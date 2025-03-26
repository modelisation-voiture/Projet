#include "force.hpp"
#include "map.hpp"
#include <iostream>
#include <fstream>
#include <cmath>

int main() {
    Voiture voiture(0.0, 0.0, 0.0, 53.0, 0.3);

    ForceMotrice moteur(500.0);
    double coeff_frottement = 0.02;
    ForceFrottement frottement(coeff_frottement);
    ForceFreinage frein(2.0);
    ForceVirage virage(0.0);
    ForceAerodynamique forceAir(0.9); // Fair = (1/2)⋅ρ⋅Cx*A*v*v      


    std::vector<Force*> forces = {&moteur, &frottement, &frein, &virage, &forceAir};
    Map carte;
    std::ofstream fichier("trajectoire.txt");

    double dt = 0.01;

    // Durée des phases cumulée
    const double t1 = 10.0; //10.0; accélération droite
    const double t2 = t1 + 5.0; //15.0; virage sinusoïdal (ellipse)
    const double t3 = t2 + 2.0; //17.0; roulage libre
    const double t4 = t3 + 2.0; //19.0; freinage en ligne droite
    const double t5 = t4 + 10.0; //29.0; ré-accélération
    const double t6 = t5 + 5.0; //34.0; virage constant sous freinage (glissement)
    const double t7 = t6 + 2.0; //36.0; roulage final

    for (double t = 0; t <= t7; t += dt) {
        double fx = 0, fy = 0;
        double angle_braquage = 0;
        int etat = 0;

        voiture.activerFrein(false);

        if (t <= t1) {
            // Phase 1 : accélération droite
            auto [fx_m, fy_m] = moteur.calculer_force(voiture);
            fx += fx_m;
            fy += fy_m;
            etat = 1;
        }
        else if (t > t1 && t <= t2) {
            // Phase 2 : virage sinusoïdal (ellipse)
            double t_local = t - t1;
            angle_braquage = 40.0 * std::sin((t_local / (t2 - t1)) * 2 * M_PI);
            auto [fx_m, fy_m] = moteur.calculer_force(voiture);
            fx += fx_m;
            fy += fy_m;
            etat = 1;
        }
        else if (t > t2 && t <= t3) {
            // Phase 3 : roulage libre
            etat = 0;
        }
        else if (t > t3 && t <= t4) {
            // Phase 4 : freinage en ligne droite
            voiture.activerFrein(true);
            auto [fx_f, fy_f] = frein.calculer_force(voiture);
            fx += fx_f;
            fy += fy_f;
            etat = -1;
        }
        else if (t > t4 && t <= t5) {
            // Phase 5 : ré-accélération
            auto [fx_m, fy_m] = moteur.calculer_force(voiture);
            fx += fx_m;
            fy += fy_m;
            etat = 1;
        }
        else if (t > t5 && t <= t6) {
            // Phase 6 : virage constant sous freinage (glissement)
            angle_braquage = -60.0;
            voiture.activerFrein(true);
            auto [fx_f, fy_f] = frein.calculer_force(voiture);
            fx += fx_f;
            fy += fy_f;
            etat = -1;
        }
        else {
            // Phase 7 : roulage final
            etat = 0;
        }

        // Frottement (toujours actif)
        auto [fx_frott, fy_frott] = frottement.calculer_force(voiture);
        fx += fx_frott;
        fy += fy_frott;

        // Virage actualisé
        virage = ForceVirage(angle_braquage);
        auto [fx_vir, fy_vir] = virage.calculer_force(voiture);
        fx += fx_vir;
        fy += fy_vir;

        // Force aérodynamique
        auto [fx_air, fy_air] = forceAir.calculer_force(voiture);
        fx += fx_air;
        fy += fy_air;

        // Mise à jour par RK4
        voiture.updatePositionRK4(dt, fx, fy, coeff_frottement, angle_braquage);

        fichier << voiture.getX() << "," << voiture.getY() << "," << etat << "\n";
        carte.ajouterPosition(voiture.getX(), voiture.getY());

        if (static_cast<int>(t * 100) % 100 == 0) {
            std::cout << "t = " << t << "s | Pos = (" << voiture.getX() << ", " << voiture.getY()
                      << ") | V = " << voiture.getVitesse() << " m/s"
                      << " | Etat = " << (etat == 1 ? "Accélère" : etat == -1 ? "Freine" : "Libre")
                      << " | Angle = " << voiture.getAngle() << "°"
                      << "\n";
        }
    }

    fichier.close();
    carte.sauvegarderTrajectoire("trajectoire.txt");
    return 0;
}
