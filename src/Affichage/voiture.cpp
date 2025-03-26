#include "voiture.hpp"
#include <cmath>

Voiture::Voiture() : speed(0.0f), power(0.0f), angle(0.0f), wheelAngle(0.0f),
                     maxPower(5.0f), maxTurnAngle(30.0f * M_PI / 180.0f) {
    position = sf::Vector2f(625, 300);
}

void Voiture::accelerate(float value) {
    power += value;
    if (power > maxPower) power = maxPower;
}

void Voiture::brake(float value) {
    power -= value;
    if (power < 0) power = 0; // Pas de vitesse négative
}

void Voiture::turn(float value) {
    wheelAngle += value;
    if (wheelAngle > maxTurnAngle) wheelAngle = maxTurnAngle;
    if (wheelAngle < -maxTurnAngle) wheelAngle = -maxTurnAngle;
}

void Voiture::updatePosition(Map& map) {
    // Vérifier si on est sur l'herbe
    bool inGrass = map.isOnGrass(position, 50);
    force.setFriction(inGrass);

    // Appliquer la puissance au mouvement
    float friction = force.computeFriction(speed);
    speed += power * 0.1f - friction; // Moteur + résistance

    if (speed > maxPower) speed = maxPower; // Limite de vitesse
    if (speed < 0.1f) speed = 0; // Arrêt si très lent

    // Modifier l'angle selon l'angle des roues
    angle += wheelAngle * (speed / maxPower) * 0.05f;

    // Mettre à jour la position
    position.x += speed * cos(angle);
    position.y += speed * sin(angle);
}
