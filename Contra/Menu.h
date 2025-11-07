#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>

class Menu
{
private:
    sf::Font m_font;                         // Font chữ
    sf::Text m_title;                        // Tiêu đề game
    std::vector<sf::Text> m_options;         // Danh sách các lựa chọn
    int m_selectedIndex;                     // Vị trí đang chọn
    sf::RenderWindow* m_window;              // Cửa sổ chính

    // Background
    sf::Texture m_backgroundTexture;
    std::unique_ptr<sf::Sprite> m_backgroundSprite;

    // Nhạc nền
    sf::Music m_music;

public:
    explicit Menu(sf::RenderWindow* window);

    void HandleInput();                      // Xử lý bàn phím
    void Draw();                             // Vẽ menu
    int GetSelectedOption() const;           // Trả về lựa chọn hiện tại
};
