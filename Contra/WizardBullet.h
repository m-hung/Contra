#pragma once
#include <SFML/Graphics.hpp>
#include "Animation.h" 

// Class này dành riêng cho đạn của EnemyWizard2
class WizardBullet {
private:
    sf::Sprite m_sprite;        // Phải khai báo TRƯỚC m_animation
    Animation m_animation;
    sf::Vector2f m_position;    // Vị trí World (luôn là sự thật)
    sf::Vector2f m_velocity;    // Vận tốc (hướng * tốc độ)
    float m_lifetime;         // Thời gian sống

    std::string m_secondAnimName; // Tên của animation 7-frame
    float m_animTransitionTimer; // Đồng hồ đếm ngược để chuyển anim

    // --- Biến tạm để tính hitbox ---
    sf::Vector2i m_frameSize;
    float m_scale;

public:
    // Constructor nhận thông tin từ struct Wizard2BulletInfo
    WizardBullet(sf::Vector2f startPos, sf::Vector2f direction, float speed);

    void Update(float dt);

    // Hàm Draw nhận scrollOffset để vẽ đúng vị trí
    void Draw(sf::RenderWindow& window, float scrollOffset);

    // Hàm lấy hitbox (ở tọa độ World)
    sf::FloatRect GetBounds() const;

    // Hàm này được gọi khi đạn trúng Player
    void Hit() { m_lifetime = 0.f; }

    // Kiểm tra xem đạn đã "chết" chưa (hết giờ hoặc đã va chạm)
    bool IsDead() const { return m_lifetime <= 0.f; }
};
