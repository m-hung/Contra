#pragma once

// Thay đổi header cho SFML 3
#include "AssetManeger.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>

// Sử dụng khai báo tiền định (forward declaration) để tối ưu
namespace sf {
    class RenderWindow;
}

class Player {
private:
    sf::Sprite m_sprite;
    sf::Vector2f m_position;
    sf::Vector2f m_velocity;
    float m_speed;
    float m_scaleFactor;
public:
    Player();
    void HandleInput(float dt);
    void Update(float dt);
    void Draw(sf::RenderWindow& window);
};