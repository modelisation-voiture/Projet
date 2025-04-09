// main.cpp - Code intégré avec menu et simulation

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "force.hpp"  // Contient déjà voiture->hpp
#include "map.hpp"
#include <cmath>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <vector>

// États du jeu
enum GameState { MENU, TRANSITION, SIMULATION };

// Structure pour les éléments sélectionnables du menu
struct SelectableItem {
    sf::Sprite sprite;
    sf::RectangleShape border;
    int id;
    bool selected = false;
    bool isHovered(const sf::Vector2f& mousePos) {
        return sprite.getGlobalBounds().contains(mousePos);
    }
};

// Structure pour représenter un point de trajectoire
struct Waypoint {
    sf::Vector2f position;
    float targetRadius;  // Rayon de courbure cible à ce point
};

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

// Fonction pour extraire des waypoints à partir d'une piste
std::vector<Waypoint> extractWaypoints(const sf::VertexArray& track) {
    std::vector<Waypoint> waypoints;
    sf::Vector2f center(400, 300);
    float centerX = 400, centerY = 300;
    float radiusX = 250, radiusY = 150;
    
    for (size_t i = 0; i < track.getVertexCount(); i += 10) {  // Extraire un waypoint tous les 10 points
        if (i % 2 == 0) {  // Prendre uniquement les points intérieurs pour éviter la duplication
            sf::Vector2f inner = track[i].position;
            sf::Vector2f outer = track[i+1].position;
            sf::Vector2f mid = (inner + outer) * 0.5f;
            
            // Calculer une approximation du rayon de courbure
            float dx = mid.x - centerX;
            float dy = mid.y - centerY;
            float angle = std::atan2(dy, dx);
            
            float curvature = 0.5f * (std::sin(3 * angle) + std::cos(2 * angle));
            float targetRadius = std::max(radiusX, radiusY) * (1.0f - 0.3f * std::abs(curvature));
            
            waypoints.push_back({mid, targetRadius});
        }
    }
    
    return waypoints;
}

// Fonction pour vérifier si la voiture est sur l'herbe
bool isOnGrass(const sf::Vector2f& carPosition, int mapId) {
    float centerX = 400, centerY = 300;
    float trackWidth = 50.0f;
    
    switch (mapId) {
        case 1: {  // Ovale
            float radiusX = 200, radiusY = 100;
            float dx = carPosition.x - centerX;
            float dy = carPosition.y - centerY;
            float distance = std::sqrt(dx*dx/(radiusX*radiusX) + dy*dy/(radiusY*radiusY));
            return (distance > 1.0f + trackWidth/radiusX || distance < 1.0f - trackWidth/radiusX);
        }
        case 2: {  // Serpent
            for (int i = 0; i < 120; i++) {
                float t = i / 10.0f;
                float x = 100 + i * 5;
                float y = 300 + std::sin(t) * 100;
                float dist = std::sqrt(std::pow(carPosition.x - x, 2) + std::pow(carPosition.y - y, 2));
                if (dist < trackWidth) return false;
            }
            return true;
        }
        case 3: {  // Carré
            float margin = trackWidth;
            return (carPosition.x < 200-margin || carPosition.x > 600+margin || 
                    carPosition.y < 200-margin || carPosition.y > 400+margin);
        }
        case 4: {  // Double boucle
            for (int i = 0; i < 100; i++) {
                float angle = i * 2 * M_PI / 100;
                float r = 100 + 80 * std::sin(2 * angle);
                float x = 400 + r * std::cos(angle);
                float y = 300 + r * std::sin(angle);
                float dist = std::sqrt(std::pow(carPosition.x - x, 2) + std::pow(carPosition.y - y, 2));
                if (dist < trackWidth) return false;
            }
            return true;
        }
        case 5: {  // Spirale
            for (int i = 0; i < 100; i++) {
                float angle = i * 0.2f;
                float r = 50 + 2 * i;
                float x = 400 + r * std::cos(angle);
                float y = 300 + r * std::sin(angle);
                float dist = std::sqrt(std::pow(carPosition.x - x, 2) + std::pow(carPosition.y - y, 2));
                if (dist < trackWidth) return false;
            }
            return true;
        }
        default:
            return false;
    }
}

