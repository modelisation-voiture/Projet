//version pour le menu

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "map1.hpp"
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
