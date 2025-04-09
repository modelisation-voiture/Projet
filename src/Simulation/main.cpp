// Version 4.0 - Implémentation PID

#include <SFML/Graphics.hpp>
#include "../Model/force.hpp"  // Contient déjà voiture.hpp
#include "../Affichage/map.hpp"
#include "../PID/pid_controller.hpp"
#include <cmath>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <vector>
#include <deque>

// Déclaration des textures et structures de base
sf::Texture trackTexture, grassTexture, borderTexture, carTexture;

// Structure pour le chronomètre
struct Chronometre {
    sf::Clock clock;
    float tempsActuel = 0.0f;
    float meilleurTemps = std::numeric_limits<float>::max();
    int tours = 0;
    bool estActif = false;
    std::vector<float> tempsParTour;  // Pour stocker le temps de chaque tour
    
    void demarrer() {
        clock.restart();
        estActif = true;
    }
    
    void arreter() {
        if (estActif) {
            tempsActuel = clock.getElapsedTime().asSeconds();
            tempsParTour.push_back(tempsActuel);  // Enregistrer le temps de ce tour
            if (tempsActuel < meilleurTemps) {
                meilleurTemps = tempsActuel;
            }
            estActif = false;
            tours++;
        }
    }
    
    float getTempsActuel() const {
        if (estActif) {
            return clock.getElapsedTime().asSeconds();
        }
        return tempsActuel;
    }
};

// Structure pour la ligne de départ/arrivée
struct LigneArrivee {
    sf::RectangleShape ligne;
    sf::Vector2f position;
    float largeur;
    bool voitureATraverse = false;
    
    LigneArrivee(float x, float y, float largeur, float epaisseur = 5.0f) 
        : position(x, y), largeur(largeur) {
        ligne.setSize(sf::Vector2f(largeur, epaisseur));
        ligne.setPosition(x, y);
        ligne.setFillColor(sf::Color(255, 255, 255, 200)); // Blanc semi-transparent
        
        // Motif damier
        sf::Vector2f tileSize(20.0f, epaisseur);
        int numTiles = static_cast<int>(largeur / tileSize.x);
        
        // Créer le motif damier comme une texture procédurale
        sf::Image checkerPattern;
        checkerPattern.create(2, 1);
        checkerPattern.setPixel(0, 0, sf::Color::Black);
        checkerPattern.setPixel(1, 0, sf::Color::White);
        
        sf::Texture checkerTexture;
        checkerTexture.loadFromImage(checkerPattern);
        checkerTexture.setRepeated(true);
        
        ligne.setTexture(&checkerTexture);
        ligne.setTextureRect(sf::IntRect(0, 0, numTiles * 2, 1));
    }
    
    bool verifierTraversee(const sf::Vector2f& posVoiture, const sf::Vector2f& posPrec, float voitureRadius = 20.0f) {
        float ligneY = ligne.getPosition().y;
        float ligneXDebut = ligne.getPosition().x;
        float ligneXFin = ligneXDebut + largeur;
        
        // Vérifier si la voiture a traversé la ligne
        bool estDansLargeur = posVoiture.x >= ligneXDebut && posVoiture.x <= ligneXFin;
        bool aTraverseY = (posPrec.y < ligneY && posVoiture.y >= ligneY) || 
                          (posPrec.y > ligneY && posVoiture.y <= ligneY);
        
        if (estDansLargeur && aTraverseY) {
            bool aTraverse = !voitureATraverse;
            voitureATraverse = true;
            return aTraverse;
        } else if (!estDansLargeur || std::abs(posVoiture.y - ligneY) > voitureRadius) {
            voitureATraverse = false;
        }
        
        return false;
    }
};

// Structure pour représenter un point de trajectoire
struct Waypoint {
    sf::Vector2f position;
    float targetRadius;  // Rayon de courbure cible à ce point
};

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

// Normaliser un angle en radians entre -PI et PI
float normalizeAngle(float angle) {
    while (angle > M_PI) angle -= 2 * M_PI;
    while (angle < -M_PI) angle += 2 * M_PI;
    return angle;
}

