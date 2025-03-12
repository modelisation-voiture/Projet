#include "VoitureRC.hpp"
#include <iostream>
#include <cmath>

#define G 9.81  // Accélération gravitationnelle

// Constructeur
VoitureRC::VoitureRC(double masse, double inertie)
    : m(masse), Iz(inertie), Vx(0), Vy(0), psi(0), delta(0) {}

// Ajouter une force au véhicule
void VoitureRC::ajouterForce(Force* force) {
    Forces.push_back(force);
}

// Appliquer toutes les forces enregistrées
void VoitureRC::appliquerForces() {
    double forceLongitudinale = 0;
    double forceLatérale = 0;

    for (auto force : Forces) {
        forceLongitudinale += force->calculer();
    }

    // Modèle simplifié : Force latérale dépend de l'angle de braquage
    forceLatérale = m * Vx * tan(delta) / 2.0;

    // Appliquer l'effet des forces sur les accélérations
    Ax = forceLongitudinale / m;  // Accélération longitudinale
    Ay = forceLatérale / m;       // Accélération latérale
}

// Mise à jour de l'état du véhicule à chaque frame
void VoitureRC::mettreAJourEtat(double dt) {
    appliquerForces();  // Calculer les forces avant de mettre à jour

    // Mettre à jour les vitesses avec les accélérations
    Vx += Ax * dt;
    Vy += Ay * dt;

    // Calcul du changement d'angle du véhicule (dérive due à la vitesse latérale)
    psi += (Vy / Vx) * dt;

    // Mise à jour de la position
    x += Vx * cos(psi) * dt - Vy * sin(psi) * dt;
    y += Vx * sin(psi) * dt + Vy * cos(psi) * dt;
}

// Définir l'angle de braquage du véhicule
void VoitureRC::setDelta(double newDelta) {
    delta = newDelta;
}

// Obtenir les vitesses
double VoitureRC::getVx() const { return Vx; }
double VoitureRC::getVy() const { return Vy; }

// Obtenir la position
double VoitureRC::getX() const { return x; }
double VoitureRC::getY() const { return y; }
double VoitureRC::getPsi() const { return psi; }
