#include "SoldierEnemy.h"
#include <cmath>
#include <iostream>
#include "AssetManeger.h"
#include <memory>

SoldierEnemy::SoldierEnemy(sf::Vector2f spawnPos, float patrolDistance)
    : m_texture(nullptr),
    m_sprite(nullptr),
    m_position(spawnPos),
    m_speed(125.0f),
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
    float chaseRange = m_chaseRange; 
    float attackRange = m_attackRange; 

    // --- Logic chuyển trạng thái ---
    if (m_currentState == SoldierState::PATROL) {
        if (CheckAttackRange(deltaX)) {
            TransitionState(SoldierState::ATTACK);
        }
        else if (std::abs(deltaX) < detectionRange) {
            TransitionState(SoldierState::CHASE);
        }
    }
    else if (m_currentState == SoldierState::CHASE) {
        if (CheckAttackRange(deltaX)) {
            TransitionState(SoldierState::ATTACK);
        }
        else if (std::abs(deltaX) > chaseRange) { // Thoát khỏi tầm rượt đuổi
            TransitionState(SoldierState::PATROL);
        }
    }
    else if (m_currentState == SoldierState::ATTACK) {
        if (!CheckAttackRange(deltaX)) {
            TransitionState(SoldierState::CHASE);
        }
    }

    // --- Xử lý trạng thái ---
    switch (m_currentState)
    {
    case SoldierState::PATROL:
        if (std::abs(deltaX) < detectionRange && !CheckAttackRange(deltaX)) {
            moveVector.x = 0.0f;
            m_facingRight = (playerPos.x > screenPos.x);
        }
        else {
            moveVector.x = m_patrolDirection;
            // Đảo hướng khi đạt giới hạn tuần tra 
            if (m_patrolDirection > 0 && m_position.x >= m_patrolEnd.x)
                m_patrolDirection = -1.0f;
            else if (m_patrolDirection < 0 && m_position.x <= m_patrolStart.x)
                m_patrolDirection = 1.0f;

            m_facingRight = (m_patrolDirection > 0);
        }
        break;

    case SoldierState::CHASE:
        // Di chuyển về phía Player
        if (deltaX > 0) {
            moveVector.x = 1.0f; // Di chuyển sang phải
        }
        else {
            moveVector.x = -1.0f; // Di chuyển sang trái
        }
        m_facingRight = (deltaX > 0);
        break;

    case SoldierState::ATTACK:
        // Đứng yên và quay mặt về phía Player
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

    // Cập nhật attackTimer
    if (m_attackTimer > 0) {
        m_attackTimer -= dt;
    }

  
    // --- Debug log ---
    /*std::cout << "FacingRight: " << m_facingRight
        << " deltaX: " << deltaX
        << " patrolStartX: " << patrolStartX
        << " patrolEndX: " << patrolEndX
        << " screenPos.x: " << screenPos.x
        << std::endl;*/
}

// --- Thêm hàm TryToAttack mới để lớp Game gọi và tạo đạn ---
std::optional<SoldierBulletInfo> SoldierEnemy::TryToAttack(float dt, float deltaX) {
    if (IsDead() || m_currentState != SoldierState::ATTACK) {
        return std::nullopt; // Không tấn công
    }

    // Logic bắn đạn
    if (m_attackTimer <= 0.0f) {
        // Tái tạo lại cooldown
        m_attackTimer = m_attackCooldown;

        // Tính toán hướng bay của đạn
        sf::Vector2f bulletDirection(m_facingRight ? 1.0f : -1.0f, 0.0f);

        SoldierBulletInfo bulletInfo;
        // Đặt vị trí xuất phát của đạn hơi lệch so với trung tâm Soldier
        float offsetX = m_facingRight ? 20.0f : -20.0f;
        bulletInfo.startPosition = m_position + sf::Vector2f(offsetX, -10.0f); // Tọa độ world
        bulletInfo.direction = bulletDirection;
        bulletInfo.speed = 400.0f; // Tốc độ đạn

        return bulletInfo; // Trả về thông tin viên đạn để lớp Game tạo
    }

    // Đã trong trạng thái ATTACK nhưng đang trong cooldown
    return std::nullopt;
}

// Hàm helper để chuyển trạng thái
void SoldierEnemy::TransitionState(SoldierState newState) {
    if (m_currentState != newState) {
        // Có thể thêm logic reset/khởi tạo khi chuyển trạng thái tại đây
        m_currentState = newState;
    }
}

// Hàm helper để kiểm tra Player có trong tầm bắn không
bool SoldierEnemy::CheckAttackRange(float deltaX) const {
    // Tầm bắn của lính
    float attackRange = 400.0f;
    return std::abs(deltaX) <= attackRange;
}



sf::FloatRect SoldierEnemy::GetBounds() const {
    if (m_texture) { 
        // --- Tính toán bounds GỐC (chính xác) ---
        sf::Vector2u texSize = m_texture->getSize();

        float scale = 0.25f;

        float width = static_cast<float>(texSize.x) * scale;
        float height = static_cast<float>(texSize.y) * scale;

        // Tính vị trí top-left (World Space) dựa trên tâm m_position
        float left = m_position.x - (width / 2.0f);
        float top = m_position.y - (height / 2.0f);

        sf::FloatRect bounds({ left, top }, { width, height });

        // --- THU NHỎ HITBOX ---
        float shrinkFactor = 0.4f;

        float paddingX = (bounds.size.x * (1.0f - shrinkFactor)) / 2.0f;
        float paddingY = (bounds.size.y * (1.0f - shrinkFactor)) / 2.0f;

        bounds.position.x += paddingX;
        bounds.position.y += paddingY;
        bounds.size.x *= shrinkFactor;
        bounds.size.y *= shrinkFactor;

        return bounds;
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
