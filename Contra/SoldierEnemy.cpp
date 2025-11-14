#include "SoldierEnemy.h"
#include <cmath>
#include <iostream>
#include "AssetManeger.h"
#include <memory>

static sf::Texture dummyTexture;

SoldierEnemy::SoldierEnemy(sf::Vector2f spawnPos, float patrolDistance)
    : m_sprite(dummyTexture),
    m_animation(m_sprite),
    m_position(spawnPos),
    m_speed(125.0f),
    m_health(5),
    m_facingRight(true),
    m_attackCooldown(2.0f),
    m_attackTimer(0.0f),
    m_patrolDirection(1.0f),
    m_currentState(SoldierState::PATROL)
{
    auto& asset = AssetManeger::getInstance();
    sf::Vector2i frameSize = { 81, 71 };

    // ATTACK 
    m_animation.AddAnimation("ATTACK", &asset.getTexture("SoldierEnemy_Attack.png"),
        8, frameSize, 0.08f);

    // FLYING
    m_animation.AddAnimation("FLYING", &asset.getTexture("SoldierEnemy_Flying.png"),
        4, frameSize, 0.1f);

    // IDLE 
    m_animation.AddAnimation("IDLE", &asset.getTexture("SoldierEnemy_Idle.png"),
        4, frameSize, 0.15f);

    // Thiết lập animation ban đầu
    m_animation.Play("IDLE");

    // Đặt Origin của sprite ở giữa (quan trọng cho việc lật hình)
    auto bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin({ bounds.position.x + bounds.size.x / 2.f,
                         bounds.position.y + bounds.size.y / 2.f });

    // Cấu hình Khu vuc Tuần tra (Patrol Range)
    m_patrolStart = sf::Vector2f(spawnPos.x - patrolDistance / 2.0f, spawnPos.y);
    m_patrolEnd = sf::Vector2f(spawnPos.x + patrolDistance / 2.0f, spawnPos.y);

    // Đặt vị trí ban đầu
    m_position = m_patrolStart;
    m_sprite.setPosition(m_position);
    m_sprite.setScale(sf::Vector2f(1.0f, 1.0f));
}

void SoldierEnemy::Update(float dt, sf::Vector2f playerPos, float scrollOffset) {
    if (IsDead()) return;

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

    // LOGIC CHỌN ANIMATION
    switch (m_currentState)
    {
    case SoldierState::PATROL:
    case SoldierState::CHASE:
        // Di chuyển (hoặc đứng yên nhưng sẵn sàng di chuyển)
        if (std::abs(moveVector.x) > 0.01f) {
            m_animation.Play("FLYING");
        }
        else {
            m_animation.Play("IDLE"); // Đứng yên nhưng quay mặt về Player
        }
        break;

    case SoldierState::ATTACK:
        // Tấn công 
        m_animation.Play("ATTACK");
        break;

    default:
        m_animation.Play("IDLE");
        break;
    }

    // --- Cập nhật vị trí enemy (theo world) dựa trên moveVector ---
    m_position += moveVector * m_speed * dt;

    // Cập nhật Animation
    m_animation.Update(dt);

    // Cập nhật vị trí World Coordinate cho sprite 
    m_sprite.setPosition(m_position);

    float baseScaleX = 1.0f;
    float newScaleX = m_facingRight ? -baseScaleX : baseScaleX;
    m_sprite.setScale(sf::Vector2f(newScaleX, baseScaleX));

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
        bulletInfo.startPosition = m_position + sf::Vector2f(offsetX, 10.0f); // Tọa độ world
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
    // Kích thước (chiều rộng, chiều cao) của hộp đỏ
    const float hitboxWidth = 65.f;
    const float hitboxHeight = 65.f;

    // Vị trí của CHÂN (offset từ tâm Player xuống)
    // Tăng số này để đẩy hộp đỏ XUỐNG DƯỚI
    // Giảm số này để kéo hộp đỏ LÊN TRÊN
    const float feetOffset = 35.f;

    // ================================================================


    // Tính vị trí bên trái (left)
    // Code này giữ cho hộp đỏ nằm ở giữa Player theo chiều ngang
    float left = m_position.x - (hitboxWidth / 2.f);

    // Tính vị trí bên trên (top)
    // Vị trí Y của chân = (Tâm Player + offset)
    // Vị trí Top của hộp = (Vị trí Y của chân) - (Chiều cao của hộp)
    float top = (m_position.y + feetOffset) - hitboxHeight;

    // Trả về hitbox mới (vẫn là Tọa độ Màn hình)
    return sf::FloatRect({ left, top }, { hitboxWidth, hitboxHeight });
}

void SoldierEnemy::TakeDamage(int damage) {
    m_health -= damage;
    if (m_health < 0) m_health = 0;
}

bool SoldierEnemy::IsDead() const {
    return m_health <= 0;
}

void SoldierEnemy::Draw(sf::RenderWindow& window) {
    if (IsDead())
        return;
    m_animation.Draw(window);
}

void SoldierEnemy::SetDrawPosition(const sf::Vector2f& pos) {
    m_sprite.setPosition(pos);
}