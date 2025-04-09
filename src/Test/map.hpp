#ifndef MAP_HPP
#define MAP_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <SFML/System.hpp>

class Map {
public:
    sf::VertexArray track, grass;
    sf::Texture trackTexture, grassTexture;

    std::vector<sf::Vector2f> outerBoundary;
    std::vector<sf::Vector2f> innerBoundary;
    std::vector<sf::Vector2f> waypoints;

    Map();
    bool loadTextures();
    void generateCircuit();
    bool isOnGrass(const sf::Vector2f& position, float trackWidth);
    void createTriangleTrack();
    void createInfinityTrack();

};

#endif // MAP_HPP


