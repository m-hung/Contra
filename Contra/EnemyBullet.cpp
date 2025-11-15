#include "EnemyBullet.h"
#include <cmath>
#include <iostream>

// Hàm helper để chuẩn hóa vector
static sf::Vector2f normalize(sf::Vector2f v) {
    float length = std::sqrt(v.x * v.x + v.y * v.y);
    if (length != 0) {
        return sf::Vector2f(v.x / length, v.y / length);
    }
    return sf::Vector2f(0.f, 0.f);
}

EnemyBullet::EnemyBullet(sf::Vector2f startPos, sf::Vector2f direction, float speed)
    : m_position(startPos),
    m_direction(normalize(direction)), // Chuẩn hóa hướng bay
    m_speed(speed),
    m_isAlive(true)
{
    // --- Tải Texture ---
    try {
        m_texture = &AssetManeger::getInstance().getTexture("EnemyBullet_image.png");
    }
    catch (const std::runtime_error& e) {
        std::cerr << "ERROR: Khong the tai texture cho EnemyBullet: " << e.what() << std::endl;
        m_texture = nullptr;
    }

    // --- Khởi tạo Sprite ---
    if (m_texture) {
        m_sprite = std::make_unique<sf::Sprite>(*m_texture);

        // Đặt tâm xoay (Origin) vào giữa sprite
        m_sprite->setOrigin(sf::Vector2f(
            m_texture->getSize().x / 2.0f,
            m_texture->getSize().y / 2.0f
        ));

        // Đặt Scale (Tùy chọn: tùy vào kích thước gốc của ảnh)
        float bulletScale = 0.5f; 
        m_sprite->setScale(sf::Vector2f(bulletScale, bulletScale));
    }

    // Đặt vị trí ban đầu (World Coordinate)
    m_position = startPos;
    if (m_sprite) {
        m_sprite->setPosition(m_position);
    }
}

// --- Hàm Update ---
void EnemyBullet::Update(float dt) {
    if (!m_isAlive) return;

    // Tính toán quãng đường di chuyển: distance = speed * time
    sf::Vector2f displacement = m_direction * m_speed * dt;

    // Cập nhật vị trí (World Coordinate)
    m_position += displacement;

    // Thêm logic kiểm tra đạn có đi quá xa màn hình không
    if (std::abs(m_position.x - m_position.x) > 1000.0f) { 
    }
}

// --- Hàm Draw ---
void EnemyBullet::Draw(sf::RenderWindow& window, float scrollOffset) {
    if (!m_sprite || !m_isAlive) return;

    // Chuyển vị trí World Coordinate sang Screen Coordinate để vẽ
    sf::Vector2f drawPos = m_position - sf::Vector2f(scrollOffset, 0.f);
    m_sprite->setPosition(drawPos);

    window.draw(*m_sprite);
}

// --- Hàm GetBounds ---
sf::FloatRect EnemyBullet::GetBounds() const {
    // === Tinh chỉnh 2 giá trị này ===
    const float hitboxWidth = 15.f;  
    const float hitboxHeight = 15.f; 
    // ==================================

    // Tính toán left/top dựa trên m_position (tâm của enemy)
    float left = m_position.x - (hitboxWidth / 2.f);
    float top = m_position.y - (hitboxHeight / 2.f);

    // Trả về hitbox mới (Tọa độ Thế giới - vì Enemy dùng World Coords)
    return sf::FloatRect({ left, top }, { hitboxWidth, hitboxHeight });
}
