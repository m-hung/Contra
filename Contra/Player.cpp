#include <iostream>
#include "Player.h"
#include "AssetManeger.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cmath>
#include "Animation.h"
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

Player::Player()
    : m_speed(200.0f),
    m_position(80.0f, 580.0f), // vị trí nhân vật
    m_sprite(AssetManeger::getInstance().getTexture("player_image.png")),
    m_animation(m_sprite),
    m_scaleFactor(1.3f),
    m_isJumping(false),
    m_jumpForce(-720.0f),//lực nhay
    m_gravity(2000.0f),//trọng lực
    m_groundY(580.0f),
    m_shootCooldown(0.5f),
    m_shootDelay(0.5f),
    m_facingDirection(1),
    m_attackBuffer(),               // 1. Khởi tạo buffer (rỗng)
    m_attackSound(m_attackBuffer),   // 2. Liên kết Sound với Buffer
    m_hitBuffer(),
    m_hitSound(m_hitBuffer),
    m_deathBuffer(),
    m_deathSound(m_deathBuffer)

{
    auto& asset = AssetManeger::getInstance();
    m_animation.AddAnimation("Idle", &asset.getTexture("player_idle.png"), 8, { 128,128 }, 0.13f);
    m_animation.AddAnimation("Run", &asset.getTexture("player_run.png"), 8, { 128,128 }, 0.1f);
    m_animation.AddAnimation("Jump", &asset.getTexture("player_jump.png"), 8, { 128,128 }, 0.1f);
    m_animation.AddAnimation("Shoot", &asset.getTexture("shoot_player.png"), 7, { 128,128 }, 0.08f);
    m_animation.AddAnimation("Death", &asset.getTexture("dead_player.png"), 4, { 128,128 }, 0.12f, false);


    m_animation.Play("Idle");
      
    if (!m_attackBuffer.loadFromFile("Sound_skill.mp3")) {
        std::cerr << "Khong the tai am thanh Sound_skill.mp3\n";
    }
    if (!m_hitBuffer.loadFromFile("damage_player.mp3")) 
    {
        std::cerr << "Khong the tai am thanh damage_player.mp3\n";
    }
    if (!m_deathBuffer.loadFromFile("player_death.mp3"))
    {
        std::cerr << "Khong the tai am thanh player_death.mp3\n";
    }
    // --- Thiết lập máu người chơi ---
    m_health = m_maxHealth;
    if (!m_heartTexture.loadFromFile("heart.png")) {
        std::cerr << "Khong the tai hinh mau (heart.png)\n";
    }
    else {
        for (int i = 0; i < m_maxHealth; ++i) {
            sf::Sprite heart(m_heartTexture);
            // kích cỡ hình tim
            heart.setScale({ 0.03f, 0.03f });

            // Đặt vị trí ở góc trái màn hình, thẳng hàng ngang
            // Mỗi tim cách nhau 60 pixel
            heart.setPosition({ 20.f + i * 60.f, 10.f });
            m_hearts.push_back(heart);
        }
    }
    

    // --- Đặt origin của nhân vật ở giữa để khi lật không bị lệch ---
    auto& animSprite = m_animation.GetSprite();
    auto bounds = animSprite.getLocalBounds();
    animSprite.setOrigin({ bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y / 2.f });
    animSprite.setPosition(m_position);
    animSprite.setScale({ m_scaleFactor, m_scaleFactor });

}


void Player::HandleInput(float dt)
{
    m_velocity.x = 0.0f;

    if (m_isDying) return;

    // --- DI CHUYỂN TRÁI ---
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        m_velocity.x = -m_speed;
        m_facingDirection = -1;
        if (!m_isJumping && !m_isShooting) m_animation.Play("Run");
    }
    // --- DI CHUYỂN PHẢI ---
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        m_velocity.x = m_speed;
        m_facingDirection = 1;
        if (!m_isJumping && !m_isShooting) m_animation.Play("Run");
    }
    else if (!m_isJumping && !m_isShooting) {
        m_animation.Play("Idle");
    }

    // --- NHẢY ---
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) && !m_isJumping)
    {
        m_animation.Play("Jump");
        m_isJumping = true;
        m_velocity.y = m_jumpForce;
    }

    // --- BẮN ---
    if (m_shootCooldown > 0.0f)
        m_shootCooldown -= dt;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::J) && m_shootCooldown <= 0.0f) {
        Shoot();
        m_shootCooldown = m_shootDelay;
        PlayAttackSound();

        // --- Animation bắn ---
        m_isShooting = true;
        m_shootTimer = 0.5f;
        m_animation.Play("Shoot");
    }
}
void Player::SetOnGround(bool value) {
    m_onGround = value;
}

bool Player::IsOnGround() const {
    return m_onGround;
}


void Player::Shoot()
{
    sf::Vector2f bulletPos = m_animation.GetSprite().getPosition();
    m_attackSound.play();
    auto bounds = m_animation.GetSprite().getGlobalBounds();

    if (m_facingDirection == 1)
        bulletPos.x += bounds.size.x * 0.1f; // ra bên phải
    else
        bulletPos.x -= bounds.size.x * 0.1f; // ra bên trái

    bulletPos.y -= -5.0f; // chỉnh tay nhân vật

    m_bullets.emplace_back(bulletPos, static_cast<float>(m_facingDirection));
}

