#include <iostream>
#include "Player.h"
#include "AssetManeger.h"   
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cmath>

Player::Player()
    : m_speed(200.0f),
    m_position(0.0f, 500.0f),
    m_sprite(AssetManeger::getInstance().getTexture("player_image.png")),
    m_scaleFactor(0.2f),
    m_isJumping(false),
    m_jumpForce(-3000.0f),   //  nhảy chậm hơn (giá trị tuyệt đối nhỏ hơn)
    m_gravity(15000.0f),      //  trọng lực hợp lý hơn (rơi chậm hơn)
    m_groundY(500.0f),        // vị trí mặt đất
    m_shootCooldown(0.5f),
    m_shootDelay(0.2f) // 0.2s giữa 2 viên đạn (bắn vừa phải)
{
    m_sprite.setPosition(m_position);
    m_sprite.setScale({ m_scaleFactor, m_scaleFactor });
}

void Player::HandleInput(float dt)
{
    m_velocity.x = 0.0f;

    // Di chuyển trái/phải
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        m_velocity.x = -m_speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        m_velocity.x = m_speed;

    // Nhảy
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && !m_isJumping)
    {
        m_isJumping = true;
        m_velocity.y = m_jumpForce;
    }

    // Giảm cooldown theo thời gian
    if (m_shootCooldown > 0.0f)
        m_shootCooldown -= dt;

    // Bắn (chỉ bắn nếu cooldown đã hết)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::J) && m_shootCooldown <= 0.0f)
    {
        Shoot();
        m_shootCooldown = m_shootDelay; // reset cooldown
    }
}

void Player::Shoot()
{
    // Vị trí bắt đầu của viên đạn (ở vị trí nhân vật)
    sf::Vector2f bulletPos = m_sprite.getPosition();

    // Lấy kích thước sprite trong SFML 3.0
    auto bounds = m_sprite.getGlobalBounds();

    // Dời viên đạn ra một chút phía trước nhân vật
    bulletPos.x += bounds.size.x * m_scaleFactor + 30.0f;
    bulletPos.y += 40.0f; //chỉnh vị trí đạn xuống bằng với nhân vật
    // Thêm viên đạn mới vào danh sách
    m_bullets.emplace_back(bulletPos, 1.0f); // 1.0f là hướng bắn sang phải

}

void Player::Update(float dt)
{
    // Áp dụng trọng lực khi nhảy
    if (m_isJumping)
    {
        // nếu đang đi lên thì giảm tốc nhẹ (cho cảm giác nhảy chậm, mượt)
        if (m_velocity.y < 0)
            m_velocity.y *= 0.98f;

        // áp dụng trọng lực
        m_velocity.y += m_gravity * dt;
        m_position.y += m_velocity.y * dt;

        // chạm đất → dừng nhảy
        if (m_position.y >= m_groundY)
        {
            m_position.y = m_groundY;
            m_velocity.y = 0.0f;
            m_isJumping = false;
        }
    }

    //  Cập nhật vị trí ngang
    m_position.x += m_velocity.x * dt;

    //  Cập nhật vị trí sprite
    m_sprite.setPosition(m_position);
    //  Cập nhật các viên đạn
    for (auto& bullet : m_bullets)
        bullet.Update(dt);

    //  Xóa đạn ra khỏi màn hình
    m_bullets.erase(
        std::remove_if(m_bullets.begin(), m_bullets.end(),
            [](const Bullet& b) { return b.IsOutOfScreen(1280.0f); }),//chinh khoang cach dan ban
        m_bullets.end());

}


void Player::Draw(sf::RenderWindow& window)
{
    window.draw(m_sprite);
    for (auto& bullet : m_bullets)
        bullet.Draw(window);
}
