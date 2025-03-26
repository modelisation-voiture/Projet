#ifndef VOITURERC_HPP
#define VOITURERC_HPP

#include <vector>
#include "Force.hpp"

class VoitureRC {
private:
    double m;   // Masse du véhicule
    double Iz;  // Moment d'inertie
    
    double Vx, Vy;  // Vitesses longitudinale et latérale
    double Ax, Ay;  // Accélérations correspondantes

    double x, y;  // Position du véhicule sur la piste
    double psi;   // Angle d'orientation du véhicule (cap)
    double delta; // Angle de braquage des roues avant

    std::vector<Force*> Forces;  // Liste des forces appliquées

public:
    // Constructeur
    VoitureRC(double masse, double inertie);

    // Ajouter une force au véhicule
    void ajouterForce(Force* force);

    // Appliquer toutes les forces enregistrées
    void appliquerForces();

    // Mettre à jour l'état du véhicule
    void mettreAJourEtat(double dt);

    // Définir l'angle de braquage
    void setDelta(double newDelta);

    // Getters pour les vitesses
    double getVx() const;
    double getVy() const;

    // Getters pour la position
    double getX() const;
    double getY() const;
    double getPsi() const;
};

#endif
