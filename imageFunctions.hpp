#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

void loadImagesFromTextFilesRecursively(int fileIndex, std::vector<std::vector<std::unique_ptr<sf::Texture>>>& textures, std::vector<std::vector<sf::Sprite>>& images, sf::RenderWindow& window);
void reloadImages(int fileIndex, std::vector<std::vector<std::unique_ptr<sf::Texture>>>& textures, std::vector<std::vector<sf::Sprite>>& images, sf::RenderWindow& window);
bool checkImageExists(const std::string& imageName);
void addImageNameToTopic(int fileIndex, const std::string& imageName);
void deleteImageNameFromTopic(int fileIndex, const std::string& imageName);
