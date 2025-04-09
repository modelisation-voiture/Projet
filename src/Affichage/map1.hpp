#ifndef MAP1_HPP
#define MAP1_HPP

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

private:
    sf::VertexArray createTrackOvale();
    sf::VertexArray createTrackSerpent();
    sf::VertexArray createTrackCarre();
    sf::VertexArray createTrackDoubleBoucle();
    sf::VertexArray createTrackSpirale();
};

#endif // MAP1_HPP
