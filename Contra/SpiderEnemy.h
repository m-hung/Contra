#pragma once

#include "IEnemy.h" 
#include <SFML/Graphics.hpp>
#include "AssetManeger.h"

class SpiderEnemy : public IEnemy {
public:
    SpiderEnemy(sf::Vector2f position);
    void Update(float dt, sf::Vector2f playerPos, float scrollOffset) override;
    void Draw(sf::RenderWindow& window) override;
    sf::FloatRect GetBounds() const override;
    void TakeDamage(int damage) override;
    bool IsDead() const override { return m_currentHP <= 0; }
    void SetDrawPosition(const sf::Vector2f& pos) override {
        m_sprite.setPosition(pos);
    }
    sf::Vector2f GetPosition() const override { return m_sprite.getPosition(); }
private:
    sf::Sprite m_sprite;
    // --- Biến Animation ---
    sf::IntRect m_currentFrame;  // Vùng cắt khung hình hiện tại
    float m_animationTimer = 0.0f;
    const float FRAME_DURATION = 0.1f; // Tốc độ chuyển khung hình (10 khung/giây)
    const int FRAME_WIDTH = 64;        // Chiều rộng mỗi khung hình
    const int FRAME_HEIGHT = 64;       // Chiều cao mỗi khung hình
    const int WALK_START_COLUMN = 0;   // Bắt đầu từ cột 0
    const int WALK_ROW = 2;            // Hàng thứ 2 (chỉ số 1)
    const int WALK_FRAME_COUNT = 8;    // Có 8 khung hình đi bộ trên hàng 2

    // Thuộc tính kẻ địch 
    int m_maxHP = 10;
    int m_currentHP;
	float m_speed = 100.0f; // Tốc độ di chuyển 

	// Hàm tính toán hướng di chuyển về phía mục tiêu
    sf::Vector2f CalculateMovementDirection(sf::Vector2f targetPosition) const;
};
