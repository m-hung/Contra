#include "GameEnd.h"
#include <iostream>

GameEnd::GameEnd(sf::RenderWindow* window)
    : m_window(window),
    m_currentState(EndState::NONE),
    m_restartRequested(false),
    m_mainText(m_font, "", 0),
	m_restartText(m_font, "", 0)
{
    // 1. Load Font (Dùng font "CinzelDecorative-Bold.ttf" đã thấy trong Menu.cpp/Game.cpp)
    if (!m_font.openFromFile("CinzelDecorative-Bold.ttf")) {
        std::cerr << "Khong the tai font CinzelDecorative-Bold.ttf cho GameEnd.\n";
    }

    sf::Vector2u windowSize = m_window->getSize();

    // 2. Thiết lập Overlay (Background trong suốt hơi tối)
    m_overlay.setSize(sf::Vector2f(windowSize));
    // Màu đen (0, 0, 0) với độ trong suốt 180 (khoảng 70% tối)
    m_overlay.setFillColor(sf::Color(0, 0, 0, 180));
    m_overlay.setPosition({ 0.f, 0.f });

    // 3. Thiết lập Main Text ("Win" / "Lose")
    m_mainText.setFont(m_font);
    m_mainText.setCharacterSize(80);

    // 4. Thiết lập Restart Text ("Push Enter to restart game")
    m_restartText.setFont(m_font);
    m_restartText.setCharacterSize(30);
    m_restartText.setString("Push Enter to restart game");
    m_restartText.setFillColor(sf::Color(200, 200, 200)); // Màu xám nhạt
}

// Hàm tiện ích: Căn giữa văn bản trên màn hình (dựa trên Origin)
void GameEnd::CenterText(sf::Text& text) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin({
        bounds.position.x + bounds.size.x * 0.5f,
        bounds.position.y + bounds.size.y * 0.5f
    });

    // Đặt vị trí X ở giữa màn hình
    text.setPosition({ m_window->getSize().x / 2.f, text.getPosition().y });
}


void GameEnd::SetEndState(EndState state) {
    m_currentState = state;
    m_restartRequested = false; // Reset cờ restart

    std::string displayText;
    sf::Color textColor = sf::Color::White;

    if (state == EndState::WIN) {
        displayText = "WIN";
        textColor = sf::Color::Green;
    }
    else if (state == EndState::LOSE) {
        displayText = "LOSE";
        textColor = sf::Color::Red;
    }
    else {
        displayText = "";
    }

    m_mainText.setString(displayText);
    m_mainText.setFillColor(textColor);

    // Căn giữa dòng chữ chính (Main Text)
    CenterText(m_mainText);
    // Vị trí Y: chính giữa màn hình (dịch lên một chút)
    m_mainText.setPosition({ m_mainText.getPosition().x, m_window->getSize().y / 2.f - 40.f });

    // Căn giữa dòng chữ restart, đặt phía dưới dòng chữ chính
    CenterText(m_restartText);
    // Vị trí Y: phía dưới dòng chữ chính
    m_restartText.setPosition({ m_restartText.getPosition().x, m_window->getSize().y / 2.f + 50.f });
}

void GameEnd::HandleInput() {
    // Chỉ xử lý input khi game đã kết thúc
    if (m_currentState != EndState::NONE) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter)) {
            m_restartRequested = true;
        }
    }
}

void GameEnd::Draw() {
    // Chỉ vẽ khi game đã kết thúc
    if (m_currentState != EndState::NONE) {
        // Vẽ overlay (nền tối)
        m_window->draw(m_overlay);

        // Vẽ dòng chữ chính ("WIN"/"LOSE")
        m_window->draw(m_mainText);

        // Vẽ dòng chữ restart
        m_window->draw(m_restartText);
    }
}

bool GameEnd::ShouldRestart() const {
    return m_restartRequested;
}