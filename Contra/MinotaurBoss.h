#pragma once

#include <SFML/Graphics.hpp>
#include "IEnemy.h"
#include "AnimationBoss.h"
#include <optional>
#include <memory>

// Các tr?ng thái c?a boss Minotaur
enum class MinotaurState {
    IDLE,           // G?m
    CHARGE,         // Lao t?i ngư?i chơi
    ATTACK,         // Đánh 2 phát
    GO_TO_CORNER,   // Đi ra góc chu?n b? húc
    ROAR_BEFORE_DASH, // G?m trư?c khi húc
    DASH_ACROSS     // Húc ngang qua màn h?nh
};

class MinotaurBoss : public IEnemy {
private:
    sf::Vector2f m_position;        // V? trí th?c t? c?a boss
    sf::Vector2f m_drawPos;         // V? trí hi?n th? (đ? tr? scroll)
    bool m_facingRight;             // Boss nh?n sang ph?i hay trái

    sf::Sprite m_sprite;            // Sprite c?a boss
    AnimationBoss m_animation;          // H? th?ng animation qu?n l? t?t c? state

    // Các thông s? cơ b?n
    int m_health;                   // Máu hi?n t?i
    float m_speed;                  // T?c đ? di chuy?n
    float m_stateTimer;             // B? đ?m th?i gian cho t?ng pha
    int m_attackCount;              // S? cú đánh trong pha attack

    // Tham s? hành vi
    float m_roarDuration;
    float m_chargeSpeed;
    float m_cornerSpeed;
    float m_dashSpeed;
    float m_attackRange;

    float m_leftCornerX;            // T?a đ? góc trái màn h?nh
    float m_rightCornerX;           // T?a đ? góc ph?i màn h?nh

    MinotaurState m_state;          // Tr?ng thái hi?n t?i

    // Các hàm x? l? n?i b?
    void TransitionState(MinotaurState newState);
    void HandleRoar(float dt);
    void HandleRoarBeforeDash(float dt);
    void HandleCharge(float dt, sf::Vector2f playerPos);
    void HandleAttack(float dt, sf::Vector2f playerPos);
    void HandleChargeAfterFirstAttack(float dt, sf::Vector2f playerPos);
    void HandleGoToCorner(float dt);
    void HandleDashAcross(float dt);

public:
    MinotaurBoss(sf::Vector2f spawnPos, float leftCornerX, float rightCornerX);

    void Update(float dt, sf::Vector2f playerPos, float scrollOffset) override;
    void Draw(sf::RenderWindow& window) override;

    sf::FloatRect GetBounds() const override;
    void TakeDamage(int damage) override;
    bool IsDead() const override;

    sf::Vector2f GetPosition() const override { return m_position; }
    void SetDrawPosition(const sf::Vector2f& pos) override;

    //std::optional<SoldierBulletInfo> TryToAttack(float dt, float deltaX) override;
};
