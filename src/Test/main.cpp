// Version 3.0 de Nadir ca va etre un banger

#include <SFML/Graphics.hpp>
#include "menu.hpp"
#include "force.hpp"  // Contient déjà voiture.hpp
#include "map.hpp"
#include <cmath>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <vector>

// Déclaration des textures globales
sf::Texture trackTexture, grassTexture, borderTexture, carTexture;

// Fonction pour charger les textures
bool loadTextures() {
    if (!trackTexture.loadFromFile("../../assets/asphalt.jpg") ||
        !grassTexture.loadFromFile("../../assets/grass.png") ||
        !borderTexture.loadFromFile("../../assets/outer.png") ||
        !carTexture.loadFromFile("../../assets/car.png")) {
        return false;
    }
    return true;
}

// Structure pour représenter un waypoint
struct Waypoint {
    sf::Vector2f position;
    float targetRadius;
};

// Fonction pour créer la zone d'herbe autour du circuit
sf::VertexArray createGrass(const sf::VertexArray& track, float extraWidth) {
    sf::VertexArray grass(sf::TriangleStrip, track.getVertexCount() + 2);
    sf::Vector2f center(400, 300);
    for (size_t i = 0; i < track.getVertexCount(); i++) {
        sf::Vector2f pos = track[i].position;
        sf::Vector2f dir = pos - center;
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len != 0) dir /= len;
        grass[i].position = pos + dir * extraWidth;
        grass[i].texCoords = sf::Vector2f(pos.x, pos.y);
    }
    grass[track.getVertexCount()] = grass[0];
    grass[track.getVertexCount() + 1] = grass[1];
    return grass;
}

// Fonction pour générer une piste avec waypoints
std::tuple<sf::VertexArray, std::vector<Waypoint>, float> createTrackWithWaypoints() {
    const int numPoints = 100;
    sf::VertexArray track(sf::TriangleStrip, numPoints * 2);
    std::vector<Waypoint> waypoints;
    float metersPerPixel = 0.1f;
    float centerX = 400, centerY = 300;
    float radiusX = 25.0f;
    float radiusY = 15.0f;
    float trackWidth = 5.0f;
    float maxDistance = 0.0f;

    for (int i = 0; i < numPoints; i++) {
        float angle = (i / (float)numPoints) * 2 * M_PI;
        float rX = radiusX + 3.0f * std::sin(3 * angle);
        float rY = radiusY + 2.0f * std::cos(2 * angle);
        float x1 = centerX + (rX - trackWidth) * std::cos(angle) / metersPerPixel;
        float y1 = centerY + (rY - trackWidth) * std::sin(angle) / metersPerPixel;
        float x2 = centerX + (rX + trackWidth) * std::cos(angle) / metersPerPixel;
        float y2 = centerY + (rY + trackWidth) * std::sin(angle) / metersPerPixel;
        track[i * 2].position = sf::Vector2f(x1, y1);
        track[i * 2 + 1].position = sf::Vector2f(x2, y2);
        if (i % 5 == 0) {
            float midX = (x1 + x2) / 2;
            float midY = (y1 + y2) / 2;
            float curvature = std::abs(std::sin(3 * angle)) + std::abs(std::cos(2 * angle));
            float targetRadius = rX * (1.0f - 0.3f * curvature);
            waypoints.push_back({sf::Vector2f(midX, midY), targetRadius});
        }
        float dist1 = std::hypot(x1 - centerX, y1 - centerY);
        float dist2 = std::hypot(x2 - centerX, y2 - centerY);
        maxDistance = std::max({maxDistance, dist1, dist2});
    }
    track[numPoints * 2 - 2].position = track[0].position;
    track[numPoints * 2 - 1].position = track[1].position;
    return {track, waypoints, maxDistance};
}

// Détecter si la voiture est sur l'herbe
bool isOnGrass(const sf::Vector2f& carPosition, const sf::VertexArray& track, float trackWidth) {
    float centerX = 400, centerY = 300;
    float radiusX = 250, radiusY = 150;
    float dx = carPosition.x - centerX;
    float dy = carPosition.y - centerY;
    float distance = std::sqrt(dx * dx + dy * dy);
    float outerRadius = std::max(radiusX, radiusY) + trackWidth;
    float innerRadius = std::min(radiusX, radiusY) - trackWidth;
    return (distance > outerRadius || distance < innerRadius);
}

