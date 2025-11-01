#include "Game.h"
#include <iostream>

Game::Game()
    : m_window(sf::VideoMode({ 800, 600 }), "PlanContra"),
    //m_spawner(sf::FloatRect(0, 0, 800, 600), 3.0f),
    m_isRunning(true)
{
    m_window.setFramerateLimit(60);
    //m_map.LoadFromFile("assets/map.txt");
}

void Game::Run() {
    while (m_isRunning && m_window.isOpen()) {
        float dt = m_clock.restart().asSeconds();
        ProcessInput();
        Update(dt);
        Render();
    }
}

void Game::ProcessInput() {
    while (auto event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>())
            m_isRunning = false;
    }

    // Điều khiển người chơi
    //m_player.HandleInput(m_clock.getElapsedTime().asSeconds());
}

void Game::Update(float dt) {
    //// Cập nhật player
    //m_player.Update(dt, m_bullets);

    //// Sinh địch mới
    //m_spawner.Update(dt, m_enemies);

    //// Cập nhật từng kẻ địch
    //for (auto& enemy : m_enemies)
    //    enemy->Update(dt, m_player.GetPosition());

    //// Cập nhật đạn
    //for (auto& bullet : m_bullets)
    //    bullet.Update(dt);

    //// Xóa đạn ngoài màn hình
    //m_bullets.erase(
    //    std::remove_if(m_bullets.begin(), m_bullets.end(),
    //        [](const Bullet& b) { return b.IsOutOfScreen(); }),
        //m_bullets.end());

    // Kiểm tra va chạm
    CheckCollisions();
}

void Game::Render() {
    m_window.clear();

    //m_map.Draw(m_window);
    //m_player.Draw(m_window);

    //for (auto& enemy : m_enemies)
    //    enemy->Draw(m_window);

    //for (auto& bullet : m_bullets)
    //    bullet.Draw(m_window);

    m_window.display();
}

void Game::CheckCollisions() {
    //// Va chạm Player - Enemy
    //CollisionManager::HandlePlayerEnemy(m_player, m_enemies);

    //// Va chạm Bullet - Enemy
    //CollisionManager::HandleBullets(m_bullets, m_enemies);
}
