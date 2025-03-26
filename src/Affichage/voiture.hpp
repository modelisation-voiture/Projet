#ifndef VOITURE_HPP
#define VOITURE_HPP

#include <SFML/Graphics.hpp>
#include "map.hpp"
#include "force.hpp"

class Voiture {
public:
    sf::Sprite sprite;
    sf::Vector2f position;
    float speed;        // Vitesse actuelle
    float power;        // Puissance du moteur
    float angle;        // Direction de la voiture
    float wheelAngle;   // Angle des roues (direction)
    float maxPower;     // Puissance maximale du moteur
    float maxTurnAngle; // Angle maximal des roues
    Force force;

    Voiture();
    void updatePosition(Map& map);
    void accelerate(float value); // Augmente la puissance
    void brake(float value);      // Réduit la puissance
    void turn(float value);       // Modifie l’angle des roues
};

#endif // VOITURE_HPP
