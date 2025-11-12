#include "EnemySpawner.h"
#include <iostream>
#include "SpiderEnemy.h"
#include <cmath>
#include <algorithm>

EnemySpawner::EnemySpawner(sf::Vector2f position, sf::Vector2f size, float rate)
    : m_spawnRate(rate),
    m_spawnTimer(0.0f),
    m_isAlive(true),
    m_sprite(AssetManeger::getInstance().getTexture("EnemySpawner_image.png")),
    m_spawnArea({ position.x, position.y }, { size.x, size.y }),
    m_worldPosition(position) // thêm biến vị trí thật
{
    // Đặt Origin về tâm của ảnh
    auto bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin(
        { bounds.position.x + bounds.size.x / 2.f,
          bounds.position.y + bounds.size.y / 2.f }
    );

    // Vị trí vẽ ban đầu (theo world)
    m_sprite.setPosition(CalculateSpawnPosition());
    float scaleFactor = 0.2f;
    m_sprite.setScale(sf::Vector2f(scaleFactor, scaleFactor));
}

// --- Cập nhật --- //
void EnemySpawner::Update(float dt, std::vector<std::unique_ptr<IEnemy>>& enemies, float scrollOffset) {
    if (!m_isAlive) return;

    m_spawnTimer += dt;

    // Cập nhật vị trí sprite trừ offset
    m_sprite.setPosition(CalculateSpawnPosition() - sf::Vector2f(scrollOffset, 0.f));

    // Khi đủ thời gian spawn
    if (m_spawnTimer >= m_spawnRate) {
        SpawnSpider(enemies, scrollOffset);
        m_spawnTimer = 0.0f;
    }
}

// --- Vẽ --- //
void EnemySpawner::Draw(sf::RenderWindow& window, float scrollOffset) const {
    if (!m_isAlive) return;

    // Luôn vẽ tại vị trí thật trừ offset (để camera cuộn)
    sf::Sprite tempSprite = m_sprite;
    tempSprite.setPosition(CalculateSpawnPosition() - sf::Vector2f(scrollOffset, 0.f));
    window.draw(tempSprite);
}

// --- Tính vị trí trung tâm (theo world) --- //
sf::Vector2f EnemySpawner::CalculateSpawnPosition() const {
    sf::Vector2f centerPosition = sf::Vector2f(
        m_spawnArea.position.x + m_spawnArea.size.x / 2.0f,
        m_spawnArea.position.y + m_spawnArea.size.y / 2.0f
    );
    centerPosition.y += 400.0f;
    centerPosition.x += 750.0f;
    return centerPosition;
}

// --- Sinh nhện --- //
void EnemySpawner::SpawnSpider(std::vector<std::unique_ptr<IEnemy>>& enemies, float scrollOffset) {
    // Spawn theo vị trí thế giới (world)
    sf::Vector2f spawnPos = CalculateSpawnPosition() - sf::Vector2f(scrollOffset, 0.f);
    auto newSpider = std::make_unique<SpiderEnemy>(spawnPos);
    enemies.push_back(std::move(newSpider));
}
