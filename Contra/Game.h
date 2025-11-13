#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"
#include <vector>
#include <memory>
#include "IEnemy.h"
#include "SoldierEnemy.h"
#include "EnemySpawner.h"
#include "Bullet.h"
#include "GameObject.h"
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
class Game {
private:
    sf::RenderWindow* m_window;
    Player m_player;
    sf::Clock m_clock;
    bool m_isRunning;
    EnemySpawner m_spiderSpawner;

    std::vector<std::unique_ptr<IEnemy>> m_enemies;

    // Ảnh nền game
    std::vector<sf::Texture> m_bgTextures;
    std::vector<sf::Sprite> m_bgSprites;

    // --- Biến dùng cho hiệu ứng cuộn nền ---
    float m_lastPlayerX = 0.f;     // lưu vị trí X của player ở frame trước
    float m_totalScroll = 0.f;     // tổng quãng cuộn nền tích lũy
    float m_bgScrollSpeed = 1.f;  // tốc độ cuộn nền (tỉ lệ so với player)

    //Vat the
    std::vector<GameObject> m_objects;

    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    void CheckCollisions();

    void InitEnemies();      
    void CleanupDeadEnemies();
    //am thanh trung dan
    sf::SoundBuffer m_impactBuffer;
    sf::Sound m_impactSound;

public:
    explicit Game(sf::RenderWindow* window);
    void Run();
};
