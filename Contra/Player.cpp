#include <iostream>
#include "Player.h"
#include "AssetManeger.h"   
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cmath>

Player::Player()
    : m_speed(200.0f),
    m_position(0.0f, 500.0f),//vi tri nhan vat
    m_sprite(AssetManeger::getInstance().getTexture("player_image.png")),
    m_scaleFactor(0.2f),
    m_isJumping(false),
    m_jumpForce(-3000.0f),   //  nhảy chậm hơn (giá trị tuyệt đối nhỏ hơn)
    m_gravity(15000.0f),      //  trọng lực hợp lý hơn (rơi chậm hơn)
    m_groundY(500.0f),        // vị trí mặt đất
    m_shootCooldown(0.5f),
    m_shootDelay(0.2f), // 0.2s giữa 2 viên đạn (bắn vừa phải)
    m_facingDirection(1) // mặc định quay phải
{
    // --- Đặt origin của nhân vật ở giữa để khi lật không bị lệch ---
    auto bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin(
        { bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y / 2.f }
    );

    // Đặt vị trí và tỉ lệ
    m_sprite.setPosition(m_position);
    m_sprite.setScale(sf::Vector2f(m_scaleFactor, m_scaleFactor));

}

void Player::HandleInput(float dt)
{
    m_velocity.x = 0.0f;

    // di chuyển trái
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
    {
        m_velocity.x = -m_speed;
        m_facingDirection = -1;
    }

    // di chuyển phải
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
    {
        m_velocity.x = m_speed;
        m_facingDirection = 1;
    }

    // nhảy
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) && !m_isJumping)
    {
        m_isJumping = true;
        m_velocity.y = m_jumpForce;
    }

    // cập nhật cooldown
    if (m_shootCooldown > 0.0f)
        m_shootCooldown -= dt;

    // detect edge: chỉ khi vừa nhấn (press), không phải giữ
    bool isShooting = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P);
    if (isShooting && !m_wasShooting && m_shootCooldown <= 0.0f)
    {
        Shoot();
        m_shootCooldown = m_shootDelay;
    }
    m_wasShooting = isShooting;

}



void Player::Shoot()
{
    // Vị trí ban đầu của viên đạn
    sf::Vector2f bulletPos = m_sprite.getPosition();

    // Lấy kích thước sprite
    auto bounds = m_sprite.getGlobalBounds();

    // Dịch viên đạn ra phía trước tùy theo hướng quay mặt
    if (m_facingDirection == 1) // Quay sang phải
    {
        bulletPos.x += bounds.size.x * m_scaleFactor + -10.0f; // ra bên phải
    }
    else // Quay sang trái
    {
        bulletPos.x -= 30.0f; // ra bên trái 1 chút
    }

    // Dời viên đạn xuống một tí cho ngang với tay nhân vật
    bulletPos.y += -10.0f;

    // Tạo đạn với hướng tương ứng
    m_bullets.emplace_back(bulletPos, static_cast<float>(m_facingDirection));
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
    if (m_facingDirection == 1)
        m_sprite.setScale({ m_scaleFactor, m_scaleFactor }); // quay phải
    else
        m_sprite.setScale({ -m_scaleFactor, m_scaleFactor }); // quay trái

    //  Cập nhật các viên đạn
    for (auto& bullet : m_bullets)
        bullet.Update(dt);

    //  Xóa đạn ra khỏi màn hình
    m_bullets.erase(
        std::remove_if(m_bullets.begin(), m_bullets.end(),
            [](const Bullet& b) { return b.IsOutOfScreen(1280.0f); }),//chinh khoang cach dan ban
        m_bullets.end());

}

sf::Vector2f Player::GetPosition() const
{
    return m_position;
}

void Player::Draw(sf::RenderWindow& window)
{
    window.draw(m_sprite);
    for (auto& bullet : m_bullets)
        bullet.Draw(window);
}
