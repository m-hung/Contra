#pragma once
#include <SFML/Graphics.hpp>

class Player
{
private:
    sf::Sprite m_sprite;
    sf::Vector2f m_position;
    sf::Vector2f m_velocity;
    float m_speed;
    float m_scaleFactor;

    bool m_isJumping;
    float m_jumpForce;
    float m_gravity;
    float m_groundY;

public:
    Player();

    void HandleInput(float dt);
    void Update(float dt);
    void Draw(sf::RenderWindow& window);
};
