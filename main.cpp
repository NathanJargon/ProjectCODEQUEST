#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include "ImageFunctions.hpp"

// note a: never input text if image does not exist in /images (done)
// note b: add delete image button
// note c: add toggle to hide or show the buttons below

// classes
// template
// OOP

template<typename CharType, typename StringType = std::basic_string<CharType>>
class TextWrapper {
public:
    static StringType wrapText(const StringType& text, unsigned width, const sf::Font& font, unsigned characterSize, bool bold = false) {
        unsigned currentLineLength = 0;
        StringType wordsBuffer;
        StringType wrappedText;
        std::basic_istringstream<CharType> words(text);
        StringType word;

        while (words >> word) {
            // Since sf::Text does not directly support std::wstring or other character types, we need to convert
            // the StringType to a string if CharType is not char. This is a limitation of SFML.
            sf::Text line(StringTypeToString(wordsBuffer + " " + word), font, characterSize);
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

    // Assuming you want to keep the StringTypeToString function simple and avoid template specialization for now:
    private:
        static std::string StringTypeToString(const StringType& text) {
            // Directly convert assuming StringType can be iterated over and contains char-like elements
            return std::string(text.begin(), text.end());
        }
    };

struct Button {
    sf::RectangleShape shape;
    sf::Text text;
    bool isActive;

    Button(const sf::Vector2f& position, const sf::Vector2f& size, const std::string& label, const sf::Font& font) {
        shape.setPosition(position);
        shape.setSize(size);
        shape.setFillColor(sf::Color::Blue); // Example color

        text.setFont(font);
        text.setString(label);
        text.setCharacterSize(20); // Example size
        text.setFillColor(sf::Color::White);
        text.setPosition(position.x + (size.x - text.getLocalBounds().width) / 2, position.y + (size.y - text.getLocalBounds().height) / 2);

        isActive = true;
    }

    void draw(sf::RenderWindow& window) {
        if (isActive) {
            window.draw(shape);
            window.draw(text);
        }
    }
};

struct InputBox {
    sf::RectangleShape shape;
    sf::Text text;
    bool isActive;
    sf::Clock inputClock; // Clock to track time since last input
    std::function<void(const std::string&)> onSubmit; // Callback for submission

    InputBox(const sf::Vector2f& position, const sf::Vector2f& size, const sf::Font& font, std::function<void(const std::string&)> onSubmitCallback = nullptr) :
        onSubmit(onSubmitCallback) {
        shape.setPosition(position);
        shape.setSize(size);
        shape.setFillColor(sf::Color::White);

        text.setFont(font);
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::Black);
        text.setPosition(position.x + 5, position.y + 5); // Small padding

        isActive = false;
    }

    void draw(sf::RenderWindow& window) {
        if (isActive) {
            window.draw(shape);
            window.draw(text);
        }
    }

    void handleInput(sf::Event event) {
        if (isActive) {
            if (event.type == sf::Event::TextEntered && inputClock.getElapsedTime().asMilliseconds() > 100) {
                if (event.text.unicode == '\b' && !text.getString().isEmpty()) { // Handle backspace
                    std::string currentText = text.getString();
                    currentText.pop_back();
                    text.setString(currentText);
                } else if (event.text.unicode < 128) { // Ignore non-ASCII characters
                    text.setString(text.getString() + static_cast<char>(event.text.unicode));
                }
                inputClock.restart(); // Restart the clock after handling input
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return) {
                if (onSubmit) {
                    onSubmit(text.getString()); // Call the submission callback
                    isActive = false; // Optionally deactivate
                }
            }
        }
    }
};


int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "CodeQuest");

    sf::Image icon;
    if (icon.loadFromFile("images/logo.png")) {
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    }

    sf::Color defaultButtonColor = sf::Color::White;
    sf::Color activeButtonColor = sf::Color::Yellow;
    sf::Color inactiveButtonColor = sf::Color(128, 128, 128);
    TextWrapper<char, std::string> wrapper;
    bool areButtonsVisible = true; 

    sf::Clock clock;
    float typewriterSpeed = 0.1f;
    const std::string texts[] = {"Welcome to Codequest!"};
    const size_t textsSize = sizeof(texts) / sizeof(texts[0]); 
    size_t currentTextIndex = 0; 
    std::string displayedText = "";
    sf::Time lastUpdate = sf::Time::Zero;

    sf::Font font;
    if (!font.loadFromFile("fonts/Montserrat Light.otf")) {
        std::cerr << "Could not load font" << std::endl;
        return -1;
    }


    std::vector<std::vector<std::unique_ptr<sf::Texture>>> textures(12);
    std::vector<std::vector<sf::Sprite>> images(12);

    Button myButton(sf::Vector2f(700, 645), sf::Vector2f(375, 50), "add image (e.g. image.png)", font);
    InputBox myInputBox({715, 575}, {350, 50}, font, [&](const std::string& inputText) {
        addImageNameToTopic(0, inputText);
        reloadImages(0, textures, images, window); 
        std::cout << "Submitted: " << inputText << std::endl;
    });

    Button deleteButton(sf::Vector2f(300, 645), sf::Vector2f(375, 50), "delete image (e.g. image.png)", font);
    InputBox deleteInputBox({315, 575}, {350, 50}, font, [&](const std::string& inputText) {
        deleteImageNameFromTopic(0, inputText);
        reloadImages(0, textures, images, window); 
        std::cout << "Submitted: " << inputText << std::endl;
    });


    sf::Text loadingText("", font, 50);
    loadingText.setPosition(360 - loadingText.getLocalBounds().width / 2, 360 - loadingText.getLocalBounds().height / 2);

    while (window.isOpen() && currentTextIndex < textsSize) { 
        sf::Time elapsedTime = clock.getElapsedTime();
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (elapsedTime - lastUpdate >= sf::seconds(typewriterSpeed) && displayedText.length() < texts[currentTextIndex].length()) {
            displayedText += texts[currentTextIndex][displayedText.length()];
            loadingText.setString(displayedText);
            lastUpdate = elapsedTime;
        }

        window.clear();
        window.draw(loadingText);
        window.display();

        if (displayedText.length() == texts[currentTextIndex].length()) {
            // Pause for a moment before moving to the next text
            sf::sleep(sf::seconds(2)); 
            currentTextIndex++; 
            displayedText = ""; 
        }
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
    sf::Text toggleButtonText("Toggle", font, 25);

    sf::RectangleShape nextButton(sf::Vector2f(180, 50));
    nextButton.setPosition(10, 50 + 5 * 90);
    nextButton.setFillColor(sf::Color::Green);

    sf::RectangleShape prevButton(sf::Vector2f(180, 50));
    prevButton.setPosition(10, 50 + 5.75 * 90);
    prevButton.setFillColor(sf::Color::Red);

    sf::Text nextImageButtonText(">", font, 25);
    sf::Text prevImageButtonText("<", font, 25);

    sf::RectangleShape nextImageButton(sf::Vector2f(50, 60));
    nextImageButton.setPosition(1200, 50 + 6.5 * 90);
    nextImageButton.setFillColor(sf::Color::Green);

    sf::RectangleShape prevImageButton(sf::Vector2f(50, 60));
    prevImageButton.setPosition(1100, 50 + 6.5 * 90);
    prevImageButton.setFillColor(sf::Color::Red);

    sf::RectangleShape toggleVisibilityButton(sf::Vector2f(180, 50));
    toggleVisibilityButton.setPosition(10, 50 + 6.5 * 90);
    toggleVisibilityButton.setFillColor(sf::Color::Blue);

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
        std::string wrappedText = wrapper.wrapText(name, 90, font, 8, false);
        sf::Text label(wrappedText, font, 15);
        label.setFillColor(sf::Color::Black);
        buttonLabels.push_back(label);
    }

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

                if (myButton.shape.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
                    myInputBox.isActive = !myInputBox.isActive; 
                }

                if (deleteButton.shape.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
                    deleteInputBox.isActive = !deleteInputBox.isActive; 
                }

                if (event.type == sf::Event::MouseButtonPressed) {
                    if (toggleVisibilityButton.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
                        areButtonsVisible = !areButtonsVisible; 
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

            myInputBox.handleInput(event);
            deleteInputBox.handleInput(event);
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

        toggleButtonText.setPosition(
            toggleVisibilityButton.getPosition().x + toggleVisibilityButton.getSize().x / 2 - toggleButtonText.getLocalBounds().width / 2,
            toggleVisibilityButton.getPosition().y + toggleVisibilityButton.getSize().y / 2 - toggleButtonText.getLocalBounds().height / 2
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

        window.draw(pageNumberText);
        window.draw(toggleVisibilityButton);
        window.draw(toggleButtonText);


        if (areButtonsVisible) {
            window.draw(nextImageButton);
            window.draw(prevImageButton);
            window.draw(nextImageButtonText);
            window.draw(prevImageButtonText);
            myInputBox.draw(window);
            deleteButton.draw(window);
            myButton.draw(window); 
            deleteInputBox.draw(window);
        }


        //window.draw(title);
        window.display();
    }

    return 0;
}
