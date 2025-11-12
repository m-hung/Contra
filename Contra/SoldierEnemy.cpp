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
    m_health(5),
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

void SoldierEnemy::Update(float dt, sf::Vector2f playerPos, float scrollOffset) {
    if (IsDead() || !m_sprite) return;

    // --- Dịch vị trí enemy theo scroll để làm việc với tọa độ màn hình ---
    sf::Vector2f screenPos = m_position - sf::Vector2f(scrollOffset, 0.f);
    sf::Vector2f directionToPlayer = playerPos - screenPos;
    float deltaX = directionToPlayer.x;

    sf::Vector2f moveVector(0.0f, 0.0f);

    // --- Trừ scrollOffset cho phạm vi tuần tra ---
    float patrolStartX = m_patrolStart.x - scrollOffset;
    float patrolEndX = m_patrolEnd.x - scrollOffset;

    float detectionRange = 400.0f;
    float stopDistance = 400.0f;

    // --- Logic chuyển trạng thái ---
    if (m_currentState == SoldierState::PATROL && std::abs(deltaX) < detectionRange) {
        m_currentState = SoldierState::CHASE;
    }
    else if (m_currentState == SoldierState::CHASE && std::abs(deltaX) > stopDistance) {
        m_currentState = SoldierState::PATROL;
    }

    // --- Xử lý trạng thái ---
    switch (m_currentState)
    {
    case SoldierState::PATROL:
        if (std::abs(deltaX) <= detectionRange) {
            moveVector.x = 0.0f;
                m_facingRight = (playerPos.x > screenPos.x);
        }
        else {
            moveVector.x = m_patrolDirection;

            if (m_patrolDirection > 0 && screenPos.x >= patrolEndX)
                m_patrolDirection = -1.0f;
            else if (m_patrolDirection < 0 && screenPos.x <= patrolStartX)
                m_patrolDirection = 1.0f;

            m_facingRight = (m_patrolDirection > 0);
        }
        break;

    case SoldierState::CHASE:
        if (std::abs(deltaX) <= detectionRange) {
            moveVector.x = 0.0f;
                m_facingRight = (playerPos.x > screenPos.x);
        }
        else {
            m_currentState = SoldierState::PATROL;
        }
        break;

    case SoldierState::ATTACK:
        moveVector = sf::Vector2f(0.0f, 0.0f);
            m_facingRight = (playerPos.x > screenPos.x);
        break;

    default:
        break;
    }

    // --- Cập nhật vị trí enemy (theo world) dựa trên moveVector ---
    m_position += moveVector * m_speed * dt;

    // --- Vẽ sprite (vị trí màn hình) ---
    m_sprite->setPosition(m_position - sf::Vector2f(scrollOffset, 0.f));

    // --- Debug log ---
    /*std::cout << "FacingRight: " << m_facingRight
        << " deltaX: " << deltaX
        << " patrolStartX: " << patrolStartX
        << " patrolEndX: " << patrolEndX
        << " screenPos.x: " << screenPos.x
        << std::endl;*/
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

void SoldierEnemy::Draw(sf::RenderWindow& window) {
    if (!m_sprite || IsDead())
        return;

    float currentScale = 0.25f;
    m_sprite->setScale(sf::Vector2f(m_facingRight ? currentScale : -currentScale, currentScale));

    m_sprite->setPosition(m_drawPos);
    window.draw(*m_sprite);
}
    void SoldierEnemy::SetDrawPosition(const sf::Vector2f & pos) {
        m_drawPos = pos;
    }
