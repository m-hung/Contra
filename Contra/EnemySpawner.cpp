#include "EnemySpawner.h"
#include <iostream>
#include "SpiderEnemy.h"
#include <cmath>
#include <algorithm>

EnemySpawner::EnemySpawner(sf::Vector2f position, sf::Vector2f size, float rate)
    : m_spawnRate(rate),
    m_spawnTimer(0.0f),
    m_sprite(AssetManeger::getInstance().getTexture("EnemySpawner_image.png")),
    m_spawnArea({ position.x, position.y }, { size.x, size.y }),
    m_spawnRadius(650.0f), // 🔹 bán kính vùng spawn

    // --- KHỞI TẠO BIẾN MỚI --- 
    m_currentHealth(m_maxHealth), // 10 máu
    m_hasExploded(false)         // Chưa nổ
{
    // Tính toán vị trí THẾ GIỚI (World) thật sự của kén (chỉ 1 lần)
    m_worldPosition = CalculateSpawnPosition();
    // Đặt Origin về tâm của ảnh
    auto bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin(
        { bounds.position.x + bounds.size.x / 2.f,
          bounds.position.y + bounds.size.y / 2.f }
    );

    // Vị trí vẽ ban đầu (theo world)
    m_sprite.setPosition(m_worldPosition);
    float scaleFactor = 0.2f;
    m_sprite.setScale(sf::Vector2f(scaleFactor, scaleFactor));
}

// --- Cập nhật --- //
void EnemySpawner::Update(float dt, std::vector<std::unique_ptr<IEnemy>>&enemies, float scrollOffset, sf::Vector2f playerPos) {
    // Nếu đã nổ thì không làm gì cả
    if (m_hasExploded) return;

    // --- LOGIC NỔ ---
    if (IsDead()) {
        m_hasExploded = true; // Đánh dấu đã nổ

        // --- Spawn 7 con nhện "tỏa ra" theo vòng tròn ---
        const int numSpiders = 7;
        const float spawnRadius = 40.0f; // Tỏa ra cách tâm 40px
        const float PI = 3.14159265f;

        // Spawn 7 con nhện ngay tại vị trí kén
        for (int i = 0; i < numSpiders; ++i) {
            // Tính góc cho từng con nhện (chia đều vòng tròn)
            // (static_cast<float>(i) / numSpiders) -> 0/7, 1/7, 2/7...
            // (2.0f * PI) -> 360 độ (tính bằng radian)
            float angle = (static_cast<float>(i) / numSpiders) * (2.0f * PI);

            // Tính vị trí offset x, y từ góc
            float offsetX = spawnRadius * cos(angle);
            float offsetY = spawnRadius * sin(angle);

            // Vị trí spawn = Vị trí kén + offset
            sf::Vector2f spawnPos = m_worldPosition + sf::Vector2f(offsetX, offsetY);
            SpawnSpider(enemies, spawnPos);
        }
        return; // Dừng hàm update
    }

    //m_spawnTimer += dt;

    // Cập nhật vị trí sprite trừ offset
    m_sprite.setPosition(m_worldPosition - sf::Vector2f(scrollOffset, 0.f));

    // Khi đủ thời gian spawn
    /*if (m_spawnTimer >= m_spawnRate) {
        SpawnSpider(enemies, scrollOffset);
        m_spawnTimer = 0.0f;
    }*/

    // Tính khoảng cách giữa player và spawner
    sf::Vector2f spawnerScreenPos = m_sprite.getPosition();
    sf::Vector2f delta = playerPos - spawnerScreenPos;
    float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);

    // Nếu player trong vùng spawn (<= 300.f)
    if (distance <= m_spawnRadius) {
        m_spawnTimer += dt;

        if (m_spawnTimer >= m_spawnRate) {
            SpawnSpider(enemies, m_worldPosition);
            m_spawnTimer = 0.0f;
        }
    }
}

// --- Vẽ --- //
void EnemySpawner::Draw(sf::RenderWindow& window, float scrollOffset) const {
    // Nếu đã nổ thì không vẽ
    if (m_hasExploded) return;

    // Vị trí sprite đã được set trong Update(), chỉ cần vẽ
    window.draw(m_sprite);
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
void EnemySpawner::SpawnSpider(std::vector<std::unique_ptr<IEnemy>>& enemies, sf::Vector2f spawnPos) {
    // Spawn theo vị trí thế giới (world)
    auto newSpider = std::make_unique<SpiderEnemy>(spawnPos);
    enemies.push_back(std::move(newSpider));
}

// =======================================================
// --- CÁC HÀM MỚI CHO LOGIC BỊ BẮN ---
// =======================================================

void EnemySpawner::TakeDamage(int damage) {
    if (IsDead()) return; // Đã chết thì không nhận thêm sát thương

    m_currentHealth -= damage;
    if (m_currentHealth < 0) {
        m_currentHealth = 0;
    }
}

bool EnemySpawner::IsDead() const {
    return m_currentHealth <= 0;
}

sf::Vector2f EnemySpawner::GetPosition() const {
    // Trả về vị trí THẾ GIỚI
    return m_worldPosition;
}

sf::FloatRect EnemySpawner::GetBounds() const {
    // 1. Kích thước (chiều rộng, chiều cao) của hộp 
    const float hitboxWidth = 140.f;  // (Giá trị cũ của bạn)
    const float hitboxHeight = 100.f; // (Giá trị cũ của bạn)

    // 2. Vị trí của CHÂN (offset từ tâm xuống)
    const float feetOffset = 50.f;   // (Giá trị cũ của bạn)

    // ================================================================


    // 1. Tính vị trí bên trái (left)
    // SỬA LỖI Ở ĐÂY: m_position.x -> m_worldPosition.x
    float left = m_worldPosition.x - (hitboxWidth / 2.f);

    // 2. Tính vị trí bên trên (top)
    // SỬA LỖI Ở ĐÂY: m_position.y -> m_worldPosition.y
    float top = (m_worldPosition.y + feetOffset) - hitboxHeight;

    // 3. Trả về hitbox mới (Đây là Tọa độ Thế giới - World Coords)
    return sf::FloatRect({ left, top }, { hitboxWidth, hitboxHeight });
}