#include <SFML/Graphics.hpp>
#include <SFML/Config.hpp>
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "CodeQuest");

    // Create a font
    sf::Font font;
    if (!font.loadFromFile("fonts/Montserrat Light.otf")) {
        std::cerr << "Could not load font" << std::endl;
    }

    // Create a title
    sf::Text title("CodeQuest", font, 150);
    title.setPosition((window.getSize().x - title.getLocalBounds().width) / 2, 150);

    // Create a start button
    sf::RectangleShape button(sf::Vector2f(200, 50));
    button.setPosition((window.getSize().x - button.getLocalBounds().width) / 2, 450);
    sf::Text buttonText("Start", font, 30);
    buttonText.setPosition(button.getPosition().x + (button.getLocalBounds().width - buttonText.getLocalBounds().width) / 2, 
                           button.getPosition().y + (button.getLocalBounds().height - buttonText.getLocalBounds().height) / 2);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close(); 

            // Check if the start button is clicked
            if (event.type == sf::Event::MouseButtonPressed) {
                if (button.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
                    std::cout << "Start button clicked" << std::endl;
                    // Start your game here
                }
            }

            // Change button color on hover
            if (button.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
                button.setFillColor(sf::Color::Cyan);
            } else {
                button.setFillColor(sf::Color::White);
            }
        }

        window.clear();
        // Draw your objects here
        window.draw(title);
        window.draw(button);
        window.draw(buttonText);
        window.display();
    }

    return 0;
}