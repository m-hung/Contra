#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class TileMap {
public:
    TileMap();
    ~TileMap();

    bool LoadFromFile(const std::string& mapFile, const std::string& tilesetFile, int tileSize);
    void Draw(sf::RenderWindow& window);
    sf::FloatRect GetTileBounds(int x, int y) const;

private:
    std::vector<std::vector<int>> m_tiles;
    sf::Texture m_tileset;
    std::unique_ptr<sf::Sprite> m_tileSprite; // dùng con trỏ thông minh
    int m_tileSize;
    int m_tilesetWidth;
};
