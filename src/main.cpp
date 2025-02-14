#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "RC Car Simulation");

    // Circuit
    sf::RectangleShape circuit(sf::Vector2f(600, 400));
    circuit.setFillColor(sf::Color::Green);
    circuit.setPosition(100, 100);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);
        window.draw(circuit);
        window.display();
    }

    return 0;
}
