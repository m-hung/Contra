#include "Bullet.h"
#include<iostream>
#include<SFML/Graphics.hpp>
#include"Animation.h"
#include "AssetManeger.h"

Bullet::Bullet(sf::Vector2f startPos, float direction)
    : m_sprite(AssetManeger::getInstance().getTexture("bullet_image")),
    m_speed(900.0f),
    m_animation(m_sprite),
    m_damage(1),
    m_lifetime(0.7f)

{
    // Lấy texture đạn (sprite sheet nhiều frame)
    auto& asset = AssetManeger::getInstance();
    const sf::Texture& bulletTex = asset.getTexture("bullet_image.png");

    // Thêm animation "Fly" cho đạn
    m_animation.AddAnimation("Fly", &bulletTex, 10, { 64, 128 }, 0.08f);
    m_animation.Play("Fly");

    // --- ĐẶT TÂM (ORIGIN) VÀO GIỮA ---
    auto bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin({ bounds.position.x + bounds.size.x / 2.f,
                         bounds.position.y + bounds.size.y / 2.f });

    // Thiết lập sprite ban đầu
    m_sprite.setPosition(startPos);
    m_sprite.setScale({ direction, 1.0f });

    // Đặt vận tốc theo hướng
    m_velocity = { m_speed * direction, 0.0f };
}

void Bullet::Update(float dt)
{
    m_lifetime -= dt;
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
bool Bullet::IsDead() const
{
    // Trả về true nếu thời gian sống đã hết
    return m_lifetime <= 0.0f;
}
sf::FloatRect Bullet::GetBounds() const
{
    // === Tinh chỉnh 2 giá trị này ===
    const float hitboxWidth = 15.f;
    const float hitboxHeight = 15.f;
    // ==================================

    // Lấy vị trí TÂM (Tọa độ Màn hình) từ sprite
    sf::Vector2f centerPos = m_sprite.getPosition();

    // Tính toán left/top
    float left = centerPos.x - (hitboxWidth / 2.f);
    float top = centerPos.y - (hitboxHeight / 2.f);

    // Trả về hitbox mới (Tọa độ Màn hình)
    return sf::FloatRect({ left, top }, { hitboxWidth, hitboxHeight });
}

