#include "Bullet.h"
#include <iostream>
#include "Player.h"
#include "AssetManeger.h"   
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Keyboard.hpp>
Bullet::Bullet(sf::Vector2f startPos, float direction)
    : m_speed(650.0f),//toc do dan bay
    m_sprite(AssetManeger::getInstance().getTexture("bullet_image.png")) //  khởi tạo sprite ngay tại đây
{
    m_sprite.setPosition(startPos);
    m_sprite.setScale({ 0.06f, 0.06f });//chinh kich thuoc dan
    m_velocity = { m_speed * direction, 0.0f };
}

void Bullet::Update(float dt) {
    m_sprite.move(m_velocity * dt);
}

void Bullet::Draw(sf::RenderWindow& window) {
    window.draw(m_sprite);
}

bool Bullet::IsOutOfScreen(float windowWidth) const {
    return m_sprite.getPosition().x > windowWidth || m_sprite.getPosition().x < 0;
}
