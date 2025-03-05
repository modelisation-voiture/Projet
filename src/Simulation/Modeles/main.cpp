#include <iostream>
#include "VoitureRC.hpp"
#include "ForceMotrice.hpp"

int main() {
    // Création d'un véhicule avec une masse de 1.5 kg et un moment d'inertie de 0.1 kg.m²
    VoitureRC voiture(3, 0.1);
    
    // Création d'une Force motrice de 50 Newtons
    ForceMotrice moteur(50.0);
    
    // Ajout de la Force motrice au véhicule
    voiture.ajouterForce(&moteur);

    // Application des Forces
    voiture.appliquerForces();

    // Mise à jour de l'état du véhicule
    voiture.mettreAJourEtat();

    // Affichage des vitesses pour vérifier les résultats
    std::cout << "Vitesse longitudinale (Vx) : " << voiture.getVx() << " m/s" << std::endl;
    std::cout << "Vitesse latérale (Vy) : " << voiture.getVy() << " m/s" << std::endl;

    return 0;
}
