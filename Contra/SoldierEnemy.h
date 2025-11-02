#pragma once
#include <SFML/Graphics.hpp>
#include "IEnemy.h"

class SoldierEnemy : public IEnemy {
private:
    sf::Sprite m_sprite;          // Hinh anh ke dich
    sf::Texture m_texture;        // Texture cua ke dich
    sf::Vector2f m_position;      // Vi tri hien tai
    float m_speed;                // Toc do di chuyen
    int m_health;                 // Mau con lai
    bool m_facingRight;           // Huong di chuyen
    float m_attackCooldown;       // Thoi gian giua cac lan ban
    float m_attackTimer;          // Bo dem thoi gian ban
public:
    SoldierEnemy(sf::Vector2f spawnPos);
    void Update(float dt, sf::Vector2f playerPos) override;
    void Draw(sf::RenderWindow& window) override;
    sf::FloatRect GetBounds() const override;
    void TakeDamage(int damage) override;
    bool IsDead() const override;
};