// Fonction pour trouver le centre de la piste à partir de la position de la voiture
sf::Vector2f findTrackCenter(const sf::Vector2f& carPos, const sf::VertexArray& track, int numSegments) {
    float centerX = 400, centerY = 300;
    sf::Vector2f centerToPos = carPos - sf::Vector2f(centerX, centerY);
    float angle = atan2(centerToPos.y, centerToPos.x);
    
    // Convertir l'angle en index du segment
    float angleNormalized = (angle + M_PI) / (2 * M_PI);  // Normaliser entre 0 et 1
    int segmentIndex = static_cast<int>(angleNormalized * numSegments) % numSegments;
    
    // Les points intérieur et extérieur du segment
    sf::Vector2f inner = track[segmentIndex * 2].position;
    sf::Vector2f outer = track[segmentIndex * 2 + 1].position;
    
    // Le centre de la piste est la moyenne des points intérieur et extérieur
    return (inner + outer) / 2.0f;
}

// Calculer l'erreur latérale (distance perpendiculaire à la ligne centrale)
float calculateLateralError(const Voiture& voiture, const sf::VertexArray& track, int numSegments) {
    sf::Vector2f carPos(voiture.getX(), voiture.getY());
    sf::Vector2f center(400, 300);
    
    // Calculer le vecteur du centre à la voiture
    sf::Vector2f centerToPos = carPos - center;
    float distanceFromCenter = sqrt(centerToPos.x * centerToPos.x + centerToPos.y * centerToPos.y);
    
    // Normaliser le vecteur
    sf::Vector2f directionToPos(0, 0);
    if (distanceFromCenter > 0) {
        directionToPos = centerToPos / distanceFromCenter;
    }
    
    // Calculer l'angle polaire de la voiture par rapport au centre
    float carPolarAngle = atan2(centerToPos.y, centerToPos.x);
    
    // Trouver le point correspondant sur la piste
    float angleNormalized = (carPolarAngle + M_PI) / (2 * M_PI);  // Normaliser entre 0 et 1
    int segmentIndex = static_cast<int>(angleNormalized * numSegments) % numSegments;
    
    // Points intérieur et extérieur de la piste à cet angle
    sf::Vector2f innerPoint = track[segmentIndex * 2].position;
    sf::Vector2f outerPoint = track[segmentIndex * 2 + 1].position;
    
    // Point central de la piste
    sf::Vector2f trackCenter = (innerPoint + outerPoint) / 2.0f;
    
    // Distance radiale idéale (distance du centre au centre de la piste)
    sf::Vector2f trackToCenter = trackCenter - center;
    float idealRadialDist = sqrt(trackToCenter.x * trackToCenter.x + trackToCenter.y * trackToCenter.y);
    
    // Erreur radiale (différence entre la distance radiale actuelle et idéale)
    float radialError = distanceFromCenter - idealRadialDist;
    
    // Direction tangentielle idéale (tangente à la piste)
    int nextSegmentIndex = (segmentIndex + 1) % numSegments;
    sf::Vector2f nextInnerPoint = track[nextSegmentIndex * 2].position;
    sf::Vector2f nextOuterPoint = track[nextSegmentIndex * 2 + 1].position;
    sf::Vector2f nextTrackCenter = (nextInnerPoint + nextOuterPoint) / 2.0f;
    
    sf::Vector2f tangent = nextTrackCenter - trackCenter;
    float tangentLength = sqrt(tangent.x * tangent.x + tangent.y * tangent.y);
    if (tangentLength > 0) {
        tangent /= tangentLength;
    }
    
    // Direction de la voiture
    float carAngle = voiture.getAngle() * M_PI / 180.0f;
    sf::Vector2f carDir(cos(carAngle), sin(carAngle));
    
    // Produit scalaire pour l'alignement (1 = parfaitement aligné, -1 = totalement opposé)
    float alignmentDot = carDir.x * tangent.x + carDir.y * tangent.y;
    
    // Produit vectoriel pour déterminer si on est à l'intérieur ou à l'extérieur
    float cross = (center.x - carPos.x) * (trackCenter.y - carPos.y) - 
                 (center.y - carPos.y) * (trackCenter.x - carPos.x);
    
    // Amplifier l'erreur quand on s'éloigne trop
    float errorFactor = std::min(1.0f, std::abs(radialError) / 50.0f + 0.7f);
    
    // L'erreur est positive si on est à l'extérieur et négative si on est à l'intérieur
    return radialError * (cross >= 0 ? 1.0f : -1.0f) * errorFactor;
}

// Vérifier si la voiture est sur l'herbe
bool isOnGrass(const sf::Vector2f& carPos, const sf::Vector2f& center, float innerRadius, float outerRadius) {
    float dx = carPos.x - center.x;
    float dy = carPos.y - center.y;
    float distance = std::sqrt(dx * dx + dy * dy);
    
    return (distance < innerRadius || distance > outerRadius);
}

