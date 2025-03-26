#include "map.hpp"
#include <cmath>

Map::Map() {
    generateCircuit();
}

bool Map::loadTextures() {
    return trackTexture.loadFromFile("../../assets/asphalt.jpg") &&
           grassTexture.loadFromFile("../../assets/grass.png");
}

void Map::generateCircuit() {
    const int numPoints = 100;
    track.setPrimitiveType(sf::TriangleStrip);
    track.resize(numPoints * 2);

    float centerX = 400, centerY = 300;
    float radiusX = 250, radiusY = 150;
    float trackWidth = 50;

    for (int i = 0; i < numPoints; i++) {
        float angle = (i / (float)numPoints) * 2 * M_PI;
        float rX = radiusX + 30 * std::sin(3 * angle);
        float rY = radiusY + 20 * std::cos(2 * angle);

        float x1 = centerX + (rX - trackWidth) * std::cos(angle);
        float y1 = centerY + (rX - trackWidth) * std::sin(angle);
        
        float x2 = centerX + (rX + trackWidth) * std::cos(angle);
        float y2 = centerY + (rX + trackWidth) * std::sin(angle);

        track[i * 2].position = sf::Vector2f(x1, y1);
        track[i * 2 + 1].position = sf::Vector2f(x2, y2);
    }
}

bool Map::isOnGrass(const sf::Vector2f& position, float trackWidth) {
    float centerX = 400, centerY = 300;
    float dx = position.x - centerX;
    float dy = position.y - centerY;
    float distance = std::sqrt(dx * dx + dy * dy);

    float outerRadius = 250 + trackWidth;
    float innerRadius = 150 - trackWidth;

    return (distance > outerRadius || distance < innerRadius);
}
