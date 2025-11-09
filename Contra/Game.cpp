#include "Game.h"
#include <SFML/Window/Event.hpp>
#include <iostream>   
#include <algorithm> 
#include "SoldierEnemy.h"
#include "SpiderEnemy.h"


Game::Game(sf::RenderWindow* window)
    : m_window(window), m_isRunning(true),
    m_spiderSpawner(sf::Vector2f(100.0f, 100.0f), sf::Vector2f(32.0f, 32.0f), 4.0f)
{
    InitEnemies();

    // --- Load Background ---
    if (!m_backgroundTexture.loadFromFile("background_game.png"))
    {
        std::cerr << "Không thể tải ảnh nền!\n";
    }
    else
    {
        m_backgroundSprite = std::make_unique<sf::Sprite>(m_backgroundTexture);

        // Tự động scale ảnh theo kích thước cửa sổ
        sf::Vector2u textureSize = m_backgroundTexture.getSize();
        sf::Vector2u windowSize = m_window->getSize();

        // Scale ảnh nền để vừa cửa sổ
        m_backgroundSprite->setScale(sf::Vector2f(
            static_cast<float>(windowSize.x) / textureSize.x,
            static_cast<float>(windowSize.y) / textureSize.y
        ));
    }

    // (Nếu cần) Load bản đồ hoặc player
    // m_map.LoadFromFile("assets/map.txt");
}

void Game::Run() {
    while (m_isRunning && m_window->isOpen()) {
        float dt = m_clock.restart().asSeconds();  // ✅ thời gian giữa 2 frame

        ProcessInput(dt);  // ✅ truyền dt vào
        Update(dt);
        Render();
    }
}

void Game::ProcessInput(float dt) {
    while (auto event = m_window->pollEvent()) {
        if (event->is<sf::Event::Closed>())
            m_isRunning = false;
    }
    m_player.HandleInput(dt);
}

void Game::InitEnemies() {
    // Kẻ địch 1: Tuần tra dài
    m_enemies.push_back(std::make_unique<SoldierEnemy>(
        sf::Vector2f(600.0f, 300.0f), // Vị trí spawn
        300.0f                        // Khoảng cách tuần tra
    ));

    // Kẻ địch 2: Tuần tra ngắn
    m_enemies.push_back(std::make_unique<SoldierEnemy>(
        sf::Vector2f(800.0f, 500.0f),
        100.0f
    ));
    std::cout << "[Game] Created " << m_enemies.size() << " initial enemies (Soldiers).\n";
    std::cout << "[Game] Initialized Spider Spawner.\n";

    std::cout << "[Game] Created " << m_enemies.size() << " enemies.\n";
}

void Game::Update(float dt) {
    m_player.Update(dt);

    sf::Vector2f playerPos = m_player.GetPosition();
    // Máy sinh sẽ tự động thêm SpiderEnemy mới vào m_enemies nếu đến lúc
    m_spiderSpawner.Update(dt, m_enemies);


    // Cập nhật Kẻ địch
    for (auto& enemy : m_enemies) {
        enemy->Update(dt, playerPos); 
    }

    // Xóa Kẻ địch đã chết
    CleanupDeadEnemies();

    CheckCollisions();
}

void Game::CleanupDeadEnemies() {
    m_enemies.erase(
        std::remove_if(m_enemies.begin(), m_enemies.end(),
            [](const std::unique_ptr<IEnemy>& e) {
                return e->IsDead();
            }),
        m_enemies.end()
    );
}


void Game::Render() {
    m_window->clear();

    // --- Vẽ background ---
    if (m_backgroundSprite)
        m_window->draw(*m_backgroundSprite);

    m_player.Draw(*m_window);
    m_spiderSpawner.Draw(*m_window);

    // Vẽ Kẻ địch
    for (auto& enemy : m_enemies) {
        enemy->Draw(*m_window);
    }

    m_window->display();
}

void Game::CheckCollisions() {
    // Tạm thời trống
}
   