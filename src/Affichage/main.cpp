//version simple du circuit avec les couleurs vertes et grises
/*#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

// Fonction pour créer la zone d'herbe autour de la piste et au centre
sf::VertexArray createGrass(const sf::VertexArray& track, float extraWidth) {
    sf::VertexArray grass(sf::TriangleStrip, track.getVertexCount() + 2);

    sf::Vector2f center(400, 300); // Centre de la piste

    for (size_t i = 0; i < track.getVertexCount(); i++) {
        sf::Vector2f pos = track[i].position;
        sf::Vector2f direction = pos - center;
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        if (length != 0) {
            direction /= length; // Normaliser
        }

        // Étendre la bordure extérieure pour l'herbe
        grass[i].position = pos + direction * extraWidth;
        grass[i].color = sf::Color(34, 139, 34); // Vert foncé
    }

    // Fermer le circuit
    grass[track.getVertexCount()] = grass[0];
    grass[track.getVertexCount() + 1] = grass[1];

    return grass;
}

// Fonction pour créer la zone centrale en herbe
sf::VertexArray createInnerGrass(float centerX, float centerY, float radiusX, float radiusY, float trackWidth) {
    const int numPoints = 100;
    sf::VertexArray innerGrass(sf::TriangleFan, numPoints + 2);
    innerGrass[0].position = sf::Vector2f(centerX, centerY); // Centre du circuit
    innerGrass[0].color = sf::Color(34, 139, 34); // Vert foncé

    for (int i = 1; i <= numPoints + 1; i++) {
        float angle = (i / (float)numPoints) * 2 * M_PI;
        float rX = radiusX + 30 * std::sin(3 * angle); 
        float rY = radiusY + 20 * std::cos(2 * angle);

        float x = centerX + (rX - trackWidth) * std::cos(angle);
        float y = centerY + (rY - trackWidth) * std::sin(angle);

        innerGrass[i].position = sf::Vector2f(x, y);
        innerGrass[i].color = sf::Color(34, 139, 34); // Même couleur d'herbe
    }

    return innerGrass;
}

// Fonction pour créer une piste fermée
sf::VertexArray createTrack() {
    const int numPoints = 100;
    sf::VertexArray track(sf::TriangleStrip, numPoints * 2);

    float centerX = 400, centerY = 300;
    float radiusX = 250, radiusY = 150;
    float trackWidth = 50;

    for (int i = 0; i < numPoints; i++) {
        float angle = (i / (float)numPoints) * 2 * M_PI;

        float rX = radiusX + 30 * std::sin(3 * angle);
        float rY = radiusY + 20 * std::cos(2 * angle);

        float x1 = centerX + (rX - trackWidth) * std::cos(angle);
        float y1 = centerY + (rY - trackWidth) * std::sin(angle);
        
        float x2 = centerX + (rX + trackWidth) * std::cos(angle);
        float y2 = centerY + (rY + trackWidth) * std::sin(angle);

        track[i * 2].position = sf::Vector2f(x1, y1);
        track[i * 2].color = sf::Color(100, 100, 100);
        
        track[i * 2 + 1].position = sf::Vector2f(x2, y2);
        track[i * 2 + 1].color = sf::Color(50, 50, 50);
    }

    // Fermer le circuit
    track[numPoints * 2 - 2].position = track[0].position;
    track[numPoints * 2 - 1].position = track[1].position;

    return track;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "RC Car Simulation");
    window.setFramerateLimit(60);

    sf::VertexArray track = createTrack();
    sf::VertexArray grass = createGrass(track, 40);
    sf::VertexArray innerGrass = createInnerGrass(400, 300, 250, 150, 50);

    sf::Texture carTexture;
    if (!carTexture.loadFromFile("../../assets/car.png")) {
        return -1;
    }

    sf::Sprite carSprite;
    carSprite.setTexture(carTexture);
    carSprite.setScale(0.1f, 0.1f);
    carSprite.setPosition(375, 275);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(grass); 
        window.draw(innerGrass); 
        window.draw(track);
        window.draw(carSprite);

        window.display();
    }

    return 0;
}
*/

