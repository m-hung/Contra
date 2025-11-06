#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"
#include <vector>
#include <memory>
#include "IEnemy.h"
#include "SoldierEnemy.h"

class Game {
private:
    sf::RenderWindow* m_window;
    Player m_player;
    sf::Clock m_clock;
    bool m_isRunning;

    std::vector<std::unique_ptr<IEnemy>> m_enemies;

    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    void CheckCollisions();

    void InitEnemies();      
    void CleanupDeadEnemies();

public:
    explicit Game(sf::RenderWindow* window);
    void Run();
};
