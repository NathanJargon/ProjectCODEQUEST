#include <SFML/Graphics.hpp>
#include <SFML/Config.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

// class that contains reusable components
class TextWrapper {
public:
    static std::string wrapText(const std::string& text, unsigned width, const sf::Font& font, unsigned characterSize, bool bold = false);
};

// Button Class
class Button {
    sf::RectangleShape shape;
    sf::Text label;
public:
    Button(const sf::Vector2f& position, const sf::Vector2f& size, const std::string& text, const sf::Font& font, unsigned characterSize);
    void draw(sf::RenderWindow& window);
    bool isClicked(const sf::Vector2f& mousePos);
};

// ImageLoader Class
class ImageLoader {
    std::vector<sf::Texture> textures;
    std::vector<sf::Sprite> images;
public:
    void loadImage(const std::string& path);
    sf::Sprite& getImage(size_t index);
};

// Sidebar Class
class Sidebar {
    sf::RectangleShape shape;
    sf::Text title;
public:
    Sidebar(const sf::Vector2f& size, const std::string& titleText, const sf::Font& font, unsigned titleSize);
    void draw(sf::RenderWindow& window);
};

// PageManager Class
class PageManager {
    size_t currentPage;
    size_t totalPages;
public:
    PageManager(size_t totalPages);
    void nextPage();
    void prevPage();
    size_t getCurrentPage() const;
};

std::string wrapText(std::string text, unsigned width, const sf::Font &font, unsigned characterSize, bool bold = false)
{
    // basic naming patterns
    unsigned currentLineLength = 0;
    std::string wordsBuffer;
    std::string wrappedText;
    std::istringstream words(text);
    std::string word;

    // loops
    while (words >> word)
    {
        sf::Text line(wordsBuffer + " " + word, font, characterSize);
        currentLineLength = line.getLocalBounds().width;

        if (currentLineLength > width)
        {
            wrappedText += wordsBuffer + "\n";
            wordsBuffer = word;
        }
        else
        {
            wordsBuffer += " " + word;
        }
    }

    return wrappedText + wordsBuffer;
}

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

    for (int i = 0; i < 5; i++) {
        sf::RectangleShape button(sf::Vector2f(180, 70));
        button.setPosition(10, 50 + i * 80 + 30);
        button.setFillColor(sf::Color::White); 
        buttons.push_back(button);
    }

    /* this uses a plain array. This is not recommended because it is not dynamic and can't be resized.
    std::array<std::string, 12> buttonNames = {
        "Elementary Programming", 
        "Functions", 
        "Loops",
        "Arrays",
        "Object and Classes",
        "Object Oriented Programming",
        "Pointers and Dynamic Memory Management",
        "Templates, Vectors, and Stacks",
        "File Input and Output",
        "Operator Overloading",
        "Exception Handling",
        "Recursion",
    };
    */

    // using a vector for dynamic size and convenience.
    std::vector<std::string> buttonNames = {
        "Elementary Programming", 
        "Functions", 
        "Loops",
        "Arrays",
        "Object and Classes",
        "Object Oriented Programming",
        "Pointers and Dynamic Memory Management",
        "Templates, Vectors, and Stacks",
        "File Input and Output",
        "Operator Overloading",
        "Exception Handling",
        "Recursion",
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
    for (int i = 0; i < 12; i++) {
        std::string wrappedText = wrapText(buttonNames[i], 90, font, 8);
        sf::Text label(wrappedText, font, 15);

        // accessing vector using index
        float x = buttons[i].getPosition().x + buttons[i].getSize().x / 2 - label.getLocalBounds().width / 2;
        float y = buttons[i].getPosition().y + buttons[i].getSize().y / 2 - label.getLocalBounds().height / 2;
        label.setPosition(x, y);
        label.setFillColor(sf::Color::Black); 
        buttonLabels.push_back(label);
    }

    std::vector<sf::Texture> textures;
    std::vector<sf::Sprite> images;
    textures.reserve(12); // reserving space for textures (fix for white images)

    // reads from texts/ to get image from images/ 
    // Input and Output (my approach for getting images instead of doing it directly just to show input and output)

    for (int i = 0; i < 12; i++) {
        std::ifstream file("texts/text" + std::to_string(i) + ".txt");
        if (!file) { // Check if the file stream has failed to open
            std::cerr << "Failed to open file: texts/text" << i << ".txt" << std::endl;
            continue;
        }
    
        std::string imageName;
        std::getline(file, imageName);
    
        if (file.bad()) { // Check for a reading error
            std::cerr << "Reading error on file: texts/text" << i << ".txt" << std::endl;
            continue;
        }

        // eof not checked because reaching end of file is not an error but normal behavior
    
        if (file.fail() && !file.eof()) { // Check for a general failure not caused by eof
            std::cerr << "General failure reading file: texts/text" << i << ".txt" << std::endl;
            continue;
        }
    
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

    sf::Sprite currentImage = images[0];

    size_t currentPage = 0;
    size_t totalPages = (buttonNames.size() + 4) / 5;

    size_t currentImageIndex = 0;

    while (window.isOpen())
    {
        sf::Event event;
        
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close(); 

            if (event.type == sf::Event::MouseButtonPressed) {
                for (size_t i = 0; i < buttons.size(); i++) {
                    if (buttons[i].getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
                        size_t index = currentPage * 5 + i;
                        if (index < images.size()) {
                            currentImageIndex = index;
                        }
                    }
                }
                    
                // Update the current page when the navigation buttons are clicked
                // and prevent going to a page that doesn't exist
                if (nextButton.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
                    if (currentPage < totalPages - 1) {
                        currentPage++;
                    }
                }
                if (prevButton.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
                    if (currentPage > 0) {
                        currentPage--;
                    }
                }
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
    
        // Only draw the buttons and labels for the current page
        // and prevent drawing buttons or labels that don't exist
        for (size_t i = 0; i < buttons.size() && i + currentPage * 5 < buttonNames.size(); i++) {
            window.draw(buttons[i]);

            // Update the position of the labels
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