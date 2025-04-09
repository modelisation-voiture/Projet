#ifndef MAP_HPP
#define MAP_HPP

#include <SFML/Graphics.hpp>

class Map {
public:
    sf::VertexArray track, grass;
    sf::Texture trackTexture, grassTexture;

    Map();
    bool loadTextures();
    void generateCircuit();
    bool isOnGrass(const sf::Vector2f& position, float trackWidth);
};

#endif // MAP_HPP