// version avec les forces et les modeles 
/*#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include "VoitureRC.hpp"
#include "ForceMotrice.hpp"

#define DT 0.1  // Pas de temps pour la simulation

// Fonction pour créer la zone d'herbe autour de la piste et au centre
sf::VertexArray createGrass(const sf::VertexArray& track, float extraWidth) {
    sf::VertexArray grass(sf::TriangleStrip, track.getVertexCount() + 2);

    sf::Vector2f center(400, 300); // Centre de la piste

    for (size_t i = 0; i < track.getVertexCount(); i++) {
        sf::Vector2f pos = track[i].position;
        sf::Vector2f direction = pos - center;
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        if (length != 0) {
            direction /= length; // Normaliser
        }

        // Étendre la bordure extérieure pour l'herbe
        grass[i].position = pos + direction * extraWidth;
        grass[i].color = sf::Color(34, 139, 34); // Vert foncé
    }

    // Fermer le circuit
    grass[track.getVertexCount()] = grass[0];
    grass[track.getVertexCount() + 1] = grass[1];

    return grass;
}

// Fonction pour créer la zone centrale en herbe
sf::VertexArray createInnerGrass(float centerX, float centerY, float radiusX, float radiusY, float trackWidth) {
    const int numPoints = 100;
    sf::VertexArray innerGrass(sf::TriangleFan, numPoints + 2);
    innerGrass[0].position = sf::Vector2f(centerX, centerY); // Centre du circuit
    innerGrass[0].color = sf::Color(34, 139, 34); // Vert foncé

    for (int i = 1; i <= numPoints + 1; i++) {
        float angle = (i / (float)numPoints) * 2 * M_PI;
        float rX = radiusX + 30 * std::sin(3 * angle); 
        float rY = radiusY + 20 * std::cos(2 * angle);

        float x = centerX + (rX - trackWidth) * std::cos(angle);
        float y = centerY + (rY - trackWidth) * std::sin(angle);

        innerGrass[i].position = sf::Vector2f(x, y);
        innerGrass[i].color = sf::Color(34, 139, 34); // Même couleur d'herbe
    }

    return innerGrass;
}
// Fonction pour créer une piste fermée
sf::VertexArray createTrack() {
    const int numPoints = 100;
    sf::VertexArray track(sf::TriangleStrip, numPoints * 2);

    float centerX = 400, centerY = 300;
    float radiusX = 250, radiusY = 150;
    float trackWidth = 50;

    for (int i = 0; i < numPoints; i++) {
        float angle = (i / (float)numPoints) * 2 * M_PI;

        float rX = radiusX + 30 * std::sin(3 * angle);
        float rY = radiusY + 20 * std::cos(2 * angle);

        float x1 = centerX + (rX - trackWidth) * std::cos(angle);
        float y1 = centerY + (rY - trackWidth) * std::sin(angle);
        
        float x2 = centerX + (rX + trackWidth) * std::cos(angle);
        float y2 = centerY + (rY + trackWidth) * std::sin(angle);

        track[i * 2].position = sf::Vector2f(x1, y1);
        track[i * 2].color = sf::Color(100, 100, 100);
        
        track[i * 2 + 1].position = sf::Vector2f(x2, y2);
        track[i * 2 + 1].color = sf::Color(50, 50, 50);
    }

    // Fermer le circuit
    track[numPoints * 2 - 2].position = track[0].position;
    track[numPoints * 2 - 1].position = track[1].position;

    return track;
}

int main() {
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 2;  // OpenGL 2.1 (compatible avec WSL)
    settings.minorVersion = 1;

    sf::RenderWindow window(sf::VideoMode(800, 600), "RC Car Simulation");
    window.setFramerateLimit(60);

    sf::VertexArray track = createTrack();

    // Charger la texture de la voiture
    sf::Texture carTexture;
    if (!carTexture.loadFromFile("../../assets/car.png")) {
        return -1;
    }

    // Initialisation du véhicule RC
    VoitureRC voiture(3, 0.1);
    ForceMotrice moteur(50.0);
    voiture.ajouterForce(&moteur);

    sf::Sprite carSprite;
    carSprite.setTexture(carTexture);
    carSprite.setScale(0.1f, 0.1f);
    carSprite.setOrigin(carTexture.getSize().x / 2, carTexture.getSize().y / 2);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Simulation de la dynamique du véhicule
        voiture.mettreAJourEtat(DT);

        // Mettre à jour l'affichage du véhicule
        carSprite.setPosition(voiture.getX(), voiture.getY());
        carSprite.setRotation(voiture.getPsi() * 180 / M_PI);  // Convertir en degrés

        window.clear();
        window.draw(track);
        window.draw(carSprite);
        window.display();
    }

    return 0;
}
*/

