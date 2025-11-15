#pragma once
#include <SFML/Graphics.hpp>
#include "AssetManeger.h"
#include <list>
#include "Bullet.h"
#include "Animation.h"
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
    
namespace sf {
    class RenderWindow;
}
enum class PlayerState {
    Idle,
    Run,
    Jump
};
class Player {
private:
    sf::Sprite m_sprite;
    Animation m_animation;   // Bộ điều khiển animation
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
    std::list<Bullet> m_bullets;
    float m_shootCooldown;     // thời gian chờ giữa 2 lần bắn
    float m_shootDelay;  // độ trễ bắn cố định (có thể thay đổi giá trị để bắn nhanh/chậm)
    int m_facingDirection; // 1 = phải, -1 = trái
    bool m_wasShooting = false;
    bool m_isShooting = false;
    float m_shootTimer = 0.0f;

 
    sf::Texture m_idleTexture;
    sf::Texture m_runTexture;
    sf::Texture m_jumpTexture;

    bool m_onGround = false;

    sf::SoundBuffer m_attackBuffer;
    sf::Sound m_attackSound;

    float m_invincibilityTimer = 0.0f;     // Bộ đếm thời gian bất tử
    const float m_invincibilityDuration = 1.5f; // Bất tử trong 1 giây
  
    int m_health;                       // Số máu hiện tại
    const int m_maxHealth = 3;          // Máu tối đa
    sf::Texture m_heartTexture;         // Texture hiển thị máu
    std::vector<sf::Sprite> m_hearts;   // Danh sách sprite trái tim
public:
    Player();
    void HandleInput(float dt);
    void Update(float dt);
    void Draw(sf::RenderWindow& window);

    //  Hàm bắn đạn
    void Shoot();

    // Hàm trả về ranh giới của nhân vật để kiểm tra va chạm
    sf::FloatRect GetBounds() const;

    //  Trả về vector đạn để kiểm tra va chạm sau này
    std::list<Bullet>& GetBullets() { return m_bullets; }
    sf::Vector2f GetPosition() const;

    void SetPosition(const sf::Vector2f& pos);
    float GetSpeed() const { return m_speed; }

    void SetOnGround(bool value);
    bool IsOnGround() const;

  
    void PlayAttackSound(); //am thanh
    void TakeDamage(int amount); // hàm trừ máu
    bool IsDead() const { return m_health <= 0; }
    bool IsInvincible() const { return m_invincibilityTimer > 0.0f; }

    void SetJumpForce(float newForce);
    void SetShootDelay(float shootDelay);
};
