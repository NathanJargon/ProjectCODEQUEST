#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <functional>

// classes
// template
// OOP

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

void loadImagesFromTextFilesRecursively(int fileIndex, std::vector<std::vector<std::unique_ptr<sf::Texture>>>& textures, std::vector<std::vector<sf::Sprite>>& images, sf::RenderWindow& window);
void reloadImages(int fileIndex, std::vector<std::vector<std::unique_ptr<sf::Texture>>>& textures, std::vector<std::vector<sf::Sprite>>& images, sf::RenderWindow& window);
bool checkImageExists(const std::string& imageName);
void addImageNameToTopic(int fileIndex, const std::string& imageName);
void deleteImageNameFromTopic(int fileIndex, const std::string& imageName);
