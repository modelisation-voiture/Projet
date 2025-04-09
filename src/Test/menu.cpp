#include <SFML/Graphics.hpp>
#include "menu.hpp"

int afficherMenu() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Menu du jeu");

    sf::Font font;
    font.loadFromFile("../../assets/Roboto-Regular.ttf");

    // Titres
    sf::Text titre("Choisissez votre circuit", font, 50);
    titre.setPosition(300, 50);
    titre.setFillColor(sf::Color::White);

    sf::RectangleShape circuitButtons[3];
    sf::Text circuitLabels[3];
    std::string noms[] = {"Circuit 1", "Circuit 2", "Circuit 3"};

    for (int i = 0; i < 3; ++i) {
        circuitButtons[i].setSize(sf::Vector2f(300, 150));
        circuitButtons[i].setPosition(100 + i * 360, 200);
        circuitButtons[i].setFillColor(sf::Color(100, 100, 200));
        circuitLabels[i].setFont(font);
        circuitLabels[i].setString(noms[i]);
        circuitLabels[i].setCharacterSize(30);
        circuitLabels[i].setFillColor(sf::Color::White);
        circuitLabels[i].setPosition(130 + i * 360, 270);
    }

    sf::RectangleShape goButton(sf::Vector2f(250, 80));
    goButton.setPosition(250, 500);
    goButton.setFillColor(sf::Color(0, 200, 0));

    sf::Text goText("GO JOUER", font, 35);
    goText.setPosition(275, 515);
    goText.setFillColor(sf::Color::Black);

    sf::RectangleShape quitButton(sf::Vector2f(250, 80));
    quitButton.setPosition(700, 500);
    quitButton.setFillColor(sf::Color(200, 0, 0));

    sf::Text quitText("QUITTER", font, 35);
    quitText.setPosition(730, 515);
    quitText.setFillColor(sf::Color::White);

    int selected = 2;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mouse(sf::Mouse::getPosition(window));

                for (int i = 0; i < 3; ++i) {
                    if (circuitButtons[i].getGlobalBounds().contains(mouse)) {
                        selected = i + 1;
                    }
                }

                if (goButton.getGlobalBounds().contains(mouse)) {
                    window.close();
                    return selected;
                }

                if (quitButton.getGlobalBounds().contains(mouse)) {
                    window.close();
                    return -1;
                }
            }
        }

        window.clear(sf::Color::Black);
        window.draw(titre);

        for (int i = 0; i < 3; ++i) {
            circuitButtons[i].setOutlineThickness(i + 1 == selected ? 5 : 0);
            circuitButtons[i].setOutlineColor(sf::Color::Yellow);
            window.draw(circuitButtons[i]);
            window.draw(circuitLabels[i]);
        }

        window.draw(goButton);
        window.draw(goText);
        window.draw(quitButton);
        window.draw(quitText);
        window.display();
    }

    return selected;
}
