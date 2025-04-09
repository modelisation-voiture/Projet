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

void Map::createTriangleTrack() {
    // Simple triangle équilatéral
    outerBoundary = {
        {300, 200}, {800, 200}, {550, 600}, {300, 200}
    };

    innerBoundary = {
        {400, 300}, {700, 300}, {550, 500}, {400, 300}
    };

    waypoints = {
        {550, 200}, {750, 400}, {350, 400}
    };
}

void Map::createInfinityTrack() {
    // Forme de 8 couché (deux cercles liés par le centre)
    outerBoundary.clear();
    innerBoundary.clear();
    waypoints.clear();

    float cx1 = 400, cy = 360;
    float cx2 = 880;
    float r = 150;
    int points = 40;

    for (int i = 0; i <= points; ++i) {
        float angle = 2 * M_PI * i / points;
        outerBoundary.push_back({cx1 + r * cos(angle), cy + r * sin(angle)});
    }
    for (int i = 0; i <= points; ++i) {
        float angle = 2 * M_PI * i / points;
        outerBoundary.push_back({cx2 + r * cos(angle), cy + r * sin(angle)});
    }

    for (int i = 0; i <= points; ++i) {
        float angle = 2 * M_PI * i / points;
        innerBoundary.push_back({cx1 + (r - 50) * cos(angle), cy + (r - 50) * sin(angle)});
    }
    for (int i = 0; i <= points; ++i) {
        float angle = 2 * M_PI * i / points;
        innerBoundary.push_back({cx2 + (r - 50) * cos(angle), cy + (r - 50) * sin(angle)});
    }

    waypoints = {
        {640, 360}, {440, 360}, {840, 360}
    };
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


