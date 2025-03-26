#include "voiture.hpp"
#include <cmath>

Voiture::Voiture() : speed(2.0f), angle(0.0f), turnSpeed(2.0f) {
    position = sf::Vector2f(375, 275);
}

void Voiture::updatePosition(Map& map) {
    bool inGrass = map.isOnGrass(position, 50);
    force.setFriction(inGrass);

    if (inGrass) {
        speed *= 0.95f; // Réduction de la vitesse sur l'herbe
    } else {
        speed = 2.0f;
    }

    angle += turnSpeed * 0.01f; // Ajustement automatique de la direction
    position.x += speed * std::cos(angle);
    position.y += speed * std::sin(angle);
}
