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

    // Ảnh nền
    sf::Texture m_backgroundTexture;
    std::unique_ptr<sf::Sprite> m_backgroundSprite;

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
