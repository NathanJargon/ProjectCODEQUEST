#include "ImageFunctions.hpp"
#include <fstream>
#include <iostream>
#include <memory>

// operator overloading

struct RecursiveCallState {
    int fileIndex;
    size_t texturesSize;
    size_t imagesSize;

    RecursiveCallState(int fileIndex, size_t texturesSize, size_t imagesSize)
        : fileIndex(fileIndex), texturesSize(texturesSize), imagesSize(imagesSize) {}
};

std::ostream& operator<<(std::ostream& os, const RecursiveCallState& state) {
    os << "Recursive Call - File Index: " << state.fileIndex
       << ", Textures Size: " << state.texturesSize
       << ", Images Size: " << state.imagesSize;
    return os;
}

void loadImagesFromTextFilesRecursively(int fileIndex, std::vector<std::vector<std::unique_ptr<sf::Texture>>>& textures, std::vector<std::vector<sf::Sprite>>& images, sf::RenderWindow& window) {
    try {
        RecursiveCallState state(fileIndex, textures.size(), images.size());
        std::cout << state << std::endl;

        if (fileIndex >= 12) return;

        std::cout << "Loading images from: texts/text" + std::to_string(fileIndex) + ".txt" << std::endl; 

        textures.resize(12);
        images.resize(12);

        std::ifstream file("texts/text" + std::to_string(fileIndex) + ".txt");
        if (!file) {
            throw std::runtime_error("Failed to open file: texts/text" + std::to_string(fileIndex) + ".txt");
        }

        std::string imageName;
        while (std::getline(file, imageName)) {
            auto texture = std::make_unique<sf::Texture>();
            if (!texture->loadFromFile("images/" + imageName)) {
                std::cerr << "Could not load image: " << imageName << std::endl;
                continue;
            }

            std::cout << "Loading texture at address: " << texture.get() << std::endl;

            textures[fileIndex].push_back(std::move(texture));

            sf::Sprite sprite(*textures[fileIndex].back());
            sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
            sprite.setPosition((window.getSize().x / 2) + 100, window.getSize().y / 2);
            sprite.setScale(1.00f, 1.00f);
            images[fileIndex].push_back(sprite);
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }

    loadImagesFromTextFilesRecursively(fileIndex + 1, textures, images, window);
}


// Function to reload images and textures
void reloadImages(int fileIndex, std::vector<std::vector<std::unique_ptr<sf::Texture>>>& textures,
                  std::vector<std::vector<sf::Sprite>>& images, sf::RenderWindow& window) {
    textures.clear();
    images.clear();
    textures.resize(12);
    images.resize(12);

    for (int i = 0; i < 12; ++i) {
        std::ifstream file("texts/text" + std::to_string(i) + ".txt");
        if (!file) {
            std::cerr << "Failed to open file: texts/text" + std::to_string(i) + ".txt" << std::endl;
            continue;
        }

        std::string imageName;
        while (std::getline(file, imageName)) {
            auto texture = std::make_unique<sf::Texture>();
            if (!texture->loadFromFile("images/" + imageName)) {
                std::cerr << "Could not load image: " << imageName << std::endl;
                continue;
            }

            textures[i].push_back(std::move(texture));
            sf::Sprite sprite(*textures[i].back());
            sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
            sprite.setPosition((window.getSize().x / 2) + 100, window.getSize().y / 2);
            sprite.setScale(1.00f, 1.00f);
            images[i].push_back(sprite);
        }
    }
}

bool checkImageExists(const std::string& imageName) {
    std::ifstream imgFile("images/" + imageName);
    bool exists = imgFile.good();
    imgFile.close();
    return exists;
}

void addImageNameToTopic(int fileIndex, const std::string& imageName) {
    if (!checkImageExists(imageName)) {
        std::cerr << "Image does not exist, not adding to file: " << imageName << std::endl;
        return;
    }

    std::string filePath = "texts/text" + std::to_string(fileIndex) + ".txt";
    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "Failed to open file for reading: " << filePath << std::endl;
        return;
    }

    bool exists = false;
    std::string line;
    while (std::getline(file, line)) {
        if (line == imageName) {
            exists = true;
            break;
        }
    }
    file.close();

    if (exists) {
        std::cout << "Image name '" << imageName << "' already exists in file: " << filePath << std::endl;
        return;
    }

    std::ofstream outFile(filePath, std::ios::app);
    if (!outFile) {
        std::cerr << "Failed to open file for appending: " << filePath << std::endl;
        return;
    }

    outFile << imageName << std::endl;
    if (outFile.fail()) {
        std::cerr << "Failed to write image name to file: " << filePath << std::endl;
    } else {
        std::cout << "Debug: Added image name '" << imageName << "' to file: " << filePath << std::endl;
    }
}

void deleteImageNameFromTopic(int fileIndex, const std::string& imageName) {
    std::string filePath = "texts/text" + std::to_string(fileIndex) + ".txt";
    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "Failed to open file for reading: " << filePath << std::endl;
        return;
    }

    std::vector<std::string> lines;
    std::string line;
    bool found = false;
    while (std::getline(file, line)) {
        if (line != imageName) {
            lines.push_back(line);
        } else {
            found = true;
        }
    }
    file.close();

    if (!found) {
        std::cout << "Image name '" << imageName << "' not found in file: " << filePath << std::endl;
        return;
    }

    std::ofstream outFile(filePath, std::ios::trunc);
    if (!outFile) {
        std::cerr << "Failed to open file for writing: " << filePath << std::endl;
        return;
    }

    for (const auto& l : lines) {
        outFile << l << std::endl;
    }

    std::cout << "Image name '" << imageName << "' has been deleted from file: " << filePath << std::endl;
}