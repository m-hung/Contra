#pragma once

// Thay đổi header cho SFML 3
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>

#include <vector>
#include <memory>
#include "Player.h"
// #include "EnemySpawner.h"
// #include "IEnemy.h"
// #include "Bullet.h"
// #include "TileMap.h"
// #include "CollisionManager.h"

class Game {
private:
    sf::RenderWindow m_window;
    sf::Clock m_clock;
    Player m_player;
    // ... các thành viên khác
    bool m_isRunning;

    void ProcessInput();
    void Update(float dt);
    void Render();
    void CheckCollisions();

public:
    Game();
    void Run();
};