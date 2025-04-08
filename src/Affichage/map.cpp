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


/*
#include "map.hpp"
#include <cmath>

bool Map::loadTextures(int mapId) {
    switch (mapId) {
        case 1:
            return trackTexture.loadFromFile("../../assets/asphalt.jpg") &&
                   grassTexture.loadFromFile("../../assets/grass.png");
        case 2:
            return trackTexture.loadFromFile("../../assets/Roads0096_1_350.jpg") &&
                   grassTexture.loadFromFile("../../assets/DebrisStone0003_1_350.jpg");
        case 3:
            return trackTexture.loadFromFile("../../assets/Roads0102_1_350.jpg") &&
                   grassTexture.loadFromFile("../../assets/grass1.png");
        case 4:
            return trackTexture.loadFromFile("../../assets/AsphaltCloseups0031_1_350.jpg") &&
                   grassTexture.loadFromFile("../../assets/DebrisStone0028_1_350.jpg");
        case 5:
            return trackTexture.loadFromFile("../../assets/Roads0136_1_350.jpg") &&
                   grassTexture.loadFromFile("../../assets/grass.png");
        default:
            return false;
    }
}

// Formes simplifiées pour les 5 pistes
sf::VertexArray Map::createTrack(int mapId) {
    switch (mapId) {
        case 1: return createTrackOvale();
        case 2: return createTrackSerpent();
        case 3: return createTrackCarre();
        case 4: return createTrackDoubleBoucle();
        case 5: return createTrackSpirale();
        default: return sf::VertexArray();
    }
}

sf::VertexArray Map::createTrackOvale() {
    const int numPoints = 100;
    sf::VertexArray track(sf::TriangleStrip, numPoints * 2);
    float cx = 400, cy = 300, rx = 200, ry = 100, width = 40;

    for (int i = 0; i < numPoints; ++i) {
        float angle = i * 2 * M_PI / numPoints;
        float dx = std::cos(angle), dy = std::sin(angle);
        float x1 = cx + (rx - width) * dx;
        float y1 = cy + (ry - width) * dy;
        float x2 = cx + (rx + width) * dx;
        float y2 = cy + (ry + width) * dy;

        track[i * 2].position = {x1, y1};
        track[i * 2 + 1].position = {x2, y2};
    }
    return track;
}

sf::VertexArray Map::createTrackSerpent() {
    const int numPoints = 120;
    sf::VertexArray track(sf::TriangleStrip, numPoints * 2);
    float width = 40;
    for (int i = 0; i < numPoints; ++i) {
        float t = i / 10.0;
        float x = 100 + i * 5;
        float y = 300 + std::sin(t) * 100;

        sf::Vector2f dir(std::cos(t), std::cos(t + M_PI/2));
        dir /= std::sqrt(dir.x*dir.x + dir.y*dir.y);

        sf::Vector2f offset = sf::Vector2f(-dir.y, dir.x) * width;

        track[i * 2].position = sf::Vector2f(x, y) - offset;
        track[i * 2 + 1].position = sf::Vector2f(x, y) + offset;
    }
    return track;
}

sf::VertexArray Map::createTrackCarre() {
    sf::VertexArray track(sf::TriangleStrip, 8);
    float w = 50;

    std::vector<sf::Vector2f> corners = {
        {200, 200}, {600, 200},
        {600, 200}, {600, 400},
        {600, 400}, {200, 400},
        {200, 400}, {200, 200}
    };

    for (int i = 0; i < 4; ++i) {
        sf::Vector2f dir = corners[i*2+1] - corners[i*2];
        sf::Vector2f normal(-dir.y, dir.x);
        float len = std::sqrt(normal.x*normal.x + normal.y*normal.y);
        normal /= len;

        track[i*2].position = corners[i*2] + normal * w;
        track[i*2+1].position = corners[i*2] - normal * w;
    }
    return track;
}

sf::VertexArray Map::createTrackDoubleBoucle() {
    const int numPoints = 100;
    sf::VertexArray track(sf::TriangleStrip, numPoints * 2);
    float width = 40;

    for (int i = 0; i < numPoints; ++i) {
        float angle = i * 2 * M_PI / numPoints;
        float r = 100 + 80 * std::sin(2 * angle);
        float x = 400 + r * std::cos(angle);
        float y = 300 + r * std::sin(angle);

        float dx = -r * std::sin(angle);
        float dy = r * std::cos(angle);
        float len = std::sqrt(dx*dx + dy*dy);
        sf::Vector2f offset(-dy/len * width, dx/len * width);

        track[i*2].position = {x - offset.x, y - offset.y};
        track[i*2+1].position = {x + offset.x, y + offset.y};
    }
    return track;
}

sf::VertexArray Map::createTrackSpirale() {
    const int numPoints = 100;
    sf::VertexArray track(sf::TriangleStrip, numPoints * 2);
    float width = 30;

    for (int i = 0; i < numPoints; ++i) {
        float angle = i * 0.2;
        float r = 50 + 2 * i;
        float x = 400 + r * std::cos(angle);
        float y = 300 + r * std::sin(angle);

        float dx = -r * std::sin(angle);
        float dy = r * std::cos(angle);
        float len = std::sqrt(dx*dx + dy*dy);
        sf::Vector2f offset(-dy/len * width, dx/len * width);

        track[i*2].position = {x - offset.x, y - offset.y};
        track[i*2+1].position = {x + offset.x, y + offset.y};
    }
    return track;
}
*/