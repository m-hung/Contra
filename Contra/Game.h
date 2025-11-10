#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"
#include <vector>
#include <memory>
#include "IEnemy.h"
#include <SFML/Audio.hpp>
#include "SoldierEnemy.h"

class Game {
private:
    sf::RenderWindow* m_window;
    Player m_player;

    std::vector<std::unique_ptr<IEnemy>> m_enemies;

    // Ảnh nền game
    std::vector<sf::Texture> m_bgTextures;
    std::vector<sf::Sprite> m_bgSprites;

    // --- Biến dùng cho hiệu ứng cuộn nền ---
    float m_lastPlayerX = 0.f;     // lưu vị trí X của player ở frame trước
    float m_totalScroll = 0.f;     // tổng quãng cuộn nền tích lũy
    float m_bgScrollSpeed = 1.f;  // tốc độ cuộn nền (tỉ lệ so với player)

    // THÊM BIẾN NHẠC
    sf::Music m_gameMusic;

    void CheckCollisions();
    void InitEnemies();      
    void CleanupDeadEnemies();

public:
    explicit Game(sf::RenderWindow* window);
    ~Game();
    void HandleInput(float dt);
    void Update(float dt);
    void Draw();
};