// Normaliser un angle en radians entre -PI et PI
float normalizeAngle(float angle) {
    while (angle > M_PI) angle -= 2 * M_PI;
    while (angle < -M_PI) angle += 2 * M_PI;
    return angle;
}

// Fonction pour trouver le waypoint le plus proche dans la direction de conduite
int findNextWaypoint(const Voiture& voiture, const std::vector<Waypoint>& waypoints, int currentWaypoint) {
    sf::Vector2f carPos(voiture.getX(), voiture.getY());
    float carAngle = voiture.getAngle() * M_PI / 180.0f;
    
    // Direction de la voiture
    sf::Vector2f carDir(std::cos(carAngle), std::sin(carAngle));
    
    // Regarder quelques points en avant
    int lookAhead = 3;
    int bestPoint = currentWaypoint;
    float bestScore = -1;
    
    int waypointsCount = waypoints.size();
    
    for (int i = 0; i < lookAhead; i++) {
        int pointIndex = (currentWaypoint + i) % waypointsCount;
        
        // Vecteur de la voiture au waypoint
        sf::Vector2f toWaypoint = waypoints[pointIndex].position - carPos;
        float distance = std::sqrt(toWaypoint.x * toWaypoint.x + toWaypoint.y * toWaypoint.y);
        
        if (distance < 10) {
            // Si on est très proche du waypoint actuel, passer au suivant
            bestPoint = (pointIndex + 1) % waypointsCount;
            break;
        }
        
        // Normaliser le vecteur
        toWaypoint /= distance;
        
        // Calculer le produit scalaire pour avoir l'angle entre la direction de la voiture
        // et la direction vers le waypoint
        float dotProduct = carDir.x * toWaypoint.x + carDir.y * toWaypoint.y;
        
        // Meilleur score = point dans la direction de la voiture mais pas trop loin
        float score = dotProduct * (1.0f - distance / 500.0f);
        
        if (score > bestScore) {
            bestScore = score;
            bestPoint = pointIndex;
        }
    }
    
    return bestPoint;
}

// Fonction pour calculer l'angle de braquage nécessaire pour atteindre le waypoint cible
float calculateAdaptiveSteeringAngle(const Voiture& voiture, const Waypoint& target) {
    // Position actuelle de la voiture
    sf::Vector2f carPos(voiture.getX(), voiture.getY());
    
    // Vecteur de la voiture au waypoint
    sf::Vector2f toTarget = target.position - carPos;
    float distance = std::sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);
    
    if (distance < 1e-3) return 0.0f;  // Éviter division par zéro
    
    // Normaliser le vecteur
    toTarget /= distance;
    
    // Angle de la voiture (en radians)
    float carAngle = voiture.getAngle() * M_PI / 180.0f;
    
    // Direction de la voiture
    sf::Vector2f carDir(std::cos(carAngle), std::sin(carAngle));
    
    // Produit vectoriel pour déterminer si le waypoint est à gauche ou à droite
    float crossProduct = carDir.x * toTarget.y - carDir.y * toTarget.x;
    
    // Produit scalaire pour avoir l'angle entre la voiture et le waypoint
    float dotProduct = carDir.x * toTarget.x + carDir.y * toTarget.y;
    
    // Calculer l'angle de braquage (positive = droite, negative = gauche)
    float angleToTarget = std::atan2(crossProduct, dotProduct);
    
    // Ajuster la sensibilité selon la distance et le rayon de courbure
    float sensitivity = 1.0f;
    float steeringAngle = angleToTarget * sensitivity;
    
    // Limiter l'angle de braquage à une plage raisonnable [-1, 1]
    steeringAngle = std::max(-1.0f, std::min(1.0f, steeringAngle));
    
    return steeringAngle;
}

