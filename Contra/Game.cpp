<<<<<<< Updated upstream
=======
﻿#include "Game.h"
#include <SFML/Window/Event.hpp> // Header cụ thể

Game::Game()
    : m_window(sf::VideoMode({ 1280, 720 }), "Contra"),
    m_player(),
    m_isRunning(true),
    m_clock()
{
    m_window.setFramerateLimit(60);
}

void Game::Run()
{
    while (m_isRunning)
    {
        sf::Time dt = m_clock.restart();
        ProcessInput();
        Update(dt.asSeconds());
        Render();
    }
}

void Game::ProcessInput()
{
  
    while (auto event = m_window.pollEvent())
    {
        // Thay đổi: sf::Event::CloseRequested
        if (event->is<sf::Event::Closed>())
        {
            m_isRunning = false;
        }
    }
}

void Game::Update(float dt)
{
    m_player.HandleInput(dt);
    m_player.Update(dt);
    // CheckCollisions();
}

void Game::Render()
{
    m_window.clear();
    m_player.Draw(m_window);
    m_window.display();
}

void Game::CheckCollisions()
{
    // Xử lý va chạm
}
>>>>>>> Stashed changes