void Player::Update(float dt)
{
    if (m_isDying)
    {
        m_animation.Update(dt); // Chỉ cập nhật animation

        if (!m_deathAnimFinished && m_animation.IsFinished()) // Đã sửa (không có "Death")
        {
            m_deathAnimFinished = true;
            // Bạn có thể thêm trigger Game Over ở đây
        }
        return; // Dừng mọi logic khác (nhảy, bắn, di chuyển)
    }
    // --- Xử lý nhảy ---
    if (m_isJumping)
    {
        m_velocity.y += m_gravity * dt;   // tăng tốc rơi dần

        // Cập nhật vị trí theo vận tốc
        m_position += m_velocity * dt;

        // Khi chạm đất
        if (m_position.y >= m_groundY)
        {
            m_position.y = m_groundY;
            m_velocity.y = 0.f;
            m_isJumping = false;
        }
    }
    else
    {
        // Nếu không nhảy, chỉ di chuyển ngang
        m_position.x += m_velocity.x * dt;
    }


    m_position.x += m_velocity.x * dt;

    // --- Nếu đang bắn ---
    if (m_isShooting)
    {
        m_shootTimer -= dt;
        if (m_shootTimer <= 0.0f)
        {
            m_isShooting = false;
            if (std::abs(m_velocity.x) > 0.1f)
                m_animation.Play("Run");
            else
                m_animation.Play("Idle");
        }
    }

    // --- Cập nhật sprite của animation ---
    auto& animSprite = m_animation.GetSprite();
    animSprite.setPosition(m_position);
    animSprite.setScale({ m_scaleFactor * m_facingDirection, m_scaleFactor });

    m_animation.Update(dt);
   
    // --- Cập nhật đạn ---
    for (auto it = m_bullets.begin(); it != m_bullets.end(); ) {
        it->Update(dt);
        if (it->IsOutOfScreen(1280.0f) || it->IsDead()) // ví dụ cửa sổ rộng 1280px
            it = m_bullets.erase(it);
        else
            ++it;
    }
    // --- Cập nhật thời gian bất tử ---
    if (m_invincibilityTimer > 0.0f) {
        m_invincibilityTimer -= dt;
    }

}
sf::Vector2f Player::GetPosition() const
{
    return m_position;
}

void Player::SetPosition(const sf::Vector2f& pos)
{
    m_position = pos;
    m_animation.GetSprite().setPosition(m_position);
}

sf::FloatRect Player::GetBounds() const
{
    // Kích thước (chiều rộng, chiều cao) của hộp đỏ
    const float hitboxWidth = 45.f;
    const float hitboxHeight = 85.f;

    // Vị trí của CHÂN (offset từ tâm Player xuống)
    // Tăng số này để đẩy hộp đỏ XUỐNG DƯỚI
    // Giảm số này để kéo hộp đỏ LÊN TRÊN
    const float feetOffset = 85.f;

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

void Player::Draw(sf::RenderWindow& window)
{
    bool shouldDrawPlayer = true; // Mặc định là vẽ

    // Nếu ĐANG CHẾT → KHÔNG nhấp nháy
    if (!m_isDying && m_invincibilityTimer > 0.0f)
    {
        const float blinkInterval = 0.1f;

        if (std::fmod(m_invincibilityTimer, blinkInterval * 2.0f) < blinkInterval)
            shouldDrawPlayer = false;
    }

    // Chỉ vẽ Player nếu "shouldDrawPlayer" là true
    if (shouldDrawPlayer) {
        m_animation.Draw(window);
    }

    // Vẽ đạn 
    for (auto& bullet : m_bullets)
        bullet.Draw(window);

    // Vẽ máu 
    for (int i = 0; i < m_health; ++i) {
        window.draw(m_hearts[i]);
    }
}

void Player::TakeDamage(int amount)
{
    // Nếu đang bất tử hoặc đã chết, không nhận thêm sát thương
    if (m_invincibilityTimer > 0.0f || IsDead()) {
        return;
    }
    m_hitSound.play();
    m_health -= amount;
    if (m_health < 0) m_health = 0;

    // BẮT ĐẦU ANIMATION CHẾT
    if (m_health == 0)
    {
        m_isDying = true;
        m_isShooting = false;
        m_velocity = { 0,0 };

        m_animation.Play("Death");
        m_deathSound.play();
        return;
    }

    // Kích hoạt thời gian bất tử (1 giây)
    m_invincibilityTimer = m_invincibilityDuration;
}

void Player::PlayAttackSound() {
    if (m_attackSound.getStatus() != sf::Sound::Status::Playing)
        m_attackSound.play();
}


// Dùng khi cường hóa nhảy
void Player::SetJumpForce(float newForce) {
    m_jumpForce = newForce;
}

void Player::SetShootDelay(float shootDelay) {
    m_shootDelay = shootDelay;
}
