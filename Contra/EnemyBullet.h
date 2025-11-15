#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "AssetManeger.h"

struct SoldierBulletInfo {
    sf::Vector2f startPosition{}; // Vị trí xuất phát (World Coordinate)
    sf::Vector2f direction {};     // Hướng di chuyển
    float speed=0.0f;                // Tốc độ đạn
};
struct WizardBulletInfo {
    sf::Vector2f startPosition{}; // Vị trí xuất phát (World Coordinate)
    sf::Vector2f direction{};     // Hướng di chuyển
    float speed=0.0f;                // Tốc độ đạn
};
struct Wizard2BulletInfo {
    sf::Vector2f startPosition{}; // Vị trí xuất phát (World Coordinate)
    sf::Vector2f direction{};     // Hướng di chuyển
    float speed=0.0f;                // Tốc độ đạn
};

class EnemyBullet {
private:
    const sf::Texture* m_texture;           // Texture của viên đạn
    std::unique_ptr<sf::Sprite> m_sprite;   // Sprite của viên đạn
    sf::Vector2f m_position;                // Vị trí hiện tại (World Coordinate)
    sf::Vector2f m_direction;               // Hướng bay (đã được chuẩn hóa)
    float m_speed;                          // Tốc độ bay
    bool m_isAlive;                         // Trạng thái sống/chết của đạn

public:
    // Constructor nhận vị trí bắt đầu, hướng và tốc độ
    EnemyBullet(sf::Vector2f startPos, sf::Vector2f direction, float speed);

    // Cập nhật vị trí đạn theo thời gian
    void Update(float dt);

    // Vẽ đạn lên cửa sổ game
    void Draw(sf::RenderWindow& window, float scrollOffset);

    // Lấy vùng giới hạn (Bounds) để kiểm tra va chạm
    sf::FloatRect GetBounds() const;

    // Lấy vị trí hiện tại
    sf::Vector2f GetPosition() const { return m_position; }

    // Đánh dấu đạn đã va chạm
    void Hit() { m_isAlive = false; }

    // Kiểm tra xem đạn còn hoạt động không
    bool IsAlive() const { return m_isAlive; }
};