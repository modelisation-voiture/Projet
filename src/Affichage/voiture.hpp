#ifndef VOITURE_HPP
#define VOITURE_HPP

#include "map.hpp"
#include "force.hpp"
#include <SFML/Graphics.hpp>

class Voiture {
public:
    sf::Sprite sprite;
    sf::Vector2f position;
    float speed;
    float angle;
    float turnSpeed;
    Force force;

    Voiture();
    void updatePosition(Map& map);
};

#endif // VOITURE_HPP