// Trouver le prochain waypoint
int findNextWaypoint(const Voiture& voiture, const std::vector<Waypoint>& waypoints, int currentWaypoint) {
    sf::Vector2f carPos(voiture.getX(), voiture.getY());
    float carAngle = voiture.getAngle() * M_PI / 180.0f;
    sf::Vector2f carDir(std::cos(carAngle), std::sin(carAngle));
    int lookAhead = 3, bestPoint = currentWaypoint;
    float bestScore = -1;
    int waypointsCount = waypoints.size();
    for (int i = 0; i < lookAhead; i++) {
        int pointIndex = (currentWaypoint + i) % waypointsCount;
        sf::Vector2f toWaypoint = waypoints[pointIndex].position - carPos;
        float distance = std::sqrt(toWaypoint.x * toWaypoint.x + toWaypoint.y * toWaypoint.y);
        if (distance < 10) {
            bestPoint = (pointIndex + 1) % waypointsCount;
            break;
        }
        toWaypoint /= distance;
        float dotProduct = carDir.x * toWaypoint.x + carDir.y * toWaypoint.y;
        float score = dotProduct * (1.0f - distance / 500.0f);
        if (score > bestScore) {
            bestScore = score;
            bestPoint = pointIndex;
        }
    }
    return bestPoint;
}

// Calculer un angle de braquage adaptatif
float calculateAdaptiveSteeringAngle(const Voiture& voiture, const Waypoint& target) {
    sf::Vector2f carPos(voiture.getX(), voiture.getY());
    sf::Vector2f toTarget = target.position - carPos;
    float distance = std::sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);
    if (distance < 1e-3) return 0.0f;
    toTarget /= distance;
    float carAngle = voiture.getAngle() * M_PI / 180.0f;
    sf::Vector2f carDir(std::cos(carAngle), std::sin(carAngle));
    float crossProduct = carDir.x * toTarget.y - carDir.y * toTarget.x;
    float dotProduct = carDir.x * toTarget.x + carDir.y * toTarget.y;
    float angleToTarget = std::atan2(crossProduct, dotProduct);
    float sensitivity = 1.0f;
    float steeringAngle = angleToTarget * sensitivity;
    steeringAngle = std::max(-1.0f, std::min(1.0f, steeringAngle));
    return steeringAngle;
}

