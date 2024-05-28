#include <SFML/Graphics.hpp>
#include <SFML/Config.hpp>
#include <iostream>
#include <fstream>
#include <vector>

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "CodeQuest");

    sf::Font font;
    if (!font.loadFromFile("fonts/Montserrat Light.otf")) {
        std::cerr << "Could not load font" << std::endl;
    }

    sf::Text title("CodeQuest", font, 30);
    title.setPosition(window.getSize().x - title.getLocalBounds().width - 25, 25);
    title.setStyle(sf::Text::Bold);

    sf::RectangleShape sidebar(sf::Vector2f(200, window.getSize().y));
    sidebar.setFillColor(sf::Color(50, 50, 50));

    std::vector<sf::RectangleShape> buttons;
    for (int i = 0; i < 3; i++) {
        sf::RectangleShape button(sf::Vector2f(180, 50));
        button.setPosition(10, 10 + i * 60);
        button.setFillColor(sf::Color::White); 
        buttons.push_back(button);
    }

    std::vector<std::string> buttonNames = {"Functions", "Classes", "OOP"};

    sf::Text sidebarTitle("Lessons", font, 30);
    sidebarTitle.setPosition(10, 10);
    sidebarTitle.setFillColor(sf::Color::White);

    std::vector<sf::Text> buttonLabels;
    for (int i = 0; i < 3; i++) {
        sf::Text label(buttonNames[i], font, 20);
        float x = buttons[i].getPosition().x + buttons[i].getSize().x / 2 - label.getLocalBounds().width / 2;
        float y = buttons[i].getPosition().y + buttons[i].getSize().y / 2 - label.getLocalBounds().height / 2;
        label.setPosition(x, y);
        label.setFillColor(sf::Color::Black); 
        buttonLabels.push_back(label);
    }

    std::vector<sf::Text> texts;
    for (int i = 0; i < 3; i++) {
        std::ifstream file("texts/text" + std::to_string(i) + ".txt");
        std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        sf::Text text(str, font, 30);
        text.setPosition(window.getSize().x / 2, window.getSize().y / 2);
        texts.push_back(text);
    }

    sf::Text currentText = texts[0];

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close(); 

            if (event.type == sf::Event::MouseButtonPressed) {
                for (int i = 0; i < buttons.size(); i++) {
                    if (buttons[i].getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
                        currentText = texts[i];
                    }
                }
            }
        }

        window.clear();
        window.draw(sidebar);
        window.draw(sidebarTitle);
        for (const auto& button : buttons) {
            window.draw(button);
        }
        for (const auto& label : buttonLabels) {
            window.draw(label);
        }
        window.draw(currentText);
        window.draw(title);
        window.display();
    }

    return 0;
}