//version ameliorée avec les textures 
/*
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>


// Déclaration des textures
sf::Texture trackTexture, grassTexture, borderTexture;

// Fonction pour charger les textures
bool loadTextures() {
    if (!trackTexture.loadFromFile("../../assets/asphalt.jpg") ||
        !grassTexture.loadFromFile("../../assets/grass.png") ||
        !borderTexture.loadFromFile("../../assets/outer.png")) {
        return false;
    }
    return true;
}

// Fonction pour créer la zone d'herbe autour de la piste et au centre
sf::VertexArray createGrass(const sf::VertexArray& track, float extraWidth) {
    sf::VertexArray grass(sf::TriangleStrip, track.getVertexCount() + 2);
    sf::Vector2f center(400, 300);

    for (size_t i = 0; i < track.getVertexCount(); i++) {
        sf::Vector2f pos = track[i].position;
        sf::Vector2f direction = pos - center;
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        if (length != 0) {
            direction /= length; // Normalisation
        }

        grass[i].position = pos + direction * extraWidth;
        grass[i].texCoords = sf::Vector2f(pos.x, pos.y); // Appliquer les coordonnées UV
    }

    // Fermer la boucle
    grass[track.getVertexCount()] = grass[0];
    grass[track.getVertexCount() + 1] = grass[1];

    return grass;
}

// Fonction pour créer la zone centrale en herbe
sf::VertexArray createInnerGrass(float centerX, float centerY, float radiusX, float radiusY, float trackWidth) {
    const int numPoints = 100;
    sf::VertexArray innerGrass(sf::TriangleFan, numPoints + 2);
    innerGrass[0].position = sf::Vector2f(centerX, centerY);

    for (int i = 1; i <= numPoints + 1; i++) {
        float angle = (i / (float)numPoints) * 2 * M_PI;
        float rX = radiusX + 30 * std::sin(3 * angle);
        float rY = radiusY + 20 * std::cos(2 * angle);

        float x = centerX + (rX - trackWidth) * std::cos(angle);
        float y = centerY + (rY - trackWidth) * std::sin(angle);

        innerGrass[i].position = sf::Vector2f(x, y);
        innerGrass[i].texCoords = sf::Vector2f(x, y); // Appliquer les coordonnées UV
    }

    return innerGrass;
}

// Fonction pour créer une piste fermée
sf::VertexArray createTrack() {
    const int numPoints = 100;
    sf::VertexArray track(sf::TriangleStrip, numPoints * 2);

    float centerX = 400, centerY = 300;
    float radiusX = 250, radiusY = 150;
    float trackWidth = 50;

    for (int i = 0; i < numPoints; i++) {
        float angle = (i / (float)numPoints) * 2 * M_PI;

        float rX = radiusX + 30 * std::sin(3 * angle);
        float rY = radiusY + 20 * std::cos(2 * angle);

        float x1 = centerX + (rX - trackWidth) * std::cos(angle);
        float y1 = centerY + (rY - trackWidth) * std::sin(angle);
        
        float x2 = centerX + (rX + trackWidth) * std::cos(angle);
        float y2 = centerY + (rY + trackWidth) * std::sin(angle);

        track[i * 2].position = sf::Vector2f(x1, y1);
        track[i * 2].texCoords = sf::Vector2f(x1, y1); // Coordonnées UV

        track[i * 2 + 1].position = sf::Vector2f(x2, y2);
        track[i * 2 + 1].texCoords = sf::Vector2f(x2, y2);
    }

    // Fermer le circuit
    track[numPoints * 2 - 2].position = track[0].position;
    track[numPoints * 2 - 1].position = track[1].position;

    return track;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "RC Car Simulation");
    window.setFramerateLimit(60);

    if (!loadTextures()) {
        std::cerr << "Erreur de chargement des textures !" << std::endl;
        return -1;
    }

    sf::VertexArray track = createTrack();
    sf::VertexArray grass = createGrass(track, 40);
    sf::VertexArray innerGrass = createInnerGrass(400, 300, 250, 150, 50);

    sf::Texture carTexture;
    if (!carTexture.loadFromFile("../../assets/car.png")) {
        return -1;
    }

    sf::Sprite carSprite;
    carSprite.setTexture(carTexture);
    carSprite.setScale(0.1f, 0.1f);
    carSprite.setPosition(375, 275);

    // États pour appliquer les textures
    sf::RenderStates trackState, grassState;
    grassTexture.setRepeated(true);
    trackTexture.setRepeated(true);
    trackState.texture = &trackTexture;
    grassState.texture = &grassTexture;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(grass, grassState);    // Appliquer la texture de l'herbe
        window.draw(innerGrass, grassState);
        window.draw(track, trackState);    // Appliquer la texture de la route
        window.draw(carSprite);

        window.display();
    }

    return 0;
}
*/


//version pour charger plusieurs circuits
/*#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>  // Bibliothèque JSON
#include <vector>
#include <cmath>

using json = nlohmann::json;

// Déclaration des textures
sf::Texture trackTexture, grassTexture, borderTexture;

// Structure pour stocker les paramètres d'un circuit
struct Circuit {
    std::string name;
    float centerX, centerY, radiusX, radiusY, trackWidth;
    std::string trackTexturePath, grassTexturePath, borderTexturePath;
};

// Fonction pour charger un circuit depuis un fichier JSON
Circuit loadCircuit(const std::string& filename) {
    Circuit circuit;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir " << filename << std::endl;
        exit(1);
    }

    json circuitData;
    file >> circuitData;

    circuit.name = circuitData["name"];
    circuit.centerX = circuitData["centerX"];
    circuit.centerY = circuitData["centerY"];
    circuit.radiusX = circuitData["radiusX"];
    circuit.radiusY = circuitData["radiusY"];
    circuit.trackWidth = circuitData["trackWidth"];
    circuit.trackTexturePath = circuitData["textures"]["track"];
    circuit.grassTexturePath = circuitData["textures"]["grass"];
    circuit.borderTexturePath = circuitData["textures"]["border"];

    return circuit;
}

// Fonction pour charger les textures du circuit
bool loadTextures(const Circuit& circuit) {
    if (!trackTexture.loadFromFile(circuit.trackTexturePath) ||
        !grassTexture.loadFromFile(circuit.grassTexturePath) ||
        !borderTexture.loadFromFile(circuit.borderTexturePath)) {
        return false;
    }
    return true;
}

// Fonction pour créer la piste avec les paramètres du circuit
sf::VertexArray createTrack(const Circuit& circuit) {
    const int numPoints = 100;
    sf::VertexArray track(sf::TriangleStrip, numPoints * 2);

    for (int i = 0; i < numPoints; i++) {
        float angle = (i / (float)numPoints) * 2 * M_PI;

        float rX = circuit.radiusX + 30 * std::sin(3 * angle);
        float rY = circuit.radiusY + 20 * std::cos(2 * angle);

        float x1 = circuit.centerX + (rX - circuit.trackWidth) * std::cos(angle);
        float y1 = circuit.centerY + (rY - circuit.trackWidth) * std::sin(angle);
        
        float x2 = circuit.centerX + (rX + circuit.trackWidth) * std::cos(angle);
        float y2 = circuit.centerY + (rY + circuit.trackWidth) * std::sin(angle);

        track[i * 2].position = sf::Vector2f(x1, y1);
        track[i * 2 + 1].position = sf::Vector2f(x2, y2);
    }

    return track;
}

int main() {
    // Demander à l'utilisateur de choisir un circuit
    std::string circuitFile;
    std::cout << "Entrez le fichier JSON du circuit (ex: circuits/circuit1.json) : ";
    std::cin >> circuitFile;

    // Charger le circuit
    Circuit circuit = loadCircuit(circuitFile);

    sf::RenderWindow window(sf::VideoMode(800, 600), "RC Car Simulation");
    window.setFramerateLimit(60);

    if (!loadTextures(circuit)) {
        std::cerr << "Erreur de chargement des textures !" << std::endl;
        return -1;
    }

    sf::VertexArray track = createTrack(circuit);

    // Charger la texture de la voiture
    sf::Texture carTexture;
    if (!carTexture.loadFromFile("../../assets/car.png")) {
        return -1;
    }

    sf::Sprite carSprite;
    carSprite.setTexture(carTexture);
    carSprite.setScale(0.1f, 0.1f);
    carSprite.setPosition(375, 275);

    // États pour appliquer les textures
    sf::RenderStates trackState;
    trackState.texture = &trackTexture;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(track, trackState);
        window.draw(carSprite);
        window.display();
    }

    return 0;
}
*/

