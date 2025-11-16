#include "EnemyWizard2.h"
#include <cmath>
#include <iostream>
#include "AssetManeger.h"
#include <memory>
#include <SFML/Audio.hpp>
static sf::Texture dummyTexture;

EnemyWizard2::EnemyWizard2(sf::Vector2f spawnPos, float patrolDistance)
    : m_sprite(dummyTexture),
    m_animation(m_sprite),
    m_position(spawnPos),
    m_speed(100.0f),
    m_health(6),
    m_facingRight(true),
    m_attackCooldown(1.46f),
    m_attackTimer(0.7f),
    m_patrolDirection(1.0f),
    m_currentState(Wizard2State::PATROL),
    m_deathAnimTimer(0.0f),
    m_attackBuffer(),
    m_attackSound(m_attackBuffer),
    m_detectBuffer(),
    m_detectSound(m_detectBuffer),
    m_hasPlayedDetectSound(false)
{
    auto& asset = AssetManeger::getInstance();
    sf::Vector2i frameSize = { 140, 140 };

    /*if (!m_attackBuffer.loadFromFile("Ewizard_attack (2).mp3"))
    {
        std::cerr << "Khong the tai am thanh Ewizard2_attack.mp3\n";
    }*/
    if (!m_detectBuffer.loadFromFile("WizardSound.mp3")) // <-- Tên file của bạn
    {
        std::cerr << "Khong the tai am thanh WizardSound\n";
    }
    // ATTACK 
    m_animation.AddAnimation("ATTACK", &asset.getTexture("Wizard2_attack.png"),
        13, frameSize, 0.09f);

    // FLYING
    m_animation.AddAnimation("FLYING", &asset.getTexture("Wizard2_run.png"),
        8, frameSize, 0.08f);

    // IDLE 
    m_animation.AddAnimation("IDLE", &asset.getTexture("Wizard2_idle.png"),
        10, frameSize, 0.1f);
    //DEATH
    m_animation.AddAnimation("DEATH", &asset.getTexture("Wizard2_death.png"),
        18, frameSize, 0.15f);

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

void EnemyWizard2::Update(float dt, sf::Vector2f playerPos, float scrollOffset) {
    if (m_currentState == Wizard2State::DEATH)
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
    if (m_currentState == Wizard2State::PATROL) {
        if (CheckAttackRange(deltaX)) {
            TransitionState(Wizard2State::ATTACK);
        }
        else if (std::abs(deltaX) < detectionRange) {
            TransitionState(Wizard2State::CHASE);
        }
    }
    else if (m_currentState == Wizard2State::CHASE) {
        if (CheckAttackRange(deltaX)) {
            TransitionState(Wizard2State::ATTACK);
        }
        else if (std::abs(deltaX) > m_chaseRange) { // Thoát khỏi tầm rượt đuổi
            TransitionState(Wizard2State::PATROL);
        }
    }
    else if (m_currentState == Wizard2State::ATTACK) {
        if (!CheckAttackRange(deltaX)) {
            TransitionState(Wizard2State::CHASE);
        }
    }

    // --- Xử lý trạng thái ---
    switch (m_currentState)
    {
    case Wizard2State::PATROL:
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

    case Wizard2State::CHASE:
        // Di chuyển về phía Player
        if (deltaX > 0) {
            moveVector.x = 1.0f; // Di chuyển sang phải
        }
        else {
            moveVector.x = -1.0f; // Di chuyển sang trái
        }
        m_facingRight = (deltaX > 0);
        break;

    case Wizard2State::ATTACK:
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
    case Wizard2State::PATROL:
    case Wizard2State::CHASE:
        // Di chuyển (hoặc đứng yên nhưng sẵn sàng di chuyển)
        if (std::abs(moveVector.x) > 0.01f) {
            m_animation.Play("FLYING");
        }
        else {
            m_animation.Play("IDLE"); // Đứng yên nhưng quay mặt về Player
        }
        break;

    case Wizard2State::ATTACK:
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
std::optional<Wizard2BulletInfo> EnemyWizard2::TryToAttack(float dt, float deltaX) {
    if (IsDead() || m_currentState != Wizard2State::ATTACK) {
        return std::nullopt; // Không tấn công
    }

    // Logic bắn đạn
    if (m_attackTimer <= 0.0f) {
        // Tái tạo lại cooldown
        m_attackTimer = m_attackCooldown;
        m_attackSound.play();
        // Tính toán hướng bay của đạn
        sf::Vector2f bulletDirection(m_facingRight ? 1.0f : -1.0f, 0.0f);

        Wizard2BulletInfo bulletInfo;
        // Đặt vị trí xuất phát của đạn hơi lệch so với trung tâm Soldier
        float offsetX = m_facingRight ? 40.0f : -40.0f;
        bulletInfo.startPosition = m_position + sf::Vector2f(offsetX, -30.0f); // Tọa độ world
        bulletInfo.direction = bulletDirection;
        bulletInfo.speed = 700.0f; // Tốc độ đạn

        return bulletInfo; // Trả về thông tin viên đạn để lớp Game tạo
    }

    // Đã trong trạng thái ATTACK nhưng đang trong cooldown
    return std::nullopt;
}

// Hàm helper để chuyển trạng thái
void EnemyWizard2::TransitionState(Wizard2State newState) {
    if (m_currentState != newState) {
        m_currentState = newState;

        // --- SỬA LỖI 1: Chơi animation 1 lần khi chuyển trạng thái ---
        switch (m_currentState)
        {
        case Wizard2State::PATROL:
            m_animation.Play("IDLE"); // Bắt đầu tuần tra, đứng yên
            
            break;
        case Wizard2State::CHASE:
            m_animation.Play("FLYING"); // Bắt đầu rượt đuổi
            if (!m_hasPlayedDetectSound)
            {
                m_detectSound.play();
                m_hasPlayedDetectSound = true; // Đánh dấu là đã phát
            }
            break;
        case Wizard2State::ATTACK:
            m_animation.Play("ATTACK"); // Chơi anim tấn công 1 LẦN
            break;
        case Wizard2State::DEATH:
            m_animation.Play("DEATH");
            // (Bạn có thể thêm âm thanh chết ở đây nếu muốn)
            break;
        }
    }
}

// Hàm helper để kiểm tra Player có trong tầm bắn không
bool EnemyWizard2::CheckAttackRange(float deltaX) const {
    // Tầm bắn của lính
    float attackRange = m_attackRange;
    return std::abs(deltaX) <= attackRange;
}

sf::FloatRect EnemyWizard2::GetBounds() const {

    // 1. Lấy TỶ LỆ PHÓNG TO (scale)
    // Con số này PHẢI GIỐNG HỆT 'baseScaleX' trong hàm Update()
    float baseScale = 2.0f;

    // 2. === ĐIỀU CHỈNH CÁC SỐ NÀY ===
    // Đây là kích thước hitbox (ở scale 1.0f).
    // Dựa trên frame 250x250 của bạn, tôi đoán con quái vật
    // thật sự chỉ RỘNG 100px và CAO 180px.

    float hitboxWidth = 40.0f; // <-- Chỉnh số này
    float hitboxHeight = 90.0f; // <-- Chỉnh số này

    // ===================================

    // 3. Tính kích thước thực tế của hitbox sau khi đã phóng to
    float finalWidth = hitboxWidth * baseScale;
    float finalHeight = hitboxHeight * baseScale;

    // 4. Trả về hitbox (code này tự động căn giữa hitbox
    //    dựa trên vị trí 'm_position' của quái vật)
    return sf::FloatRect(
        // Vị trí (left, top)
        sf::Vector2f(m_position.x - finalWidth / 2.f,
            m_position.y - finalHeight / 2.f),
        // Kích thước (width, height)
        sf::Vector2f(finalWidth, finalHeight)
    );
}

void EnemyWizard2::TakeDamage(int damage) {
    // Nếu đã chết rồi thì không nhận thêm sát thương
    if (m_currentState == Wizard2State::DEATH) return;

    m_health -= damage;

    // --- SỬA LOGIC Ở ĐÂY ---
    if (m_health <= 0 && m_currentState != Wizard2State::DEATH)
    {
        m_health = 0; // Đảm bảo máu không bị âm
        TransitionState(Wizard2State::DEATH);

        // ĐẶT ĐỒNG HỒ ĐẾM NGƯỢC
        // Phải bằng: (số frame) * (thời gian 1 frame)
        // Ví dụ: 10 frames * 0.1f = 1.0 giây
        m_deathAnimTimer = 0.7f; // <-- THAY SỐ NÀY (nếu cần)
    }
}

bool EnemyWizard2::IsDead() const {
    return m_health <= 0 && m_deathAnimTimer <= 0.0f;
}

void EnemyWizard2::Draw(sf::RenderWindow& window) {
    if (IsDead())
        return;
    m_animation.Draw(window);
}

void EnemyWizard2::SetDrawPosition(const sf::Vector2f& pos) {
    m_sprite.setPosition(pos);
}