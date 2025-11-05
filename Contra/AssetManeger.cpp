#include "AssetManeger.h"
#include <iostream>

// Lấy instance duy nhất của AssetManager
AssetManeger& AssetManeger::getInstance() {
    static AssetManeger instance;
    return instance;
}

// Tải texture nếu chưa có, sau đó trả về reference
const sf::Texture& AssetManeger::getTexture(const std::string& filename) {
    auto it = m_texture.find(filename);
    if (it != m_texture.end()) {
        return it->second;
    }

    sf::Texture texture;
    if (!texture.loadFromFile(filename)) {
        std::cerr << "?? [AssetManeger] Failed to load texture: " << filename << std::endl;
    }
    else {
        texture.setSmooth(false);
        std::cout << "? [AssetManeger] Loaded texture: " << filename << std::endl;
    }

    m_texture[filename] = std::move(texture);
    return m_texture[filename];
}

// Tải font nếu chưa có, sau đó trả về reference
const sf::Font& AssetManeger::getFont(const std::string& filename) {
    auto it = m_fonts.find(filename);
    if (it != m_fonts.end()) {
        return it->second;
    }

    sf::Font font;
    if (!font.openFromFile(filename)) {
        std::cerr << "?? [AssetManager] Failed to load font: " << filename << std::endl;
    }
    else {
        std::cout << "? [AssetManager] Loaded font: " << filename << std::endl;
    }

    m_fonts[filename] = std::move(font);
    return m_fonts[filename];
}
