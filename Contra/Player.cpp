#include <iostream>
#include "Player.h"
#include "AssetManeger.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cmath>
#include "Animation.h"

Player::Player()
    : m_speed(200.0f),
    m_position(0.0f, 580.0f), // vị trí nhân vật
    m_sprite(AssetManeger::getInstance().getTexture("player_image.png")),
    m_animation(m_sprite),
    m_scaleFactor(1.3f),
    m_isJumping(false),
    m_jumpForce(-720.0f),//lực nhay
    m_gravity(2000.0f),//trọng lực
    m_groundY(580.0f),
    m_shootCooldown(0.5f),
    m_shootDelay(0.2f),
    m_facingDirection(1)
{
    auto& asset = AssetManeger::getInstance();
    m_animation.AddAnimation("Idle", &asset.getTexture("player_idle.png"), 8, { 128,128 }, 0.13f);
    m_animation.AddAnimation("Run", &asset.getTexture("player_run.png"), 8, { 128,128 }, 0.1f);
    m_animation.AddAnimation("Jump", &asset.getTexture("player_jump.png"), 8, { 128,128 }, 0.1f);
    m_animation.AddAnimation("Shoot", &asset.getTexture("shoot_player.png"), 7, { 128,128 }, 0.04f);

    m_animation.Play("Idle");

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

        // --- Animation bắn ---
        m_isShooting = true;
        m_shootTimer = 0.3f;
        m_animation.Play("Shoot");
    }
}

void Player::Shoot()
{
    sf::Vector2f bulletPos = m_animation.GetSprite().getPosition();
    auto bounds = m_animation.GetSprite().getGlobalBounds();

    if (m_facingDirection == 1)
        bulletPos.x += bounds.size.x * 0.2f; // ra bên phải
    else
        bulletPos.x -= bounds.size.x * 0.2f; // ra bên trái

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
        if (it->IsOutOfScreen(1280.0f)) // ví dụ cửa sổ rộng 1280px
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

void Player::Draw(sf::RenderWindow& window)
{
    m_animation.Draw(window);
    for (auto& bullet : m_bullets)
        bullet.Draw(window);
}
