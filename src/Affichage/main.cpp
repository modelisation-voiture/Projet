// Version 3.0 de Nadir ca va etre un banger

#include <SFML/Graphics.hpp>
#include "force.hpp"  // Contient déjà voiture.hpp
#include "map.hpp"
#include <cmath>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <vector>

// Déclaration des textures et structures de base (inchangé)
sf::Texture trackTexture, grassTexture, borderTexture, carTexture;

// Fonction pour charger les textures (inchangé)
bool loadTextures() {
    if (!trackTexture.loadFromFile("../../assets/asphalt.jpg") ||
        !grassTexture.loadFromFile("../../assets/grass.png") ||
        !borderTexture.loadFromFile("../../assets/outer.png") ||
        !carTexture.loadFromFile("../../assets/car.png")) {
        return false;
    }
    return true;
}

// Structure pour représenter un point de trajectoire
struct Waypoint {
    sf::Vector2f position;
    float targetRadius;  // Rayon de courbure cible à ce point
};

// Fonction pour créer la zone d'herbe autour de la piste (inchangé)
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

// Fonction pour créer une piste fermée et extraire ses points médians
std::tuple<sf::VertexArray, std::vector<Waypoint>, float> createTrackWithWaypoints() {
    const int numPoints = 100;
    sf::VertexArray track(sf::TriangleStrip, numPoints * 2);
    std::vector<Waypoint> waypoints;
    
    float metersPerPixel = 1.0f / 10.0f;  // 1m = 10px
    float centerX = 400, centerY = 300;
    float radiusX = 25.0f;  // en mètres
    float radiusY = 15.0f;  // en mètres
    float trackWidth = 5.0f; // largeur piste en mètres

    float maxDistance = 0.0f;

    for (int i = 0; i < numPoints; i++) {
        float angle = (i / (float)numPoints) * 2 * M_PI;

        float rX = radiusX + 3.0f * std::sin(3 * angle);  // +/- 3m de variation
        float rY = radiusY + 2.0f * std::cos(2 * angle);

        float x1 = centerX + (rX - trackWidth) * std::cos(angle) / metersPerPixel;
        float y1 = centerY + (rY - trackWidth) * std::sin(angle) / metersPerPixel;

        float x2 = centerX + (rX + trackWidth) * std::cos(angle) / metersPerPixel;
        float y2 = centerY + (rY + trackWidth) * std::sin(angle) / metersPerPixel;

        track[i * 2].position = sf::Vector2f(x1, y1);
        track[i * 2 + 1].position = sf::Vector2f(x2, y2);

        // Waypoints (médian de la route)
        if (i % 5 == 0) {
            float midX = (x1 + x2) / 2;
            float midY = (y1 + y2) / 2;

            float curvature = std::abs(std::sin(3 * angle)) + std::abs(std::cos(2 * angle));
            float targetRadius = rX * (1.0f - 0.3f * curvature);

            waypoints.push_back({sf::Vector2f(midX, midY), targetRadius});
        }

        // Mesurer la distance max pour limite extérieure
        float dist1 = std::hypot(x1 - centerX, y1 - centerY);
        float dist2 = std::hypot(x2 - centerX, y2 - centerY);
        maxDistance = std::max({maxDistance, dist1, dist2});
    }

    track[numPoints * 2 - 2].position = track[0].position;
    track[numPoints * 2 - 1].position = track[1].position;

    return {track, waypoints, maxDistance};
}


