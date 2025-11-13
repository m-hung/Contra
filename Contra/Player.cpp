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
    m_attackSound(m_attackBuffer)   // 2. Liên kết Sound với Buffer

{
    auto& asset = AssetManeger::getInstance();
    m_animation.AddAnimation("Idle", &asset.getTexture("player_idle.png"), 8, { 128,128 }, 0.13f);
    m_animation.AddAnimation("Run", &asset.getTexture("player_run.png"), 8, { 128,128 }, 0.1f);
    m_animation.AddAnimation("Jump", &asset.getTexture("player_jump.png"), 8, { 128,128 }, 0.1f);
    m_animation.AddAnimation("Shoot", &asset.getTexture("shoot_player.png"), 7, { 128,128 }, 0.08f);

    m_animation.Play("Idle");
      
    if (!m_attackBuffer.loadFromFile("Sound_skill.mp3")) {
        std::cerr << "Khong the tai am thanh Sound_skill.mp3\n";
    }
    //  m_attackSound = sf::Sound{ m_attackBuffer };

    // --- Thiết lập máu người chơi ---
    m_health = m_maxHealth;
    if (!m_heartTexture.loadFromFile("heart.png")) {
        std::cerr << "Không thể tải hình máu (heart.png)\n";
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
{// --- Xử lý nhảy ---
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
    // Lấy sprite hiện tại từ animation
    const auto& animSprite = m_animation.GetSprite();

    // Lấy bounds gốc của sprite
    sf::FloatRect bounds = animSprite.getGlobalBounds();

    // Lấy kích thước thực tế sau khi scale
    float width = bounds.size.x;
    float height = bounds.size.y;

    return sf::FloatRect(
        // position (left, top)
        sf::Vector2f(m_position.x - width / 2.f, m_position.y - height / 2.f),
        // size (width, height)
        sf::Vector2f(width, height)
    );
}

void Player::Draw(sf::RenderWindow& window)
{
    m_animation.Draw(window);
    for (auto& bullet : m_bullets)
        bullet.Draw(window);

    // --- Vẽ máu người chơi ---
    for (int i = 0; i < m_health; ++i) {
        window.draw(m_hearts[i]);
    }
}

void Player::TakeDamage(int amount)
{
    m_health -= amount;
    if (m_health < 0) m_health = 0;
}
sf::FloatRect Player::GetBounds() const
{
    return m_sprite.getGlobalBounds();
}
void Player::PlayAttackSound() {
    if (m_attackSound.getStatus() != sf::Sound::Status::Playing)
        m_attackSound.play();
}
