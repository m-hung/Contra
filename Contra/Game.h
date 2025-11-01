#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
//#include "Player.h"
//#include "EnemySpawner.h"
//#include "IEnemy.h"
//#include "Bullet.h"
//#include "TileMap.h"
//#include "CollisionManager.h"

class Game {
private:
    sf::RenderWindow m_window;                       // C?a s? game
    sf::Clock m_clock;                               // Dùng ?? tính deltaTime
    //Player m_player;                                 // Ng??i ch?i
    //EnemySpawner m_spawner;                          // Máy sinh ??ch
    //std::vector<std::unique_ptr<IEnemy>> m_enemies;  // Danh sách k? ??ch
    //std::vector<Bullet> m_bullets;                   // Danh sách ??n
    //TileMap m_map;                                   // B?n ??
    bool m_isRunning;                                // Tr?ng thái game

    void ProcessInput();
    void Update(float dt);
    void Render();
    void CheckCollisions();

public:
    Game();
    void Run();
};