//test delimitation
/*
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>

// Textures globales
sf::Texture trackTexture, grassTexture, borderTexture;

// Fonction pour charger les textures
bool loadTextures() {
    if (!trackTexture.loadFromFile("../../assets/track.png") ||
        !grassTexture.loadFromFile("../../assets/grass.png") ||
        !borderTexture.loadFromFile("../../assets/border.png")) {
        return false;
    }
    return true;
}

// Fonction pour créer la zone d'herbe autour de la piste et au centre
sf::VertexArray createGrass(const sf::VertexArray& track, float extraWidth) {
    sf::VertexArray grass(sf::TriangleStrip, track.getVertexCount() + 2);
    sf::Vector2f center(400, 300);

    for (size_t i = 0; i < track.getVertexCount(); i++) {
        sf::Vector2f pos = track[i].position;
        sf::Vector2f direction = pos - center;
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        if (length != 0) {
            direction /= length; // Normalisation
        }

        grass[i].position = pos + direction * extraWidth;
        grass[i].texCoords = sf::Vector2f(pos.x, pos.y); // Appliquer les coordonnées UV
    }

    // Fermer la boucle
    grass[track.getVertexCount()] = grass[0];
    grass[track.getVertexCount() + 1] = grass[1];

    return grass;
}

// Fonction pour créer la zone centrale en herbe
sf::VertexArray createInnerGrass(float centerX, float centerY, float radiusX, float radiusY, float trackWidth) {
    const int numPoints = 100;
    sf::VertexArray innerGrass(sf::TriangleFan, numPoints + 2);
    innerGrass[0].position = sf::Vector2f(centerX, centerY);

    for (int i = 1; i <= numPoints + 1; i++) {
        float angle = (i / (float)numPoints) * 2 * M_PI;
        float rX = radiusX + 30 * std::sin(3 * angle);
        float rY = radiusY + 20 * std::cos(2 * angle);

        float x = centerX + (rX - trackWidth) * std::cos(angle);
        float y = centerY + (rY - trackWidth) * std::sin(angle);

        innerGrass[i].position = sf::Vector2f(x, y);
        innerGrass[i].texCoords = sf::Vector2f(x, y); // Appliquer les coordonnées UV
    }

    return innerGrass;
}

// Fonction pour créer la piste avec les bordures rouges et blanches
sf::VertexArray createTrackWithBorders() {
    const int numPoints = 100;
    sf::VertexArray track(sf::TriangleStrip, numPoints * 2);
    sf::VertexArray trackBorders(sf::TriangleStrip, numPoints * 2);  // Pour les bandes rouges et blanches

    float centerX = 400, centerY = 300;
    float radiusX = 250, radiusY = 150;
    float trackWidth = 50;

    // Coordonnées des virages
    float angleStep = M_PI / 50;  // Étapes d'angle plus petites pour les virages plus nets

    for (int i = 0; i < numPoints; i++) {
        float angle = (i / (float)numPoints) * 2 * M_PI;

        float rX = radiusX + 30 * std::sin(3 * angle);
        float rY = radiusY + 20 * std::cos(2 * angle);

        float x1 = centerX + (rX - trackWidth) * std::cos(angle);
        float y1 = centerY + (rY - trackWidth) * std::sin(angle);

        float x2 = centerX + (rX + trackWidth) * std::cos(angle);
        float y2 = centerY + (rY + trackWidth) * std::sin(angle);

        track[i * 2].position = sf::Vector2f(x1, y1);
        track[i * 2 + 1].position = sf::Vector2f(x2, y2);

        // Ajouter les bandes rouges et blanches
        if (i % 2 == 0) { // Bande rouge
            trackBorders[i * 2].position = sf::Vector2f(x1, y1);
            trackBorders[i * 2 + 1].position = sf::Vector2f(x2, y2);
        } else {  // Bande blanche
            trackBorders[i * 2].position = sf::Vector2f(x1 + 5, y1 + 5);
            trackBorders[i * 2 + 1].position = sf::Vector2f(x2 + 5, y2 + 5);
        }
    }

    return trackBorders;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "RC Car Simulation");
    window.setFramerateLimit(60);

    // Charger les textures
    if (!loadTextures()) {
        std::cerr << "Erreur de chargement des textures !" << std::endl;
        return -1;
    }

    // Créer le circuit avec les bordures
    sf::VertexArray trackBorders = createTrackWithBorders();  // Déclarer ici
    sf::VertexArray track = createTrackWithBorders();
    sf::VertexArray grass = createGrass(track, 40);
    sf::VertexArray innerGrass = createInnerGrass(400, 300, 250, 150, 50);

    // Charger la texture de la voiture
    sf::Texture carTexture;
    if (!carTexture.loadFromFile("../../assets/car.png")) {
        return -1;
    }

    sf::Sprite carSprite;
    carSprite.setTexture(carTexture);
    carSprite.setScale(0.1f, 0.1f);
    carSprite.setPosition(375, 275);

    // États pour appliquer les textures
    sf::RenderStates trackState, grassState, borderState;
    trackState.texture = &trackTexture;
    grassState.texture = &grassTexture;
    borderState.texture = &borderTexture;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(grass, grassState);    // Appliquer la texture de l'herbe
        window.draw(innerGrass, grassState);
        window.draw(track, trackState);    // Appliquer la texture de la route
        window.draw(trackBorders, borderState);  // Appliquer les bandes rouges et blanches
        window.draw(carSprite);

        window.display();
    }

    return 0;
}
*/

