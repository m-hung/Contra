#include "Player.h"
#include <SFML/Graphics/RenderWindow.hpp> 
#include <SFML/Window/Keyboard.hpp> 
#include <iostream>
#include <cmath> 

Player::Player()
    : m_speed(150.0f),
    m_position(100.0f, 100.0f),
    m_sprite(AssetManeger::getInstance().getTexture("player_image.png")),
    m_scaleFactor(0.2f)

{

    m_sprite.setPosition(m_position);
   m_sprite.setScale({m_scaleFactor, m_scaleFactor});
}

void Player::HandleInput(float dt)
{
    m_velocity.x = 0.0f;
    m_velocity.y = 0.0f;

    // Thay đổi: sf::Keyboard::isPressed và sf::Keyboard::Key::[Tên phím]
    // Phím A (Trái)
    if (sf::Keyboard::isKeyPressed (sf::Keyboard::Key::A))
    {
        m_velocity.x = -m_speed;
    }
    // Phím D (Phải)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
    {
        m_velocity.x = m_speed;
    }
    // Phím W (Lên)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
    {
        m_velocity.y = -m_speed;
    }
    // Phím S (Xuống)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
    {
        m_velocity.y = m_speed;
    }

    // Chuẩn hóa vector (giữ nguyên)
    if (m_velocity.x != 0.0f && m_velocity.y != 0.0f)
    {
        float length = std::sqrt(m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y);
        m_velocity.x = (m_velocity.x / length) * m_speed;
        m_velocity.y = (m_velocity.y / length) * m_speed;
    }
}

void Player::Update(float dt)
{
    m_position += m_velocity * dt;
    m_sprite.setPosition(m_position);
}

void Player::Draw(sf::RenderWindow& window)
{
    window.draw(m_sprite);
}