#include <SFML/Graphics.hpp>
#include <SFML/Config.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

// classes for reusable components and removing needless repitition
class TextWrapper {
public:
    static std::string wrapText(const std::string& text, unsigned width, const sf::Font& font, unsigned characterSize, bool bold = false);
};

class Button {
    sf::RectangleShape shape;
    sf::Text label;
public:
    Button(const sf::Vector2f& position, const sf::Vector2f& size, const std::string& text, const sf::Font& font, unsigned characterSize);
    void draw(sf::RenderWindow& window);
    bool isClicked(const sf::Vector2f& mousePos);
};

class ImageLoader {
    std::vector<sf::Texture> textures;
    std::vector<sf::Sprite> images;
public:
    void loadImage(const std::string& path);
    sf::Sprite& getImage(size_t index);
};

class Sidebar {
    sf::RectangleShape shape;
    sf::Text title;
public:
    Sidebar(const sf::Vector2f& size, const std::string& titleText, const sf::Font& font, unsigned titleSize);
    void draw(sf::RenderWindow& window);
};

class PageManager {
    size_t currentPage;
    size_t totalPages;
public:
    PageManager(size_t totalPages);
    void nextPage();
    void prevPage();
    size_t getCurrentPage() const;
};


// function to fit text based on button width
std::string wrapText(std::string text, unsigned width, const sf::Font &font, unsigned characterSize, bool bold = false) {
    unsigned currentLineLength = 0;
    std::string wordsBuffer;
    std::string wrappedText;
    std::istringstream words(text);
    std::string word;

    while (words >> word) {
        sf::Text line(wordsBuffer + " " + word, font, characterSize);
        currentLineLength = line.getLocalBounds().width;

        if (currentLineLength > width) {
            wrappedText += wordsBuffer + "\n";
            wordsBuffer = word;
        } else {
            wordsBuffer += " " + word;
        }
    }

    return wrappedText + wordsBuffer;
}

