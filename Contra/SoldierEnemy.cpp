#include "SoldierEnemy.h"
#include <cmath>
#include <iostream>
#include "AssetManeger.h"
#include <memory>

SoldierEnemy::SoldierEnemy(sf::Vector2f spawnPos, float patrolDistance)
    : m_texture(nullptr),
    m_sprite(nullptr),
    m_position(spawnPos),
    m_speed(150.0f),
    m_health(100),
    m_facingRight(true),
    m_attackCooldown(2.0f),
    m_attackTimer(0.0f),
    m_patrolDirection(1.0f),
    m_currentState(SoldierState::PATROL)
{
    try {
        m_texture = &AssetManeger::getInstance().getTexture("SoldierEnemy_image.png");
    }
    catch (const std::runtime_error& e) {
        std::cerr << "ERROR: Khong the tai texture cho SoldierEnemy: " << e.what() << std::endl;
        m_texture = nullptr;
    }

    // Khởi tạo Sprite
    if (m_texture) {
        // KHỞI TẠO SPRITE VỚI TEXTURE
        m_sprite = std::make_unique<sf::Sprite>(*m_texture);

        m_sprite->setOrigin(sf::Vector2f(
            m_texture->getSize().x / 2.0f,
            m_texture->getSize().y / 2.0f
        ));
    }

    // Cấu hình Khu vuc Tuần tra (Patrol Range)
    m_patrolStart = sf::Vector2f(spawnPos.x - patrolDistance / 2.0f, spawnPos.y);
    m_patrolEnd = sf::Vector2f(spawnPos.x + patrolDistance / 2.0f, spawnPos.y);

    // Đặt vị trí ban đầu
    m_position = m_patrolStart;
    if (m_sprite) {
        m_sprite->setPosition(m_position);
    }
}

void SoldierEnemy::Update(float dt, sf::Vector2f playerPos) {
    if (IsDead() || !m_sprite) return; // Đảm bảo sprite đã được tạo

    sf::Vector2f directionToPlayer = playerPos - m_position;
    float distanceToPlayer = std::sqrt(directionToPlayer.x * directionToPlayer.x + directionToPlayer.y * directionToPlayer.y);

    float currentScale = 0.25f;
    if (m_facingRight) {
        m_sprite->setScale(sf::Vector2f(currentScale, currentScale));
    }
    else {
        m_sprite->setScale(sf::Vector2f(-currentScale, currentScale));
    }

    if (distanceToPlayer <= m_attackRange && m_currentState != SoldierState::ATTACK) {
        m_currentState = SoldierState::ATTACK; // Chuyển sang tấn công nếu đủ gần
    }
    else if (distanceToPlayer > m_attackRange && distanceToPlayer <= m_chaseRange && m_currentState != SoldierState::CHASE) {
        m_currentState = SoldierState::CHASE; // Chuyển sang đuổi theo nếu ở giữa tấn công và tuần tra
    }
    else if (distanceToPlayer > m_chaseRange && m_currentState != SoldierState::PATROL) {
        m_currentState = SoldierState::PATROL; // Chuyển sang tuần tra nếu quá xa
    }

    sf::Vector2f moveVector(0.0f, 0.0f);

    switch (m_currentState) {

    case SoldierState::CHASE: {
        if (distanceToPlayer > 0.1f) {
            moveVector = directionToPlayer / distanceToPlayer;
        }

        if (moveVector.x != 0.0f) {
            m_facingRight = (moveVector.x > 0);
        }
        break;
    }

    case SoldierState::PATROL: {
        moveVector.x = m_patrolDirection;
        if (m_patrolDirection > 0 && m_position.x >= m_patrolEnd.x) {
            m_patrolDirection = -1.0f;
        }
        else if (m_patrolDirection < 0 && m_position.x <= m_patrolStart.x) {
            m_patrolDirection = 1.0f;
        }
        m_facingRight = (m_patrolDirection > 0);
        break;
    }

    case SoldierState::ATTACK: {
        moveVector = sf::Vector2f(0.0f, 0.0f); // Đứng yên khi tấn công

        // LOGIC THỰC HIỆN TẤN CÔNG: Đếm ngược
        m_attackTimer += dt;
        if (m_attackTimer >= m_attackCooldown) {

            std::cout << "Soldier ATTACK! (Can ban dan/gay sat thuong)" << std::endl;

            m_attackTimer = 0.0f; // Đặt lại bộ đếm
        }

        // Cập nhật hướng quay (đảm bảo quay về phía Player khi tấn công)
        if (directionToPlayer.x != 0.0f) {
            m_facingRight = (directionToPlayer.x > 0);
        }

        break;
    }

    default: break;
    }

    // Áp dụng vị trí và hình ảnh
    m_position += moveVector * m_speed * dt;
    m_sprite->setPosition(m_position);
}

void SoldierEnemy::Draw(sf::RenderWindow& window) {
    if (!IsDead() && m_sprite) window.draw(*m_sprite);
}

sf::FloatRect SoldierEnemy::GetBounds() const {
    if (m_sprite) {
        return m_sprite->getGlobalBounds();
    }

    sf::FloatRect bounds;

    // Khởi tạo position (left, top)
    bounds.position = sf::Vector2f(m_position.x - 1.0f, m_position.y - 1.0f);

    // Khởi tạo size (width, height)
    bounds.size = sf::Vector2f(2.0f, 2.0f);

    return bounds;
}

void SoldierEnemy::TakeDamage(int damage) {
    m_health -= damage;
    if (m_health < 0) m_health = 0;
}

bool SoldierEnemy::IsDead() const {
    return m_health <= 0;
}