//version avec les force de nadir
/*

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>

// Déclaration des textures
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

// Fonction pour créer la zone d'herbe autour de la piste
sf::VertexArray createGrass(const sf::VertexArray& track, float extraWidth) {
    sf::VertexArray grass(sf::TriangleStrip, track.getVertexCount() + 2);
    sf::Vector2f center(400, 300);

    for (size_t i = 0; i < track.getVertexCount(); i++) {
        sf::Vector2f pos = track[i].position;
        sf::Vector2f direction = pos - center;
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        if (length != 0) {
            direction /= length; // Normalisation
        }

        grass[i].position = pos + direction * extraWidth;
        grass[i].texCoords = sf::Vector2f(pos.x, pos.y);
    }

    // Fermer la boucle
    grass[track.getVertexCount()] = grass[0];
    grass[track.getVertexCount() + 1] = grass[1];

    return grass;
}

// Fonction pour créer une piste fermée
sf::VertexArray createTrack() {
    const int numPoints = 100;
    sf::VertexArray track(sf::TriangleStrip, numPoints * 2);

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

    // Fermer le circuit
    track[numPoints * 2 - 2].position = track[0].position;
    track[numPoints * 2 - 1].position = track[1].position;

    return track;
}

// Vérifie si la voiture est sur l'herbe en fonction de sa position
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

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "RC Car Simulation");
    window.setFramerateLimit(60);

    if (!loadTextures()) {
        std::cerr << "Erreur de chargement des textures !" << std::endl;
        return -1;
    }

    sf::VertexArray track = createTrack();
    sf::VertexArray grass = createGrass(track, 40);

    sf::Sprite carSprite;
    carSprite.setTexture(carTexture);
    carSprite.setScale(0.15f, 0.15f);
    carSprite.setPosition(100, 100);

    // États pour appliquer les textures
    sf::RenderStates trackState, grassState;
    grassTexture.setRepeated(true);
    trackTexture.setRepeated(true);
    trackState.texture = &trackTexture;
    grassState.texture = &grassTexture;

    // Variables pour le déplacement de la voiture
    sf::Vector2f carPosition = carSprite.getPosition();
    float carSpeed = 2.0f;
    float carAngle = 0.0f;
    float turnSpeed = 2.0f;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Gestion du déplacement autonome
        if (isOnGrass(carPosition, track, 50)) {
            carSpeed *= 0.95f;  // Réduction de la vitesse sur l'herbe
        } else {
            carSpeed = 2.0f;  // Retour à la vitesse normale sur la piste
        }

        carAngle += turnSpeed * 0.01f;  // Ajustement automatique de la direction
        carPosition.x += carSpeed * std::cos(carAngle);
        carPosition.y += carSpeed * std::sin(carAngle);
        carSprite.setPosition(carPosition);
        carSprite.setRotation(carAngle * 180 / M_PI);

        window.clear();
        window.draw(grass, grassState);
        window.draw(track, trackState);
        window.draw(carSprite);
        window.display();
    }

    return 0;
}
*/

//version 2.0 force de Nadir  avec les touches clavier 

// #include <SFML/Graphics.hpp>
// #include "map.hpp"
// #include "voiture.hpp"
// #include <math.h>

// int main() {
//     sf::RenderWindow window(sf::VideoMode(800, 600), "RC Car Simulation");
//     window.setFramerateLimit(60);

//     Map map;
//     if (!map.loadTextures()) {
//         return -1;
//     }

//     Voiture voiture;
//     sf::Texture carTexture;
//     if (!carTexture.loadFromFile("../../assets/car.png")) {
//         return -1;
//     }
//     voiture.sprite.setTexture(carTexture);
//     voiture.sprite.setScale(0.15f, 0.15f);
//     voiture.sprite.setPosition(625, 300);

