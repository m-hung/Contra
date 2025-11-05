#include "Game.h"
#include <SFML/Window/Event.hpp>

Game::Game(sf::RenderWindow* window)
    : m_window(window), m_isRunning(true)
{
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

    // ✅ Gọi HandleInput đúng với dt
    m_player.HandleInput(dt);
}

void Game::Update(float dt) {
    // ✅ Cập nhật player (nếu bạn đang dùng code Player.cpp ở trên)
    m_player.Update(dt);

    CheckCollisions();
}

void Game::Render() {
    m_window->clear();
    m_player.Draw(*m_window);
    m_window->display();
}

void Game::CheckCollisions() {
    // Tạm thời trống
}
