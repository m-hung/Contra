#include "TileMap.h"
#include <fstream>
#include <sstream>
#include <iostream>

TileMap::TileMap() : m_tileSize(0), m_tilesetWidth(0) {}

TileMap::~TileMap() = default;

bool TileMap::LoadFromFile(const std::string& mapFile, const std::string& tilesetFile, int tileSize) {
    m_tileSize = tileSize;

    if (!m_tileset.loadFromFile(tilesetFile)) {
        std::cerr << "Lỗi: Không thể tải tileset từ " << tilesetFile << "\n";
        return false;
    }

    // ✅ Sprite chỉ được tạo sau khi texture có dữ liệu
    m_tileSprite = std::make_unique<sf::Sprite>(m_tileset);
    m_tilesetWidth = m_tileset.getSize().x / m_tileSize;

    std::ifstream file(mapFile);
    if (!file.is_open()) {
        std::cerr << "Lỗi: Không thể mở file map " << mapFile << "\n";
        return false;
    }

    m_tiles.clear();
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::vector<int> row;
        int tile;
        while (ss >> tile)
            row.push_back(tile);
        m_tiles.push_back(row);
    }

    std::cout << "Đã load map thành công từ " << mapFile << "\n";
    return true;
}

void TileMap::Draw(sf::RenderWindow& window) {
    if (!m_tileSprite) return;

    for (size_t y = 0; y < m_tiles.size(); ++y) {
        for (size_t x = 0; x < m_tiles[y].size(); ++x) {
            int tileIndex = m_tiles[y][x];
            if (tileIndex < 0) continue;

            int tu = tileIndex % m_tilesetWidth;
            int tv = tileIndex / m_tilesetWidth;

            m_tileSprite->setTextureRect(
                sf::IntRect(
                    sf::Vector2i(tu * m_tileSize, tv * m_tileSize),
                    sf::Vector2i(m_tileSize, m_tileSize)
                )
            );

            m_tileSprite->setPosition(sf::Vector2f(
                static_cast<float>(x * m_tileSize),
                static_cast<float>(y * m_tileSize)
            ));

            window.draw(*m_tileSprite);
        }
    }
}

sf::FloatRect TileMap::GetTileBounds(int x, int y) const {
    return sf::FloatRect(
        sf::Vector2f(static_cast<float>(x * m_tileSize),
            static_cast<float>(y * m_tileSize)),
        sf::Vector2f(static_cast<float>(m_tileSize),
            static_cast<float>(m_tileSize))
    );
}
