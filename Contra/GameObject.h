#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class GameObject {
public:
    GameObject(const std::string& texturePath,
        sf::Vector2f position,
        sf::Vector2f scale = { 1.f, 1.f });

    void Draw(sf::RenderWindow& window, float scrollOffset = 0.0f);
    sf::FloatRect GetBounds() const;
    sf::Vector2f GetPosition() const { return m_sprite.getPosition(); }

    void SetScale(sf::Vector2f newScale);
    void SetPosition(sf::Vector2f newPos);

private:
    sf::Sprite m_sprite;
    sf::Texture m_texture;
};
