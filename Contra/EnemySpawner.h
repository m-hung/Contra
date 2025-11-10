#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "IEnemy.h"      
#include "SpiderEnemy.h" 
#include "AssetManeger.h"

class EnemySpawner {
public:
    EnemySpawner(sf::Vector2f position, sf::Vector2f size, float rate);
    void Draw(sf::RenderWindow& window) const;

    void Update(float dt, std::vector<std::unique_ptr<IEnemy>>& enemies);
    void Destroy() { m_isAlive = false; }

    bool IsAlive() const { return m_isAlive; }
    sf::FloatRect GetBounds() const { return m_spawnArea; }

private:
    float m_spawnRate;       // Thời gian giữa các lần sinh quái
    float m_spawnTimer;      // Bộ đếm để theo dõi thời gian sinh
    sf::FloatRect m_spawnArea; // Vùng/Vị trí của máy sinh enemy
    bool m_isAlive;           // Trạng thái hoạt động

    sf::Sprite m_sprite;

    // Tính toán vị trí sinh (tại trung tâm máy sinh)
    sf::Vector2f CalculateSpawnPosition() const;

    // Hàm chỉ định: Sinh ra Quái Nhện
    void SpawnSpider(std::vector<std::unique_ptr<IEnemy>>& enemies);
};