int main() {
    const int WIN_WIDTH = 800, WIN_HEIGHT = 600;
    sf::RenderWindow window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "PolyRacing Game");
    window.setFramerateLimit(60);

    // ===== CHARGEMENT DES RESSOURCES =====
    // Chargement des polices et sons
    sf::Font font;
    if (!font.loadFromFile("../../assets/Roboto-Regular.ttf")) {
        std::cerr << "Erreur chargement police\n";
        return -1;
    }

    sf::SoundBuffer hoverBuffer, selectBuffer;
    if (!hoverBuffer.loadFromFile("../../assets/sound.wav") ||
        !selectBuffer.loadFromFile("../../assets/select.wav")) {
        std::cerr << "Erreur chargement sons\n";
        // Continuer sans sons si nécessaire
    }
    sf::Sound hoverSound(hoverBuffer);
    sf::Sound selectSound(selectBuffer);

    // Fond d'écran pour le menu
    sf::Texture bgTexture;
    if (!bgTexture.loadFromFile("../../assets/accueil.png")) {
        std::cerr << "Erreur chargement fond d'écran\n";
        bgTexture.create(800, 600); // Fallback pour éviter crash
        // Continuer avec un fond noir si nécessaire
    }
    sf::Sprite background(bgTexture);
    sf::Vector2u textureSize = bgTexture.getSize();
    sf::Vector2u windowSize = window.getSize();
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    background.setScale(scaleX, scaleY);

    // ===== INITIALISATION DU MENU =====
    GameState state = MENU;
    int selectedMap = -1, selectedCar = -1;
    bool hoverPlayedMap[5] = {}, hoverPlayedCar[5] = {};

    // Chemins vers les images pour les aperçus
    std::vector<std::string> mapPreviewPaths = {
        "../../assets/suv.png", "../../assets/suv.png", "../../assets/suv.png",
        "../../assets/suv.png", "../../assets/suv.png"
    };
    std::vector<std::string> carPreviewPaths = {
        "../../assets/car.png", "../../assets/car_blue.png", "../../assets/car_pink.png",
        "../../assets/car_gray.png", "../../assets/car_green.png"
    };
    std::vector<std::string> carTexturePaths = carPreviewPaths;

    // Textures pour les aperçus
    std::vector<sf::Texture> mapTextures(5), carTextures(5);
    std::vector<SelectableItem> mapItems, carItems;

    // Création des éléments sélectionnables
    for (int i = 0; i < 5; ++i) {
        // Chargement des textures (utiliser des placeholders si les fichiers n'existent pas)
        if (!mapTextures[i].loadFromFile(mapPreviewPaths[i])) {
            mapTextures[i].create(120, 80);  // Texture vide de secours
        }
        if (!carTextures[i].loadFromFile(carPreviewPaths[i])) {
            carTextures[i].create(120, 80);  // Texture vide de secours
        }

        // Créer les éléments sélectionnables pour les maps
        SelectableItem m;
        m.sprite.setTexture(mapTextures[i]);
        m.sprite.setScale(0.5f, 0.5f);
        m.sprite.setPosition(60 + i * 140, 120);
        m.border.setSize({m.sprite.getGlobalBounds().width, m.sprite.getGlobalBounds().height});
        m.border.setFillColor(sf::Color::Transparent);
        m.border.setOutlineThickness(3);
        m.border.setOutlineColor(sf::Color::Transparent);
        m.border.setPosition(m.sprite.getPosition());
        m.id = i + 1;  // IDs de map de 1 à 5
        mapItems.push_back(m);

        // Créer les éléments sélectionnables pour les voitures
        SelectableItem c;
        c.sprite.setTexture(carTextures[i]);
        c.sprite.setScale(0.25f, 0.25f);
        c.sprite.setPosition(60 + i * 140, 350);
        c.border.setSize({c.sprite.getGlobalBounds().width, c.sprite.getGlobalBounds().height});
        c.border.setFillColor(sf::Color::Transparent);
        c.border.setOutlineThickness(3);
        c.border.setOutlineColor(sf::Color::Transparent);
        c.border.setPosition(c.sprite.getPosition());
        c.id = i;  // IDs de voitures de 0 à 4
        carItems.push_back(c);
    }

    // Textes du menu
    sf::Text gameTitle("PolyRacing Game", font, 40);
    gameTitle.setPosition(270, 10);
    gameTitle.setFillColor(sf::Color::White);

    sf::Text title("Selectionnez une carte et une voiture", font, 24);
    title.setPosition(60, 60);
    title.setFillColor(sf::Color::White);

    sf::Text goText("GO JOUER", font, 20);
    goText.setFillColor(sf::Color::White);
    goText.setPosition(680, 530);

    sf::RectangleShape goButton(sf::Vector2f(140, 40));
    goButton.setPosition(670, 520);
    goButton.setFillColor(sf::Color(70, 150, 70));
    goButton.setOutlineThickness(2);
    goButton.setOutlineColor(sf::Color::White);

    // Overlay de transition
    sf::RectangleShape transitionOverlay(sf::Vector2f(WIN_WIDTH, WIN_HEIGHT));
    transitionOverlay.setFillColor(sf::Color::Black);
    float transitionAlpha = 0.0f;
    bool transitionDone = false;
    sf::Clock transitionClock;

    // ===== INITIALISATION DE LA SIMULATION =====
    // Sprite de la voiture
    sf::Texture carTexture;
    sf::Sprite carSprite;
    carSprite.setScale(0.12f, 0.12f);

    // Triangle de direction
    sf::ConvexShape directionIndicator;
    directionIndicator.setPointCount(3);
    directionIndicator.setPoint(0, sf::Vector2f(0, -20));
    directionIndicator.setPoint(1, sf::Vector2f(10, 10));
    directionIndicator.setPoint(2, sf::Vector2f(-10, 10));
    directionIndicator.setFillColor(sf::Color::Blue);
    directionIndicator.setOrigin(0, 0);

    // HUD texte
    sf::Text hudText;
    hudText.setFont(font);
    hudText.setCharacterSize(18);
    hudText.setFillColor(sf::Color::White);
    hudText.setPosition(10, 10);

    // Ligne pour visualiser la direction cible
    sf::Vertex targetLine[2];
    targetLine[0].color = sf::Color::Red;
    targetLine[1].color = sf::Color::Red;

    // Gestion du temps
    float tempsDepuisDernierUpdateTexte = 0.0f;
    sf::Clock clockDt;

    // Variables pour la simulation
    Map map;
    sf::VertexArray track, grass;
    std::vector<Waypoint> waypoints;
    int currentWaypoint = 0;
    bool autonomousMode = true;
    
    // Visualisation des waypoints
    sf::CircleShape waypointMarker(3);
    waypointMarker.setFillColor(sf::Color::Yellow);
    waypointMarker.setOrigin(3, 3);

    // Voiture et forces
    Voiture* voiture = nullptr;
    std::vector<Force*> forces;
    ForceMotriceProgressive* moteur = nullptr;
    ForceFrottement* frottement = nullptr;
    ForceFreinage* frein = nullptr;
    ForceAerodynamique* air = nullptr;
    ForceVirage* virage = nullptr;
    ForceFreinGlisse* freinGlisse = nullptr;

    // Textures map 
    sf::RenderStates trackState, grassState;
    map.trackTexture.setRepeated(true);
    map.grassTexture.setRepeated(true);
    trackState.texture = &map.trackTexture;
    grassState.texture = &map.grassTexture;


    // Boucle principale
    while (window.isOpen()) {
        sf::Event event;
        sf::Vector2f mousePos(sf::Mouse::getPosition(window));

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (state == MENU && event.type == sf::Event::MouseButtonPressed) {
                // Sélection de carte et voiture
                for (int i = 0; i < 5; ++i) {
                    if (mapItems[i].isHovered(mousePos)) {
                        selectedMap = mapItems[i].id;
                        selectSound.play();
                        for (auto& m : mapItems) m.selected = false;
                        mapItems[i].selected = true;
                    }
                    if (carItems[i].isHovered(mousePos)) {
                        selectedCar = carItems[i].id;
                        selectSound.play();
                        for (auto& c : carItems) c.selected = false;
                        carItems[i].selected = true;
                    }
                }

                // Bouton GO
                if (selectedMap > 0 && selectedCar >= 0 &&
                    goButton.getGlobalBounds().contains(mousePos)) {
                    
                    // Chargement de la map
                    if (!map.loadTextures(selectedMap)) {
                        std::cerr << "Erreur chargement textures map\n";
                        return -1;
                    }
                    
                    // Chargement texture voiture
                    if (!carTexture.loadFromFile(carTexturePaths[selectedCar])) {
                        std::cerr << "Erreur chargement texture voiture\n";
                        return -1;
                    }
                    carSprite.setTexture(carTexture);
                    
                    // Génération de la piste selon la sélection
                    switch (selectedMap) {
                        case 1: track = map.createTrackOvale(); break;
                        case 2: track = map.createTrackSerpent(); break;
                        case 3: track = map.createTrackCarre(); break;
                        case 4: track = map.createTrackDoubleBoucle(); break;
                        case 5: track = map.createTrackSpirale(); break;
                        default: track = map.createTrackOvale(); break;
                    }
                    
                    // Génération de la zone d'herbe
                    grass = createGrass(track, 40);
                    
                    // Extraction des waypoints
                    waypoints = extractWaypoints(track);
                    currentWaypoint = 0;
                    
                    // Création de la voiture et des forces
                    if (voiture) delete voiture;
                    voiture = new Voiture(150, 300, 0, 20.0, 0.3);
                    
                    // Nettoyage des anciennes forces (sauf celles qui sont des pointeurs locaux)
                    forces.clear();
                    
                    moteur = new ForceMotriceProgressive(10000.0, 10.0);
                    frottement = new ForceFrottement(0.02);
                    frein = new ForceFreinage(0.3);
                    air = new ForceAerodynamique(0.0072);
                    virage = new ForceVirage(0.0);
                    freinGlisse = new ForceFreinGlisse(10);
                    
                    forces = {moteur, frottement, air, frein, virage, freinGlisse};
                    
                    // Initialisation de la transition
                    transitionAlpha = 0;
                    transitionOverlay.setFillColor(sf::Color(0, 0, 0, 0));
                    state = TRANSITION;
                    transitionClock.restart();
                    clockDt.restart();
                }
            }
            
            // Touche pour activer/désactiver le mode autonome
            if ((state == TRANSITION || state == SIMULATION) && 
                event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A) {
                autonomousMode = !autonomousMode;
            }
            
            // Touche pour retourner au menu
            if ((state == TRANSITION || state == SIMULATION) && 
                event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                state = MENU;
            }
        }

        // ==== LOGIQUE DU MENU ====
        if (state == MENU) {
            // Mise à jour des survols et sélections
            for (int i = 0; i < 5; ++i) {
                auto& m = mapItems[i];
                auto& c = carItems[i];

                m.border.setOutlineColor(m.selected ? sf::Color::Yellow :
                    (m.isHovered(mousePos) ? sf::Color::Green : sf::Color::Transparent));
                c.border.setOutlineColor(c.selected ? sf::Color::Yellow :
                    (c.isHovered(mousePos) ? sf::Color::Cyan : sf::Color::Transparent));

                if (m.isHovered(mousePos) && !hoverPlayedMap[i]) {
                    hoverSound.play(); hoverPlayedMap[i] = true;
                } else if (!m.isHovered(mousePos)) hoverPlayedMap[i] = false;

                if (c.isHovered(mousePos) && !hoverPlayedCar[i]) {
                    hoverSound.play(); hoverPlayedCar[i] = true;
                } else if (!c.isHovered(mousePos)) hoverPlayedCar[i] = false;
            }
        }
        // ==== LOGIQUE DE TRANSITION ====
        else if (state == TRANSITION) {
            float elapsed = transitionClock.getElapsedTime().asSeconds();
            if (elapsed >= 1.0f) {
                state = SIMULATION;
            }
        }
        // ==== LOGIQUE DE SIMULATION ====
        else if (state == SIMULATION && voiture) {
            float dt = clockDt.restart().asSeconds();
            tempsDepuisDernierUpdateTexte += dt;

            // === CONTRÔLES ===
            int etat = 0;
            double angle_braquage = 0.0;
            voiture->activerFrein(false);

            if (autonomousMode) {
                // Mode autonome : calcul de l'angle de braquage adaptatif
                
                // Trouver le prochain waypoint
                currentWaypoint = findNextWaypoint(*voiture, waypoints, currentWaypoint);
                
                // Calculer l'angle de braquage adaptatif
                angle_braquage = calculateAdaptiveSteeringAngle(*voiture, waypoints[currentWaypoint]);
                
                // Ajuster la vitesse en fonction du rayon de courbure
                float speedFactor = waypoints[currentWaypoint].targetRadius / 250.0f;
                speedFactor = std::max(0.3f, std::min(1.0f, speedFactor)); // Limiter entre 0.3 et 1.0
                
                // Accélérer plus dans les lignes droites, moins dans les virages
                etat = 1;
                
                // Vérifier si on est sur l'herbe, et ajuster si nécessaire
                sf::Vector2f carPos(voiture->getX(), voiture->getY());
                if (isOnGrass(carPos, selectedMap)) {
                    // Si sur l'herbe, chercher le waypoint le plus proche qui est sur la piste
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
                    angle_braquage = calculateAdaptiveSteeringAngle(*voiture, waypoints[currentWaypoint]);
                    etat = 1; // Accélérer pour sortir de l'herbe
                }
                
                // Visualiser la direction cible
                targetLine[0].position = sf::Vector2f(voiture->getX(), voiture->getY());
                targetLine[1].position = waypoints[currentWaypoint].position;
            } else {
                // Mode manuel - contrôles utilisateur
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                    etat = 1; // Accélère
                    voiture->setAccelerationActive(true);
                } else {
                    voiture->setAccelerationActive(false);
                }
                
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
                    voiture->activerFrein(true);
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
            voiture->setFreinMainActif(freinMainActif);
            voiture->setAccelerationActive(etat == 1); // pour que la force motrice sache si on accélère

            // === FORCES PHYSIQUES ===
            double fx = 0, fy = 0;
            *virage = ForceVirage(angle_braquage);

            for (auto* f : forces) {
                if (f == moteur && etat != 1) continue;
                if (f == frein && !voiture->isFreinActif()) continue;
                if (f == freinGlisse && !freinMainActif) continue;
            
                auto [fx_i, fy_i] = f->calculer_force(*voiture);
                fx += fx_i;
                fy += fy_i;
            }
            
            voiture->appliquerForce(fx, fy);
            voiture->updatePositionRK4(0.01, fx, fy, 0.02, angle_braquage);


            // === EMPÊCHER DE SORTIR DU CADRE ===
            float carWidth = carSprite.getGlobalBounds().width;
            float carHeight = carSprite.getGlobalBounds().height;

            if (voiture->getX() < 0) voiture->setX(0);
            if (voiture->getX() > window.getSize().x - carWidth) voiture->setX(window.getSize().x - carWidth);
            if (voiture->getY() < 0) voiture->setY(0);
            if (voiture->getY() > window.getSize().y - carHeight) voiture->setY(window.getSize().y - carHeight);

            // === RENDU VISUEL ===
            carSprite.setPosition(voiture->getX(), voiture->getY());
            carSprite.setRotation(voiture->getAngle()+90); // Ajustement de l'angle pour correspondre à la rotation de la voiture

            directionIndicator.setPosition(voiture->getX(), voiture->getY());
            directionIndicator.setRotation(voiture->getAngle() + angle_braquage+90); // Ajustement de l'angle pour correspondre à la direction de la voiture

            // === Affichage de la vitesse et angle ===
            // Calcul du temps écoulé entre les frames
            static sf::Clock clock;
            //float dt = clock.restart().asSeconds();
            tempsDepuisDernierUpdateTexte += dt;

            if (tempsDepuisDernierUpdateTexte >= 0.25f) {  // toutes les 250 ms
                std::ostringstream oss;
                oss << "Vitesse : " << std::fixed << std::setprecision(2) << voiture->getVitesse() << " m/s\n"
                    << "Angle   : " << std::fixed << std::setprecision(2) << voiture->getAngle() << " degres\n"
                    << "Waypoint actuel : " << currentWaypoint << "/" << waypoints.size() << "\n"
                    << "Mode : " << (autonomousMode ? "AUTONOME (A pour desactiver)" : "MANUEL (A pour activer)");
                hudText.setString(oss.str());

                tempsDepuisDernierUpdateTexte = 0.0f;
            }
            
            window.clear();
            window.draw(grass, grassState);
            window.draw(track, trackState);
            
            // Afficher les waypoints
            if (autonomousMode) {
                for (int i = 0; i < waypoints.size(); i++) {
                    waypointMarker.setPosition(waypoints[i].position);
                    waypointMarker.setFillColor((i == currentWaypoint) ? sf::Color::Red : sf::Color::Blue);
                    window.draw(waypointMarker);
                }
                
                // Afficher la ligne vers le waypoint cible
                window.draw(targetLine, 2, sf::Lines);
            }
            
            window.draw(carSprite);
            window.draw(directionIndicator);
            window.draw(hudText);
            window.display();
        }

        return 0;
    }
}