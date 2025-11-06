#pragma once
#include "AssetManeger.h"
#include <vector>
#include "Bullet.h"

namespace sf {
    class RenderWindow;
}

class Player {
private:
    sf::Sprite m_sprite;
    sf::Vector2f m_position;
    sf::Vector2f m_velocity;
    float m_speed;
    float m_scaleFactor;

    //  Các biến nhảy
    bool m_isJumping;
    float m_jumpForce;
    float m_gravity;
    float m_groundY;

    // Danh sách đạn
    std::vector<Bullet> m_bullets;
    float m_shootCooldown;     // thời gian chờ giữa 2 lần bắn
    const float m_shootDelay;  // độ trễ bắn cố định (có thể thay đổi giá trị để bắn nhanh/chậm)
    int m_facingDirection; // 1 = phải, -1 = trái
    bool m_wasShooting = false;
public:
    Player();

    void HandleInput(float dt);
    void Update(float dt);
    void Draw(sf::RenderWindow& window);

    //  Hàm bắn đạn
    void Shoot();

    //  Trả về vector đạn để kiểm tra va chạm sau này
    std::vector<Bullet>& GetBullets() { return m_bullets; }
    sf::Vector2f GetPosition() const;
};
