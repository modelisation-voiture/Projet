#include "ForceMotrice.hpp"

ForceMotrice::ForceMotrice(double puissance) : puissanceMoteur(puissance) {}

double ForceMotrice::calculer() {
    valeur = puissanceMoteur;  // Modèle simplifié
    return valeur;
}