// Fonction pour calculer la vitesse cible en fonction de la courbure
float calculateTargetSpeed(float curvature, float maxSpeed, float minSpeed) {
    // Plus la courbure est grande, plus la vitesse est réduite
    float speedFactor = std::max(0.0f, 1.0f - std::abs(curvature));
    return minSpeed + speedFactor * (maxSpeed - minSpeed);
}



int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "RC Car Simulation - PID Controller");
    window.setFramerateLimit(60);

    float tempsDepuisDernierUpdateTexte = 0.0f;

    // Créer le circuit
    auto [track, waypoints, maxDistance] = createTrackWithWaypoints();
    sf::VertexArray grass = createGrass(track, 40);
    
    // Centre du circuit et rayon pour la détection de l'herbe
    sf::Vector2f center(400, 300);
    float innerRadius = 100;  // Rayon intérieur de la piste
    float outerRadius = 300;  // Rayon extérieur de la piste

    // Initialiser le contrôleur PID pour la direction
    PIDController steeringPID(1.5f, 0.02f, 0.5f, 10.0f, 1.0f);

    
    // Initialiser le contrôleur PID pour la vitesse
    PIDController speedPID(0.5f, 0.001f, 0.1f, 10.0f, 1.0f);

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

    // Visualisation de la ligne centrale
    sf::CircleShape centerMarker(3);
    centerMarker.setFillColor(sf::Color::Yellow);
    centerMarker.setOrigin(3, 3);

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
    Voiture voiture(150, 300, -64, 20.0, 0.3); // x, y, angle, masse, empattement
    ForceMotriceProgressive moteur(8000, 10.0, 5.0);
    ForceFrottement frottement(0.1);
    ForceFreinage frein(0.3);
    ForceAerodynamique air(0.0072);
    ForceVirage virage(0.0);
    ForceFreinGlisse freinGlisse(10);
    std::vector<Force*> forces = {&moteur, &frottement, &air, &frein, &virage, &freinGlisse};

    // Mode autonome activé par défaut
    bool autonomousMode = true;
    
    // Paramètres pour le PID
    float targetSpeed = 40.0f;    // Vitesse cible en m/s
    float maxSpeed = 60.0f;       // Vitesse maximale en m/s
    float minSpeed = 20.0f;       // Vitesse minimale dans les virages
    
    // Variables pour visualiser le PID
    sf::Vertex errorLine[2];
    errorLine[0].color = sf::Color::Red;
    errorLine[1].color = sf::Color::Red;
    
    sf::Vertex centerLine[2];
    centerLine[0].color = sf::Color::Green;
    centerLine[1].color = sf::Color::Green;

    // Initialiser le chronomètre
    Chronometre chrono;

    // Créer une ligne de départ/arrivée en bas du circuit
    float trackWidth = 100.0f; // Largeur approximative de la piste
    float voitureInitX = 150.0f;
    float voitureInitY = 300.0f;
    LigneArrivee ligneArrivee(voitureInitX - trackWidth/2, voitureInitY, trackWidth);
    bool premierPassage = true;

    // Position précédente de la voiture pour détection de traversée
    sf::Vector2f positionPrecedente(voiture.getX(), voiture.getY());

    // Horloge pour le calcul du delta time
    sf::Clock clock;
    
    while (window.isOpen()) {
        // Calculer le temps écoulé
        float dt = clock.restart().asSeconds();
        tempsDepuisDernierUpdateTexte += dt;
        
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A) {
                // Touche A pour activer/désactiver le mode autonome
                autonomousMode = !autonomousMode;
                steeringPID.reset();  // Réinitialiser le PID lors du changement de mode
                speedPID.reset();
            }
        }

        // === CONTRÔLES ===
        int etat = 0;
        double angle_braquage = 0.0;
        voiture.activerFrein(false);

        if (autonomousMode) {
            // Trouver le waypoint le plus proche devant la voiture
            sf::Vector2f carPos(voiture.getX(), voiture.getY());
            float carAngle = voiture.getAngle() * M_PI / 180.0f;
            sf::Vector2f carDir(cos(carAngle), sin(carAngle));
            
            // Trouver le waypoint le plus proche et approprié
            sf::Vector2f targetWaypoint;
            float minDist = std::numeric_limits<float>::max();
            size_t targetIdx = 0;
            
            for (size_t i = 0; i < waypoints.size(); ++i) {
                sf::Vector2f waypoint = waypoints[i].position;
                sf::Vector2f toWaypoint = waypoint - carPos;
                float dist = std::sqrt(toWaypoint.x * toWaypoint.x + toWaypoint.y * toWaypoint.y);
                
                // Calculer le produit scalaire pour voir si le waypoint est devant la voiture
                float dotProduct = carDir.x * toWaypoint.x + carDir.y * toWaypoint.y;
                
                // Ne considérer que les waypoints qui sont devant et pas trop loin
                if (dotProduct > 0 && dist < 200 && dist < minDist) {
                    minDist = dist;
                    targetWaypoint = waypoint;
                    targetIdx = i;
                }
            }
            
            // Si aucun waypoint trouvé devant, prendre le plus proche
            if (minDist == std::numeric_limits<float>::max()) {
                for (size_t i = 0; i < waypoints.size(); ++i) {
                    sf::Vector2f waypoint = waypoints[i].position;
                    sf::Vector2f toWaypoint = waypoint - carPos;
                    float dist = std::sqrt(toWaypoint.x * toWaypoint.x + toWaypoint.y * toWaypoint.y);
                    
                    if (dist < minDist) {
                        minDist = dist;
                        targetWaypoint = waypoint;
                        targetIdx = i;
                    }
                }
            }
            
            // Calculer l'angle vers le waypoint cible
            sf::Vector2f toTarget = targetWaypoint - carPos;
            float targetAngle = atan2(toTarget.y, toTarget.x);
            
            // Différence d'angle normalisée entre -PI et PI
            float angleDiff = normalizeAngle(targetAngle - carAngle);
            
            // Calculer l'erreur latérale par rapport à la ligne vers le waypoint
            float lateralError = minDist * sin(angleDiff);
            
            // Appliquer un PID pour l'angle de braquage avec des limites plus strictes à haute vitesse
            float speed = voiture.getVitesse();
            float maxSteeringAngle = std::min(1.0f, 20.0f / (speed + 10.0f)); // Limiter le braquage à haute vitesse
            
            // Calculer l'angle de braquage avec le PID et les limites
            float rawSteeringAngle = steeringPID.calculate(lateralError, dt);
            angle_braquage = std::clamp(rawSteeringAngle, -maxSteeringAngle, maxSteeringAngle);
            
            // Empêcher les demi-tours en vérifiant si on s'éloigne trop de la direction de la piste
            float trackDirection = atan2(toTarget.y, toTarget.x);
            float carTrackAngleDiff = normalizeAngle(carAngle - trackDirection);
            
            if (std::abs(carTrackAngleDiff) > M_PI * 0.75f) {
                // La voiture est dans la mauvaise direction, forcer une correction
                angle_braquage = (carTrackAngleDiff > 0) ? -maxSteeringAngle : maxSteeringAngle;
            }
            
            // Calculer la courbure de la piste au waypoint actuel
            float targetRadius = waypoints[targetIdx].targetRadius;
            float curvature = 1.0f / std::max(1.0f, targetRadius);
            
            // Calculer la vitesse cible en fonction de la courbure et de l'erreur latérale
            float speedFactor = 1.0f - std::min(1.0f, std::abs(curvature) * 5.0f + std::abs(lateralError) / 100.0f);
            float speedTarget = minSpeed + speedFactor * (maxSpeed - minSpeed);
            
            // Ralentir davantage si on est loin du centre ou dans l'herbe
            if (std::abs(lateralError) > 30.0f) {
                speedTarget *= 0.8f;
            }
            
            // Vérifier si on est sur l'herbe
            bool onGrass = isOnGrass(carPos, center, innerRadius, outerRadius);
            if (onGrass) {
                speedTarget *= 0.5f; // Ralentir fortement sur l'herbe
            }
            
            // Calculer l'erreur de vitesse
            float speedError = speedTarget - voiture.getVitesse();
            
            // Appliquer le PID pour la vitesse
            float throttleControl = speedPID.calculate(speedError, dt);
            
            // Appliquer les contrôles
            if (throttleControl > 0.1f) {
                etat = 1;  // Accélérer
                
                voiture.setAccelerationActive(true);
            } else if (throttleControl < -0.1f) {
                voiture.activerFrein(true);  // Freiner
                etat = -1;
            } else {
                voiture.setAccelerationActive(false);
            }
            
            // Visualiser la cible
            centerLine[0].position = carPos;
            centerLine[1].position = targetWaypoint;
            
            // Visualiser l'erreur latérale
            sf::Vector2f perpDir(-toTarget.y, toTarget.x);
            float perpLength = std::sqrt(perpDir.x * perpDir.x + perpDir.y * perpDir.y);
            if (perpLength > 0) {
                perpDir /= perpLength;
            }
            
            errorLine[0].position = carPos;
            errorLine[1].position = carPos + perpDir * lateralError;
        } else {   // Mode manuel - contrôles utilisateur
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                voiture.setAccelerationActive(etat=1);
            } else {
                voiture.setAccelerationActive(etat=0);
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
            if (f == &moteur && !dynamic_cast<ForceMotriceProgressive*>(f)) {
                if (etat != 1) continue;
            }
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
        if (tempsDepuisDernierUpdateTexte >= 0.25f) {  // toutes les 250 ms
            auto [kp, ki, kd] = steeringPID.getConstants();
            
            std::ostringstream oss;
            oss << "Vitesse : " << std::fixed << std::setprecision(2) << voiture.getVitesse() << " m/s\n"
                << "Angle   : " << std::fixed << std::setprecision(2) << voiture.getAngle() << " degres\n"
                << "Angle braquage : " << std::fixed << std::setprecision(2) << angle_braquage << "\n"
                << "Mode : " << (autonomousMode ? "AUTONOME - PID (A pour desactiver)" : "MANUEL (A pour activer)")
                << "\nPID: Kp=" << kp << ", Ki=" << ki << ", Kd=" << kd << "\n"
                << "Accel. moteur : " 
                << std::fixed << std::setprecision(2) 
                << moteur.getAccelerationCourante() << " m/s²\n";
                
            hudText.setString(oss.str());

            tempsDepuisDernierUpdateTexte = 0.0f;
        }
        
        sf::Vector2f positionVoiture(voiture.getX(), voiture.getY());
        if (ligneArrivee.verifierTraversee(positionVoiture, positionPrecedente)) {
            if (premierPassage) {
                // Premier passage de la ligne, on démarre simplement le chrono
                chrono.demarrer();
                premierPassage = false;
            } else {
                // Passages suivants, on arrête le chrono actuel, on enregistre le temps
                // et on redémarre immédiatement pour le tour suivant
                chrono.arreter();
                chrono.demarrer();
            }
        }
        positionPrecedente = positionVoiture;

        // Afficher les informations du chronomètre
        std::ostringstream chronoText;
        chronoText << "Tours: " << chrono.tours << "\n";

        // Afficher le temps du tour en cours
        if (chrono.estActif) {
            chronoText << "Tour actuel: " << std::fixed << std::setprecision(2) 
                    << chrono.getTempsActuel() << " s\n";
        }

        // Afficher le meilleur temps
        if (chrono.tours > 0) {
            chronoText << "Meilleur temps: " << std::fixed << std::setprecision(2) 
                    << chrono.meilleurTemps << " s\n";
        }

        // Afficher les temps des tours précédents (limité aux 3 derniers)
        if (!chrono.tempsParTour.empty()) {
            chronoText << "Derniers tours:\n";
            int start = std::max(0, static_cast<int>(chrono.tempsParTour.size()) - 3);
            for (int i = start; i < chrono.tempsParTour.size(); i++) {
                chronoText << "Tour " << (i + 1) << ": " << std::fixed << std::setprecision(2) 
                        << chrono.tempsParTour[i] << " s\n";
            }
        }

        sf::Text chronoDisplay;
        chronoDisplay.setFont(font);
        chronoDisplay.setCharacterSize(18);
        chronoDisplay.setFillColor(sf::Color::Yellow);
        chronoDisplay.setPosition(10, 150);
        chronoDisplay.setString(chronoText.str());

        window.clear();
        window.draw(grass, grassState);
        window.draw(track, trackState);
        
        // Afficher les lignes d'aide du PID en mode autonome
        if (autonomousMode) {
            window.draw(centerLine, 2, sf::Lines);
            window.draw(errorLine, 2, sf::Lines);
            
            // Afficher le centre de piste détecté
            sf::Vector2f trackCenter = findTrackCenter(sf::Vector2f(voiture.getX(), voiture.getY()), track, 100);
            centerMarker.setPosition(trackCenter);
            window.draw(centerMarker);
        }
        
        window.draw(carSprite);
        window.draw(directionIndicator);
        window.draw(hudText);
        window.draw(ligneArrivee.ligne);
        window.draw(chronoDisplay);
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