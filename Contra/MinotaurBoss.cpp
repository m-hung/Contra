#include "MinotaurBoss.h"
#include "AssetManeger.h"
#include "AnimationBoss.h" 
#include <iostream>
#include <cmath>

MinotaurBoss::MinotaurBoss(sf::Vector2f spawnPos, float leftCornerX, float rightCornerX)
    : m_position(spawnPos),
    m_sprite(AssetManeger::getInstance().getTexture("MinotaurBoss.png")),
    m_facingRight(true),
    m_health(36),
    m_speed(150.f),
    m_stateTimer(0.f),
    m_attackCount(0),
    m_roarDuration(2.5f),
    m_chargeSpeed(320.f),
    m_cornerSpeed(200.f),
    m_dashSpeed(550.f),
    m_attackRange(80.f),
    m_leftCornerX(leftCornerX),
    m_rightCornerX(rightCornerX),
    m_state(MinotaurState::IDLE),
    m_animation(m_sprite), // m_animation ph?i là ki?u AnimationBoss
    m_roarBuffer(), // Buffer chứa dữ liệu âm thanh
    m_roarSound(m_roarBuffer), // Đối tượng Sound để phát âm thanh
    m_attackBuffer(), // Buffer chứa dữ liệu âm thanh
    m_attackSound(m_attackBuffer) // Đối tượng Sound để phát âm thanh
{
    // T?i texture
    auto& tex = AssetManeger::getInstance();
    m_sprite.setScale(sf::Vector2f(4.f, 4.f));

    // Kích thư?c m?i khung h?nh là 64x64 pixel
    sf::Vector2i frameSize(64, 64);
    const int FRAME_H = 64; // Chi?u cao m?i hàng frame

    // S?A: C?P NH?T ANIMATION V?I B? C?C M?I (4, 6, 10, 10 frames)
    // AnimationBoss::AddAnimation(name, texture, frameCount, frameSize, frameDuration, startY)

    // Hàng 1 (Y=0): IDLE (4 frames)
    m_animation.AddAnimation("idle", &tex.getTexture("MinotaurBoss.png"), 5, frameSize, 0.4f, 0 * FRAME_H);

    // Hàng 2 (Y=64): ATTACK (6 frames)
    m_animation.AddAnimation("attack", &tex.getTexture("MinotaurBoss.png"), 6, frameSize, 0.15f, 1 * FRAME_H);

    // Hàng 3 (Y=128): CHARGE / GO_TO_CORNER (10 frames)
    m_animation.AddAnimation("charge", &tex.getTexture("MinotaurBoss.png"), 10, frameSize, 0.1f, 2 * FRAME_H);

    // Hàng 4 (Y=192): DASH_ACROSS (10 frames)
    m_animation.AddAnimation("dash", &tex.getTexture("MinotaurBoss.png"), 10, frameSize, 0.1f, 4 * FRAME_H);

    m_animation.Play("idle");

    // Đặt Origin của sprite ở giữa
    auto bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin({ bounds.position.x + bounds.size.x / 2.f,
                         bounds.position.y + bounds.size.y / 2.f });

    // Thêm đoạn code TẢI ÂM THANH ROAR
    if (!m_roarBuffer.loadFromFile("RoarBoss.mp3")) {
        std::cerr << "Lỗi: Không tải được file âm thanh RoarBoss.mp3!" << std::endl;
    }
    else {
        m_roarSound.setVolume(80.f);
        m_roarSound.play();
        // Có thể tùy chỉnh âm lượng, lặp lại, v.v.
    }

    // Thêm đoạn code TẢI ÂM THANH ATTACK
    if (!m_attackBuffer.loadFromFile("AttackBossTest.mp3")) {
        std::cerr << "Lỗi: Không tải được file âm thanh AttackBoss.mp3!" << std::endl;
    }

    std::cout << "MinotaurBoss xuất hiện!" << std::endl;
}

