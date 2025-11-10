#pragma once
#include <SFML/Graphics.hpp>
#include "AssetManeger.h"
#include "Animation.h"
class Bullet {
private:
    sf::Sprite m_sprite;
    sf::Vector2f m_velocity;
    float m_speed;
    Animation  m_animation;  // Đối tượng animation
public:
    Bullet(sf::Vector2f startPos, float direction = 1.0f);

    void Update(float dt);
    void Draw(sf::RenderWindow& window);
    bool IsOutOfScreen(float windowWidth) const;
};