//     // États pour appliquer les textures
//     sf::RenderStates trackState, grassState;
//     map.grassTexture.setRepeated(true);
//     map.trackTexture.setRepeated(true);
//     trackState.texture = &map.trackTexture;
//     grassState.texture = &map.grassTexture;

//     // Variables for car control
//     float acceleration = 0.2f;  // Acceleration factor
//     float deceleration = 0.1f;  // Deceleration factor
//     float maxSpeed = 5.0f;      // Maximum speed
//     float turnSpeed = 3.0f;     // Turning speed
//     float speed = 0.0f;         // Current speed

//     while (window.isOpen()) {
//         sf::Event event;
//         while (window.pollEvent(event)) {
//             if (event.type == sf::Event::Closed)
//                 window.close();
//         }

//         // Handle user input for car control
//         if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
//             speed += acceleration;  // Accelerate
//             if (speed > maxSpeed) speed = maxSpeed;
//         } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
//             speed -= deceleration;  // Decelerate
//             if (speed < -maxSpeed / 2) speed = -maxSpeed / 2;  // Reverse speed limit
//         } else {
//             // Gradually reduce speed when no input is given
//             if (speed > 0) {
//                 speed -= deceleration;
//                 if (speed < 0) speed = 0;
//             } else if (speed < 0) {
//                 speed += deceleration;
//                 if (speed > 0) speed = 0;
//             }
//         }

//         if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
//             voiture.angle -= turnSpeed * (speed / maxSpeed)/10;  // Turn left
//         }
//         if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
//             voiture.angle += turnSpeed * (speed / maxSpeed)/10;  // Turn right
//         }

//         // Update car position based on speed and angle
//         voiture.position.x -= speed * sin(voiture.angle);
//         voiture.position.y -= speed * cos(voiture.angle);

//         // Empêcher la voiture de sortir du cadre de la fenêtre
//         float carWidth = voiture.sprite.getGlobalBounds().width;
//         float carHeight = voiture.sprite.getGlobalBounds().height;

//         if (voiture.position.x < 0)
//             voiture.position.x = 0;
//         if (voiture.position.x > window.getSize().x - carWidth)
//             voiture.position.x = window.getSize().x - carWidth;

//         if (voiture.position.y < 0)
//             voiture.position.y = 0;
//         if (voiture.position.y > window.getSize().y - carHeight)
//             voiture.position.y = window.getSize().y - carHeight;


//         // Update car sprite position and rotation
//         voiture.sprite.setPosition(voiture.position);
//         voiture.sprite.setRotation(voiture.angle * 180 / M_PI);

//         // Render the scene
//         window.clear();
//         window.draw(map.grass, grassState);
//         window.draw(map.track, trackState);
//         window.draw(voiture.sprite);
//         window.display();
//     }

//     return 0;
// }

/*

//Version force de Nadir et contrôle du déplacement (On peut supprimer car code corrigé au dessus)

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>

// Déclaration des textures
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

// Fonction pour créer la zone d'herbe autour de la piste
sf::VertexArray createGrass(const sf::VertexArray& track, float extraWidth) {
    sf::VertexArray grass(sf::TriangleStrip, track.getVertexCount() + 2);
    sf::Vector2f center(400, 300);

    for (size_t i = 0; i < track.getVertexCount(); i++) {
        sf::Vector2f pos = track[i].position;
        sf::Vector2f direction = pos - center;
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        if (length != 0) {
            direction /= length; // Normalisation
        }

        grass[i].position = pos + direction * extraWidth;
        grass[i].texCoords = sf::Vector2f(pos.x, pos.y);
    }

    // Fermer la boucle
    grass[track.getVertexCount()] = grass[0];
    grass[track.getVertexCount() + 1] = grass[1];

    return grass;
}

// Fonction pour créer une piste fermée
sf::VertexArray createTrack() {
    const int numPoints = 100;
    sf::VertexArray track(sf::TriangleStrip, numPoints * 2);

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

    // Fermer le circuit
    track[numPoints * 2 - 2].position = track[0].position;
    track[numPoints * 2 - 1].position = track[1].position;

    return track;
}

// Vérifie si la voiture est sur l'herbe en fonction de sa position
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

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "RC Car Simulation");
    window.setFramerateLimit(60);

    if (!loadTextures()) {
        std::cerr << "Erreur de chargement des textures !" << std::endl;
        return -1;
    }

    sf::VertexArray track = createTrack();
    sf::VertexArray grass = createGrass(track, 40);

    sf::Sprite carSprite;
    carSprite.setTexture(carTexture);
    carSprite.setScale(0.2f, 0.2f);
    carSprite.setPosition(100, 100);

    // États pour appliquer les textures
    sf::RenderStates trackState, grassState;
    grassTexture.setRepeated(true);
    trackTexture.setRepeated(true);
    trackState.texture = &trackTexture;
    grassState.texture = &grassTexture;

    // Variables pour le déplacement de la voiture
    sf::Vector2f carPosition = carSprite.getPosition();
    float carSpeed = 0.0f;
    float maxSpeed = 4.0f;
    float acceleration = 0.1f;
    float friction = 0.98f;
    float turnSpeed = 1.0f;
    float carAngle = 0.0f;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Gestion des touches pour le contrôle
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            carSpeed += acceleration;  // Accélère
            if (carSpeed > maxSpeed)
                carSpeed = maxSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            carSpeed -= acceleration;  // Freine
            if (carSpeed < -maxSpeed / 2)
                carSpeed = -maxSpeed / 2;  // Marche arrière plus lente
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            carAngle -= turnSpeed * (carSpeed / maxSpeed);  // Tourne à gauche
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            carAngle += turnSpeed * (carSpeed / maxSpeed);  // Tourne à droite
        }

        // Appliquer friction (ralentissement naturel)
        carSpeed *= friction;

        // Déplacer la voiture en fonction de l'angle et de la vitesse
        carPosition.x -= carSpeed * std::sin(carAngle);
        carPosition.y -= carSpeed * std::cos(carAngle);
        carSprite.setPosition(carPosition);
        carSprite.setRotation(carAngle * 180 / M_PI);

        // Vérifier si la voiture est sur l'herbe
        if (isOnGrass(carPosition, track, 50)) {
            carSpeed *= 0.8f;  // Ralentissement sur l'herbe
        }

        window.clear();
        window.draw(grass, grassState);
        window.draw(track, trackState);
        window.draw(carSprite);
        window.display();
    }

    return 0;
}
*/

