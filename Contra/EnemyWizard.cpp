#include "EnemyWizard.h"
#include <cmath>
#include <iostream>
#include "AssetManeger.h"
#include <memory>
#include <SFML/Audio.hpp>
static sf::Texture dummyTexture;

EnemyWizard::EnemyWizard(sf::Vector2f spawnPos, float patrolDistance)
    : m_sprite(dummyTexture),
    m_animation(m_sprite),
    m_position(spawnPos),
    m_speed(200.0f),
    m_health(10),
    m_facingRight(true),
    m_attackCooldown(2.0f),
    m_attackTimer(0.5f),
    m_patrolDirection(1.0f),
    m_currentState(WizardState::PATROL),
    m_deathAnimTimer(0.0f),
    m_attackBuffer(),
    m_attackSound(m_attackBuffer)
{
    auto& asset = AssetManeger::getInstance();
    sf::Vector2i frameSize = { 250, 250 };

  /*  if (!m_attackBuffer.loadFromFile("Ewizard_attack (2).mp3"))
    {
        std::cerr << "Khong the tai am thanh Ewizard_attack.mp3\n";
    }*/

    // ATTACK 
    m_animation.AddAnimation("ATTACK", &asset.getTexture("Attack2_monster.png"),
        8, frameSize, 0.15f);

    // FLYING
    m_animation.AddAnimation("FLYING", &asset.getTexture("Run_monster.png"),
        8, frameSize, 0.1f);

    // IDLE 
    m_animation.AddAnimation("IDLE", &asset.getTexture("Idle_monster.png"),
        8, frameSize, 0.15f);
    //DEATH
    m_animation.AddAnimation("DEATH", &asset.getTexture("Death_monster.png"),
        7, frameSize, 0.15f);

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

void EnemyWizard::Update(float dt, sf::Vector2f playerPos, float scrollOffset) {
    if (m_currentState == WizardState::DEATH)
    {
        m_animation.Update(dt);
        if (m_deathAnimTimer > 0.0f)
        {
            m_deathAnimTimer -= dt;
        }
        return; // Dừng, không chạy AI bên dưới
    }
    // --- KẾT THÚC ---
    if (IsDead()) return;

    // --- Dịch vị trí enemy theo scroll để làm việc với tọa độ màn hình ---
    sf::Vector2f screenPos = m_position - sf::Vector2f(scrollOffset, 0.f);
    sf::Vector2f directionToPlayer = playerPos - screenPos;
    float deltaX = directionToPlayer.x;

    sf::Vector2f moveVector(0.0f, 0.0f);

    // --- Trừ scrollOffset cho phạm vi tuần tra ---
    float patrolStartX = m_patrolStart.x - scrollOffset;
    float patrolEndX = m_patrolEnd.x - scrollOffset;

    float detectionRange = m_chaseRange;
    float chaseRange = m_chaseRange;
    float attackRange = m_attackRange;

    // --- Logic chuyển trạng thái ---
    if (m_currentState == WizardState::PATROL) {
        if (CheckAttackRange(deltaX)) {
            TransitionState(WizardState::ATTACK);
        }
        else if (std::abs(deltaX) < detectionRange) {
            TransitionState(WizardState::CHASE);
        }
    }
    else if (m_currentState == WizardState::CHASE) {
        if (CheckAttackRange(deltaX)) {
            TransitionState(WizardState::ATTACK);
        }
        else if (std::abs(deltaX) > m_chaseRange) { // Thoát khỏi tầm rượt đuổi
            TransitionState(WizardState::PATROL);
        }
    }
    else if (m_currentState == WizardState::ATTACK) {
        if (!CheckAttackRange(deltaX)) {
            TransitionState(WizardState::CHASE);
        }
    }

    // --- Xử lý trạng thái ---
    switch (m_currentState)
    {
    case WizardState::PATROL:
        if (std::abs(deltaX) < detectionRange && !CheckAttackRange(deltaX)) {
            moveVector.x = 0.0f;
            m_facingRight = (playerPos.x > screenPos.x);
        }
        else {
            moveVector.x = 0.0f;
            // Đảo hướng khi đạt giới hạn tuần tra 
            if (m_patrolDirection > 0 && m_position.x >= m_patrolEnd.x)
                m_patrolDirection = 0.0f;
            else if (m_patrolDirection < 0 && m_position.x <= m_patrolStart.x)
                m_patrolDirection = 0.0f;

            m_facingRight = (m_patrolDirection > 0);
        }
        break;

    case WizardState::CHASE:
        // Di chuyển về phía Player
        if (deltaX > 0) {
            moveVector.x = 1.0f; // Di chuyển sang phải
        }
        else {
            moveVector.x = -1.0f; // Di chuyển sang trái
        }
        m_facingRight = (deltaX > 0);
        break;

    case WizardState::ATTACK:
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
    case WizardState::PATROL:
    case WizardState::CHASE:
        // Di chuyển (hoặc đứng yên nhưng sẵn sàng di chuyển)
        if (std::abs(moveVector.x) > 0.01f) {
            m_animation.Play("FLYING");
        }
        else {
            m_animation.Play("IDLE"); // Đứng yên nhưng quay mặt về Player
        }
        break;

    case WizardState::ATTACK:
        // Tấn công 
       // m_animation.Play("ATTACK");
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

    float baseScaleX = 2.0f;
    //m_facingRight = true; //(Player ở bên phải) -> scaleX = 1.0f (Nhìn sang phải)
    //m_facingRight = false;//(Player ở bên trái) -> scaleX = -1.0f (Nhìn sang trái)
    float newScaleX = m_facingRight ? baseScaleX : -baseScaleX;

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
std::optional<WizardBulletInfo> EnemyWizard::TryToAttack(float dt, float deltaX) {
        return std::nullopt; // Không tấn công;
}

// Hàm helper để chuyển trạng thái
void EnemyWizard::TransitionState(WizardState newState) {
    if (m_currentState != newState) {
        m_currentState = newState;

        // --- SỬA LỖI 1: Chơi animation 1 lần khi chuyển trạng thái ---
        switch (m_currentState)
        {
        case WizardState::PATROL:
            m_animation.Play("IDLE"); // Bắt đầu tuần tra, đứng yên
            break;
        case WizardState::CHASE:
            m_animation.Play("FLYING"); // Bắt đầu rượt đuổi
            break;
        case WizardState::ATTACK:
            m_animation.Play("ATTACK"); // Chơi anim tấn công 1 LẦN
            
            break;
        case WizardState::DEATH:
            m_animation.Play("DEATH");
            // (Bạn có thể thêm âm thanh chết ở đây nếu muốn)
            break;
        }
    }
}

// Hàm helper để kiểm tra Player có trong tầm bắn không
bool EnemyWizard::CheckAttackRange(float deltaX) const {
    // Tầm bắn của lính
    float attackRange = m_attackRange;
    return std::abs(deltaX) <= attackRange;
}

sf::FloatRect EnemyWizard::GetBounds() const {
    // Kích thước (chiều rộng, chiều cao) của hộp
    const float hitboxWidth = 130.f;
    const float hitboxHeight = 130.f;

    // Vị trí của CHÂN (offset từ tâm Boss xuống)
    // Tăng số này để đẩy hộp xuống, giảm để kéo hộp lên
    const float feetOffset = 85.f;

    // ================================================================

    // Tính vị trí bên trái (left)
    // Dùng m_position.x (vì đây là Tọa độ Thế giới của Boss)
    float left = m_position.x - (hitboxWidth / 2.f);

    // Tính vị trí bên trên (top)
    // Dùng m_position.y (vì đây là Tọa độ Thế giới của Boss)
    float top = (m_position.y + feetOffset) - hitboxHeight;

    // Trả về hitbox mới (Tọa độ Thế giới)
    return sf::FloatRect({ left, top }, { hitboxWidth, hitboxHeight });
}

void EnemyWizard::TakeDamage(int damage) {
    // Nếu đã chết rồi thì không nhận thêm sát thương
    if (m_currentState == WizardState::DEATH) return;

    m_health -= damage;

    // --- SỬA LOGIC Ở ĐÂY ---
    if (m_health <= 0 && m_currentState != WizardState::DEATH)
    {
        m_health = 0; // Đảm bảo máu không bị âm
        TransitionState(WizardState::DEATH);

        // ĐẶT ĐỒNG HỒ ĐẾM NGƯỢC
        // Phải bằng: (số frame) * (thời gian 1 frame)
        // Ví dụ: 10 frames * 0.1f = 1.0 giây
        m_deathAnimTimer = 0.7f; // <-- THAY SỐ NÀY (nếu cần)
    }
}

bool EnemyWizard::IsDead() const {
    return m_health <= 0 && m_deathAnimTimer <= 0.0f;
}

void EnemyWizard::Draw(sf::RenderWindow& window) {
    if (IsDead())
        return;
    m_animation.Draw(window);
}

void EnemyWizard::SetDrawPosition(const sf::Vector2f& pos) {
    m_sprite.setPosition(pos);
}