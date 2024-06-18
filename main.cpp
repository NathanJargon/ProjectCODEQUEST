#include <SFML/Graphics.hpp>
#include <SFML/Config.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <sstream>

class TextWrapper {
public:
    static std::string wrapText(const std::string& text, unsigned width, const sf::Font& font, unsigned characterSize, bool bold = false) {
        unsigned currentLineLength = 0;
        std::string wordsBuffer;
        std::string wrappedText;
        std::istringstream words(text);
        std::string word;

        while (words >> word) {
            sf::Text line(wordsBuffer + " " + word, font, characterSize);
            if (bold) {
                line.setStyle(sf::Text::Bold);
            }
            currentLineLength = line.getLocalBounds().width;

            if (currentLineLength > width) {
                wrappedText += wordsBuffer + "\n";
                wordsBuffer = word;
            } else {
                if (!wordsBuffer.empty()) {
                    wordsBuffer += " ";
                }
                wordsBuffer += word;
            }
        }

        return wrappedText + wordsBuffer;
    }
};

void loadImagesFromTextFilesRecursively(int fileIndex, std::vector<std::vector<std::unique_ptr<sf::Texture>>>& textures, std::vector<std::vector<sf::Sprite>>& images, sf::RenderWindow& window) {
    if (fileIndex >= 12) return; 

    textures.resize(12);
    images.resize(12);

    std::ifstream file("texts/text" + std::to_string(fileIndex) + ".txt");
    if (!file) {
        std::cerr << "Failed to open file: texts/text" << fileIndex << ".txt" << std::endl;
        loadImagesFromTextFilesRecursively(fileIndex + 1, textures, images, window); 
        return;
    }

    std::string imageName;
    while (std::getline(file, imageName)) {
        auto texture = std::make_unique<sf::Texture>();
        if (!texture->loadFromFile("images/" + imageName)) {
            std::cerr << "Could not load image: " << imageName << std::endl;
            continue;
        }

        textures[fileIndex].push_back(std::move(texture));

        sf::Sprite sprite(*textures[fileIndex].back());
        sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
        sprite.setPosition((window.getSize().x / 2) + 100, window.getSize().y / 2);
        sprite.setScale(1.00f, 1.00f);
        images[fileIndex].push_back(sprite);
    }

    loadImagesFromTextFilesRecursively(fileIndex + 1, textures, images, window); 
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "CodeQuest");
    sf::Color defaultButtonColor = sf::Color::White;
    sf::Color activeButtonColor = sf::Color::Yellow;
    sf::Color inactiveButtonColor = sf::Color(128, 128, 128); 

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

    sf::Text pageNumberText;
    pageNumberText.setFont(font); 
    pageNumberText.setCharacterSize(20);
    pageNumberText.setFillColor(sf::Color::White); 

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
    
    sf::Text nextImageButtonText(">", font, 25);
    sf::Text prevImageButtonText("<", font, 25);
    
    sf::RectangleShape nextImageButton(sf::Vector2f(50, 60));
    nextImageButton.setPosition(1200, 50 + 6.5 * 90);
    nextImageButton.setFillColor(sf::Color::Green); 
    
    sf::RectangleShape prevImageButton(sf::Vector2f(50, 60));
    prevImageButton.setPosition(1100, 50 + 6.5 * 90); 
    prevImageButton.setFillColor(sf::Color::Red); 

    float verticalAdjustment = 20.0f; 

    sf::FloatRect nextImageButtonBounds = nextImageButton.getGlobalBounds();
    sf::FloatRect nextImageTextBounds = nextImageButtonText.getLocalBounds();
    nextImageButtonText.setOrigin(nextImageTextBounds.width / 2.0f, nextImageTextBounds.height / 2.0f);
    nextImageButtonText.setPosition(nextImageButtonBounds.left + nextImageButtonBounds.width / 2.0f, nextImageButtonBounds.top + nextImageButtonBounds.height / 2.0f - verticalAdjustment);

    sf::FloatRect prevImageButtonBounds = prevImageButton.getGlobalBounds();
    sf::FloatRect prevImageTextBounds = prevImageButtonText.getLocalBounds();
    prevImageButtonText.setOrigin(prevImageTextBounds.width / 2.0f, prevImageTextBounds.height / 2.0f);
    prevImageButtonText.setPosition(prevImageButtonBounds.left + prevImageButtonBounds.width / 2.0f, prevImageButtonBounds.top + prevImageButtonBounds.height / 2.0f - verticalAdjustment);

    std::vector<sf::Text> buttonLabels;
    for (const auto& name : buttonNames) {
        std::string wrappedText = TextWrapper::wrapText(name, 90, font, 8);
        sf::Text label(wrappedText, font, 15);
        label.setFillColor(sf::Color::Black);
        buttonLabels.push_back(label);
    }
    
    std::vector<std::vector<std::unique_ptr<sf::Texture>>> textures(12);
    std::vector<std::vector<sf::Sprite>> images(12);
    
    loadImagesFromTextFilesRecursively(0, textures, images, window);

    size_t currentPage = 0;
    size_t totalPages = (buttonNames.size() + 4) / 5;
    size_t currentButtonIndex = 0;
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
                            currentButtonIndex = index;
                            currentImageIndex = 0;
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

                if (nextImageButton.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
                    if (currentImageIndex < images[currentButtonIndex].size() - 1) currentImageIndex++;
                }
                if (prevImageButton.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
                    if (currentImageIndex > 0) currentImageIndex--;
                }
            }
        }

        for (size_t i = 0; i < buttons.size(); ++i) {
            size_t globalIndex = currentPage * 5 + i;
            buttons[i].setFillColor(defaultButtonColor);

            if (globalIndex == currentButtonIndex) {
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

        nextImageButtonText.setPosition(
            nextImageButton.getPosition().x + nextImageButton.getSize().x / 2 - nextImageButtonText.getLocalBounds().width / 2,
            nextImageButton.getPosition().y + nextImageButton.getSize().y / 2 - nextImageButtonText.getLocalBounds().height / 2
        );

        prevImageButtonText.setPosition(
            prevImageButton.getPosition().x + prevImageButton.getSize().x / 2 - prevImageButtonText.getLocalBounds().width / 2,
            prevImageButton.getPosition().y + prevImageButton.getSize().y / 2 - prevImageButtonText.getLocalBounds().height / 2
        );

        if (currentImageIndex == 0) {
            prevImageButton.setFillColor(inactiveButtonColor);
        } else {
            prevImageButton.setFillColor(sf::Color::Red);
        }

        if (currentImageIndex == images[currentButtonIndex].size() - 1) {
            nextImageButton.setFillColor(inactiveButtonColor);
        } else {
            nextImageButton.setFillColor(sf::Color::Green);
        }

        if (!images[currentButtonIndex].empty()) {
            std::string pageNumberString = std::to_string(currentImageIndex + 1) + "/" + std::to_string(images[currentButtonIndex].size());
            pageNumberText.setString(pageNumberString);

            // Position the text at the top right. Adjust the offset as needed to fit your window size and layout
            pageNumberText.setPosition(window.getSize().x - pageNumberText.getLocalBounds().width - 20, 20);
        }



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

        window.draw(images[currentButtonIndex][currentImageIndex]);

        window.draw(nextImageButton);
        window.draw(prevImageButton);
        window.draw(nextImageButtonText);
        window.draw(prevImageButtonText);
        window.draw(pageNumberText);
        //window.draw(title);
        window.display();
    }

    return 0;
}