// Version 3.0 de Nadir ca va etre un banger
#include <SFML/Graphics.hpp>
#include "force.hpp"  // Contient déjà voiture.hpp
#include "map.hpp"
#include <cmath>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <vector>

// Déclaration des textures
sf::Texture trackTexture, grassTexture, borderTexture, carTexture;

// Normaliser un angle en radians entre -PI et PI
float normalizeAngle(float angle) {
    while (angle > M_PI) angle -= 2 * M_PI;
    while (angle < -M_PI) angle += 2 * M_PI;
    return angle;
}

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

// Fonction pour créer la zone d'herbe autour de la piste
sf::VertexArray createGrass(const sf::VertexArray& track, float extraWidth) {
    sf::VertexArray grass(sf::TriangleStrip, track.getVertexCount() + 2);
    sf::Vector2f center(400, 300);

    for (size_t i = 0; i < track.getVertexCount(); i++) {
        sf::Vector2f pos = track[i].position;
        sf::Vector2f direction = pos - center;
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        if (length != 0) {
            direction /= length; // Normalisation
        }

        grass[i].position = pos + direction * extraWidth;
        grass[i].texCoords = sf::Vector2f(pos.x, pos.y);
    }

    // Fermer la boucle
    grass[track.getVertexCount()] = grass[0];
    grass[track.getVertexCount() + 1] = grass[1];

    return grass;
}

// Fonction pour créer une piste fermée
sf::VertexArray createTrack() {
    const int numPoints = 100;
    sf::VertexArray track(sf::TriangleStrip, numPoints * 2);

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

    // Fermer le circuit
    track[numPoints * 2 - 2].position = track[0].position;
    track[numPoints * 2 - 1].position = track[1].position;

    return track;
}

// Vérifie si la voiture est sur l'herbe en fonction de sa position
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

