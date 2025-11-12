#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "IEnemy.h"
#include "AssetManeger.h"

class EnemySpawner {
public:
    EnemySpawner(sf::Vector2f position, sf::Vector2f size, float rate);

    void Update(float dt, std::vector<std::unique_ptr<IEnemy>>& enemies, float scrollOffset);
    void Draw(sf::RenderWindow& window, float scrollOffset) const;

private:
    sf::Vector2f CalculateSpawnPosition() const;
    void SpawnSpider(std::vector<std::unique_ptr<IEnemy>>& enemies, float scrollOffset);

private:
    float m_spawnRate;
    float m_spawnTimer;
    bool m_isAlive;

    sf::Sprite m_sprite;
    sf::FloatRect m_spawnArea;
    sf::Vector2f m_worldPosition; // thêm vị trí thực (world)
};