void MinotaurBoss::TransitionState(MinotaurState newState) {
    m_stateTimer = 0.f;
    m_state = newState;
    //m_attackCount = 0;

    switch (newState) {
    case MinotaurState::IDLE:
        m_animation.Play("idle");
        std::cout << "Chờ đợi..." << std::endl;
        break;
    case MinotaurState::CHARGE:
        m_animation.Play("charge");
        std::cout << "Lao về phía ngườii chơi!" << std::endl;
        break;
    case MinotaurState::ATTACK:
        m_animation.Play("attack");
        std::cout << "Tấn công cận chiến!" << std::endl;
        
        break;
    case MinotaurState::GO_TO_CORNER:
        m_animation.Play("charge");
        std::cout << "Đi về góc màn hình!" << std::endl;
        break;
    case MinotaurState::ROAR_BEFORE_DASH:
        m_animation.Play("idle");
        std::cout << "Gầm trước khi húc" << std::endl;
        m_roarSound.play();
        break;
    case MinotaurState::DASH_ACROSS:
        m_animation.Play("dash");
        std::cout << "Húc ngang qua toàn màn hình!" << std::endl;
        break;
    }
}

void MinotaurBoss::HandleRoar(float dt) {
    m_stateTimer += dt;
    m_animation.Update(dt);
    if (m_stateTimer >= m_roarDuration)
        TransitionState(MinotaurState::CHARGE);
}

void MinotaurBoss::HandleRoarBeforeDash(float dt) {
    m_stateTimer += dt;
    m_animation.Update(dt);
    if (m_stateTimer >= m_roarDuration)
        TransitionState(MinotaurState::DASH_ACROSS);
}

void MinotaurBoss::HandleCharge(float dt, sf::Vector2f playerPos) {
    m_stateTimer += dt;
    m_animation.Update(dt);

    float dir = (playerPos.x > m_position.x) ? 1.f : -1.f;
    m_facingRight = (dir > 0);
    m_position.x += dir * m_chargeSpeed * dt;

    float dist = std::abs(playerPos.x - m_position.x);
    if (dist < m_attackRange) {
        TransitionState(MinotaurState::ATTACK);
    }
/*   else if (m_stateTimer > 4.f)
        TransitionState(MinotaurState::GO_TO_CORNER);*/
}

void MinotaurBoss::HandleAttack(float dt, sf::Vector2f playerPos) {
    // In dt để debug
    //std::cout << "m_stateTimer: " << m_stateTimer << " | dt: " << dt << std::endl;


    m_stateTimer += dt;
    m_animation.Update(dt);

    if (m_stateTimer >= 0.9f) {
        m_attackSound.play();
        m_attackCount++;
        std::cout << "Minotaur đánh phát thứ " << m_attackCount << std::endl;
        m_stateTimer = 0.f;

        if (m_attackCount == 1) {
            // Phát đánh 1 xong: Chuyển sang đuổi theo người chơi (CHARGE) để đánh phát 2
            TransitionState(MinotaurState::CHARGE);
        } else if (m_attackCount >= 2) {
            // Phát đánh 2 xong: Chuyển sang GO_TO_CORNER
            TransitionState(MinotaurState::GO_TO_CORNER);
        }
    }
}

void MinotaurBoss::HandleGoToCorner(float dt) {
    m_stateTimer += dt;
    m_animation.Update(dt);

    if (m_facingRight) {
        m_position.x += m_cornerSpeed * dt;
        if (m_position.x >= m_rightCornerX) {
            m_facingRight = false;
            m_stateTimer = 0.f;
            TransitionState(MinotaurState::ROAR_BEFORE_DASH);
        }
    }
    else {
        m_facingRight = true;
        m_position.x += m_cornerSpeed * dt;
        if (m_position.x >= m_rightCornerX) {
            m_facingRight = false;
            m_stateTimer = 0.f;
            TransitionState(MinotaurState::ROAR_BEFORE_DASH);
        }
    }
}

void MinotaurBoss::HandleDashAcross(float dt) {
    m_attackCount = 0;
    m_stateTimer += dt;
    m_animation.Update(dt);

    float dir = m_facingRight ? 1.f : -1.f;
    m_position.x += dir * m_dashSpeed * dt;

    // S?A: Dùng th?i gian c? đ?nh 2.5s đ? mô ph?ng Dash ngang màn h?nh
    if (m_stateTimer >= 2.5f) { // Th?i gian này có th? c?n đi?u ch?nh
        TransitionState(MinotaurState::IDLE);
    }
}

