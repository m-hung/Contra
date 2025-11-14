#include "MinotaurBoss.h"
#include "AssetManeger.h"
#include "AnimationBoss.h" // S?A: Dùng l?p AnimationBoss m?i
#include <iostream>
#include <cmath>

MinotaurBoss::MinotaurBoss(sf::Vector2f spawnPos, float leftCornerX, float rightCornerX)
    : m_position(spawnPos),
    m_sprite(AssetManeger::getInstance().getTexture("MinotaurBoss.png")),
    m_facingRight(true),
    m_health(1000),
    m_speed(150.f),
    m_stateTimer(0.f),
    m_attackCount(0),
    m_roarDuration(2.5f),
    m_chargeSpeed(320.f),
    m_cornerSpeed(200.f),
    m_dashSpeed(550.f),
    m_attackRange(120.f),
    m_leftCornerX(leftCornerX),
    m_rightCornerX(rightCornerX),
    m_state(MinotaurState::IDLE),
    m_animation(m_sprite) // m_animation ph?i là ki?u AnimationBoss
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

    std::cout << "MinotaurBoss xuất hiện!" << std::endl;
}

void MinotaurBoss::TransitionState(MinotaurState newState) {
    m_state = newState;
    m_stateTimer = 0.f;
    m_attackCount = 0;

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
        m_animation.Play("dash");
        std::cout << "Đi về góc màn hình!" << std::endl;
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

void MinotaurBoss::HandleCharge(float dt, sf::Vector2f playerPos) {
    m_stateTimer += dt;
    m_animation.Update(dt);

    float dir = (playerPos.x > m_position.x) ? 1.f : -1.f;
    m_facingRight = (dir > 0);
    m_position.x += dir * m_chargeSpeed * dt;

    float dist = std::abs(playerPos.x - m_position.x);
    if (dist < m_attackRange)
        TransitionState(MinotaurState::ATTACK);
    else if (m_stateTimer > 4.f)
        TransitionState(MinotaurState::GO_TO_CORNER);
}

void MinotaurBoss::HandleAttack(float dt, sf::Vector2f) {
    m_stateTimer += dt;
    m_animation.Update(dt);

    if (m_stateTimer > 0.6f) {
        m_attackCount++;
        std::cout << "Minotaur ?ánh phát th? " << m_attackCount << std::endl;
        m_stateTimer = 0.f;

        if (m_attackCount >= 2)
            TransitionState(MinotaurState::GO_TO_CORNER);
    }
}

void MinotaurBoss::HandleGoToCorner(float dt) {
    m_stateTimer += dt;
    m_animation.Update(dt);

    if (m_facingRight) {
        m_position.x += m_cornerSpeed * dt;
        if (m_position.x >= m_rightCornerX) {
            m_facingRight = false;
            TransitionState(MinotaurState::DASH_ACROSS);
        }
    }
    else {
        m_position.x -= m_cornerSpeed * dt;
        if (m_position.x <= m_leftCornerX) {
            m_facingRight = true;
            TransitionState(MinotaurState::DASH_ACROSS);
        }
    }
}

void MinotaurBoss::HandleDashAcross(float dt) {
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
    switch (m_state) {
    case MinotaurState::IDLE:         HandleRoar(dt); break;
    case MinotaurState::CHARGE:       HandleCharge(dt, playerPos); break;
    case MinotaurState::ATTACK:       HandleAttack(dt, playerPos); break;
    case MinotaurState::GO_TO_CORNER: HandleGoToCorner(dt); break;
    case MinotaurState::DASH_ACROSS:  HandleDashAcross(dt); break;
    }

    // L?t sprite theo h??ng nh?n
    m_sprite.setScale(sf::Vector2f(m_facingRight ? 4.f : -4.f, 4.f));

    // C?p nh?t v? trí hi?n th?
    m_drawPos = { m_position.x - scrollOffset, m_position.y };
    m_sprite.setPosition(m_drawPos);
}

void MinotaurBoss::Draw(sf::RenderWindow& window) {
    m_animation.Draw(window);
}

sf::FloatRect MinotaurBoss::GetBounds() const {
    return m_sprite.getGlobalBounds();
}

void MinotaurBoss::TakeDamage(int damage) {
    m_health -= damage;
    std::cout << "?? Boss trúng ??n! C?n " << m_health << " máu.\n";
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