// Fonction pour calculer l'angle de braquage nécessaire pour suivre le circuit
float calculateSteeringAngle(const Voiture& voiture, const sf::Vector2f& center, float idealRadius) {
    // Position actuelle de la voiture
    float carX = voiture.getX();
    float carY = voiture.getY();
    
    // Vecteur du centre vers la voiture
    float dx = carX - center.x;
    float dy = carY - center.y;
    
    // Distance actuelle par rapport au centre
    float currentRadius = std::sqrt(dx * dx + dy * dy);
    
    // Angle de la voiture par rapport au centre (en radians)
    float angleToCenter = std::atan2(dy, dx);
    
    // Angle actuel de la voiture (converti en radians)
    float carAngle = voiture.getAngle() * M_PI / 180.0;
    
    // Direction tangentielle idéale (perpendiculaire au rayon)
    float idealTangentAngle = angleToCenter + M_PI / 2;
    
    // Différence entre l'angle actuel et l'angle idéal
    float angleDiff = normalizeAngle(idealTangentAngle - carAngle);
    
    // Ajustement en fonction de la distance par rapport au rayon idéal
    float radiusError = (currentRadius - idealRadius) / 50.0; // Facteur d'échelle
    
    // Combiner la correction d'angle et la correction de rayon
    float steeringAngle = angleDiff * 30.0 + radiusError;
    
    // Limiter l'angle de braquage à une plage raisonnable
    steeringAngle = std::max(-1.0f, std::min(1.0f, steeringAngle));
    
    return steeringAngle;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "RC Car Simulation - Autonomous Mode");
    window.setFramerateLimit(60);

    float tempsDepuisDernierUpdateTexte = 0.0f;

    sf::VertexArray track = createTrack();
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

    // Chargement de la map
    Map map;
    if (!map.loadTextures()) return -1;

    // Sprite de la voiture
    sf::Texture carTexture;
    if (!carTexture.loadFromFile("../../assets/car.png")) return -1;
    sf::Sprite carSprite;
    carSprite.setTexture(carTexture);
    carSprite.setScale(0.15f, 0.15f);

    // Triangle de direction
    sf::ConvexShape directionIndicator;
    directionIndicator.setPointCount(3);
    directionIndicator.setPoint(0, sf::Vector2f(0, -20));
    directionIndicator.setPoint(1, sf::Vector2f(10, 10));
    directionIndicator.setPoint(2, sf::Vector2f(-10, 10));
    directionIndicator.setFillColor(sf::Color::Blue);
    directionIndicator.setOrigin(0, 0);

    // Textures map
    sf::RenderStates trackState, grassState;
    map.trackTexture.setRepeated(true);
    map.grassTexture.setRepeated(true);
    trackState.texture = &map.trackTexture;
    grassState.texture = &map.grassTexture;

    // Voiture et forces
    Voiture voiture(625, 300, 0, 20.0, 0.3); // x, y, angle, masse, empattement
    ForceMotrice moteur(600000.0, 200.0);
    ForceFrottement frottement(0.02);
    ForceFreinage frein(0.3);
    ForceAerodynamique air(0.0072);
    ForceVirage virage(0.0);
    ForceFreinGlisse freinGlisse(0.1);  // A ajuster selon l’effet visuel souhaité
    std::vector<Force*> forces = {&moteur, &frottement, &air, &frein, &virage, &freinGlisse};

    // Centre du circuit et rayon idéal pour la voiture
    sf::Vector2f center(400, 300);
    float idealRadius = 200.0f; // Rayon idéal pour la trajectoire
    
    // Mode autonome activé par défaut
    bool autonomousMode = true;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A) {
                // Touche A pour activer/désactiver le mode autonome
                autonomousMode = !autonomousMode;
            }
        }

        // === CONTRÔLES ===
        int etat = 0;
        double angle_braquage = 0.0;
        voiture.activerFrein(false);

        if (autonomousMode) {
            // Mode autonome : calcul de l'angle de braquage et accélération constante
            etat = 1; // Toujours accélérer en mode autonome
            angle_braquage = calculateSteeringAngle(voiture, center, idealRadius);
            
            // Vérifier si on est sur l'herbe, et ajuster si nécessaire
            sf::Vector2f carPos(voiture.getX(), voiture.getY());
            if (isOnGrass(carPos, track, 50)) {
                // Si sur l'herbe, tourner vers le centre
                float dx = center.x - voiture.getX();
                float dy = center.y - voiture.getY();
                float angleToCenter = atan2(dy, dx) * 180.0 / M_PI;
                float carAngle = voiture.getAngle();
                float angleDiff = angleToCenter - carAngle;
                
                // Normaliser la différence d'angle
                while (angleDiff > 180) angleDiff -= 360;
                while (angleDiff < -180) angleDiff += 360;
                
                // Déterminer la direction de braquage
                angle_braquage = (angleDiff > 0) ? 1.0 : -1.0;
            }
        } else {
            // Mode manuel - contrôles utilisateur
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                etat = 1; // Accélère
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
                voiture.activerFrein(true);
                etat = -1; // Frein
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                angle_braquage = -1.0; // ← Tourne à gauche
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                angle_braquage = 1.0;  // → Tourne à droite
            }
        }
        
        bool freinMainActif = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
        voiture.setFreinMainActif(freinMainActif);

        // === FORCES PHYSIQUES ===
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

        // === EMPÊCHER DE SORTIR DU CADRE ===
        float carWidth = carSprite.getGlobalBounds().width;
        float carHeight = carSprite.getGlobalBounds().height;

        if (voiture.getX() < 0) voiture.setX(0);
        if (voiture.getX() > window.getSize().x - carWidth) voiture.setX(window.getSize().x - carWidth);
        if (voiture.getY() < 0) voiture.setY(0);
        if (voiture.getY() > window.getSize().y - carHeight) voiture.setY(window.getSize().y - carHeight);

        // === RENDU VISUEL ===
        carSprite.setPosition(voiture.getX(), voiture.getY());
        carSprite.setRotation(voiture.getAngle()+90); // Ajustement de l'angle pour correspondre à la rotation de la voiture

        directionIndicator.setPosition(voiture.getX(), voiture.getY());
        directionIndicator.setRotation(voiture.getAngle() + angle_braquage+90); // Ajustement de l'angle pour correspondre à la direction de la voiture

        // === Affichage de la vitesse et angle ===
        // Calcul du temps écoulé entre les frames
        static sf::Clock clock;
        float dt = clock.restart().asSeconds();
        tempsDepuisDernierUpdateTexte += dt;

        if (tempsDepuisDernierUpdateTexte >= 0.25f) {  // toutes les 250 ms
            std::ostringstream oss;
            oss << "Vitesse : " << std::fixed << std::setprecision(2) << voiture.getVitesse() << " m/s\n"
                << "Angle   : " << std::fixed << std::setprecision(2) << voiture.getAngle() << " degres\n"
                << "Frein a main : " << (freinMainActif ? "OUI" : "non") << "\n"
                << "Mode : " << (autonomousMode ? "AUTONOME (A pour désactiver)" : "MANUEL (A pour activer)");
            hudText.setString(oss.str());

            tempsDepuisDernierUpdateTexte = 0.0f;
        }
        
        window.clear();
        window.draw(grass, grassState);
        window.draw(track, trackState);
        window.draw(carSprite);
        window.draw(directionIndicator);
        window.draw(hudText);
        window.display();
    }

    return 0;
}