void MinotaurBoss::Update(float dt, sf::Vector2f playerPos, float scrollOffset) {
    // --- LOGIC MỚI: KHI BOSS CHẾT ---
    if (m_isFadingOut) {
        // Nếu đang mờ dần, chỉ chạy logic fade
        if (m_fadeTimer < m_fadeDuration) {
            m_fadeTimer += dt;
            if (m_fadeTimer > m_fadeDuration) {
                m_fadeTimer = m_fadeDuration;
            }

            // Tính toán độ trong suốt (alpha) từ 255 (rõ) -> 0 (mờ)
            float alpha_float = 255.f * (1.f - (m_fadeTimer / m_fadeDuration));

            // Ép kiểu float về kiểu 8-bit (0-255)
            unsigned char alpha_char = static_cast<unsigned char>(alpha_float);

            // Áp dụng màu mới cho sprite
            m_sprite.setColor(sf::Color(255, 255, 255, alpha_char));
        }
    }
    // --- LOGIC CŨ: KHI BOSS CÒN SỐNG ---
    else
    {
        // Nếu boss chưa chết, chạy AI bình thường
        switch (m_state) {
        case MinotaurState::IDLE:           HandleRoar(dt); break;
        case MinotaurState::CHARGE:         HandleCharge(dt, playerPos); break;
        case MinotaurState::ATTACK:         HandleAttack(dt, playerPos); break;
        case MinotaurState::GO_TO_CORNER:   HandleGoToCorner(dt); break;
        case MinotaurState::ROAR_BEFORE_DASH: HandleRoarBeforeDash(dt); break;
        case MinotaurState::DASH_ACROSS:    HandleDashAcross(dt); break;
        }

        // Lật sprite theo hướng nhìn
        m_sprite.setScale(sf::Vector2f(m_facingRight ? 4.f : -4.f, 4.f));
    }

    // --- CẬP NHẬT VỊ TRÍ (LUÔN CHẠY) ---
    // Cập nhật vị trí hiển thị (kể cả khi đang mờ dần)
    m_drawPos = { m_position.x - scrollOffset, m_position.y };
    m_sprite.setPosition(m_drawPos);
}

void MinotaurBoss::Draw(sf::RenderWindow& window) {
    m_animation.Draw(window);
}

sf::FloatRect MinotaurBoss::GetBounds() const {
    // Kích thước (chiều rộng, chiều cao) của hộp
    const float hitboxWidth = 110.f;
    const float hitboxHeight = 190.f;

    // Vị trí của CHÂN (offset từ tâm Boss xuống)
    // Tăng số này để đẩy hộp xuống, giảm để kéo hộp lên
    const float feetOffset = 130.f;

    // ================================================================

    // Tính vị trí bên trái (left)
    // Dùng m_position.x (vì đây là Tọa độ Thế giới của Boss)
    float left = m_position.x - (hitboxWidth / 2.f);

    // Tính vị trí bên trên (top)
    float top = (m_position.y + feetOffset) - hitboxHeight;

    // Trả về hitbox mới (Tọa độ Thế giới)
    return sf::FloatRect({ left, top }, { hitboxWidth, hitboxHeight });
}

void MinotaurBoss::TakeDamage(int damage) {
    m_health -= damage;
    std::cout << " Boss trúng đạn! Còn " << m_health << " máu.\n";
    // --- LOGIC KHI CHẾT ---
    if (IsDead()) {
        m_isFadingOut = true; // Kích hoạt trạng thái mờ dần
        m_fadeTimer = 0.0f;   // Bắt đầu đếm

        std::cout << "Boss đã bị tiêu diệt!\n";
    }
}

bool MinotaurBoss::IsDead() const {
    return m_health <= 0;
}

void MinotaurBoss::SetDrawPosition(const sf::Vector2f& pos) {
    m_drawPos = pos;
    m_sprite.setPosition(m_drawPos);
}

//std::optional<SoldierBulletInfo> MinotaurBoss::TryToAttack(float, float) {
//    return std::nullopt; // Boss không b?n ??n
//}