// fetches image by reading from text files and using the image names to find the image
void loadImagesFromTextFiles(std::vector<sf::Texture>& textures, std::vector<sf::Sprite>& images, sf::RenderWindow& window) {
    for (int i = 0; i < 12; ++i) {
        std::ifstream file("texts/text" + std::to_string(i) + ".txt");
        if (!file) {
            std::cerr << "Failed to open file: texts/text" << i << ".txt" << std::endl;
            continue;
        }

        std::string imageName;
        std::getline(file, imageName);

        textures.emplace_back();
        if (!textures.back().loadFromFile("images/" + imageName)) {
            std::cerr << "Could not load image: " << imageName << std::endl;
            continue;
        }

        sf::Sprite sprite(textures.back());
        sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
        sprite.setPosition((window.getSize().x / 2) + 100, window.getSize().y / 2);
        sprite.setScale(1.00f, 1.00f);
        images.push_back(sprite);
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "CodeQuest");
    sf::Color defaultButtonColor = sf::Color::White;
    sf::Color activeButtonColor = sf::Color::Yellow; 

    sf::Font font;
    if (!font.loadFromFile("fonts/Montserrat Light.otf")) {
        std::cerr << "Could not load font" << std::endl;
    }

    sf::Text title("CodeQuest", font, 30);
    title.setPosition(window.getSize().x - title.getLocalBounds().width - 25, 25);
    title.setStyle(sf::Text::Bold);

    sf::RectangleShape sidebar(sf::Vector2f(200, window.getSize().y));
    sidebar.setFillColor(sf::Color(50, 50, 50));

    std::vector<sf::RectangleShape> buttons(5, sf::RectangleShape(sf::Vector2f(180, 70)));
    for (int i = 0; i < 5; ++i) {
        buttons[i].setPosition(10, 50 + i * 80 + 30);
        buttons[i].setFillColor(sf::Color::White);
    }

    std::vector<std::string> buttonNames = {
        "Elementary Programming", "Functions", "Loops", "Arrays", "Object and Classes",
        "Object Oriented Programming", "Pointers and Dynamic Memory Management",
        "Templates, Vectors, and Stacks", "File Input and Output", "Operator Overloading",
        "Exception Handling", "Recursion"
    };

    sf::Text sidebarTitle("Lessons", font, 40);
    sidebarTitle.setPosition(25, 10);
    sidebarTitle.setFillColor(sf::Color::White);

    sf::Text nextButtonText("Next", font, 25);
    sf::Text prevButtonText("Previous", font, 25);

    sf::RectangleShape nextButton(sf::Vector2f(180, 70));
    nextButton.setPosition(10, 50 + 5 * 90);
    nextButton.setFillColor(sf::Color::Green);

    sf::RectangleShape prevButton(sf::Vector2f(180, 70));
    prevButton.setPosition(10, 50 + 6 * 90);
    prevButton.setFillColor(sf::Color::Red);

    std::vector<sf::Text> buttonLabels;
    for (const auto& name : buttonNames) {
        std::string wrappedText = wrapText(name, 90, font, 8);
        sf::Text label(wrappedText, font, 15);
        label.setFillColor(sf::Color::Black);
        buttonLabels.push_back(label);
    }

    std::vector<sf::Texture> textures;
    std::vector<sf::Sprite> images;
    textures.reserve(12);
    images.reserve(12);

    loadImagesFromTextFiles(textures, images, window);

    sf::Sprite currentImage = images[0];

    size_t currentPage = 0;
    size_t totalPages = (buttonNames.size() + 4) / 5;
    size_t currentImageIndex = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                for (size_t i = 0; i < buttons.size(); ++i) {
                    if (buttons[i].getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
                        size_t index = currentPage * 5 + i;
                        if (index < buttonNames.size()) { 
                            currentImageIndex = index;
                            break;
                        }
                    }
                }

                if (nextButton.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
                    if (currentPage < totalPages - 1) currentPage++;
                }
                if (prevButton.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
                    if (currentPage > 0) currentPage--;
                }
            }
        }

        for (size_t i = 0; i < buttons.size(); ++i) {
            size_t globalIndex = currentPage * 5 + i;
            buttons[i].setFillColor(defaultButtonColor);

            if (globalIndex == currentImageIndex) {
                buttons[i].setFillColor(activeButtonColor);
            }
        }

        nextButtonText.setPosition(
            nextButton.getPosition().x + nextButton.getSize().x / 2 - nextButtonText.getLocalBounds().width / 2,
            nextButton.getPosition().y + nextButton.getSize().y / 2 - nextButtonText.getLocalBounds().height / 2
        );

        prevButtonText.setPosition(
            prevButton.getPosition().x + prevButton.getSize().x / 2 - prevButtonText.getLocalBounds().width / 2,
            prevButton.getPosition().y + prevButton.getSize().y / 2 - prevButtonText.getLocalBounds().height / 2
        );

        window.clear();
        window.draw(sidebar);
        window.draw(sidebarTitle);

        for (size_t i = 0; i < buttons.size() && i + currentPage * 5 < buttonNames.size(); ++i) {
            window.draw(buttons[i]);
            float x = buttons[i].getPosition().x + buttons[i].getSize().x / 2 - buttonLabels[i + currentPage * 5].getLocalBounds().width / 2;
            float y = buttons[i].getPosition().y + buttons[i].getSize().y / 2 - buttonLabels[i + currentPage * 5].getLocalBounds().height / 2;
            buttonLabels[i + currentPage * 5].setPosition(x, y);
            window.draw(buttonLabels[i + currentPage * 5]);
        }

        window.draw(nextButton);
        window.draw(prevButton);
        window.draw(nextButtonText);
        window.draw(prevButtonText);
        window.draw(images[currentImageIndex]);
        window.draw(title);
        window.display();
    }

    return 0;
}