// Vérifie si la voiture est sur l'herbe en fonction de sa position (inchangé)
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
    
    // Ajuster la vitesse en fonction du rayon de courbure
    // Plus le rayon est petit, plus on doit ralentir
    float speedFactor = target.targetRadius / 250.0f;
    
    return steeringAngle;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "RC Car Simulation - Adaptive Steering");
    window.setFramerateLimit(60);

    float tempsDepuisDernierUpdateTexte = 0.0f;

    // Créer le circuit avec waypoints
    auto [track, waypoints, maxDistance] = createTrackWithWaypoints();
    sf::VertexArray grass = createGrass(track, 40);

    // Visualisation des waypoints
    sf::CircleShape waypointMarker(3);
    waypointMarker.setFillColor(sf::Color::Yellow);
    waypointMarker.setOrigin(3, 3);

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
    if (!carTexture.loadFromFile("../../assets/car_pink.png")) return -1;
    sf::Sprite carSprite;
    carSprite.setTexture(carTexture);
    carSprite.setScale(0.12f, 0.12f);

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
    Voiture voiture(150, 300, 0, 20.0, 0.3); // x, y, angle, masse, empattement
    //ForceMotrice moteur(100000.0, 80.0);
    // ForceMotriceProgressive moteur(10000.0, 10.0); // 5 m/s² atteints en 3 secondes
    ForceMotriceProgressive moteur(1e4, 10.0, 20.0);
    ForceFrottement frottement(0.0002);
    ForceFreinage frein(0.3);
    ForceAerodynamique air(0.0072);
    ForceVirage virage(0.0);
    ForceFreinGlisse freinGlisse(10);  // A ajuster selon l'effet visuel souhaité
    std::vector<Force*> forces = {&moteur, &frottement, &air, &frein, &virage, &freinGlisse};

    // Mode autonome activé par défaut
    bool autonomousMode = true;
    
    // Waypoint actuel
    int currentWaypoint = 0;
    
    // Ligne pour visualiser la direction cible
    sf::Vertex targetLine[2];
    targetLine[0].color = sf::Color::Red;
    targetLine[1].color = sf::Color::Red;

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
            // Mode autonome : calcul de l'angle de braquage adaptatif
            
            // Trouver le prochain waypoint
            currentWaypoint = findNextWaypoint(voiture, waypoints, currentWaypoint);
            
            // Calculer l'angle de braquage adaptatif
            angle_braquage = calculateAdaptiveSteeringAngle(voiture, waypoints[currentWaypoint]);
            
            // Ajuster la vitesse en fonction du rayon de courbure
            float speedFactor = waypoints[currentWaypoint].targetRadius / 250.0f;
            speedFactor = std::max(0.3f, std::min(1.0f, speedFactor)); // Limiter entre 0.3 et 1.0
            
            // Accélérer plus dans les lignes droites, moins dans les virages
            etat = 1;
            
            // Vérifier si on est sur l'herbe, et ajuster si nécessaire
            sf::Vector2f carPos(voiture.getX(), voiture.getY());
            if (isOnGrass(carPos, track, 50)) {
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
                angle_braquage = calculateAdaptiveSteeringAngle(voiture, waypoints[currentWaypoint]);
                etat = 1; // Accélérer pour sortir de l'herbe
            }
            
            // Visualiser la direction cible
            targetLine[0].position = sf::Vector2f(voiture.getX(), voiture.getY());
            targetLine[1].position = waypoints[currentWaypoint].position;
        } else {
            // Mode manuel - contrôles utilisateur
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                etat = 1; // Accélère
                voiture.setAccelerationActive(true);
            } else {
                voiture.setAccelerationActive(false);
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
        voiture.setAccelerationActive(etat == 1); // pour que la force motrice sache si on accélère


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
                << "Waypoint actuel : " << currentWaypoint << "/" << waypoints.size() << "\n"
                << "Mode : " << (autonomousMode ? "AUTONOME (A pour desactiver)" : "MANUEL (A pour activer)")
                << "Accél. moteur : " 
                << std::fixed << std::setprecision(2) 
                << moteur.getAccelerationCourante() << " m/s²\n";
                
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


//version pour le menu
/*
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "map.hpp"
#include <iostream>
#include <vector>

enum GameState { MENU, TRANSITION, SIMULATION };

struct SelectableItem {
    sf::Sprite sprite;
    sf::RectangleShape border;
    int id;
    bool selected = false;
    bool isHovered(const sf::Vector2f& mousePos) {
        return sprite.getGlobalBounds().contains(mousePos);
    }
};

int main() {
    const int WIN_WIDTH = 1000, WIN_HEIGHT = 700;
    sf::RenderWindow window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "PolyRacing Game", sf::Style::Close);
    window.setFramerateLimit(60);
    window.setView(sf::View(sf::FloatRect(0, 0, WIN_WIDTH, WIN_HEIGHT)));

    sf::Font font;
    font.loadFromFile("../../assets/Roboto-Regular.ttf");

    sf::Texture bgTexture;
    bgTexture.loadFromFile("../../assets/accueil.png");
    sf::Sprite background(bgTexture);
    sf::Vector2u textureSize = bgTexture.getSize();
    sf::Vector2u windowSize = window.getSize();
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    background.setScale(scaleX, scaleY);

    sf::SoundBuffer hoverBuffer, selectBuffer;
    hoverBuffer.loadFromFile("../../assets/sound.wav");
    selectBuffer.loadFromFile("../../assets/select.wav");
    sf::Sound hoverSound(hoverBuffer);
    sf::Sound selectSound(selectBuffer);

    GameState state = MENU;
    int selectedMap = -1, selectedCar = -1;
    bool hoverPlayedMap[5] = {}, hoverPlayedCar[5] = {};

    std::vector<std::string> mapPreviewPaths = {
        "../../assets/suv.png", "../../assets/suv.png", "../../assets/suv.png",
        "../../assets/suv.png", "../../assets/suv.png"
    };
    std::vector<std::string> carPreviewPaths = {
        "../../assets/car.png", "../../assets/car_blue.png", "../../assets/car_pink.png",
        "../../assets/car_gray.png", "../../assets/car_green.png"
    };
    std::vector<std::string> carTexturePaths = carPreviewPaths;

    std::vector<sf::Texture> mapTextures(5), carTextures(5);
    std::vector<SelectableItem> mapItems, carItems;

    for (int i = 0; i < 5; ++i) {
        mapTextures[i].loadFromFile(mapPreviewPaths[i]);
        carTextures[i].loadFromFile(carPreviewPaths[i]);

        SelectableItem m, c;
        m.sprite.setTexture(mapTextures[i]);
        m.sprite.setScale(0.5f, 0.5f);
        m.sprite.setPosition(60 + i * 180, 120);
        m.border.setSize({m.sprite.getGlobalBounds().width, m.sprite.getGlobalBounds().height});
        m.border.setFillColor(sf::Color::Transparent);
        m.border.setOutlineThickness(3);
        m.border.setOutlineColor(sf::Color::Transparent);
        m.border.setPosition(m.sprite.getPosition());
        m.id = i + 1;
        mapItems.push_back(m);

        c.sprite.setTexture(carTextures[i]);
        c.sprite.setScale(0.25f, 0.25f);
        c.sprite.setPosition(60 + i * 180, 380);
        c.border.setSize({c.sprite.getGlobalBounds().width, c.sprite.getGlobalBounds().height});
        c.border.setFillColor(sf::Color::Transparent);
        c.border.setOutlineThickness(3);
        c.border.setOutlineColor(sf::Color::Transparent);
        c.border.setPosition(c.sprite.getPosition());
        c.id = i;
        carItems.push_back(c);
    }

    sf::Text gameTitle("PolyRacing Game", font, 40);
    gameTitle.setPosition(320, 10);
    gameTitle.setFillColor(sf::Color::White);

    sf::Text title("Selectionnez une carte et une voiture", font, 24);
    title.setPosition(60, 60);
    title.setFillColor(sf::Color::White);

    sf::Text goText("GO JOUER", font, 20);
    goText.setFillColor(sf::Color::White);
    goText.setPosition(830, 630);

    sf::RectangleShape goButton(sf::Vector2f(140, 40));
    goButton.setPosition(820, 620);
    goButton.setFillColor(sf::Color(70, 150, 70));
    goButton.setOutlineThickness(2);
    goButton.setOutlineColor(sf::Color::White);

    sf::RectangleShape transitionOverlay(sf::Vector2f(WIN_WIDTH, WIN_HEIGHT));
    transitionOverlay.setFillColor(sf::Color::Black);
    float transitionAlpha = 0.0f;
    bool transitionDone = false;

    Map map;
    sf::Texture carTexture;
    sf::Sprite carSprite;
    carSprite.setScale(0.15f, 0.15f);
    sf::RenderStates trackState, grassState;

    sf::Clock transitionClock;

    while (window.isOpen()) {
        sf::Event event;
        sf::Vector2f mousePos(sf::Mouse::getPosition(window));

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (state == MENU && event.type == sf::Event::MouseButtonPressed) {
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

                if (selectedMap > 0 && selectedCar >= 0 &&
                    goButton.getGlobalBounds().contains(mousePos)) {
                    if (!map.loadTextures(selectedMap)) return -1;
                    if (!carTexture.loadFromFile(carTexturePaths[selectedCar])) return -1;
                    carSprite.setTexture(carTexture);
                    map.track = map.createTrack(selectedMap);
                    map.grass = sf::VertexArray(sf::Quads, 4);
                    map.grass[0].position = {0, 0};
                    map.grass[1].position = {1000, 0};
                    map.grass[2].position = {1000, 700};
                    map.grass[3].position = {0, 700};
                    grassState.texture = &map.grassTexture;
                    trackState.texture = &map.trackTexture;
                    carSprite.setPosition(625, 300);
                    transitionAlpha = 0;
                    transitionOverlay.setFillColor(sf::Color(0, 0, 0, 0));
                    state = TRANSITION;
                    transitionClock.restart();
                }
            }
        }

        window.clear();
        if (state == MENU) {
            window.draw(background);
            window.draw(gameTitle);
            window.draw(title);

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

                window.draw(m.sprite); window.draw(m.border);
                window.draw(c.sprite); window.draw(c.border);
            }

            if (selectedMap > 0 && selectedCar >= 0) {
                window.draw(goButton);
                window.draw(goText);
            }

        } else if (state == TRANSITION) {
            window.draw(map.grass, grassState);
            window.draw(map.track, trackState);
            window.draw(carSprite);

            float elapsed = transitionClock.getElapsedTime().asSeconds();
            if (elapsed < 1.0f) {
                transitionAlpha = 255 * (1 - elapsed);
                transitionOverlay.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(transitionAlpha)));
                window.draw(transitionOverlay);
            } else {
                state = SIMULATION;
            }

        } else if (state == SIMULATION) {
            window.draw(map.grass, grassState);
            window.draw(map.track, trackState);
            window.draw(carSprite);
        }

        window.display();
    }

    return 0;
}
*/