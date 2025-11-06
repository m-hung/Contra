#include "Game.h"
#include <SFML/Window/Event.hpp>

Game::Game()
    : m_window(sf::VideoMode({ 1280, 720 }), "PlanContra"),
    m_isRunning(true)
{
    m_window.setFramerateLimit(60);
}

void Game::Run() {
    while (m_isRunning && m_window.isOpen()) {
        float dt = m_clock.restart().asSeconds();
        ProcessInput(dt);
        Update(dt);
        Render();
    }
}

void Game::ProcessInput(float dt) {
    while (auto event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>())
            m_isRunning = false;
    }
    m_player.HandleInput(dt);
}

void Game::Update(float dt) {
    m_player.Update(dt);
    CheckCollisions();
}

void Game::Render() {
    m_window.clear();
    m_player.Draw(m_window);
    m_window.display();
}

void Game::CheckCollisions() {
}
