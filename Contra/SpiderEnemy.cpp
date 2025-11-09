#include "SpiderEnemy.h"
#include <iostream>
#include <cmath>    

SpiderEnemy::SpiderEnemy(sf::Vector2f position)
    : m_currentHP(m_maxHP),
    m_sprite(AssetManeger::getInstance().getTexture("SpiderEnemy_SpriteSheet.png"))
{
    // Khởi tạo Khung hình (Frame) đầu tiên
    m_currentFrame = sf::IntRect(
        { WALK_START_COLUMN * FRAME_WIDTH, WALK_ROW * FRAME_HEIGHT }, // position {x, y}
        { FRAME_WIDTH, FRAME_HEIGHT }                                // size {w, h}
    );
    m_sprite.setTextureRect(m_currentFrame);
    m_sprite.setOrigin(sf::Vector2f(FRAME_WIDTH / 2.0f, FRAME_HEIGHT / 2.0f));
    m_sprite.setPosition(position);
    m_sprite.setScale(sf::Vector2f(1.0f, 1.0f));
}

void SpiderEnemy::Update(float dt, sf::Vector2f playerPos) {
    if (IsDead()) return;

    // Tính toán Hướng di chuyển
    sf::Vector2f direction = CalculateMovementDirection(playerPos);
    m_sprite.move(direction * m_speed * dt);

    // Cập nhật Hoạt ảnh (Animation)
    m_animationTimer += dt;

    if (m_animationTimer >= FRAME_DURATION) {
        m_currentFrame.position.x += FRAME_WIDTH;

        // Quay vòng Hoạt ảnh Đi bộ
        int endColumn = WALK_START_COLUMN + WALK_FRAME_COUNT;

        if (m_currentFrame.position.x >= endColumn * FRAME_WIDTH) {
            m_currentFrame.position.x = WALK_START_COLUMN * FRAME_WIDTH;
        }

        m_animationTimer -= FRAME_DURATION;
    }

    // Áp dụng khung hình mới
    m_sprite.setTextureRect(m_currentFrame);

    // Lat anh
    if (direction.x < 0) {
        m_sprite.setScale(sf::Vector2f(1.0f, 1.0f)); // Hướng trái
    }
    else if (direction.x > 0) {
        m_sprite.setScale(sf::Vector2f(-1.0f, 1.0f)); // Hướng phải (Lật)
    }
}

sf::Vector2f SpiderEnemy::CalculateMovementDirection(sf::Vector2f targetPosition) const {
    sf::Vector2f currentPos = m_sprite.getPosition();
    sf::Vector2f directionVector = targetPosition - currentPos;

    float length = std::sqrt(directionVector.x * directionVector.x + directionVector.y * directionVector.y);

    if (length > 0) {
        return sf::Vector2f(directionVector.x / length, directionVector.y / length);
    }
    return sf::Vector2f(0.0f, 0.0f);
}

void SpiderEnemy::Draw(sf::RenderWindow& window) {
    if (!IsDead()) {
        window.draw(m_sprite);
    }
}

void SpiderEnemy::TakeDamage(int damage) {
    if (!IsDead()) {
        m_currentHP -= damage;
        if (m_currentHP < 0) {
            m_currentHP = 0;
        }
    }
}

sf::FloatRect SpiderEnemy::GetBounds() const {
    return m_sprite.getGlobalBounds();
}