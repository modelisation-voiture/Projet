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
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>


// Déclaration des textures
sf::Texture trackTexture, grassTexture, borderTexture;

// Fonction pour charger les textures
bool loadTextures() {
    if (!trackTexture.loadFromFile("../../assets/track.png") ||
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