int main() {
    // ======== Intégration du menu avant la simulation =========
    int circuitChoisi = afficherMenu();
    if (circuitChoisi == -1) return 0; // quitter si demandé

    sf::RenderWindow window(sf::VideoMode(800, 600), "RC Car Simulation - Adaptive Steering");
    window.setFramerateLimit(60);

    float tempsDepuisDernierUpdateTexte = 0.0f;

    // ======== Création du circuit selon le choix ============
    Map map;
    switch (circuitChoisi) {
        case 1:
            map.generateCircuit();
            break;
        case 2:
            map.createTriangleTrack();
            break;
        case 3:
            map.createInfinityTrack();
            break;
    }

    // Chargement de la map
    if (!map.loadTextures()) return -1;

    auto [track, waypoints, maxDistance] = createTrackWithWaypoints();
    sf::VertexArray grass = createGrass(track, 40);

    sf::Font font;
    if (!font.loadFromFile("../../assets/Roboto-Regular.ttf")) {
        std::cerr << "Erreur chargement police\n";
        return -1;
    }

    sf::Text hudText;
    hudText.setFont(font);
    hudText.setCharacterSize(18);
    hudText.setFillColor(sf::Color::White);
    hudText.setPosition(10, 10);

    sf::CircleShape waypointMarker(3);
    waypointMarker.setFillColor(sf::Color::Yellow);
    waypointMarker.setOrigin(3, 3);

    sf::Texture carTexture;
    if (!carTexture.loadFromFile("../../assets/car_pink.png")) return -1;
    sf::Sprite carSprite;
    carSprite.setTexture(carTexture);
    carSprite.setScale(0.12f, 0.12f);

    sf::ConvexShape directionIndicator;
    directionIndicator.setPointCount(3);
    directionIndicator.setPoint(0, sf::Vector2f(0, -20));
    directionIndicator.setPoint(1, sf::Vector2f(10, 10));
    directionIndicator.setPoint(2, sf::Vector2f(-10, 10));
    directionIndicator.setFillColor(sf::Color::Blue);
    directionIndicator.setOrigin(0, 0);

    sf::RenderStates trackState, grassState;
    map.trackTexture.setRepeated(true);
    map.grassTexture.setRepeated(true);
    trackState.texture = &map.trackTexture;
    grassState.texture = &map.grassTexture;

    Voiture voiture(150, 300, 0, 20.0, 0.3);
    ForceMotriceProgressive moteur(10000.0, 10.0);
    ForceFrottement frottement(0.02);
    ForceFreinage frein(0.3);
    ForceAerodynamique air(0.0072);
    ForceVirage virage(0.0);
    ForceFreinGlisse freinGlisse(10);
    std::vector<Force*> forces = {&moteur, &frottement, &air, &frein, &virage, &freinGlisse};

    bool autonomousMode = true;
    int currentWaypoint = 0;
    sf::Vertex targetLine[2];
    targetLine[0].color = sf::Color::Red;
    targetLine[1].color = sf::Color::Red;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A)
                autonomousMode = !autonomousMode;
        }

        int etat = 0;
        double angle_braquage = 0.0;
        voiture.activerFrein(false);

        if (autonomousMode) {
            currentWaypoint = findNextWaypoint(voiture, waypoints, currentWaypoint);
            angle_braquage = calculateAdaptiveSteeringAngle(voiture, waypoints[currentWaypoint]);
            float speedFactor = waypoints[currentWaypoint].targetRadius / 250.0f;
            speedFactor = std::max(0.3f, std::min(1.0f, speedFactor));
            etat = 1;

            sf::Vector2f carPos(voiture.getX(), voiture.getY());
            if (isOnGrass(carPos, track, 50)) {
                float minDist = 1000000;
                int closestWaypoint = currentWaypoint;
                for (int i = 0; i < waypoints.size(); i++) {
                    float dx = waypoints[i].position.x - carPos.x;
                    float dy = waypoints[i].position.y - carPos.y;
                    float dist = dx*dx + dy*dy;
                    if (dist < minDist) {
                        minDist = dist;
                        closestWaypoint = i;
                    }
                }
                currentWaypoint = closestWaypoint;
                angle_braquage = calculateAdaptiveSteeringAngle(voiture, waypoints[currentWaypoint]);
                etat = 1;
            }

            targetLine[0].position = sf::Vector2f(voiture.getX(), voiture.getY());
            targetLine[1].position = waypoints[currentWaypoint].position;
        } else {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                etat = 1;
                voiture.setAccelerationActive(true);
            } else {
                voiture.setAccelerationActive(false);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
                voiture.activerFrein(true);
                etat = -1;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) angle_braquage = -1.0;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) angle_braquage = 1.0;
        }

        bool freinMainActif = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
        voiture.setFreinMainActif(freinMainActif);
        voiture.setAccelerationActive(etat == 1);

        double fx = 0, fy = 0;
        virage = ForceVirage(angle_braquage);
        for (auto* f : forces) {
            if (f == &moteur && etat != 1) continue;
            if (f == &frein && !voiture.isFreinActif()) continue;
            if (f == &freinGlisse && !freinMainActif) continue;
            auto [fx_i, fy_i] = f->calculer_force(voiture);
            fx += fx_i;
            fy += fy_i;
        }

        voiture.appliquerForce(fx, fy);
        voiture.updatePositionRK4(0.01, fx, fy, 0.02, angle_braquage);

        float carWidth = carSprite.getGlobalBounds().width;
        float carHeight = carSprite.getGlobalBounds().height;
        if (voiture.getX() < 0) voiture.setX(0);
        if (voiture.getX() > window.getSize().x - carWidth) voiture.setX(window.getSize().x - carWidth);
        if (voiture.getY() < 0) voiture.setY(0);
        if (voiture.getY() > window.getSize().y - carHeight) voiture.setY(window.getSize().y - carHeight);

        carSprite.setPosition(voiture.getX(), voiture.getY());
        carSprite.setRotation(voiture.getAngle() + 90);
        directionIndicator.setPosition(voiture.getX(), voiture.getY());
        directionIndicator.setRotation(voiture.getAngle() + angle_braquage + 90);

        static sf::Clock clock;
        float dt = clock.restart().asSeconds();
        tempsDepuisDernierUpdateTexte += dt;
        if (tempsDepuisDernierUpdateTexte >= 0.25f) {
            std::ostringstream oss;
            oss << "Vitesse : " << std::fixed << std::setprecision(2) << voiture.getVitesse() << " m/s\n"
                << "Angle   : " << std::fixed << std::setprecision(2) << voiture.getAngle() << " degres\n"
                << "Waypoint actuel : " << currentWaypoint << "/" << waypoints.size() << "\n"
                << "Mode : " << (autonomousMode ? "AUTONOME (A pour desactiver)" : "MANUEL (A pour activer)");
            hudText.setString(oss.str());
            tempsDepuisDernierUpdateTexte = 0.0f;
        }

        window.clear();
        window.draw(grass, grassState);
        window.draw(track, trackState);

        if (autonomousMode) {
            for (int i = 0; i < waypoints.size(); i++) {
                waypointMarker.setPosition(waypoints[i].position);
                waypointMarker.setFillColor((i == currentWaypoint) ? sf::Color::Red : sf::Color::Blue);
                window.draw(waypointMarker);
            }
            window.draw(targetLine, 2, sf::Lines);
        }

        window.draw(carSprite);
        window.draw(directionIndicator);
        window.draw(hudText);
        window.display();
    }

    return 0;
}

