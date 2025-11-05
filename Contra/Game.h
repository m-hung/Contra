#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"

class Game {
private:
    sf::RenderWindow* m_window;
    Player m_player;
    sf::Clock m_clock;
    bool m_isRunning;

    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    void CheckCollisions();

public:
    explicit Game(sf::RenderWindow* window);
    void Run();
};
