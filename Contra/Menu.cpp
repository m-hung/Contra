#include "Menu.h"
#include <iostream>
#include <stdexcept>

//--------------------------------------------------------------
// Constructor: Khởi tạo menu và các thành phần hiển thị
//--------------------------------------------------------------
Menu::Menu(sf::RenderWindow* window)
    : m_window(window)
    , m_selectedIndex(0)
    , m_title(m_font, "", 0)
{
    // Load background
    if (!m_backgroundTexture.loadFromFile("menu_bg.png"))
    {
        std::cerr << "Không thể tải ảnh nền: menu_bg.png\n";
    }
    else
    {
        // GÁN texture cho sprite sau khi load thành công
        m_backgroundSprite = std::make_unique<sf::Sprite>(m_backgroundTexture);


        sf::Vector2u windowSize = m_window->getSize();
        sf::Vector2u textureSize = m_backgroundTexture.getSize();
        float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
        float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
        m_backgroundSprite->setScale({ scaleX, scaleY });
        m_backgroundSprite->setPosition({ 0.f, 0.f });
    }

    // Load font
    if (!m_font.openFromFile("Quantico-Bold.ttf"))
        std::cerr << "Không thể tải font: Quantico-Bold.ttf\n";

    // Load music
    if (!m_music.openFromFile("musicBG_menu.mp3"))
        std::cerr << "Không thể tải nhạc nền: musicBG_menu.mp3\n";
    else {
        m_music.setLooping(true);
        m_music.setVolume(100.f);
        m_music.play();
    }

    // Title
    m_title.setString("CONTRA");
    m_title.setCharacterSize(64);
    m_title.setFillColor(sf::Color::Yellow);
    m_title.setPosition({ 200.f, 100.f });  // ✅ sửa cú pháp

    // Menu options
    std::vector<std::string> labels = { "Start Game", "Exit" };
    for (size_t i = 0; i < labels.size(); ++i) {
        sf::Text text(m_font, labels[i], 36);
        text.setFillColor(i == 0 ? sf::Color::Red : sf::Color::White);
        text.setPosition({ 250.f, 300.f + static_cast<float>(i) * 60.f }); // ✅ sửa cú pháp
        m_options.push_back(std::move(text));
    }
}

//--------------------------------------------------------------
// HandleInput: Xử lý sự kiện theo SFML 3 (pollEvent trả về std::optional)
//--------------------------------------------------------------
void Menu::HandleInput()
{
    while (auto event = m_window->pollEvent())
    {
        // Đóng cửa sổ
        if (event->is<sf::Event::Closed>())
        {
            m_window->close();
            continue;
        }

        // Nếu là phím nhấn
        if (auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            using Sc = sf::Keyboard::Scancode;

            if (keyPressed->scancode == Sc::Down)
            {
                m_selectedIndex = (m_selectedIndex + 1) % static_cast<int>(m_options.size());
            }
            else if (keyPressed->scancode == Sc::Up)
            {
                m_selectedIndex = (m_selectedIndex - 1 + static_cast<int>(m_options.size())) % static_cast<int>(m_options.size());
            }
            else if (keyPressed->scancode == Sc::Enter)
            {
                // xử lý khi nhấn Enter
                if (m_selectedIndex == 1) // Exit
                {
                    m_window->close();
                }
                else if (m_selectedIndex == 0) // Start Game
                {
                    // Dừng nhạc nền khi bắt đầu game
                    if (m_music.getStatus() == sf::SoundSource::Status::Playing)
                        m_music.stop();

                    // (Tùy bạn) Gọi scene gameplay ở đây
                    std::cout << "Start game!\n";

                    // TODO: chuyển scene sang gameplay hoặc set flag
                    // ví dụ: m_startRequested = true;
                }
            }
        }
    }

    // Cập nhật màu menu: mục đang chọn = đỏ, còn lại trắng
    for (std::size_t i = 0; i < m_options.size(); ++i)
    {
        m_options[i].setFillColor(i == static_cast<std::size_t>(m_selectedIndex)
            ? sf::Color::Red
            : sf::Color::White);
    }
}

//--------------------------------------------------------------
// Draw: Vẽ menu
//--------------------------------------------------------------
void Menu::Draw()
{
    // Vẽ background nếu có
    if (m_backgroundSprite)
        m_window->draw(*m_backgroundSprite);

    // Vẽ title và các lựa chọn
    m_window->draw(m_title);
    for (auto& option : m_options)
        m_window->draw(option);

    m_window->display();
}

int Menu::GetSelectedOption() const
{
    return m_selectedIndex;
}

//--------------------------------------------------------------
// StopMusic: Dừng nhạc nền của menu
//--------------------------------------------------------------
void Menu::StopMusic()
{
    if (m_music.getStatus() == sf::SoundSource::Status::Playing)
        m_music.stop();
}

