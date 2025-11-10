#include "Bullet.h"
#include<iostream>
#include<SFML/Graphics.hpp>
#include"Animation.h"
Bullet::Bullet(sf::Vector2f startPos, float direction)
    : m_sprite(AssetManeger::getInstance().getTexture("bullet_image")),
    m_speed(800.0f),
    m_animation(m_sprite)
{
    // Lấy texture đạn (sprite sheet nhiều frame)
    auto& asset = AssetManeger::getInstance();
    const sf::Texture& bulletTex = asset.getTexture("bullet_image.png");
    // Thêm animation "Fly" cho đạn
    m_animation.AddAnimation("Fly", &bulletTex, 10, { 64, 128 }, 0.08f);
    m_animation.Play("Fly");

    // Thiết lập sprite ban đầu
    m_sprite.setPosition(startPos);
    m_sprite.setScale({ direction, 1.0f });

    // Đặt vận tốc theo hướng
    m_velocity = { m_speed * direction, 0.0f };
}

void Bullet::Update(float dt)
{
    // Cập nhật vị trí
    m_sprite.move(m_velocity * dt);

    // Cập nhật animation bay
    m_animation.Update(dt);
    m_animation.GetSprite().setPosition(m_sprite.getPosition());
}

void Bullet::Draw(sf::RenderWindow& window)
{
    m_animation.Draw(window);
}

bool Bullet::IsOutOfScreen(float windowWidth) const
{
    float x = m_sprite.getPosition().x;
    return (x < -100.f || x > windowWidth + 100.f);
}
