#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "IEnemy.h"
#include "AssetManeger.h"

class EnemySpawner {
public:
    EnemySpawner(sf::Vector2f position, sf::Vector2f size, float rate);

    void Update(float dt, std::vector<std::unique_ptr<IEnemy>>& enemies, float scrollOffset, sf::Vector2f playerPos);
    void Draw(sf::RenderWindow& window, float scrollOffset) const;

    // Hàm để nhận sát thương (giống quái)
    void TakeDamage(int damage);

    // Hàm để kiểm tra va chạm (lấy hitbox)
    sf::FloatRect GetBounds() const;

    // Hàm kiểm tra xem đã nổ chưa
    bool IsDead() const;

    // Hàm lấy vị trí
    sf::Vector2f GetPosition() const;

private:
    sf::Vector2f CalculateSpawnPosition() const;
    void SpawnSpider(std::vector<std::unique_ptr<IEnemy>>& enemies, sf::Vector2f spawnPos);

private:
    float m_spawnRate;
    float m_spawnTimer;
    bool m_isAlive;

    sf::Sprite m_sprite;
    sf::FloatRect m_spawnArea;
    sf::Vector2f m_worldPosition; // thêm vị trí thực (world)

    float m_spawnRadius; // bán kính spawn nhện

    // --- BIẾN MỚI CHO LOGIC NỔ ---
    static const int m_maxHealth = 5; // 10 máu = 10 phát bắn
    int m_currentHealth;
    bool m_hasExploded; // Flag để đảm bảo nó chỉ nổ 1 LẦN
};
