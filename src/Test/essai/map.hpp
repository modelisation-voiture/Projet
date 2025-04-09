/*
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

*/

#ifndef MAP_HPP
#define MAP_HPP

#include <SFML/Graphics.hpp>
#include <string>

class Map {
public:
    sf::Texture trackTexture;
    sf::Texture grassTexture;

    sf::VertexArray track;
    sf::VertexArray grass;

    bool loadTextures(int mapId);
    sf::VertexArray createTrack(int mapId);

    sf::VertexArray createTrackOvale();
    sf::VertexArray createTrackSerpent();
    sf::VertexArray createTrackCarre();
    sf::VertexArray createTrackDoubleBoucle();
    sf::VertexArray createTrackSpirale();
};

#endif // MAP_HPP
