#include "VoitureRC.hpp"
#include <iostream>

VoitureRC::VoitureRC(double masse, double inertie)
    : m(masse), Iz(inertie), Vx(0), Vy(0), psi(0) {}

void VoitureRC::ajouterForce(Force* Force) {
    Forces.push_back(Force);
}

void VoitureRC::appliquerForces() {
    for (auto Force : Forces) {
        Force->calculer();
    }
}

void VoitureRC::mettreAJourEtat() {
    std::cout << "Mise à jour de l'état du véhicule..." << std::endl;
    // Calcul des vitesses longitudinale et latérale
    // (Modèle simplifié, à remplacer par un modèle plus réaliste)
    Vx = Forces[0]->calculer() / m;
    Vy = 0;
    psi = 0;
    
}

double VoitureRC::getVx() const { return Vx; }
double VoitureRC::getVy() const { return Vy; }
