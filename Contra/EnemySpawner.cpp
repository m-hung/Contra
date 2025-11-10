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
        m_spawnArea({ position.x, position.y }, { size.x, size.y })
    {
        
        // Đặt Origin về tâm của ảnh
        auto bounds = m_sprite.getLocalBounds();
        m_sprite.setOrigin(
            { bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y + bounds.size.y / 2.f }
        );

        // Đặt vị trí của Sprite tại trung tâm vùng sinh
        m_sprite.setPosition(CalculateSpawnPosition());
        float scaleFactor = 0.2f;
        m_sprite.setScale(sf::Vector2f(scaleFactor, scaleFactor));
    }

    void EnemySpawner::Draw(sf::RenderWindow& window) const {
        if (m_isAlive) {
            window.draw(m_sprite);
        }
    }

    void EnemySpawner::Update(float dt, std::vector<std::unique_ptr<IEnemy>>& enemies) {
        if (!m_isAlive) {
            return;
        }

        m_spawnTimer += dt;

        if (m_spawnTimer >= m_spawnRate) {
            SpawnSpider(enemies);
            m_spawnTimer -= m_spawnRate;
        }
    }

    sf::Vector2f EnemySpawner::CalculateSpawnPosition() const {
        sf::Vector2f centerPosition = sf::Vector2f(
            m_spawnArea.position.x + m_spawnArea.size.x / 2.0f,
            m_spawnArea.position.y + m_spawnArea.size.y / 2.0f
        );
        centerPosition.y += 400.0f;
        centerPosition.x += 750.0f;
        return centerPosition;
    }


    void EnemySpawner::SpawnSpider(std::vector<std::unique_ptr<IEnemy>>& enemies) {
        sf::Vector2f spawnPos = CalculateSpawnPosition();

        auto newSpider = std::make_unique<SpiderEnemy>(spawnPos);

        enemies.push_back(std::move(newSpider));
    }